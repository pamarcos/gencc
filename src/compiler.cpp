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

    if (jsonObj.find(Constants::BUILD) == jsonObj.end() || jsonObj.find(Constants::SHARED_MEMORY_SIZE) == jsonObj.end()) {
        throw std::runtime_error(std::string("Error parsing ") + Constants::GENCC_OPTIONS + ": "
            + Constants::BUILD + " or " + Constants::SHARED_MEMORY_SIZE + " missing");
    }
    m_options->build = jsonObj[Constants::BUILD];
    m_options->sharedMemSize = jsonObj[Constants::SHARED_MEMORY_SIZE];

    m_directory = cwd;
    ss.str("");
    ss.clear();
    ss << m_options->compiler;
    for (size_t i = 1; i < params.size(); ++i) {
        ss << " " << params.at(i);
    }
    LOG("%s\n", ss.str().c_str());
    parseParameters(params);

    if (!m_files.empty()) {
        writeCompilationDb();
    }

    if (m_options->build) {
        if (int ret = m_utils->runCommand(ss.str())) {
            LOG("The command %s exited with error code %d\n", ss.str().c_str(), ret);
        }
    }
}

void Compiler::parseParameters(const std::vector<std::string>& params)
{
    bool ignoreCompilerCall = false;
    bool compiling = false;
    bool firstSource = true;

    m_command.emplace_back(m_options->compiler);

    for (size_t i = 1; i < params.size(); ++i) {
        std::string param = params.at(i);

        // Ignore compiler calls used to generate dependencies that do not actually compile
        if (Constants::COMPILER_GEN_DEP_OPTIONS.find(param) != Constants::COMPILER_GEN_DEP_OPTIONS.end()) {
            ignoreCompilerCall = true;
        }
        if (param == Constants::PARAM_OUTPUT || param == Constants::PARAM_DONT_LINK) {
            compiling = true;
        }

        size_t pos;
        if (((param.size() >= strlen(Constants::C_EXT))
                && ((pos = param.find(Constants::C_EXT)) == param.size() - strlen(Constants::C_EXT)))
            || (param.size() >= strlen(Constants::CPP_EXT)
                   && (pos = param.find(Constants::CPP_EXT)) == param.size() - strlen(Constants::CPP_EXT))) {
            m_files.emplace_back(param);

            if (firstSource) {
                m_firstSrcPos = m_command.size();
                m_command.emplace_back(param);
                firstSource = false;
            }
        } else if (!param.empty()) {
            m_command.emplace_back(param);
        }
    }

    if (ignoreCompilerCall && !compiling) {
        m_files.clear();
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

    char* data = sharedMem->rawData();
    ss << data;
    if (!ss.str().empty()) {
        try {
            jsonDb << ss;
        } catch (const std::exception& ex) {
            throw std::runtime_error(std::string("Error parsing shared memory JSON: ") + ex.what());
        }
    }

    json jsonObj;
    std::string command;
    for (auto it = m_files.begin(); it != m_files.end(); ++it) {
        jsonObj.clear();
        jsonObj[Constants::DIRECTORY] = m_directory;
        command.clear();
        for (size_t i = 0; i < m_command.size(); ++i) {
            if (i == m_firstSrcPos) {
                command += *it;
            } else {
                command += m_command.at(i);
            }
            if (i != m_command.size() - 1) {
                command += " ";
            }
        }
        jsonObj[Constants::COMMAND] = command;
        jsonObj[Constants::FILE] = m_directory + "/" + *it;
        jsonDb.push_back(jsonObj);
    }

    ss.str("");
    ss.clear();
    ss << jsonDb.dump();

    if (ss.str().size() > sharedMem->getSize()) {
        throw std::runtime_error(std::string("JSON with size ") + std::to_string(ss.str().size())
            + " needs a bigger shared memory. Current size: " + std::to_string(sharedMem->getSize()));
    }

    memcpy(reinterpret_cast<void*>(data), reinterpret_cast<const void*>(ss.str().c_str()), ss.str().size());
}
