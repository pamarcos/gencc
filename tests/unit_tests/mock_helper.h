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

#ifndef MOCK_HELPER_H
#define MOCK_HELPER_H

#include "gmock/gmock.h"
#include "helper.h"

class MockHelper : public Helper {
public:
    MOCK_CONST_METHOD2(getEnvVar, bool(const char*, std::string&));
    MOCK_CONST_METHOD2(setEnvVar, void(const char*, const std::string&));
    MOCK_METHOD1(getCwd, bool(std::string&));
    MOCK_CONST_METHOD1(runCommand, int(const std::string& str));
    MOCK_CONST_METHOD1(msleep, void(unsigned));
    MOCK_CONST_METHOD1(fileExists, bool(const std::string& filename));
    MOCK_CONST_METHOD1(removeFile, void(const std::string& filename));
    MOCK_CONST_METHOD1(getFileLock, std::unique_ptr<FileLock>(const std::string& filename));
    MOCK_CONST_METHOD1(getFileIstream, std::unique_ptr<std::istream>(const std::string& filename));
    MOCK_CONST_METHOD1(getFileOstream, std::unique_ptr<std::ostream>(const std::string& filename));
};

#endif // MOCK_HELPER_H
