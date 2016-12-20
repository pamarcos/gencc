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
 * along with gencc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "builder.h"
#include "mock_helper.h"

using ::testing::_;
using ::testing::Return;

class BuilderTest : public ::testing::Test {
public:
    BuilderTest()
        : m_builder(&m_genccOptions, &m_helper)
    {
    }

    void SetUp()
    {
        Logger::getInstance().disable();
    }

    Builder m_builder;
    GenccOptions m_genccOptions;
    MockHelper m_helper;
};

TEST_F(BuilderTest, ErrorGettingCWD)
{
    EXPECT_CALL(m_helper, getCwd(_)).WillOnce(Return(false));
    EXPECT_THROW(m_builder.doWork(std::vector<std::string>{ "foo" }), std::runtime_error);
}

TEST_F(BuilderTest, OneArgument)
{
    EXPECT_CALL(m_helper, getCwd(_)).WillOnce(Return(true));
    EXPECT_CALL(m_helper, removeFile(_)).WillRepeatedly(Return());
    EXPECT_CALL(m_helper, setEnvVar(Constants::GENCC_OPTIONS, _)).WillOnce(Return());
    EXPECT_CALL(m_helper, setEnvVar(Constants::CXX, _)).WillOnce(Return());
    EXPECT_CALL(m_helper, setEnvVar(Constants::CC, _)).WillOnce(Return());
    EXPECT_CALL(m_helper, runCommand(_)).WillOnce(Return(0));
    m_builder.doWork(std::vector<std::string>{ "foo" });
}

TEST_F(BuilderTest, SeveralArguments)
{
    EXPECT_CALL(m_helper, getCwd(_)).WillOnce(Return(true));
    EXPECT_CALL(m_helper, removeFile(_)).WillRepeatedly(Return());
    EXPECT_CALL(m_helper, setEnvVar(Constants::GENCC_OPTIONS, _)).WillOnce(Return());
    EXPECT_CALL(m_helper, setEnvVar(Constants::CXX, _)).WillOnce(Return());
    EXPECT_CALL(m_helper, setEnvVar(Constants::CC, _)).WillOnce(Return());
    EXPECT_CALL(m_helper, runCommand(_)).WillOnce(Return(0));
    m_builder.doWork(std::vector<std::string>{ "foo", "bar", "foo", "bar" });
}

TEST_F(BuilderTest, RemoveFiles)
{
    m_genccOptions.dbFilename = "foo";
    EXPECT_CALL(m_helper, getCwd(_)).WillOnce(Return(true));
    EXPECT_CALL(m_helper, removeFile("/" + m_genccOptions.dbFilename)).WillOnce(Return());
    EXPECT_CALL(m_helper, removeFile("/" + m_genccOptions.dbFilename + Constants::COMPILATION_DB_LOCK_EXT)).WillOnce(Return());
    EXPECT_CALL(m_helper, setEnvVar(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(m_helper, runCommand(_)).WillOnce(Return(0));
    m_builder.doWork(std::vector<std::string>{ "foo" });
}

TEST_F(BuilderTest, RunCommandError)
{
    EXPECT_CALL(m_helper, getCwd(_)).WillOnce(Return(true));
    EXPECT_CALL(m_helper, removeFile(_)).WillRepeatedly(Return());
    EXPECT_CALL(m_helper, setEnvVar(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(m_helper, runCommand(_)).WillOnce(Return(-1));
    m_builder.doWork(std::vector<std::string>{ "foo", "bar", "foo", "bar" });
}
