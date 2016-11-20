#ifndef GENCC_H
#define GENCC_H

#include <string>
#include <vector>

static const char* VERSION = "0.1";
static const char* NAME = "gencc";
static const char* CXX = "CXX";
static const char* CC = "CC";
static const char* DB_FILENAME = "compile_commands.json";
static const char* DB_LOCK_FILENAME_EXT = ".lock";
static const char* GENCC_OPTIONS = "GENCC_OPTIONS";
static const char* C_EXT = ".c";

static const int MAX_DB_RETRIES = 100;
static const int MAX_FALLBACK_SLEEP_IN_MS = 50;

enum class gencc_mode {
    BUILDER,
    COMPILER
};

using GenccOptions = struct GenccOptions_s {
    gencc_mode mode = gencc_mode::BUILDER;
    bool build = false;
    std::string dbFilename = DB_FILENAME;
    std::string cxx;
    std::string cc;
    std::string compiler;
    int retries = MAX_DB_RETRIES;
    int fallback = MAX_FALLBACK_SLEEP_IN_MS;
};

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

class Common {
public:
    Common(GenccOptions* options, Helper* helper);

    Helper* getHelper() const noexcept { return m_helper; }
    void setHelper(Helper* value) noexcept { m_helper = value; }

    GenccOptions* getOptions() const noexcept { return m_options; }
    void setOptions(GenccOptions* value) noexcept { m_options = value; }

protected:
    GenccOptions* m_options;
    Helper* m_helper;
};

class GenccWorker {
public:
    virtual ~GenccWorker() = default;
    virtual void doWork(const std::vector<std::string>& params) = 0;
};

class Builder final : public Common, public GenccWorker {
public:
    Builder(GenccOptions* options, Helper* helper);
    void doWork(const std::vector<std::string>& params) override;
};

class Compiler final : public Common, public GenccWorker {
public:
    Compiler(GenccOptions* options, Helper* helper);
    void doWork(const std::vector<std::string>& params) override;
    void writeToDb(const std::string& directory, const std::string& command, const std::string& file);
};

class Gencc final : public Common {
public:
    Gencc(GenccOptions* options, Helper* helper);
    int init(int argc, char* argv[]);

private:
    bool parseArgs(std::vector<std::string>& params);
    void help();
    std::unique_ptr<GenccWorker> m_worker;
};

#endif // GENCC_H
