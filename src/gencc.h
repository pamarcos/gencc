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

using gencc_options_t = struct gencc_options {
    gencc_mode mode = gencc_mode::BUILDER;
    bool build = false;
    std::string dbFilename = DB_FILENAME;
    std::string cxx;
    std::string cc;
    std::string compiler;
    int retries = MAX_DB_RETRIES;
    int fallback = MAX_FALLBACK_SLEEP_IN_MS;
};

int gencc(int argc, char* argv[]);
bool parse_args(gencc_options_t& options, std::vector<std::string>& params);
void compiler_call(gencc_options_t& options, const std::vector<std::string>& params);
void write_to_db(const gencc_options_t& options, const std::string& directory, const std::string& command, const std::string& file);
void build_call(gencc_options_t& options, const std::vector<std::string>& params);

bool get_cwd(std::string& str);
void set_env_var(const char* name, const std::string& value);
bool get_env_var(const char* name, std::string& str);
void help();

#endif // GENCC_H
