#include "gencc.h"
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

using json = nlohmann::json;

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

bool get_env_var(const char* name, std::string& str)
{
    const char* ptr = getenv(name);
    if (ptr == nullptr) {
        str.clear();
        return false;
    }
    str = ptr;
    return true;
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

bool get_cwd(std::string& str)
{
    char buffer[256];
    if (getcwd(reinterpret_cast<char*>(buffer), sizeof(buffer)) == nullptr) {
        str.clear();
        return false;
    }
    str = reinterpret_cast<char*>(buffer);
    return true;
}

/* Helper function to simply set the PATH correctly so that this binary
 * is called instead of the default CXX one */
void build_call(gencc_options_t& options, const std::vector<std::string>& params)
{
    std::string tmp, cwd;
    std::stringstream ss;

    if (options.cxx.empty() && get_env_var(CXX, tmp)) {
        options.cxx = tmp;
    }
    if (options.cc.empty() && get_env_var(CC, tmp)) {
        options.cc = tmp;
    }

    if (!get_cwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    std::cout << "Original CXX = " << options.cxx << '\n';
    std::cout << "Original CC = " << options.cc << '\n';
    std::cout << "CWD = " << cwd << '\n';

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

void write_to_db(const gencc_options_t& options, const std::string& directory, const std::string& command, const std::string& file)
{
    std::string dbFilepath = options.dbFilename;
    std::string dbLockFilepath = dbFilepath + DB_LOCK_FILENAME_EXT;

    int retries = 0;
    do {
        std::ifstream iLockFile(dbLockFilepath);
        if (iLockFile.good()) {
            int fallbackValue = arc4random() % options.fallback;
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

void compiler_call(gencc_options_t& options, const std::vector<std::string>& params)
{
    std::stringstream ss;
    std::string cwd, directory, command, file;

    if (!get_cwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    // Deserialize options embedded in the env variable
    std::string genccOptions;
    json jsonObj;
    if (!get_env_var(GENCC_OPTIONS, genccOptions)) {
        throw std::runtime_error(std::string("Couldn't read env var ") + GENCC_OPTIONS);
    }
    ss << genccOptions;
    jsonObj << ss;

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
    write_to_db(options, directory, command, file);

    if (options.build) {
        if (int ret = system(ss.str().c_str())) {
            std::cout << "The command " << ss.str() << " exited with error code "
                      << ret << '\n';
        }
    }
}

bool parse_args(gencc_options_t& options, std::vector<std::string>& params)
{
    auto it = params.begin();
    for (; it != params.end(); ++it) {
        if (it == params.begin() + 1 && it->find("-") != std::string::npos) {
            break;
        }
    }

    for (; it != params.end();) {
        std::string param = *it;
        if (param.find('-') == std::string::npos) {
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
            return false;
        }
    }

    return true;
}

int gencc(int argc, char* argv[])
{
    std::cout << NAME << " v" << VERSION << ": ";

    if (argc < 2) {
        help();
        return 0;
    }

    std::vector<std::string> params;
    gencc_options_t options;
    std::string genccComand = argv[0];

    // Ensure the GenCC command uses the absolute path
    if (genccComand.find_first_of('/') != 0) {
        std::string cwd;
        if (!get_cwd(cwd)) {
            throw std::runtime_error("Couldn't get current working dir");
        }
        genccComand = cwd + "/" + genccComand;
    }
    params.emplace_back(genccComand);

    for (int i = 1; i < argc; ++i) {
        params.emplace_back(argv[i]);
    }

    std::string mode;
    if (!get_env_var(GENCC_OPTIONS, mode)) {
        options.mode = gencc_mode::BUILDER;
    } else {
        options.mode = gencc_mode::COMPILER;
    }

    if (!parse_args(options, params)) {
        std::cout << "Error parsing arguments\n";
        help();
        return -1;
    }

    try {
        if (options.mode == gencc_mode::BUILDER) {
            std::cout << '\n';
            build_call(options, params);
        } else {
            compiler_call(options, params);
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << '\n';
    }

    return 0;
}
