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

#include "mutex.h"

std::string Mutex::getName() const
{
    return m_name;
}

MutexImpl::MutexImpl()
    : m_locked(false)
{
}

bool MutexImpl::create(const char* name)
{
    m_name = name;
    return m_mutex.Create(name);
}

void MutexImpl::lock(unsigned wait)
{
    if (!m_locked) {
        // Spinlock
        while (!m_mutex.Lock(wait)) {
        }
        m_locked = true;
    }
}

void MutexImpl::unlock(bool all)
{
    if (m_locked) {
        // Spin-unlock
        while (!m_mutex.Unlock(all)) {
        }
        m_locked = false;
    }
}
