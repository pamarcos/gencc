#ifndef HELPER_H
#define HELPER_H

#include <string>

class IHelper {
public:
    virtual ~IHelper() = default;
    virtual bool getEnvVar(const char* name, std::string& str) const noexcept = 0;
    virtual void setEnvVar(const char* name, const std::string& value) const = 0;
    virtual bool getCwd(std::string& str) noexcept = 0;
};

class Helper final : public IHelper {
public:
    bool getEnvVar(const char* name, std::string& str) const noexcept override;
    void setEnvVar(const char* name, const std::string& value) const override;
    bool getCwd(std::string& str) noexcept override;

public:
    char m_buffer[128];
};

#endif // HELPER_H
