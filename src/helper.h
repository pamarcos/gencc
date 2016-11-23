#ifndef HELPER_H
#define HELPER_H

#include "file_lock.h"

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
    virtual void msleep(int ms) const = 0;

    // File commands
    virtual bool fileExists(const std::string& filename) const = 0;
    virtual std::unique_ptr<FileLock> getFileLock(const std::string& filename) const = 0;
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
    virtual void msleep(int ms) const override;

    // File commands
    virtual bool fileExists(const std::string& filename) const override;
    virtual std::unique_ptr<FileLock> getFileLock(const std::string& filename) const override;
    virtual std::unique_ptr<std::istream> getFileIstream(const std::string& filename) const override;
    virtual std::unique_ptr<std::ostream> getFileOstream(const std::string& filename) const override;

public:
    char m_buffer[128];
};

#endif // HELPER_H
