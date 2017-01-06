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

#include "utils.h"
#include "common.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <thread>

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#define setenv(x, y, z) _putenv_s(x, y)
#else
#include <unistd.h>
#endif

bool UtilsImpl::getEnvVar(const char* name, std::string& str) const
{
    const char* ptr = getenv(name);
    if (ptr == nullptr) {
        str.clear();
        return false;
    }
    str = ptr;
    return true;
}

void UtilsImpl::setEnvVar(const char* name, const std::string& value) const
{
    std::string var;
    setenv(name, value.c_str(), 1);
    getEnvVar(name, var);
    if (var != value) {
        throw std::runtime_error(std::string("Couldn't set new var ") + name + " to " + value);
    }
}

bool UtilsImpl::getCwd(std::string& str)
{
    if (getcwd(reinterpret_cast<char*>(m_buffer.data()), sizeof(m_buffer)) == nullptr) {
        str.clear();
        return false;
    }
    str = reinterpret_cast<char*>(m_buffer.data());
    return true;
}

int UtilsImpl::runCommand(const std::string& str) const
{
    return system(str.c_str());
}

void UtilsImpl::msleep(unsigned ms) const
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

std::unique_ptr<SharedMem> UtilsImpl::createSharedMem(const std::string& name, size_t size) const
{
    std::unique_ptr<SharedMem> sharedMem(new SharedMemImpl());
    if (!sharedMem->create(name.c_str(), size)) {
        throw std::runtime_error(std::string("Error creating shared memory \"")
            + sharedMem->getName() + "\" with size " + std::to_string(sharedMem->getSize()));
    }
    return sharedMem;
}

bool UtilsImpl::fileExists(const std::string& filename) const
{
    std::ifstream file(filename);
    return file.good();
}

void UtilsImpl::removeFile(const std::string& filename) const
{
    std::remove(filename.c_str());
}

std::unique_ptr<std::ostream> UtilsImpl::getFileOstream(const std::string& filename) const
{
    std::unique_ptr<std::ostream> ofs(new std::ofstream(filename));
    return ofs;
}
