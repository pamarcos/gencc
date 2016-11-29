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
    static const char* COMPILATION_DB;
    static const char* COMPILATION_DB_LOCK_EXT;
    static const char* GENCC_OPTIONS;
    static const char* C_EXT;

    static const int MAX_CDB_RETRIES;
    static const int MAX_CDB_FALLBACK_SLEEP_IN_MS;
};

class Helper;

enum class GenccMode {
    NONE,
    BUILDER,
    COMPILER
};

using GenccOptions = struct GenccOptions_s {
    GenccMode mode = GenccMode::NONE;
    bool build = false;
    std::string dbFilename = Constants::COMPILATION_DB;
    std::string cxx;
    std::string cc;
    std::string compiler;
    unsigned retries = Constants::MAX_CDB_RETRIES;
    unsigned fallback = Constants::MAX_CDB_FALLBACK_SLEEP_IN_MS;
};

class GenccWorker {
public:
    virtual ~GenccWorker() = default;
    virtual void doWork(const std::vector<std::string>& params) = 0;
};

class Common {
public:
    Common(GenccOptions* options, Helper* helper)
        : m_options(options)
        , m_helper(helper)
    {
    }

    virtual ~Common() = default;

    Helper* getHelper() const noexcept { return m_helper; }
    void setHelper(Helper* value) noexcept { m_helper = value; }

    GenccOptions* getOptions() const noexcept { return m_options; }
    void setOptions(GenccOptions* value) noexcept { m_options = value; }

protected:
    GenccOptions* m_options;
    Helper* m_helper;
};

#endif // COMMON_H
