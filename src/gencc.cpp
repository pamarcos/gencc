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

#include "gencc.h"
#include "builder.h"
#include "common.h"
#include "compiler.h"
#include "helper.h"

#include "json.hpp"
#include <exception>
#include <iostream>

Gencc::Gencc(Helper* helper)
    : Common(&m_genccOptions, helper)
{
}

bool Gencc::parseArgs(std::vector<std::string>& params)
{
    std::string envValue;
    if (m_helper->getEnvVar(Constants::CXX, envValue)) {
        m_options->cxx = envValue;
    }
    if (m_helper->getEnvVar(Constants::CC, envValue)) {
        m_options->cc = envValue;
    }

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
            m_options->fallback = std::stoul(*it);
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
    LOG("\nUsage:\n"
        "\t-cxx    [value] - CXX compiler\n"
        "\t-cc     [value] - CC compiler\n"
        "\t-o      [value] - DB file\n"
        "\t-r      [value] - Number of retries if DB locked\n"
        "\t-f      [value] - Max fallback time in ms in case of DB locked\n"
        "\t-build          - Call the actual compiler\n");
}

void Gencc::setWorker(std::unique_ptr<GenccWorker>& worker)
{
    m_worker = std::move(worker);
}

int Gencc::init(std::vector<std::string>& params)
{
    if (m_options == nullptr || m_helper == nullptr) {
        LOG("Options and helper cannot be null\n");
        return -1;
    }

    LOG("%s v%s: ", Constants::NAME, Constants::VERSION);

    if (params.size() < 2) {
        help();
        return -1;
    }

    std::string genccComand = params.at(0);

    // Ensure the GenCC command uses the absolute path
    if (genccComand.find_first_of('/') != 0) {
        std::string cwd;
        if (!m_helper->getCwd(cwd)) {
            throw std::runtime_error("Couldn't get current working dir");
        }
        genccComand = cwd + "/" + genccComand;
        params[0] = genccComand;
    }

    std::string mode;
    if (!m_helper->getEnvVar(Constants::GENCC_OPTIONS, mode)) {
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
            m_worker = std::unique_ptr<GenccWorker>(new Builder(m_options, m_helper));
        } else if (m_options->mode == GenccMode::COMPILER) {
            m_worker = std::unique_ptr<GenccWorker>(new Compiler(m_options, m_helper));
        }
    }

    m_worker->doWork(params);

    return 0;
}
