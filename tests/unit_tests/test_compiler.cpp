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
#include "mock_utils.h"
#include "test_utils.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;

class CompilerTest : public ::testing::Test {
public:
    CompilerTest()
        : m_compiler(&m_genccOptions, &m_utils)
    {
    }

    void SetUp() override
    {
        Logger::getInstance().disable();
    }

    std::vector<std::string> m_params;
    Compiler m_compiler;
    GenccOptions m_genccOptions;
    MockUtils m_utils;
};

TEST_F(CompilerTest, ErrorGettingCWD)
{
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(false));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, ErrorGettingGenccOptionsEnvVar)
{
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(Return(false));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, GenccOptionsEnvVarsMissing)
{
    std::string genccOptions = "{}";
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(DoAll(SetArgReferee<1>(genccOptions), Return(true)));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, GenccOptionsEnvVarDbFilenameMissing)
{
    std::string genccOptions = "{ \"build\":false }";
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(DoAll(SetArgReferee<1>(genccOptions), Return(true)));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, GenccOptionsEnvVarBuildMissingMissing)
{
    std::string genccOptions = "{ \"dbFilename\":\"foo\" }";
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(Constants::GENCC_OPTIONS, _))
        .WillOnce(DoAll(SetArgReferee<1>(genccOptions), Return(true)));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}
