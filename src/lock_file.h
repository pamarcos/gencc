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
 * along with gencc.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILE_LOCK_H
#define FILE_LOCK_H

#include <fstream>
#include <memory>
#include <string>

class LockFile {
public:
    explicit LockFile(const std::string& filename);
    virtual ~LockFile() = default;

    virtual void createFile() const = 0;
    virtual void removeFile() const = 0;
    virtual bool writeToFile(const std::string& from) const = 0;
    virtual bool readFromFile(std::string& to) const = 0;

    std::string m_filename;
};

class LockFileGuard {
public:
    explicit LockFileGuard(std::unique_ptr<LockFile> lockFile);
    ~LockFileGuard();

    LockFile* getLockFile() const;
    void removeFile(bool remove);

private:
    std::unique_ptr<LockFile> m_lockFile;
    bool m_removeFile;
};

class LockFileImpl : public LockFile {
public:
    explicit LockFileImpl(const std::string& filename);

    void createFile() const override;
    void removeFile() const override;
    bool writeToFile(const std::string& from) const override;
    bool readFromFile(std::string& to) const override;
};

#endif // FILE_LOCK_H
