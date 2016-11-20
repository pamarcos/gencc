#ifndef BUILDER_H
#define BUILDER_H

#include "common.h"

class Builder final : public Common, public GenccWorker {
public:
    Builder(GenccOptions* options, Helper* helper);
    void doWork(const std::vector<std::string>& params) override;
};

#endif // BUILDER_H
