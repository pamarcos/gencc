#ifndef COMPILER_H
#define COMPILER_H

#include "common.h"

class Compiler final : public Common, public GenccWorker {
public:
    Compiler(GenccOptions* options, Helper* helper);
    void doWork(const std::vector<std::string>& params) override;

private:
    void writeCompilationDB() const;
    void fallback(unsigned retries) const;

    std::string m_directory;
    std::string m_command;
    std::string m_file;
};

#endif // COMPILER_H
