/**
 * gencc is an application that generates compilation databases for clang
 *
 * Copyright (C) 2017 Pablo Marcos Oltra
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
 * along with gencc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "builder.h"
#include "utils.h"

#include "json/json.hpp"
#include <iostream>

using json = nlohmann::json;

Builder::Builder(GenccOptions* options, Utils* utils)
    : Common(options, utils)
{
}

void Builder::doWork(const std::vector<std::string>& params)
{
    std::string cwd;
    std::stringstream ss;

    if (!m_utils->getCwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    LOG("\tOriginal CXX = %s\n", m_options->cxx.c_str());
    LOG("\tOriginal CC = %s\n", m_options->cc.c_str());
    LOG("\tCWD = %s\n", cwd.c_str());

    if (m_options->cxx.empty() && m_options->cc.empty()) {
        throw std::runtime_error("No CXX nor CC set either in the environment or through a parameter");
    }

    m_options->dbFilename = cwd + "/" + m_options->dbFilename;
    m_utils->removeFile(m_options->dbFilename);

    // Serialize the options through an environment variable
    json jsonObj;
    jsonObj[Constants::BUILD] = m_options->build;
    jsonObj[Constants::SHARED_MEMORY_SIZE] = m_options->sharedMemSize;
    ss.str("");
    ss.clear();
    ss << jsonObj;
    m_utils->setEnvVar(Constants::GENCC_OPTIONS, ss.str());
    m_utils->setEnvVar(Constants::CXX, params.at(0) + " " + Constants::GENCC_COMPILER_PARAM + " " + m_options->cxx);
    m_utils->setEnvVar(Constants::CC, params.at(0) + " " + Constants::GENCC_COMPILER_PARAM + " " + m_options->cc);

    ss.str("");
    ss.clear();
    for (size_t i = 1; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }
    }

    std::unique_ptr<SharedMem> sharedMem = m_utils->createSharedMem(Constants::SHARED_MEM_NAME, m_options->sharedMemSize);
    char* sharedMemData = sharedMem->rawData();
    memset(sharedMemData, 0, m_options->sharedMemSize);
    sharedMem->unlockMutex();

    LOG("\tCreated shared memory \"%s\" with size %u at %p\n",
        sharedMem->getName().c_str(), sharedMem->getSize(), sharedMemData);

    LOG("\tRunning build command: %s\n", ss.str().c_str());
    if (int ret = m_utils->runCommand(ss.str())) {
        LOG("The command %s exited with error code %d\n", ss.str().c_str(), ret);
    }

    ss.str("");
    ss.clear();
    ss << sharedMem->rawData();
    jsonObj.clear();

    if (!ss.str().empty()) {
        try {
            ss >> jsonObj;
        } catch (const std::exception& ex) {
            throw std::runtime_error(std::string("Error parsing shared memory: ") + ex.what());
        }

        std::unique_ptr<std::ostream> dbStream = m_utils->getFileOstream(m_options->dbFilename);
        *dbStream << jsonObj.dump(4);
    }
}
