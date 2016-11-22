#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

#include "logger.h"

static const char* VERSION = "0.1";
static const char* NAME = "gencc";
static const char* CXX = "CXX";
static const char* CC = "CC";
static const char* COMPILATION_DB = "compile_commands.json";
static const char* COMPILATION_DB_LOCK_EXT = ".lock";
static const char* GENCC_OPTIONS = "GENCC_OPTIONS";
static const char* C_EXT = ".c";

static const int MAX_CDB_RETRIES = 100;
static const int MAX_CDB_FALLBACK_SLEEP_IN_MS = 50;

class Helper;

enum class GenccMode {
    NONE,
    BUILDER,
    COMPILER
};

using GenccOptions = struct GenccOptions_s {
    GenccMode mode = GenccMode::NONE;
    bool build = false;
    std::string dbFilename = COMPILATION_DB;
    std::string cxx;
    std::string cc;
    std::string compiler;
    int retries = MAX_CDB_RETRIES;
    int fallback = MAX_CDB_FALLBACK_SLEEP_IN_MS;
};

class GenccWorker {
public:
    virtual ~GenccWorker() = default;
    virtual void doWork(const std::vector<std::string>& params) = 0;
};

class Common {
public:
    Common(GenccOptions* options, Helper* helper)
        : m_options(options)
        , m_helper(helper)
    {
    }

    virtual ~Common() = default;

    Helper* getHelper() const noexcept { return m_helper; }
    void setHelper(Helper* value) noexcept { m_helper = value; }

    GenccOptions* getOptions() const noexcept { return m_options; }
    void setOptions(GenccOptions* value) noexcept { m_options = value; }

protected:
    GenccOptions* m_options;
    Helper* m_helper;
};

#endif // COMMON_H
