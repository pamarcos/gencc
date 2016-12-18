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

#ifndef HELPER_H
#define HELPER_H

#include "lock_file.h"

#include <fstream>
#include <memory>
#include <string>

class Helper {
public:
    virtual ~Helper() = default;

    // Environment variables
    virtual bool getEnvVar(const char* name, std::string& str) const = 0;
    virtual void setEnvVar(const char* name, const std::string& value) const = 0;
    virtual bool getCwd(std::string& str) = 0;

    // System commands
    virtual int runCommand(const std::string& str) const = 0;
    virtual void msleep(unsigned ms) const = 0;

    // File commands
    virtual bool fileExists(const std::string& filename) const = 0;
    virtual void removeFile(const std::string& filename) const = 0;
    virtual std::unique_ptr<LockFile> getLockFile(const std::string& filename) const = 0;
    virtual std::unique_ptr<std::istream> getFileIstream(const std::string& filename) const = 0;
    virtual std::unique_ptr<std::ostream> getFileOstream(const std::string& filename) const = 0;
};

class HelperImpl final : public Helper {
public:
    // Environment variables
    bool getEnvVar(const char* name, std::string& str) const override;
    void setEnvVar(const char* name, const std::string& value) const override;
    bool getCwd(std::string& str) override;

    // System commands
    int runCommand(const std::string& str) const override;
    virtual void msleep(unsigned ms) const override;

    // File commands
    virtual bool fileExists(const std::string& filename) const override;
    virtual void removeFile(const std::string& filename) const override;
    virtual std::unique_ptr<LockFile> getLockFile(const std::string& filename) const override;
    virtual std::unique_ptr<std::istream> getFileIstream(const std::string& filename) const override;
    virtual std::unique_ptr<std::ostream> getFileOstream(const std::string& filename) const override;

public:
    char m_buffer[128];
};

#endif // HELPER_H
