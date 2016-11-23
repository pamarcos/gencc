#include "helper.h"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <thread>

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

bool HelperImpl::getEnvVar(const char* name, std::string& str) const
{
    const char* ptr = getenv(name);
    if (ptr == nullptr) {
        str.clear();
        return false;
    }
    str = ptr;
    return true;
}

void HelperImpl::setEnvVar(const char* name, const std::string& value) const
{
    std::string var;
    setenv(name, value.c_str(), 1);
    getEnvVar(name, var);
    if (var != value) {
        throw std::runtime_error(std::string("Couldn't set new var ") + name + " to " + value);
    }
}

bool HelperImpl::getCwd(std::string& str)
{
    if (getcwd(reinterpret_cast<char*>(m_buffer), sizeof(m_buffer)) == nullptr) {
        str.clear();
        return false;
    }
    str = reinterpret_cast<char*>(m_buffer);
    return true;
}

int HelperImpl::runCommand(const std::string& str) const
{
    return system(str.c_str());
}

void HelperImpl::msleep(int ms) const
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool HelperImpl::fileExists(const std::string& filename) const
{
    std::ifstream file(filename);
    return file.good();
}

std::unique_ptr<FileLock> HelperImpl::getFileLock(const std::string& filename) const
{
    return std::unique_ptr<FileLock>(new FileLockImpl(filename));
}

std::unique_ptr<std::istream> HelperImpl::getFileIstream(const std::string& filename) const
{
    std::unique_ptr<std::istream> ifs(new std::ifstream(filename));
    return ifs;
}

std::unique_ptr<std::ostream> HelperImpl::getFileOstream(const std::string& filename) const
{
    std::unique_ptr<std::ostream> ofs(new std::ofstream(filename));
    return ofs;
}
