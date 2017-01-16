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

#include "builder.h"
#include "mock_shared_mem.h"
#include "mock_utils.h"
#include "test_utils.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ByMove;
using ::testing::StrEq;

class BuilderTest : public ::testing::Test {
public:
    BuilderTest()
        : m_builder(&m_genccOptions, &m_utils)
        , m_uniqueSharedMem(new MockSharedMem())
        , m_mockSharedMem(static_cast<MockSharedMem*>(m_uniqueSharedMem.get()))
        , m_ostream(new std::stringstream())
    {
    }

    void SetUp() override
    {
        Logger::getInstance().disable();
        strncpy(m_compilerBuffer.data(), test_utils::JSON_DB_GOOD, strlen(test_utils::JSON_DB_GOOD));
        m_genccOptions.cxx = "foo";
    }

    std::vector<std::string> m_params;
    Builder m_builder;
    GenccOptions m_genccOptions;
    MockUtils m_utils;
    std::unique_ptr<SharedMem> m_uniqueSharedMem;
    MockSharedMem* m_mockSharedMem;
    std::array<char, Constants::SHARED_MEM_SIZE> m_builderBuffer;
    std::array<char, Constants::SHARED_MEM_SIZE> m_compilerBuffer;
    std::unique_ptr<std::ostream> m_ostream;
};

TEST_F(BuilderTest, ErrorGettingCWD)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(false));
    EXPECT_THROW(m_builder.doWork(m_params), std::runtime_error);
}

TEST_F(BuilderTest, WrongJson)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, removeFile(_))
        .WillOnce(Return());
    EXPECT_CALL(m_utils, setEnvVar(_, _))
        .WillRepeatedly(Return());
    EXPECT_CALL(m_utils, createSharedMem(_, _))
        .WillOnce(Return(ByMove(std::move(m_uniqueSharedMem))));

    strncpy(m_compilerBuffer.data(), "1", 1);
    EXPECT_CALL(*m_mockSharedMem, rawData())
        .Times(2)
        .WillOnce(Return(m_builderBuffer.data()))
        .WillOnce(Return(m_compilerBuffer.data()));
    EXPECT_CALL(*m_mockSharedMem, unlockMutex())
        .WillOnce(Return());
    EXPECT_CALL(*m_mockSharedMem, getSize())
        .WillOnce(Return(m_builderBuffer.size()));

    EXPECT_CALL(m_utils, runCommand(_))
        .WillOnce(Return(0));
    EXPECT_THROW(m_builder.doWork(m_params), std::runtime_error);
}

TEST_F(BuilderTest, OneArgument)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, removeFile(_))
        .WillOnce(Return());

    EXPECT_CALL(m_utils, setEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(Return());
    EXPECT_CALL(m_utils, setEnvVar(StrEq(Constants::CXX), _))
        .WillOnce(Return());
    EXPECT_CALL(m_utils, setEnvVar(StrEq(Constants::CC), _))
        .WillOnce(Return());
    EXPECT_CALL(m_utils, createSharedMem(Constants::SHARED_MEM_NAME, Constants::SHARED_MEM_SIZE))
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

    EXPECT_CALL(m_utils, getFileOstream("/" + m_genccOptions.dbFilename))
        .WillOnce(Return(ByMove(std::move(m_ostream))));

    m_builder.doWork(m_params);
}

TEST_F(BuilderTest, SeveralArguments)
{
    test_utils::generateParams(m_params, "foo bar foo bar");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, removeFile(_))
        .WillRepeatedly(Return());

    EXPECT_CALL(m_utils, setEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(Return());
    EXPECT_CALL(m_utils, setEnvVar(StrEq(Constants::CXX), _))
        .WillOnce(Return());
    EXPECT_CALL(m_utils, setEnvVar(StrEq(Constants::CC), _))
        .WillOnce(Return());
    EXPECT_CALL(m_utils, createSharedMem(Constants::SHARED_MEM_NAME, Constants::SHARED_MEM_SIZE))
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

    EXPECT_CALL(m_utils, getFileOstream("/" + m_genccOptions.dbFilename))
        .WillOnce(Return(ByMove(std::move(m_ostream))));

    m_builder.doWork(m_params);
}

TEST_F(BuilderTest, RemoveFiles)
{
    test_utils::generateParams(m_params, "foo");
    m_genccOptions.dbFilename = "foo";
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, removeFile("/" + m_genccOptions.dbFilename))
        .WillOnce(Return());
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

    m_builder.doWork(m_params);
}

TEST_F(BuilderTest, RunCommandError)
{
    test_utils::generateParams(m_params, "foo bar foo bar");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, removeFile(_))
        .WillOnce(Return());
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
        .WillOnce(Return(-1));

    EXPECT_CALL(m_utils, getFileOstream(_))
        .WillOnce(Return(ByMove(std::move(m_ostream))));

    m_builder.doWork(m_params);
}
