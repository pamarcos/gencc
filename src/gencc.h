#ifndef GENCC_H
#define GENCC_H

#include "common.h"
#include <memory>
#include <string>
#include <vector>

class Gencc final : public Common {
public:
    Gencc(GenccOptions* options, Helper* helper);
    int init(std::vector<std::string>& params);

    void setWorker(std::unique_ptr<GenccWorker>& worker);

private:
    bool parseArgs(std::vector<std::string>& params);
    void help();
    std::unique_ptr<GenccWorker> m_worker;
};

#endif // GENCC_H
