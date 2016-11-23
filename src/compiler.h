#ifndef COMPILER_H
#define COMPILER_H

#include "common.h"

class Compiler final : public Common, public GenccWorker {
public:
    Compiler(GenccOptions* options, Helper* helper);
    void doWork(const std::vector<std::string>& params) override;

private:
    void writeCompilationDB(const std::string& directory, const std::string& command, const std::string& file) const;
};

#endif // COMPILER_H
