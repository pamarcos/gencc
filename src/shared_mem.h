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

#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include "cross-platform-cpp/sync/sync_sharedmem.h"
#include "mutex.h"
#include <string>

class SharedMem {
public:
    virtual ~SharedMem() = default;

    virtual bool create(const char* name, size_t size) = 0;
    virtual bool first() = 0;
    virtual size_t getSize() = 0;
    virtual char* rawData() = 0;
    virtual void unlockMutex() = 0;

    std::string getName();

protected:
    std::string m_name;
};

class SharedMemImpl : public SharedMem {
public:
    bool create(const char* name, size_t size) override;
    bool first() override;
    size_t getSize() override;
    char* rawData() override;
    void unlockMutex() override;

private:
    CubicleSoft::Sync::SharedMem m_sharedMem;
    MutexImpl m_mutex;
};

#endif // SHARED_MEM_H
