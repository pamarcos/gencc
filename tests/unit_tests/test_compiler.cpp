/**
 * gencc is an application that generates compilation databases for clang
 *
 * Copyright (C) 2017 Pablo Marcos Oltra
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
 * along with gencc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <unordered_set>

#include "common.h"
#include "compiler.h"
#include "mock_helper.h"
#include "mock_lock_file.h"
#include "test_utils.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SaveArg;
using ::testing::ReturnRef;

class CompilerTest : public ::testing::Test {
public:
    CompilerTest()
        : m_compiler(&m_genccOptions, &m_helper)
    {
    }

    void SetUp() override
    {
        Logger::getInstance().disable();
    }

    std::vector<std::string> m_params;
    Compiler m_compiler;
    GenccOptions m_genccOptions;
    MockHelper m_helper;
};

class CompilerMock : public Compiler {
public:
    CompilerMock(GenccOptions* options, Helper* helper)
        : Compiler(options, helper)
    {
    }

    void fallbackWrap(unsigned retries) { return fallback(retries); }
};

TEST_F(CompilerTest, ErrorGettingCWD)
{
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(false));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, ErrorGettingGenccOptionsEnvVar)
{
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(Return(false));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, GenccOptionsEnvVarsMissing)
{
    std::string genccOptions = "{}";
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(DoAll(SetArgReferee<1>(genccOptions), Return(true)));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, GenccOptionsEnvVarDbFilenameMissing)
{
    std::string genccOptions = "{ \"build\":false }";
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(DoAll(SetArgReferee<1>(genccOptions), Return(true)));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, GenccOptionsEnvVarBuildMissingMissing)
{
    std::string genccOptions = "{ \"dbFilename\":\"foo\" }";
    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(DoAll(SetArgReferee<1>(genccOptions), Return(true)));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

/* TEST_F(CompilerTest, GenccOptionsSuccess)
{
    std::string genccOptions = "{ \"build\":false, \"dbFilename\":\"foo\" }";
    std::string dbFilename = "foo";
    MockLockFile* lockFile = new MockLockFile(dbFilename + Constants::COMPILE_DB_LOCK_EXT);
    std::string command = "foo bar";
    test_utils::generateParams(m_params, "compiler " + command);
    std::stringstream ss;

    EXPECT_CALL(m_helper, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_helper, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(DoAll(SetArgReferee<1>(genccOptions), Return(true)));
    EXPECT_CALL(m_helper, fileExists(lockFile->getFilename()))
        .WillOnce(Return(false));
    EXPECT_CALL(m_helper, fileExists(dbFilename))
        .WillOnce(Return(false));
    EXPECT_CALL(m_helper, getLockFileProxy(lockFile->getFilename()))
        .WillOnce(Return(lockFile));

    EXPECT_CALL(*lockFile, createFile())
        .WillOnce(Return());
    EXPECT_CALL(*lockFile, removeFile())
        .WillOnce(Return());
    EXPECT_CALL(*lockFile, writeToFile(command))
        .WillOnce(Return(true));
    EXPECT_CALL(*lockFile, readFromFile(_))
        .WillOnce(DoAll(SetArgReferee<0>(command), Return(true)));

    EXPECT_CALL(m_helper, getFileOstream(dbFilename))
        .WillOnce(::testing::ReturnPointee(static_cast<std::ostream*>(&ss)));
    EXPECT_CALL(m_helper, getFileIstream(dbFilename))
        .WillOnce(::testing::ReturnPointee(static_cast<std::istream*>(&ss)));

    m_compiler.doWork(m_params);
} */

TEST_F(CompilerTest, FallbackValuesAreDifferent)
{
    const unsigned MAX_FALLBACK_VALUE = 100;
    m_genccOptions.fallback = MAX_FALLBACK_VALUE;
    CompilerMock compilerMock(&m_genccOptions, &m_helper);

    unsigned value = 0;
    std::unordered_set<unsigned> set;
    EXPECT_CALL(m_helper, msleep(_))
        .Times(MAX_FALLBACK_VALUE)
        .WillRepeatedly(SaveArg<0>(&value));

    for (unsigned i = 0; i < MAX_FALLBACK_VALUE; ++i) {
        compilerMock.fallbackWrap(0);
        set.insert(value);
    }

    // Ensure at least half of the total values have been used by the fallback function
    EXPECT_GT(set.size(), MAX_FALLBACK_VALUE / 2);
}
