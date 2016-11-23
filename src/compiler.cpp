#include "compiler.h"
#include "helper.h"
#include "json.hpp"

#include <cstdlib>
#include <iostream>
#include <random>

#ifndef _WIN32
#include <unistd.h>
#endif

using json = nlohmann::json;

Compiler::Compiler(GenccOptions* options, Helper* helper)
    : Common(options, helper)
{
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

    LOG("%s\n", command.c_str());
    writeCompilationDB(directory, command, file);

    if (m_options->build) {
        if (int ret = m_helper->runCommand(ss.str())) {
            LOG("The command %s exited with error code %d\n", ss.str().c_str(), ret);
        }
    }
}

void Compiler::writeCompilationDB(const std::string& directory, const std::string& command, const std::string& file) const
{
    std::string dbFilepath = m_options->dbFilename;
    std::string dbLockFilepath = dbFilepath + COMPILATION_DB_LOCK_EXT;

    int retries = 1;
    do {
        if (m_helper->fileExists(dbLockFilepath)) {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_int_distribution<int> dist(1, m_options->fallback);
            int fallbackValue = dist(mt);
            LOG("Lock file %s already exists. Trying again in %d ms\n", dbLockFilepath.c_str(), fallbackValue);
            m_helper->msleep(fallbackValue);
            continue;
        }
        FileLockGuard dbLockFile(m_helper->getFileLock(dbLockFilepath));

        json jsonDb = json::array();
        if (m_helper->fileExists(m_options->dbFilename)) {
            std::unique_ptr<std::istream> dbStream = m_helper->getFileIstream(m_options->dbFilename);
            *dbStream >> jsonDb;
        }

        json jsonObj;
        jsonObj["directory"] = directory;
        jsonObj["command"] = command;
        jsonObj["file"] = file;
        jsonDb.push_back(jsonObj);

        std::unique_ptr<std::ostream> dbStream = m_helper->getFileOstream(m_options->dbFilename);
        *dbStream << jsonDb.dump(4);
        dbStream->flush();

        break;
    } while (++retries <= m_options->retries);
}
