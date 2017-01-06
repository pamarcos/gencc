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

#ifndef MOCK_SHARED_MEM_H
#define MOCK_SHARED_MEM_H

#include "gmock/gmock.h"
#include "shared_mem.h"

class MockSharedMem : public SharedMem {
public:
    MOCK_METHOD2(create, bool(const char* name, size_t size));
    MOCK_METHOD0(first, bool());
    MOCK_METHOD0(getSize, size_t());
    MOCK_METHOD0(rawData, char*());
    MOCK_METHOD0(unlockMutex, void());
};

#endif // MOCK_SHARED_MEM_H
