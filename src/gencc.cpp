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

bool Helper::getEnvVar(const char* name, std::string& str) const noexcept
{
    const char* ptr = getenv(name);
    if (ptr == nullptr) {
        str.clear();
        return false;
    }
    str = ptr;
    return true;
}

void Helper::setEnvVar(const char* name, const std::string& value) const
{
    std::string var;
    setenv(name, value.c_str(), 1);
    getEnvVar(name, var);
    if (var != value) {
        throw std::runtime_error(std::string("Couldn't set new var ") + name + " to " + value);
    }
}

bool Helper::getCwd(std::string& str) noexcept
{
    if (getcwd(reinterpret_cast<char*>(m_buffer), sizeof(m_buffer)) == nullptr) {
        str.clear();
        return false;
    }
    str = reinterpret_cast<char*>(m_buffer);
    return true;
}

Common::Common(GenccOptions* options, Helper* helper)
    : m_options(options)
    , m_helper(helper)
{
}

Builder::Builder(GenccOptions* options, Helper* helper)
    : Common(options, helper)
{
}

/* Helper function to simply set the PATH correctly so that this binary
 * is called instead of the default CXX one */
void Builder::doWork(const std::vector<std::string>& params)
{
    std::string tmp, cwd;
    std::stringstream ss;

    if (m_options->cxx.empty() && m_helper->getEnvVar(CXX, tmp)) {
        m_options->cxx = tmp;
    }
    if (m_options->cc.empty() && m_helper->getEnvVar(CC, tmp)) {
        m_options->cc = tmp;
    }

    if (!m_helper->getCwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    std::cout << "Original CXX = " << m_options->cxx << '\n';
    std::cout << "Original CC = " << m_options->cc << '\n';
    std::cout << "CWD = " << cwd << '\n';

    m_options->dbFilename = cwd + "/" + m_options->dbFilename;
    std::remove(m_options->dbFilename.c_str());
    std::remove((m_options->dbFilename + DB_LOCK_FILENAME_EXT).c_str());

    // Serialize the options through an environment variable
    json jsonObj;
    jsonObj["build"] = m_options->build;
    jsonObj["dbFilename"] = m_options->dbFilename;
    ss.str("");
    ss.clear();
    ss << jsonObj;
    m_helper->setEnvVar(GENCC_OPTIONS, ss.str());
    m_helper->setEnvVar(CXX, params.at(0) + " -gencc-compiler " + m_options->cxx);
    m_helper->setEnvVar(CC, params.at(0) + " -gencc-compiler " + m_options->cc);

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

Compiler::Compiler(GenccOptions* options, Helper* helper)
    : Common(options, helper)
{
}

void Compiler::writeToDb(const std::string& directory, const std::string& command, const std::string& file)
{
    std::string dbFilepath = m_options->dbFilename;
    std::string dbLockFilepath = dbFilepath + DB_LOCK_FILENAME_EXT;

    int retries = 0;
    do {
        std::ifstream iLockFile(dbLockFilepath);
        if (iLockFile.good()) {
            int fallbackValue = arc4random() % m_options->fallback;
            std::cout << dbLockFilepath << " already exists. Trying again in "
                      << fallbackValue << " ms"
                      << '\n';
            usleep(fallbackValue * 1000u);
            continue;
        }
        iLockFile.close();

        std::ofstream oLockFile(dbLockFilepath);
        oLockFile.close();

        std::ifstream idb(m_options->dbFilename);

        json jsonDb = json::array();
        if (idb.good()) {
            idb >> jsonDb;
        }

        json jsonObj;
        jsonObj["directory"] = directory;
        jsonObj["command"] = command;
        jsonObj["file"] = file;
        jsonDb.push_back(jsonObj);

        std::ofstream odb(m_options->dbFilename);
        odb << jsonDb.dump(4);

        std::remove(dbLockFilepath.c_str());
        return;
    } while (++retries <= m_options->retries);
}

void Compiler::doWork(const std::vector<std::string>& params)
{
    std::stringstream ss;
    std::string cwd, directory, command, file;

    if (!m_helper->getCwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    // Deserialize options embedded in the env variable
    std::string genccOptions;
    json jsonObj;
    if (!m_helper->getEnvVar(GENCC_OPTIONS, genccOptions)) {
        throw std::runtime_error(std::string("Couldn't read env var ") + GENCC_OPTIONS);
    }
    ss << genccOptions;
    jsonObj << ss;

    m_options->build = jsonObj["build"];
    m_options->dbFilename = jsonObj["dbFilename"];

    ss.str("");
    ss.clear();
    ss << m_options->compiler << " ";

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
    writeToDb(directory, command, file);

    if (m_options->build) {
        if (int ret = system(ss.str().c_str())) {
            std::cout << "The command " << ss.str() << " exited with error code "
                      << ret << '\n';
        }
    }
}

Gencc::Gencc(GenccOptions* options, Helper* helper)
    : Common(options, helper)
{
}

bool Gencc::parseArgs(std::vector<std::string>& params)
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
            m_options->compiler = *it;
            params.erase(it);

            // The compiler is the only parameter that needs to be parsed in compiler mode
            break;
        } else if (param == "-cxx" && it + 1 != params.end()) {
            params.erase(it);
            m_options->cxx = *it;
            params.erase(it);
        } else if (param == "-cc" && it + 1 != params.end()) {
            params.erase(it);
            m_options->cc = *it;
            params.erase(it);
        } else if (param == "-o" && it + 1 != params.end()) {
            params.erase(it);
            m_options->dbFilename = *it;
            params.erase(it);
        } else if (param == "-r" && it + 1 != params.end()) {
            params.erase(it);
            m_options->retries = std::stoi(*it);
            params.erase(it);
        } else if (param == "-f" && it + 1 != params.end()) {
            params.erase(it);
            m_options->fallback = std::stoi(*it);
            params.erase(it);
        } else if (param == "-build") {
            params.erase(it);
            m_options->build = true;
        } else {
            return false;
        }
    }

    return true;
}

void Gencc::help()
{
    std::cout << "Help:\n"
                 "\t-cxx    [value] - CXX compiler\n"
                 "\t-cc     [value] - CC compiler\n"
                 "\t-o      [value] - DB file\n"
                 "\t-r      [value] - Number of retries if DB locked\n"
                 "\t-f      [value] - Max fallback time in ms in case of DB locked\n"
                 "\t-build          - Call the actual compiler\n";
}

int Gencc::init(int argc, char* argv[])
{
    if (m_options == nullptr || m_helper == nullptr) {
        std::cout << "Options and helper cannot be null\n";
        return -1;
    }

    std::cout << NAME << " v" << VERSION << ": ";

    if (argc < 2) {
        help();
        return 0;
    }

    std::vector<std::string> params;
    std::string genccComand = argv[0];

    // Ensure the GenCC command uses the absolute path
    if (genccComand.find_first_of('/') != 0) {
        std::string cwd;
        if (!m_helper->getCwd(cwd)) {
            throw std::runtime_error("Couldn't get current working dir");
        }
        genccComand = cwd + "/" + genccComand;
    }
    params.emplace_back(genccComand);

    for (int i = 1; i < argc; ++i) {
        params.emplace_back(argv[i]);
    }

    std::string mode;
    if (!m_helper->getEnvVar(GENCC_OPTIONS, mode)) {
        m_options->mode = gencc_mode::BUILDER;
    } else {
        m_options->mode = gencc_mode::COMPILER;
    }

    if (!parseArgs(params)) {
        std::cout << "Error parsing arguments\n";
        help();
        return -1;
    }

    try {
        if (m_options->mode == gencc_mode::BUILDER) {
            std::cout << '\n';
            m_worker = std::unique_ptr<GenccWorker>(new Builder(m_options, m_helper));
        } else {
            m_worker = std::unique_ptr<GenccWorker>(new Compiler(m_options, m_helper));
        }

        m_worker->doWork(params);
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << '\n';
        return -1;
    }

    return 0;
}
