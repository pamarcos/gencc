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

#include "file_lock.h"

#include <fstream>

FileLock::FileLock(const std::string& filename)
    : m_filename(filename)
{
}

FileLockGuard::FileLockGuard(std::unique_ptr<FileLock> lockFile)
    : m_lockFile(std::move(lockFile))
    , m_removeFile(true)
{
    m_lockFile->createFile();
}

FileLockGuard::~FileLockGuard()
{
    if (m_removeFile) {
        m_lockFile->removeFile();
    }
}

FileLock* FileLockGuard::getLockFile() const
{
    return m_lockFile.get();
}

void FileLockGuard::removeFile(bool remove)
{
    m_removeFile = remove;
}

FileLockImpl::FileLockImpl(const std::string& filename)
    : FileLock(filename)
{
}

void FileLockImpl::createFile()
{
    std::ofstream ofs(m_filename);
}

void FileLockImpl::removeFile()
{
    std::remove(m_filename.c_str());
}

bool FileLockImpl::writeToFile(const std::string& from)
{
    std::ofstream ofs(m_filename, std::ios::app);
    if (ofs.good()) {
        ofs << from << '\n';
        ofs.flush();
        return true;
    }
    return false;
}

bool FileLockImpl::readFromFile(std::string& to)
{
    std::ifstream ifs(m_filename);
    if (ifs.good()) {
        ifs.seekg(0);
        std::string line;
        while (!ifs.eof()) {
            std::getline(ifs, line);
            to += line;
        }
        return true;
    }
    return false;
}
