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

#include "compiler.h"
#include "utils.h"
#include "json/json.hpp"

#include <cstdlib>
#include <iostream>
#include <random>

#ifndef _WIN32
#include <unistd.h>
#endif

using json = nlohmann::json;

Compiler::Compiler(GenccOptions* options, Utils* utils)
    : Common(options, utils)
{
}

void Compiler::doWork(const std::vector<std::string>& params)
{
    std::stringstream ss;
    std::string cwd;

    if (!m_utils->getCwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    // Deserialize options embedded in the env variable
    std::string genccOptions;
    json jsonObj;
    if (!m_utils->getEnvVar(Constants::GENCC_OPTIONS, genccOptions)) {
        throw std::runtime_error(std::string("Couldn't read env var ") + Constants::GENCC_OPTIONS);
    }
    ss << genccOptions;

    try {
        jsonObj << ss;
    } catch (const std::exception& ex) {
        throw std::runtime_error(std::string("Error parsing ") + Constants::GENCC_OPTIONS + ": " + ex.what());
    }

    if (jsonObj.find(Constants::BUILD) == jsonObj.end()) {
        throw std::runtime_error(std::string("Error parsing ") + Constants::GENCC_OPTIONS + ": "
            + Constants::BUILD + " missing");
    }
    m_options->build = jsonObj[Constants::BUILD];

    ss.str("");
    ss.clear();

    ss << m_options->compiler;

    m_directory = cwd;
    for (size_t i = 1; i < params.size(); ++i) {
        std::string param = params.at(i);
        if (!param.empty()) {
            if (!ss.str().empty()) {
                ss << " ";
            }
            ss << param;
        }

        if (param.find(Constants::C_EXT) != std::string::npos) {
            m_file = m_directory + "/" + param;
        }
    }
    m_command = ss.str();

    LOG("%s\n", m_command.c_str());
    writeCompilationDb();

    if (m_options->build) {
        if (int ret = m_utils->runCommand(ss.str())) {
            LOG("The command %s exited with error code %d\n", ss.str().c_str(), ret);
        }
    }
}

void Compiler::writeCompilationDb() const
{
    std::unique_ptr<SharedMem> sharedMem = m_utils->createSharedMem(Constants::SHARED_MEM_NAME, m_options->sharedMemSize);

    json jsonDb = json::array();
    std::stringstream ss;

    if (sharedMem->first()) {
        throw std::runtime_error("Shared memory needs to be created first by builder process");
    }

    ss << sharedMem->rawData();
    if (!ss.str().empty()) {
        jsonDb << ss;
    }

    json jsonObj;
    jsonObj[Constants::DIRECTORY] = m_directory;
    jsonObj[Constants::COMMAND] = m_command;
    jsonObj[Constants::FILE] = m_file;
    jsonDb.push_back(jsonObj);

    ss.str("");
    ss.clear();
    ss << jsonDb.dump();

    if (ss.str().size() > sharedMem->getSize()) {
        throw std::runtime_error(std::string("JSON with size ") + std::to_string(ss.str().size())
            + " needs a bigger shared memory. Current size: " + std::to_string(sharedMem->getSize()));
    }

    memcpy(reinterpret_cast<void*>(sharedMem->rawData()), reinterpret_cast<const void*>(ss.str().c_str()), ss.str().size());
}
