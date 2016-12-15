/**
 * gencc is an application that generates compilation databases for clang
 *
 * Copyright (C) 2016 Pablo Marcos Oltra
 *
 * This file is part of gencc.
 *
 * gencc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gencc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

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
            size_t pos = str.find(' ', lastPos);
            params.emplace_back(str.substr(lastPos, pos - lastPos));
            lastPos = (pos != std::string::npos) ? pos + 1 : pos;
        }
    }

    Gencc gencc;
    MockHelper helper;
    std::vector<std::string> params;
    MockGenccWorker* worker;
};

TEST_F(GenccTest, GenerateParams)
{
    generateParams("onetwo threefour fivesix seveneight nineten");
    std::vector<std::string> checkParams = { "onetwo", "threefour", "fivesix", "seveneight", "nineten" };
    EXPECT_EQ(params, checkParams);
}

TEST_F(GenccTest, LoggerEnabled)
{
    Logger::getInstance().enable();
    Logger::getInstance().log("Logger test\n");
}

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

TEST_F(GenccTest, WorkerNullBuilderMode)
{
    generateParams("gencc echo");
    std::unique_ptr<GenccWorker> nullWorker(nullptr);
    gencc.setWorker(nullWorker);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(Constants::GENCC_OPTIONS, _)).WillRepeatedly(Return(false));
    EXPECT_CALL(helper, removeFile(_)).WillRepeatedly(Return());
    EXPECT_CALL(helper, setEnvVar(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(helper, runCommand(_)).WillOnce(Return(0));
    EXPECT_EQ(gencc.init(params), 0);
}

TEST_F(GenccTest, WorkerNullCompilerMode)
{
    generateParams("gencc echo");
    std::unique_ptr<GenccWorker> nullWorker(nullptr);
    gencc.setWorker(nullWorker);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(Constants::GENCC_OPTIONS, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, removeFile(_)).WillRepeatedly(Return());
    EXPECT_CALL(helper, setEnvVar(_, _)).WillRepeatedly(Return());
    EXPECT_THROW(gencc.init(params), std::runtime_error);
}

/* Parameters */
TEST_F(GenccTest, CompilerParamNoValue)
{
    generateParams(std::string("gencc ") + Constants::GENCC_COMPILER_PARAM);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, CompilerParamValue)
{
    generateParams(std::string("gencc ") + Constants::GENCC_COMPILER_PARAM + " foo");
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*worker, doWork(_));
    EXPECT_EQ(gencc.init(params), 0);
}

TEST_F(GenccTest, CxxParamNoValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_CXX);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, CxxParamValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_CXX + " foo");
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*worker, doWork(_));
    EXPECT_EQ(gencc.init(params), 0);
}

TEST_F(GenccTest, CcParamNoValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_CC);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, CcParamValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_CC + " foo");
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*worker, doWork(_));
    EXPECT_EQ(gencc.init(params), 0);
}

TEST_F(GenccTest, OutputParamNoValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_OUTPUT);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, OutputParamValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_OUTPUT + " foo");
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*worker, doWork(_));
    EXPECT_EQ(gencc.init(params), 0);
}

TEST_F(GenccTest, RetriesParamNoValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_RETRIES);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, RetriesParamValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_RETRIES + " 1");
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*worker, doWork(_));
    EXPECT_EQ(gencc.init(params), 0);
}

TEST_F(GenccTest, FallbackParamNoValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_FALLBACK);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_NE(gencc.init(params), 0);
}

TEST_F(GenccTest, FallbackParamValue)
{
    generateParams(std::string("gencc ") + Constants::PARAM_FALLBACK + " 1");
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*worker, doWork(_));
    EXPECT_EQ(gencc.init(params), 0);
}

TEST_F(GenccTest, BuildParam)
{
    generateParams(std::string("gencc ") + Constants::PARAM_BUILD);
    EXPECT_CALL(helper, getCwd(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(helper, getEnvVar(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(*worker, doWork(_));
    EXPECT_EQ(gencc.init(params), 0);
}
