/**
 * m_gencc is an application that generates compilation databases for clang
 *
 * Copyright (C) 2016 Pablo Marcos Oltra
 *
 * This file is part of m_gencc.
 *
 * m_gencc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * m_gencc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with m_gencc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "gencc.h"
#include "mock_gencc_worker.h"
#include "mock_helper.h"
#include "test_utils.h"

using ::testing::_;
using ::testing::Return;

class GenccTest : public ::testing::Test {
public:
    GenccTest()
        : m_gencc(nullptr)
        , m_worker(nullptr)
    {
    }

    void SetUp() override
    {
        m_worker = new MockGenccWorker();
        std::unique_ptr<GenccWorker> genccWorker(m_worker);
        m_gencc.setHelper(&m_helper);
        m_gencc.setWorker(genccWorker);
        Logger::getInstance().disable();
    }

    Gencc m_gencc;
    MockHelper m_helper;
    std::vector<std::string> m_params;
    MockGenccWorker* m_worker;
};

TEST_F(GenccTest, LoggerEnabled)
{
    Logger::getInstance().enable();
    Logger::getInstance().log("Logger test\n");
}

TEST_F(GenccTest, NoHelper)
{
    m_gencc.setHelper(nullptr);
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, NotEnoughParameters)
{
    utils::generateParams(m_params, "m_gencc");
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, ErrorGettingCWD)
{
    utils::generateParams(m_params, "m_gencc -h");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(false));
    EXPECT_THROW(m_gencc.init(m_params), std::runtime_error);
}

TEST_F(GenccTest, AddCWDtoBinaryPath)
{
    utils::generateParams(m_params, "m_gencc -h");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(false));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, AbsoluteBinaryPath)
{
    utils::generateParams(m_params, "/my/absolute/path/m_gencc -h");
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(false));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CompilerModeFail)
{
    utils::generateParams(m_params, "m_gencc -h");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CompilerModeSuccess)
{
    utils::generateParams(m_params, "m_gencc foo");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, WorkerNullBuilderMode)
{
    utils::generateParams(m_params, "m_gencc foo");
    std::unique_ptr<GenccWorker> nullWorker(nullptr);
    m_gencc.setWorker(nullWorker);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillRepeatedly(Return(false));
    EXPECT_CALL(m_helper, removeFile(_))
        .WillRepeatedly(Return());
    EXPECT_CALL(m_helper, setEnvVar(_, _))
        .WillRepeatedly(Return());
    EXPECT_CALL(m_helper, runCommand(_))
        .WillOnce(Return(0));
    EXPECT_EQ(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, WorkerNullCompilerMode)
{
    utils::generateParams(m_params, "m_gencc foo");
    std::unique_ptr<GenccWorker> nullWorker(nullptr);
    m_gencc.setWorker(nullWorker);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, removeFile(_))
        .WillRepeatedly(Return());
    EXPECT_CALL(m_helper, setEnvVar(_, _))
        .WillRepeatedly(Return());
    EXPECT_THROW(m_gencc.init(m_params), std::runtime_error);
}

/* Parameters */
TEST_F(GenccTest, CompilerParamNoValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::GENCC_COMPILER_PARAM);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CompilerParamValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::GENCC_COMPILER_PARAM + " foo");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().compiler, "foo");
}

TEST_F(GenccTest, CxxParamNoValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_CXX);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CxxParamValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_CXX + " foo");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().cxx, "foo");
}

TEST_F(GenccTest, CcParamNoValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_CC);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CcParamValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_CC + " foo");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().cc, "foo");
}

TEST_F(GenccTest, OutputParamNoValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_OUTPUT);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, OutputParamValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_OUTPUT + " foo");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().dbFilename, "foo");
}

TEST_F(GenccTest, RetriesParamNoValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_RETRIES);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, RetriesParamValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_RETRIES + " 123");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().retries, 123u);
}

TEST_F(GenccTest, FallbackParamNoValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_FALLBACK);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, FallbackParamValue)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_FALLBACK + " 456");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().fallback, 456u);
}

TEST_F(GenccTest, BuildParam)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_BUILD);
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_TRUE(m_gencc.getGenccOptions().build);
}

TEST_F(GenccTest, BuildParamWithMoreParams)
{
    utils::generateParams(m_params, std::string("m_gencc ") + Constants::PARAM_BUILD + " foo bar");
    EXPECT_CALL(m_helper, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_TRUE(m_gencc.getGenccOptions().build);
}
