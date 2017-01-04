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

#ifndef MOCK_UTILS_H
#define MOCK_UTILS_H

#include "gmock/gmock.h"
#include "utils.h"

class MockUtils : public Utils {
public:
    virtual std::unique_ptr<LockFile> getLockFile(const std::string& filename) const
    {
        return std::unique_ptr<LockFile>(getLockFileProxy(filename));
    }

    MOCK_CONST_METHOD2(getEnvVar, bool(const char*, std::string&));
    MOCK_CONST_METHOD2(setEnvVar, void(const char*, const std::string&));
    MOCK_METHOD1(getCwd, bool(std::string&));
    MOCK_CONST_METHOD1(runCommand, int(const std::string& str));
    MOCK_CONST_METHOD1(msleep, void(unsigned));
    MOCK_CONST_METHOD1(fileExists, bool(const std::string& filename));
    MOCK_CONST_METHOD1(removeFile, void(const std::string& filename));
    MOCK_CONST_METHOD1(getLockFileProxy, LockFile*(const std::string& filename));
    MOCK_CONST_METHOD1(getFileIstream, std::unique_ptr<std::istream>(const std::string& filename));
    MOCK_CONST_METHOD1(getFileOstream, std::unique_ptr<std::ostream>(const std::string& filename));
};

#endif // MOCK_UTILS_H
