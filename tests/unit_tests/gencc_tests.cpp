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
        worker = std::unique_ptr<GenccWorker>(new MockGenccWorker());
        gencc.setHelper(&helper);
        gencc.setWorker(worker);
        Logger::getInstance().disable();
    }

    Gencc gencc;
    MockHelper helper;
    std::vector<std::string> params;
    std::unique_ptr<GenccWorker> worker;
};

TEST_F(GenccTest, NoHelper)
{
    gencc.setHelper(nullptr);
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, NotEnoughParameters)
{
    params = { "gencc" };
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, ErrorGettingCWD)
{
    params = { "gencc", "-h" };
    EXPECT_CALL(helper, getCwd(_)).WillOnce(Return(false));
    EXPECT_THROW(gencc.init(params), std::runtime_error);
}
