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

#include "shared_mem.h"

std::string SharedMem::getName() const
{
    return m_name;
}

bool SharedMemImpl::create(const char* name, size_t size)
{
    if (!m_mutex.create(name)) {
        return false;
    }
    m_mutex.lock();
    m_name = name;
    return m_sharedMem.Create(name, size);
}

bool SharedMemImpl::first()
{
    return m_sharedMem.First();
}

size_t SharedMemImpl::getSize()
{
    return m_sharedMem.GetSize();
}

char* SharedMemImpl::rawData()
{
    return m_sharedMem.RawData();
}

void SharedMemImpl::unlockMutex()
{
    return m_mutex.unlock(true);
}
