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

#ifndef MOCK_LOCK_FILE_H
#define MOCK_LOCK_FILE_H

#include "gmock/gmock.h"
#include "lock_file.h"

class MockLockFile : public LockFile {
public:
    MockLockFile(const std::string& filename)
        : LockFile(filename)
    {
    }

    MOCK_CONST_METHOD0(createFile, void());
    MOCK_CONST_METHOD0(removeFile, void());
    MOCK_CONST_METHOD1(writeToFile, bool(const std::string& from));
    MOCK_CONST_METHOD1(readFromFile, bool(std::string& to));
};

#endif // MOCK_LOCK_FILE_H
