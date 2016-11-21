#include "compiler.h"
#include "helper.h"
#include "json.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

#ifndef _WIN32
#include <unistd.h>
#endif

using json = nlohmann::json;

Compiler::Compiler(GenccOptions* options, Helper* helper)
    : Common(options, helper)
{
}

void Compiler::writeCompilationDB(const std::string& directory, const std::string& command, const std::string& file) const noexcept
{
    std::string dbFilepath = m_options->dbFilename;
    std::string dbLockFilepath = dbFilepath + COMPILATION_DB_LOCK_EXT;

    int retries = 0;
    do {
        std::ifstream iLockFile(dbLockFilepath);
        if (iLockFile.good()) {
            int fallbackValue = rand() % m_options->fallback;
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
    writeCompilationDB(directory, command, file);

    if (m_options->build) {
        if (int ret = system(ss.str().c_str())) {
            std::cout << "The command " << ss.str() << " exited with error code "
                      << ret << '\n';
        }
    }
}
