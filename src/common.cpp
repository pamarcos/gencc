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

#include "common.h"

const char* Constants::VERSION = "0.2.0";
const char* Constants::NAME = "gencc";
const char* Constants::CXX = "CXX";
const char* Constants::CC = "CC";
const char* Constants::COMPILE_DB = "compile_commands.json";
const char* Constants::GENCC_OPTIONS = "GENCC_OPTIONS";
const char* Constants::C_EXT = ".c";
const char* Constants::GENCC_COMPILER_PARAM = "-gencc-compiler";
const char* Constants::DIRECTORY = "directory";
const char* Constants::COMMAND = "command";
const char* Constants::FILE = "file";
const char* Constants::BUILD = "build";
const char* Constants::DB_FILENAME = "dbFilename";
const unsigned Constants::MAX_CDB_RETRIES = 100;
const unsigned Constants::MAX_CDB_FALLBACK_SLEEP_IN_MS = 50;
const char* Constants::PARAM_CXX = "-cxx";
const char* Constants::PARAM_CC = "-cc";
const char* Constants::PARAM_OUTPUT = "-o";
const char* Constants::PARAM_RETRIES = "-r";
const char* Constants::PARAM_FALLBACK = "-f";
const char* Constants::PARAM_BUILD = "-build";
const char* Constants::SHARED_MEM_NAME = Constants::NAME;
const unsigned Constants::SHARED_MEM_SIZE = 8192;
