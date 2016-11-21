#ifndef MOCK_GENCC_WORKER_H
#define MOCK_GENCC_WORKER_H

#include "common.h"
#include "gmock/gmock.h"

class MockGenccWorker : public GenccWorker {
public:
    MOCK_METHOD1(doWork, void(const std::vector<std::string>& params));
};

#endif // MOCK_GENCC_WORKER_H
