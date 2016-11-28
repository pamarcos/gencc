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
        worker = new MockGenccWorker();
        std::unique_ptr<GenccWorker> genccWorker(worker);
        gencc.setHelper(&helper);
        gencc.setWorker(genccWorker);
        Logger::getInstance().disable();
    }

    void generateParams(const std::string& str = "")
    {
        params.clear();
        if (str.empty()) {
            return;
        }
        size_t lastPos = 0;
        while (lastPos != std::string::npos) {
            size_t pos = str.find(' ', lastPos + 1);
            params.emplace_back(str.substr(lastPos, pos));
            lastPos = pos;
        }
    }

    Gencc gencc;
    MockHelper helper;
    std::vector<std::string> params;
    MockGenccWorker* worker;
};

TEST_F(GenccTest, NoHelper)
{
    gencc.setHelper(nullptr);
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, NotEnoughParameters)
{
    generateParams("gencc");
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, ErrorGettingCWD)
{
    generateParams("gencc -h");
    EXPECT_CALL(helper, getCwd(_)).WillOnce(Return(false));
    EXPECT_THROW(gencc.init(params), std::runtime_error);
}

TEST_F(GenccTest, AddCWDtoBinaryPath)
{
    generateParams("gencc -h");
    EXPECT_CALL(helper, getCwd(_)).WillOnce(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(false));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, AbsoluteBinaryPath)
{
    generateParams("/my/absolute/path/gencc -h");
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(false));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, CompilerMode)
{
    generateParams("gencc -h");
    EXPECT_CALL(helper, getCwd(_)).WillOnce(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, CompilerModeWork)
{
    generateParams("gencc foo");
    EXPECT_CALL(helper, getCwd(_)).WillOnce(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*worker, doWork(_));
    EXPECT_EQ(gencc.init(params), 0);
}
