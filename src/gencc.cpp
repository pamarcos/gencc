#include "json.hpp"
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

static const char* VERSION = "0.1";
static const char* NAME = "gencc";
static const char* PATH = "PATH";
static const char* CXX = "CXX";
static const char* CC = "CC";
static const char* DB_FILENAME = "compile_commands.json";
static const char* DB_LOCK_FILENAME_EXT = ".lock";
static const char* ORIG_CXX = "ORIG_CXX";
static const char* ORIG_CC = "ORIG_CC";
static const char* GENCC_MODE = "GENCC_MODE";
static const char* COMPILER = "COMPILER";
static const char* GENCC_DB_FILE_ENV = "GCD_DB_FILE_ENV";
static const char* GENCC_DB_LOCK_FILE_ENV = "GCD_DB_LOCK_FILE_ENV";
static const char* C_EXT = ".c";

static const int MAX_DB_RETRIES = 100;
static const int MAX_FALLBACK_SLEEP_IN_MS = 50;

enum class gencc_mode {
    BUILDER,
    COMPILER
};

typedef struct gencc_options {
    gencc_mode mode = gencc_mode::BUILDER;
    std::string compiler;
    std::string dbFilename = DB_FILENAME;
} gencc_options_t;

using json = nlohmann::json;

static gencc_options_t options;

void help()
{
    std::cout << "Help"
              << "\n";
}

int get_env_var(const char* name, std::string& str)
{
    const char* ptr = getenv(name);
    if (ptr) {
        str = ptr;
        return 0;
    } else {
        str.clear();
        return -1;
    }
}

int get_cwd(std::string& str)
{
    char buffer[256];
    if (!getcwd(buffer, sizeof(buffer))) {
        std::cout << "Couldn't get working directory"
                  << "\n";
        str.clear();
        return -1;
    }
    str = buffer;
    return 0;
}

/* Helper function to simply set the PATH correctly so that this binary
 * is called instead of the default CXX one */
void build_call(const std::vector<std::string>& params)
{
    std::string origPath, origCXX, origCC, cwd, newPath, newPathCheck;
    get_env_var(PATH, origPath);

    if (!get_env_var(CXX, origCXX)) {
        setenv(ORIG_CXX, origCXX.c_str(), 1);
    }
    if (!get_env_var(CC, origCC)) {
        setenv(ORIG_CC, origCC.c_str(), 1);
    }

    if (get_cwd(cwd)) {
        throw std::runtime_error("Couldn't get CWD");
    }

    std::cout << "Original PATH = " << origPath << "\n";
    std::cout << "Original CXX = " << origCXX << "\n";
    std::cout << "CWD = " << cwd << "\n";

    newPath = cwd + ":" + origPath;

    setenv(CXX, params.at(0).c_str(), 1);
    setenv(CC, params.at(0).c_str(), 1);
    setenv(PATH, newPath.c_str(), 1);
    get_env_var(PATH, newPathCheck);
    if (newPathCheck != newPath) {
        throw std::runtime_error("Couldn't set new PATH to " + newPath);
    }

    std::string dbFilepath = cwd + "/" + DB_FILENAME;
    std::string dbLockFilepath = cwd + "/" + DB_FILENAME + DB_LOCK_FILENAME_EXT;
    setenv(GENCC_DB_FILE_ENV, dbFilepath.c_str(), 1);
    setenv(GENCC_DB_LOCK_FILE_ENV, dbLockFilepath.c_str(), 1);
    std::remove(dbFilepath.c_str());
    std::remove(dbLockFilepath.c_str());

    std::stringstream ss;
    for (size_t i = 1; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }
    }

    setenv(GENCC_MODE, COMPILER, 1);
    if (int ret = system(ss.str().c_str())) {
        std::cout << "The command " << ss.str() << " exited with error code "
                  << ret << "\n";
    }
}

void write_to_db(const std::string& directory, const std::string& command, const std::string& file)
{
    std::string dbFilepath, dbLockFilepath;
    if (get_env_var(GENCC_DB_FILE_ENV, dbFilepath)) {
        dbFilepath = DB_FILENAME;
    }
    if (get_env_var(GENCC_DB_LOCK_FILE_ENV, dbLockFilepath)) {
        dbLockFilepath = dbFilepath + DB_LOCK_FILENAME_EXT;
    }

    int retries = 0;
    do {
        std::ifstream iLockFile(dbLockFilepath);
        if (iLockFile.good()) {
            unsigned int fallbackValue = rand() % MAX_FALLBACK_SLEEP_IN_MS;
            std::cout << dbLockFilepath << " already exists. Trying again in "
                      << fallbackValue << " ms"
                      << "\n";
            usleep(fallbackValue * 1000u);
            continue;
        }
        iLockFile.close();

        std::ofstream oLockFile(dbLockFilepath);
        oLockFile.close();

        std::ifstream idb(dbFilepath);

        json jsonDb = json::array();
        if (idb.good()) {
            idb >> jsonDb;
        }

        json jsonObj;
        jsonObj["directory"] = directory;
        jsonObj["command"] = command;
        jsonObj["file"] = file;
        jsonDb.push_back(jsonObj);

        std::ofstream odb(dbFilepath);
        odb << jsonDb.dump(4);

        std::remove(dbLockFilepath.c_str());
        return;
    } while (++retries <= MAX_DB_RETRIES);
}

void compiler_call(const std::vector<std::string>& params)
{
    std::stringstream ss;
    std::string origCXX, origCC, cwd, directory, command, file;

    if (get_cwd(cwd)) {
        throw std::runtime_error("Couldn't get CWD");
    }

    get_env_var(ORIG_CXX, origCXX);
    get_env_var(ORIG_CC, origCC);

    if (!options.compiler.empty()) {
        ss << options.compiler << " ";
    } else if (!origCXX.empty()) {
        ss << origCXX << " ";
    } else if (!origCC.empty()) {
        ss << origCC << " ";
    } else {
        ss << params[0] << " ";
    }

    directory = cwd;
    for (size_t i = 1; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }

        if (params.at(i).find(C_EXT) != std::string::npos) {
            file = directory + "/" + params.at(i);
        }
    }
    command = ss.str();

    std::cout << command << "\n";

    write_to_db(directory, command, file);
}

void parse_args(std::vector<std::string>& params)
{
    size_t firstParamPos = 0;
    for (size_t i = 0; i < params.size(); ++i) {
        if (firstParamPos == 0 && params.at(i).find("-") != std::string::npos) {
            firstParamPos = i;
        }
    }
}

int main(int argc, char* argv[])
{
    std::cout << NAME << " v" << VERSION << ": ";

    if (argc < 2) {
        help();
        return 0;
    }

    std::vector<std::string> params;
    for (int i = 0; i < argc; ++i) {
        params.emplace_back(argv[i]);
    }
    std::size_t pos = params[0].find_last_of('/');
    if (pos != std::string::npos) {
        params[0] = params[0].substr(pos + 1);
    }

    // TODO: parse arguments to do different behavior
    //parse_args(params);

    std::string genccMode;
    get_env_var(GENCC_MODE, genccMode);
    if (genccMode == COMPILER) {
        options.mode = gencc_mode::COMPILER;
    }

    try {
        if (options.mode == gencc_mode::BUILDER) {
            std::cout << "\n";
            build_call(params);
        } else {
            compiler_call(params);
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
    }

    return 0;
}
