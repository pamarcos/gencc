#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "gencc.h"
#include "mock_gencc_worker.h"
#include "mock_helper.h"

using ::testing::_;
using ::testing::Return;

class GenccTest : public ::testing::Test {
public:
    GenccTest()
        : gencc(nullptr)
    {
    }

    void SetUp()
    {
        gencc.setHelper(&helper);
        gencc.setWorker(std::unique_ptr<GenccWorker>(new MockGenccWorker()));
    }

    Gencc gencc;
    MockHelper helper;
    std::vector<std::string> params;
    std::unique_ptr<GenccWorker> worker;
};

TEST_F(GenccTest, paramError)
{
    params = { "gencc" };
    EXPECT_FALSE(gencc.init(params));
}

TEST_F(GenccTest, getCwdError)
{
    params = { "gencc", "-h" };
    EXPECT_CALL(helper, getCwd(_)).WillOnce(Return(false));
    EXPECT_THROW(gencc.init(params), std::runtime_error);
}
