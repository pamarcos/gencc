/**
 * gencc is an application that generates compilation databases for clang
 *
 * Copyright (C) 2016 Pablo Marcos Oltra
 *
 * This file is part of gencc.
 *
 * gencc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gencc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    std::string cwd;

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

    m_directory = cwd;
    for (size_t i = 1; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }

        if (params.at(i).find(C_EXT) != std::string::npos) {
            m_file = m_directory + "/" + params.at(i);
        }
    }
    m_command = ss.str();

    LOG("%s\n", m_command.c_str());
    writeCompilationDB();

    if (m_options->build) {
        if (int ret = m_helper->runCommand(ss.str())) {
            LOG("The command %s exited with error code %d\n", ss.str().c_str(), ret);
        }
    }
}

void Compiler::writeCompilationDB() const
{
    std::string dbFilepath = m_options->dbFilename;
    std::string dbLockFilepath = dbFilepath + COMPILATION_DB_LOCK_EXT;

    unsigned retries = 1;
    do {
        if (m_helper->fileExists(dbLockFilepath)) {
            fallback(retries);
            continue;
        }
        FileLockGuard dbLockFile(m_helper->getFileLock(dbLockFilepath));

        if (!dbLockFile.getLockFile()->writeToFile(m_command)) {
            fallback(retries);
            continue;
        }

        json jsonDb = json::array();
        if (m_helper->fileExists(m_options->dbFilename)) {
            std::unique_ptr<std::istream> dbStream = m_helper->getFileIstream(m_options->dbFilename);
            *dbStream >> jsonDb;
        }

        json jsonObj;
        jsonObj["directory"] = m_directory;
        jsonObj["command"] = m_command;
        jsonObj["file"] = m_file;
        jsonDb.push_back(jsonObj);

        std::string checkStr;

        if (!dbLockFile.getLockFile()->readFromFile(checkStr)) {
            fallback(retries);
            continue;
        }

        // Check to ensure that there was no race condition when creating dbLockFile
        // In case the string read now is not the same as the one written after creating
        // that file, we know a different process created it at the same time
        if (checkStr == m_command) {
            std::unique_ptr<std::ostream> dbStream = m_helper->getFileOstream(m_options->dbFilename);
            *dbStream << jsonDb.dump(4);
            dbStream->flush();
            break;
        } else {
            // Do not remove the dbLockFile because the other process is the one that
            // created it. However, there might be another race condition when checking
            // the content, so we need to check again to ensure the file is removed
            dbLockFile.removeFile(false);
            fallback(retries);
            std::string newCheck;
            dbLockFile.getLockFile()->readFromFile(newCheck);
            if (newCheck == checkStr) {
                dbLockFile.removeFile(true);
            }
            continue;
        }
    } while (++retries <= m_options->retries);
}

void Compiler::fallback(unsigned retries) const
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<unsigned> dist(1, m_options->fallback);
    unsigned fallbackValue = dist(mt);
    LOG("%s - Lock file already exists. Trying again in %d ms. Attempt #%d\n",
        m_command.c_str(), fallbackValue, retries);
    m_helper->msleep(fallbackValue);
}
