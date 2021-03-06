/**
 * m_gencc is an application that generates compilation databases for clang
 *
 * Copyright (C) 2017 Pablo Marcos Oltra
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
#include "mock_shared_mem.h"
#include "mock_utils.h"
#include "test_utils.h"

using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::ByMove;
using ::testing::SetArgReferee;

class GenccTest : public ::testing::Test {
public:
    GenccTest()
        : m_gencc(nullptr)
        , m_worker(nullptr)
        , m_uniqueSharedMem(new MockSharedMem())
        , m_mockSharedMem(static_cast<MockSharedMem*>(m_uniqueSharedMem.get()))
        , m_ostream(new std::stringstream())
    {
    }

    void SetUp() override
    {
        m_worker = new MockGenccWorker();
        std::unique_ptr<GenccWorker> genccWorker(m_worker);
        m_gencc.setUtils(&m_utils);
        m_gencc.setWorker(genccWorker);
        Logger::getInstance().disable();
        strncpy(m_compilerBuffer.data(), test_utils::JSON_DB_GOOD, strlen(test_utils::JSON_DB_GOOD));
    }

    Gencc m_gencc;
    MockUtils m_utils;
    std::vector<std::string> m_params;
    MockGenccWorker* m_worker;
    std::unique_ptr<SharedMem> m_uniqueSharedMem;
    MockSharedMem* m_mockSharedMem;
    std::array<char, Constants::SHARED_MEM_SIZE> m_builderBuffer;
    std::array<char, Constants::SHARED_MEM_SIZE> m_compilerBuffer;
    std::unique_ptr<std::ostream> m_ostream;
};

TEST_F(GenccTest, LoggerEnabled)
{
    Logger::getInstance().enable();
    Logger::getInstance().log("Logger test\n");
}

TEST_F(GenccTest, NoUtils)
{
    m_gencc.setUtils(nullptr);
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, NotEnoughParameters)
{
    test_utils::generateParams(m_params, "gencc");
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, ErrorGettingCWD)
{
    test_utils::generateParams(m_params, "gencc -h");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(false));
    EXPECT_THROW(m_gencc.init(m_params), std::runtime_error);
}

TEST_F(GenccTest, AddCWDtoBinaryPath)
{
    test_utils::generateParams(m_params, "gencc -h");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(false));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, AbsoluteBinaryPath)
{
    test_utils::generateParams(m_params, "/my/absolute/path/m_gencc -h");
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(false));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CompilerModeFail)
{
    test_utils::generateParams(m_params, "gencc -h");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CompilerModeSuccess)
{
    test_utils::generateParams(m_params, "gencc foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, WorkerNullBuilderMode)
{
    test_utils::generateParams(m_params, "gencc " + std::string(Constants::PARAM_CXX) + " foo bar");
    std::unique_ptr<GenccWorker> nullWorker(nullptr);
    m_gencc.setWorker(nullWorker);
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillRepeatedly(Return(false));
    EXPECT_CALL(m_utils, removeFile(_))
        .WillRepeatedly(Return());
    EXPECT_CALL(m_utils, setEnvVar(_, _))
        .WillRepeatedly(Return());

    EXPECT_CALL(m_utils, createSharedMem(_, _))
        .WillOnce(Return(ByMove(std::move(m_uniqueSharedMem))));

    EXPECT_CALL(*m_mockSharedMem, rawData())
        .Times(2)
        .WillOnce(Return(m_builderBuffer.data()))
        .WillOnce(Return(m_compilerBuffer.data()));
    EXPECT_CALL(*m_mockSharedMem, getSize())
        .WillOnce(Return(m_builderBuffer.size()));
    EXPECT_CALL(*m_mockSharedMem, unlockMutex())
        .WillOnce(Return());

    EXPECT_CALL(m_utils, runCommand(_))
        .WillOnce(Return(0));

    EXPECT_CALL(m_utils, getFileOstream(_))
        .WillOnce(Return(ByMove(std::move(m_ostream))));

    EXPECT_EQ(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, WorkerNullCompilerMode)
{
    test_utils::generateParams(m_params, "gencc foo");
    std::unique_ptr<GenccWorker> nullWorker(nullptr);
    m_gencc.setWorker(nullWorker);
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_FALSE), Return(true)));
    EXPECT_CALL(m_utils, removeFile(_))
        .WillRepeatedly(Return());
    EXPECT_CALL(m_utils, setEnvVar(_, _))
        .WillRepeatedly(Return());

    EXPECT_EQ(m_gencc.init(m_params), 0);
}

/* Parameters */
TEST_F(GenccTest, CompilerParamNoValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::GENCC_COMPILER_PARAM);
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CompilerParamValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::GENCC_COMPILER_PARAM + " foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().compiler, "foo");
}

TEST_F(GenccTest, CxxParamNoValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_CXX);
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CxxParamValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_CXX + " foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().cxx, "foo");
}

TEST_F(GenccTest, CcParamNoValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_CC);
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, CcParamValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_CC + " foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().cc, "foo");
}

TEST_F(GenccTest, OutputParamNoValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_OUTPUT);
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, OutputParamValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_OUTPUT + " foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().dbFilename, "foo");
}

TEST_F(GenccTest, BuildParam)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_BUILD);
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_TRUE(m_gencc.getGenccOptions().build);
}

TEST_F(GenccTest, BuildParamWithMoreParams)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_BUILD + " foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*m_worker, doWork(_));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_TRUE(m_gencc.getGenccOptions().build);
}

TEST_F(GenccTest, SharedMemoryParamNoValue)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_SHARED_MEMORY);
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_NE(m_gencc.init(m_params), 0);
}

TEST_F(GenccTest, SharedMemoryParam)
{
    test_utils::generateParams(m_params, std::string("gencc ") + Constants::PARAM_SHARED_MEMORY + " 123");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(m_gencc.init(m_params), 0);
    EXPECT_EQ(m_gencc.getGenccOptions().sharedMemSize, 123u);
}
