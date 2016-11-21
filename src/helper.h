#ifndef HELPER_H
#define HELPER_H

#include <string>

class Helper {
public:
    virtual ~Helper() = default;
    virtual bool getEnvVar(const char* name, std::string& str) const = 0;
    virtual void setEnvVar(const char* name, const std::string& value) const = 0;
    virtual bool getCwd(std::string& str) = 0;
};

class HelperImpl final : public Helper {
public:
    bool getEnvVar(const char* name, std::string& str) const override;
    void setEnvVar(const char* name, const std::string& value) const override;
    bool getCwd(std::string& str) override;

public:
    char m_buffer[128];
};

#endif // HELPER_H
