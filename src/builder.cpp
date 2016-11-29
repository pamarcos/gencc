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

#include "builder.h"
#include "helper.h"

#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

Builder::Builder(GenccOptions* options, Helper* helper)
    : Common(options, helper)
{
}

void Builder::doWork(const std::vector<std::string>& params)
{
    std::string cwd;
    std::stringstream ss;

    if (!m_helper->getCwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    LOG("Original CXX = %s\n", m_options->cxx.c_str());
    LOG("Original CC = %s\n", m_options->cc.c_str());
    LOG("CWD = %s\n", cwd.c_str());

    m_options->dbFilename = cwd + "/" + m_options->dbFilename;
    m_helper->removeFile(m_options->dbFilename);
    m_helper->removeFile(m_options->dbFilename + Constants::COMPILATION_DB_LOCK_EXT);

    // Serialize the options through an environment variable
    json jsonObj;
    jsonObj[Constants::BUILD] = m_options->build;
    jsonObj[Constants::DB_FILENAME] = m_options->dbFilename;
    ss.str("");
    ss.clear();
    ss << jsonObj;
    m_helper->setEnvVar(Constants::GENCC_OPTIONS, ss.str());
    m_helper->setEnvVar(Constants::CXX, params.at(0) + " " + Constants::GENCC_COMPILER_PARAM + " " + m_options->cxx);
    m_helper->setEnvVar(Constants::CC, params.at(0) + " " + Constants::GENCC_COMPILER_PARAM + " " + m_options->cc);

    ss.str("");
    ss.clear();
    for (size_t i = 1; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }
    }

    if (int ret = m_helper->runCommand(ss.str())) {
        LOG("The command %s exited with error code %d\n", ss.str().c_str(), ret);
    }
}
