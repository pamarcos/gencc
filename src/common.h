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

#include <set>
#include <string>
#include <vector>

#include "logger.h"

namespace Constants {
static constexpr char const* VERSION = "0.2.1";
static constexpr char const* NAME = "gencc";
static constexpr char const* CXX = "CXX";
static constexpr char const* CC = "CC";
static constexpr char const* COMPILE_DB = "compile_commands.json";
static constexpr char const* GENCC_OPTIONS = "GENCC_OPTIONS";
static constexpr char const* C_EXT = ".c";
static constexpr char const* CPP_EXT = ".cpp";
static constexpr char const* GENCC_COMPILER_PARAM = "-gencc-compiler";
static constexpr char const* DIRECTORY = "directory";
static constexpr char const* COMMAND = "command";
static constexpr char const* FILE = "file";
static constexpr char const* BUILD = "build";
static constexpr char const* DB_FILENAME = "dbFilename";
static constexpr char const* PARAM_CXX = "-cxx";
static constexpr char const* PARAM_CC = "-cc";
static constexpr char const* PARAM_OUTPUT = "-o";
static constexpr char const* PARAM_DONT_LINK = "-c";
static constexpr char const* PARAM_BUILD = "-build";
static constexpr char const* SHARED_MEM_NAME = NAME;
static constexpr unsigned SHARED_MEM_SIZE = 8192;
static constexpr char const* PARAM_SHARED_MEMORY = "-m";
static constexpr char const* SHARED_MEMORY_SIZE = "shared_memory_size";

static const std::set<std::string> COMPILER_GEN_DEP_OPTIONS = {
    "-M",
    "-MM",
    "-MF",
    "-MQ",
    "-MP",
    "-MG",
    "-MT",
    "-MD",
    "-MMD"
};
}

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
