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
static const char* GENCC_OPTIONS = "GENCC_OPTIONS";
static const char* C_EXT = ".c";

static const int MAX_DB_RETRIES = 100;
static const int MAX_FALLBACK_SLEEP_IN_MS = 50;

enum class gencc_mode {
    BUILDER,
    COMPILER
};

typedef struct gencc_options {
    gencc_mode mode = gencc_mode::BUILDER;
    bool build = false;
    std::string dbFilename = DB_FILENAME;
    std::string cxx;
    std::string cc;
    std::string compiler;
    int retries = MAX_DB_RETRIES;
    int fallback = MAX_FALLBACK_SLEEP_IN_MS;
} gencc_options_t;

using json = nlohmann::json;

static gencc_options_t options;

void help()
{
    std::cout << "Help:\n"
                 "\t-cxx    [value] - CXX compiler\n"
                 "\t-cc     [value] - CC compiler\n"
                 "\t-o      [value] - DB file\n"
                 "\t-r      [value] - Number of retries if DB locked\n"
                 "\t-f      [value] - Max fallback time in ms in case of DB locked\n"
                 "\t-build          - Call the actual compiler\n";
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

void set_env_var(const char* name, const std::string& value)
{
    std::string var;
    setenv(name, value.c_str(), 1);
    get_env_var(name, var);
    if (var != value) {
        throw std::runtime_error(std::string("Couldn't set new var ") + name + " to " + value);
    }
}

int get_cwd(std::string& str)
{
    char buffer[256];
    if (!getcwd(buffer, sizeof(buffer))) {
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
    std::string origPath, tmp, cwd, newPath;
    std::stringstream ss;

    get_env_var(PATH, origPath);
    if (options.cxx.empty() && !get_env_var(CXX, tmp)) {
        options.cxx = tmp;
    }
    if (options.cc.empty() && !get_env_var(CC, tmp)) {
        options.cc = tmp;
    }

    if (get_cwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    std::cout << "Original PATH = " << origPath << '\n';
    std::cout << "Original CXX = " << options.cxx << '\n';
    std::cout << "Original CC = " << options.cc << '\n';
    std::cout << "CWD = " << cwd << '\n';

    newPath = cwd + ":" + origPath;
    set_env_var(PATH, newPath);

    options.dbFilename = cwd + "/" + options.dbFilename;
    std::remove(options.dbFilename.c_str());
    std::remove((options.dbFilename + DB_LOCK_FILENAME_EXT).c_str());

    // Serialize the options through an environment variable
    json jsonObj;
    jsonObj["build"] = options.build;
    jsonObj["dbFilename"] = options.dbFilename;
    ss.str("");
    ss.clear();
    ss << jsonObj;
    set_env_var(GENCC_OPTIONS, ss.str());
    set_env_var(CXX, params.at(0) + " -gencc-compiler " + options.cxx);
    set_env_var(CC, params.at(0) + " -gencc-compiler " + options.cc);

    ss.str("");
    ss.clear();
    for (size_t i = 1; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }
    }

    if (int ret = system(ss.str().c_str())) {
        std::cout << "The command " << ss.str() << " exited with error code "
                  << ret << '\n';
    }
}

void write_to_db(const std::string& directory, const std::string& command, const std::string& file)
{
    std::string dbFilepath = options.dbFilename;
    std::string dbLockFilepath = dbFilepath + DB_LOCK_FILENAME_EXT;

    int retries = 0;
    do {
        std::ifstream iLockFile(dbLockFilepath);
        if (iLockFile.good()) {
            int fallbackValue = rand() % options.fallback;
            std::cout << dbLockFilepath << " already exists. Trying again in "
                      << fallbackValue << " ms"
                      << '\n';
            usleep(fallbackValue * 1000u);
            continue;
        }
        iLockFile.close();

        std::ofstream oLockFile(dbLockFilepath);
        oLockFile.close();

        std::ifstream idb(options.dbFilename);

        json jsonDb = json::array();
        if (idb.good()) {
            idb >> jsonDb;
        }

        json jsonObj;
        jsonObj["directory"] = directory;
        jsonObj["command"] = command;
        jsonObj["file"] = file;
        jsonDb.push_back(jsonObj);

        std::ofstream odb(options.dbFilename);
        odb << jsonDb.dump(4);

        std::remove(dbLockFilepath.c_str());
        return;
    } while (++retries <= options.retries);
}

void compiler_call(const std::vector<std::string>& params)
{
    std::stringstream ss;
    std::string cwd, directory, command, file;

    if (get_cwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    // Deserialize options embedded in the env variable
    std::string genccOptions;
    json jsonObj;
    if (get_env_var(GENCC_OPTIONS, genccOptions)) {
        throw std::runtime_error(std::string("Couldn't read env var ") + GENCC_OPTIONS);
    }
    ss << genccOptions;
    jsonObj << ss;

    //std::cout << GENCC_OPTIONS << " = " << genccOptions << '\n';
    options.build = jsonObj["build"];
    options.dbFilename = jsonObj["dbFilename"];

    ss.str("");
    ss.clear();
    ss << options.compiler << " ";

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

    std::cout << command << '\n';
    write_to_db(directory, command, file);

    if (options.build) {
        if (int ret = system(ss.str().c_str())) {
            std::cout << "The command " << ss.str() << " exited with error code "
                      << ret << '\n';
        }
    }
}

int parse_args(std::vector<std::string>& params)
{
    auto it = params.begin();
    for (; it != params.end(); ++it) {
        if (it == params.begin() + 1 && it->find("-") != std::string::npos) {
            break;
        }
    }

    for (; it != params.end();) {
        std::string param = *it;
        if (param.find("-") == std::string::npos) {
            break;
        }

        if (param == "-gencc-compiler" && it + 1 != params.end()) {
            params.erase(it);
            options.compiler = *it;
            params.erase(it);

            // The compiler is the only parameter that needs to be parsed in compiler mode
            break;
        } else if (param == "-cxx" && it + 1 != params.end()) {
            params.erase(it);
            options.cxx = *it;
            params.erase(it);
        } else if (param == "-cc" && it + 1 != params.end()) {
            params.erase(it);
            options.cc = *it;
            params.erase(it);
        } else if (param == "-o" && it + 1 != params.end()) {
            params.erase(it);
            options.dbFilename = *it;
            params.erase(it);
        } else if (param == "-r" && it + 1 != params.end()) {
            params.erase(it);
            options.retries = std::stoi(*it);
            params.erase(it);
        } else if (param == "-f" && it + 1 != params.end()) {
            params.erase(it);
            options.fallback = std::stoi(*it);
            params.erase(it);
        } else if (param == "-build") {
            params.erase(it);
            options.build = true;
        } else {
            return -1;
        }
    }

    return 0;
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

    std::string mode;
    if (get_env_var(GENCC_OPTIONS, mode)) {
        options.mode = gencc_mode::BUILDER;
    } else {
        options.mode = gencc_mode::COMPILER;
    }

    if (parse_args(params)) {
        std::cout << "Error parsing arguments"
                  << '\n';
        help();
        return -1;
    }

    try {
        if (options.mode == gencc_mode::BUILDER) {
            std::cout << '\n';
            build_call(params);
        } else {
            compiler_call(params);
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << '\n';
    }

    return 0;
}
