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
static const char* DB_FILENAME = "compile_commands.json";
static const char* DB_LOCK_FILENAME = ".compile_commands.json.lock";
static const char* GCD_DB_FILE_ENV = "GCD_DB_FILE_ENV";
static const char* GCD_DB_LOCK_FILE_ENV = "GCD_DB_LOCK_FILE_ENV";
static const char* C_EXT = ".c";

static const std::set<std::string> BUILD_COMMANDS{ "make", "ninja" };

using json = nlohmann::json;

void help()
{
    std::cout << "Help" << std::endl;
}

int get_env_var(const char* name, std::string& str)
{
    const char* ptr = getenv(name);
    if (ptr) {
        str = ptr;
        return 0;
    } else {
        std::cout << "Environment variable is empty: " << name << std::endl;
        str.clear();
        return -1;
    }
}

int get_cwd(std::string& str)
{
    char buffer[256];
    if (!getcwd(buffer, sizeof(buffer))) {
        std::cout << "Couldn't get working directory" << std::endl;
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
    std::string origPath, cwd, newPath, newPathCheck;
    get_env_var(PATH, origPath);

    if (get_cwd(cwd)) {
        throw std::runtime_error("Couldn't get CWD");
    }

    std::cout << "Original PATH = " << origPath << std::endl;
    std::cout << "CWD = " << cwd << std::endl;

    newPath = cwd + ":" + origPath;

    setenv(CXX, params.at(0).c_str(), 1);
    setenv(PATH, newPath.c_str(), 1);
    get_env_var(PATH, newPathCheck);
    if (newPathCheck != newPath) {
        throw std::runtime_error("Couldn't set new PATH to " + newPath);
    }

    std::string dbFilepath = cwd + "/" + DB_FILENAME;
    std::string dbLockFilepath = cwd + "/" + DB_LOCK_FILENAME;
    setenv(GCD_DB_FILE_ENV, dbFilepath.c_str(), 1);
    setenv(GCD_DB_LOCK_FILE_ENV, dbLockFilepath.c_str(), 1);
    std::remove(dbFilepath.c_str());
    std::remove(dbLockFilepath.c_str());

    std::stringstream ss;
    for (size_t i = 1; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }
    }
    if (int ret = system(ss.str().c_str())) {
        std::cout << "The command " << ss.str() << " exited with error code "
                  << ret << std::endl;
    }
}

void compiler_call(const std::vector<std::string>& params)
{
    std::stringstream ss;
    std::string cwd, directory, command, file;

    if (get_cwd(cwd)) {
        throw std::runtime_error("Couldn't get CWD");
    }

    directory = cwd;
    for (size_t i = 0; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }

        if (params.at(i).find(C_EXT) != std::string::npos) {
            file = directory + "/" + params.at(i);
        }
    }
    command = ss.str();

    std::cout << command << std::endl;

    std::string dbFilepath;
    if (get_env_var(GCD_DB_FILE_ENV, dbFilepath)) {
        dbFilepath = DB_FILENAME;
    }
    std::string dbLockFilepath;
    if (get_env_var(GCD_DB_LOCK_FILE_ENV, dbLockFilepath)) {
        dbLockFilepath = DB_LOCK_FILENAME;
    }

    int retries = 0;
    do {
        std::ifstream iLockFile(dbLockFilepath);
        if (iLockFile.good()) {
            unsigned int fallbackValue = rand() % 50;
            std::cout << dbLockFilepath << " already exists. Trying again in "
                      << fallbackValue << " ms" << std::endl;
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
    } while (++retries <= 100);
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

    try {
        if (BUILD_COMMANDS.find(params.at(1)) != BUILD_COMMANDS.end()) {
            std::cout << std::endl;
            build_call(params);
        } else {
            compiler_call(params);
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}
