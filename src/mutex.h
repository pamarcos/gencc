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

#ifndef MUTEX_H
#define MUTEX_H

#include "cross-platform-cpp/sync/sync_mutex.h"
#include <memory>
#include <string>

class Mutex {
public:
    virtual ~Mutex() = default;

    virtual bool create(const char* name = nullptr) = 0;
    virtual void lock(unsigned wait = INFINITE) = 0;
    virtual void unlock(bool all = false) = 0;

    std::string getName() const;

protected:
    std::string m_name;
};

class MutexImpl : public Mutex {
public:
    MutexImpl();

    bool create(const char* name = nullptr) override;
    void lock(unsigned wait = INFINITE) override;
    void unlock(bool all = false) override;

private:
    CubicleSoft::Sync::Mutex m_mutex;
    bool m_locked;
};

#endif // MUTEX_H
