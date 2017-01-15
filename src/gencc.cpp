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

#include "gencc.h"
#include "builder.h"
#include "common.h"
#include "compiler.h"
#include "utils.h"

#include <iostream>

Gencc::Gencc(Utils* utils)
    : Common(&m_genccOptions, utils)
{
}

bool Gencc::parseArgs(std::vector<std::string>& params)
{
    std::string envValue;
    if (m_utils->getEnvVar(Constants::CXX, envValue)) {
        m_options->cxx = envValue;
    }
    if (m_utils->getEnvVar(Constants::CC, envValue)) {
        m_options->cc = envValue;
    }

    auto it = params.begin() + 1;

    // In case this is build mode and there are no parameters to process, return inmediately
    if (it->find("-") == std::string::npos) {
        return true;
    }

    for (; it != params.end();) {
        std::string param = *it;

        // If every gencc parameter has already been parsed, break the loop
        if (param.find('-') == std::string::npos) {
            break;
        }

        if (param == Constants::GENCC_COMPILER_PARAM && it + 1 != params.end()) {
            it = params.erase(it);
            m_options->compiler = *it;
            params.erase(it);

            // The compiler is the only parameter that needs to be parsed in compiler mode
            break;
        } else if (param == Constants::PARAM_CXX && it + 1 != params.end()) {
            it = params.erase(it);
            m_options->cxx = *it;
            params.erase(it);
        } else if (param == Constants::PARAM_CC && it + 1 != params.end()) {
            it = params.erase(it);
            m_options->cc = *it;
            params.erase(it);
        } else if (param == Constants::PARAM_OUTPUT && it + 1 != params.end()) {
            it = params.erase(it);
            m_options->dbFilename = *it;
            params.erase(it);
        } else if (param == Constants::PARAM_SHARED_MEMORY && it + 1 != params.end()) {
            it = params.erase(it);
            m_options->sharedMemSize = std::stoi(*it);
            params.erase(it);
        } else if (param == Constants::PARAM_BUILD) {
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
    LOG("\nUsage:\n"
        "\t-cxx    [value] - CXX compiler\n"
        "\t-cc     [value] - CC compiler\n"
        "\t-o      [value] - Output file\n"
        "\t-m      [value] - Size of shared memory to use in bytes\n"
        "\t-build          - Call the actual compiler\n");
}

GenccOptions Gencc::getGenccOptions() const
{
    return m_genccOptions;
}

void Gencc::setWorker(std::unique_ptr<GenccWorker>& worker)
{
    m_worker = std::move(worker);
}

int Gencc::init(std::vector<std::string>& params)
{
    if (m_options == nullptr || m_utils == nullptr) {
        LOG("Options and utils cannot be null\n");
        return -1;
    }

    LOG("%s v%s: ", Constants::NAME, Constants::VERSION);

    if (params.size() < 2) {
        help();
        return -1;
    }

    std::string genccComand = params.at(0);

    // Ensure the GenCC command uses the absolute path
    size_t pos = genccComand.find_first_of('/');
    if (pos != 0) {
        std::string cwd;
        if (!m_utils->getCwd(cwd)) {
            throw std::runtime_error("Couldn't get current working dir");
        }
        genccComand = cwd + "/" + genccComand;
        params[0] = genccComand;
    }

    std::string mode;
    if (!m_utils->getEnvVar(Constants::GENCC_OPTIONS, mode)) {
        m_options->mode = GenccMode::BUILDER;
    } else {
        m_options->mode = GenccMode::COMPILER;
    }

    if (!parseArgs(params)) {
        LOG("Error parsing arguments\n");
        help();
        return -1;
    }

    if (m_worker == nullptr) {
        if (m_options->mode == GenccMode::BUILDER) {
            LOG("\n");
            m_worker = std::unique_ptr<GenccWorker>(new Builder(m_options, m_utils));
        } else if (m_options->mode == GenccMode::COMPILER) {
            m_worker = std::unique_ptr<GenccWorker>(new Compiler(m_options, m_utils));
        }
    }

    m_worker->doWork(params);

    return 0;
}
