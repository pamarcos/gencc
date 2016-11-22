#ifndef MOCK_HELPER_H
#define MOCK_HELPER_H

#include "gmock/gmock.h"
#include "helper.h"

class MockHelper : public Helper {
public:
    MOCK_CONST_METHOD2(getEnvVar, bool(const char*, std::string&));
    MOCK_CONST_METHOD2(setEnvVar, void(const char*, const std::string&));
    MOCK_METHOD1(getCwd, bool(std::string&));
    MOCK_METHOD1(runCommand, int(const std::string& str));
};

#endif // MOCK_HELPER_H
