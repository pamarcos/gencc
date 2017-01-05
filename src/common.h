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

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

#include "logger.h"

struct Constants {
    static const char* VERSION;
    static const char* NAME;
    static const char* CXX;
    static const char* CC;
    static const char* COMPILE_DB;
    static const char* GENCC_OPTIONS;
    static const char* C_EXT;
    static const char* GENCC_COMPILER_PARAM;
    static const char* DIRECTORY;
    static const char* COMMAND;
    static const char* FILE;
    static const char* BUILD;
    static const char* DB_FILENAME;
    static const unsigned MAX_CDB_RETRIES;
    static const unsigned MAX_CDB_FALLBACK_SLEEP_IN_MS;
    static const char* PARAM_CXX;
    static const char* PARAM_CC;
    static const char* PARAM_OUTPUT;
    static const char* PARAM_RETRIES;
    static const char* PARAM_FALLBACK;
    static const char* PARAM_BUILD;
    static const char* SHARED_MEM_NAME;
    static const unsigned SHARED_MEM_SIZE;
};

class Utils;

enum class GenccMode {
    NONE,
    BUILDER,
    COMPILER
};

using GenccOptions = struct GenccOptions_s {
    GenccMode mode = GenccMode::NONE;
    bool build = false;
    std::string dbFilename = Constants::COMPILE_DB;
    std::string cxx;
    std::string cc;
    std::string compiler;
    unsigned retries = Constants::MAX_CDB_RETRIES;
    unsigned fallback = Constants::MAX_CDB_FALLBACK_SLEEP_IN_MS;
    std::string sharedMemName = Constants::NAME;
    unsigned sharedMemSize = Constants::SHARED_MEM_SIZE;
};

class GenccWorker {
public:
    virtual ~GenccWorker() = default;
    virtual void doWork(const std::vector<std::string>& params) = 0;
};

class Common {
public:
    Common(GenccOptions* options, Utils* utils)
        : m_options(options)
        , m_utils(utils)
    {
    }

    virtual ~Common() = default;

    Utils* getUtils() const noexcept { return m_utils; }
    void setUtils(Utils* value) noexcept { m_utils = value; }

    GenccOptions* getOptions() const noexcept { return m_options; }
    void setOptions(GenccOptions* value) noexcept { m_options = value; }

protected:
    GenccOptions* m_options;
    Utils* m_utils;
};

#endif // COMMON_H
