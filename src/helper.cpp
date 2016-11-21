#include "helper.h"

#include <cstdlib>
#include <exception>
#include <stdexcept>

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

bool Helper::getEnvVar(const char* name, std::string& str) const noexcept
{
    const char* ptr = getenv(name);
    if (ptr == nullptr) {
        str.clear();
        return false;
    }
    str = ptr;
    return true;
}

void Helper::setEnvVar(const char* name, const std::string& value) const
{
    std::string var;
    setenv(name, value.c_str(), 1);
    getEnvVar(name, var);
    if (var != value) {
        throw std::runtime_error(std::string("Couldn't set new var ") + name + " to " + value);
    }
}

bool Helper::getCwd(std::string& str) noexcept
{
    if (getcwd(reinterpret_cast<char*>(m_buffer), sizeof(m_buffer)) == nullptr) {
        str.clear();
        return false;
    }
    str = reinterpret_cast<char*>(m_buffer);
    return true;
}
