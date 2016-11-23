#ifndef MOCK_HELPER_H
#define MOCK_HELPER_H

#include "gmock/gmock.h"
#include "helper.h"

class MockHelper : public Helper {
public:
    MOCK_CONST_METHOD2(getEnvVar, bool(const char*, std::string&));
    MOCK_CONST_METHOD2(setEnvVar, void(const char*, const std::string&));
    MOCK_METHOD1(getCwd, bool(std::string&));
    MOCK_CONST_METHOD1(runCommand, int(const std::string& str));
    MOCK_CONST_METHOD1(msleep, void(int));
    MOCK_CONST_METHOD1(fileExists, bool(const std::string& filename));
    MOCK_CONST_METHOD1(getFileLock, std::unique_ptr<FileLock>(const std::string& filename));
    MOCK_CONST_METHOD1(getFileIstream, std::unique_ptr<std::istream>(const std::string& filename));
    MOCK_CONST_METHOD1(getFileOstream, std::unique_ptr<std::ostream>(const std::string& filename));
};

#endif // MOCK_HELPER_H
