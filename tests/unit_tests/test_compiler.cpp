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

#include "compiler.h"
#include "mock_shared_mem.h"
#include "mock_utils.h"
#include "test_utils.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ByMove;
using ::testing::StrEq;
using ::testing::DoAll;
using ::testing::SetArgReferee;

class CompilerTest : public ::testing::Test {
public:
    CompilerTest()
        : m_compiler(&m_genccOptions, &m_utils)
        , m_uniqueSharedMem(new MockSharedMem())
        , m_mockSharedMem(static_cast<MockSharedMem*>(m_uniqueSharedMem.get()))
    {
    }

    void SetUp() override
    {
        Logger::getInstance().disable();
        memset(m_compilerBuffer.data(), 0, m_compilerBuffer.size());
    }

    std::vector<std::string> m_params;
    Compiler m_compiler;
    GenccOptions m_genccOptions;
    MockUtils m_utils;
    std::unique_ptr<SharedMem> m_uniqueSharedMem;
    MockSharedMem* m_mockSharedMem;
    std::array<char, Constants::SHARED_MEM_SIZE> m_compilerBuffer;
};

TEST_F(CompilerTest, ErrorGettingCWD)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(false));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, ErrorGettingEnvVar)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillOnce(Return(false));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, ErrorDeserializingGenccOptions)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(_, _))
        .WillOnce(Return(true));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, NoBuildOptionDeserializing)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>("{}"), Return(true)));
    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, BuildParameterFalse)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_FALSE), Return(true)));
    m_compiler.doWork(m_params);
}

TEST_F(CompilerTest, BuildParameterTrue)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_TRUE), Return(true)));
    EXPECT_CALL(m_utils, runCommand(_))
        .WillOnce(Return(0));
    m_compiler.doWork(m_params);
}

TEST_F(CompilerTest, BuildExecutionError)
{
    test_utils::generateParams(m_params, "foo");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_TRUE), Return(true)));
    EXPECT_CALL(m_utils, runCommand(_))
        .WillOnce(Return(-1));
    m_compiler.doWork(m_params);
}

TEST_F(CompilerTest, FirstCreatingSharedMem)
{
    test_utils::generateParams(m_params, "foo bar.c");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_FALSE), Return(true)));

    EXPECT_CALL(m_utils, createSharedMem(_, _))
        .WillOnce(Return(ByMove(std::move(m_uniqueSharedMem))));
    EXPECT_CALL(*m_mockSharedMem, first())
        .WillOnce(Return(true));

    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, BadJsonInSharedMemory)
{
    test_utils::generateParams(m_params, "foo bar.c");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_FALSE), Return(true)));

    EXPECT_CALL(m_utils, createSharedMem(_, _))
        .WillOnce(Return(ByMove(std::move(m_uniqueSharedMem))));
    EXPECT_CALL(*m_mockSharedMem, first())
        .WillOnce(Return(false));

    strncpy(m_compilerBuffer.data(), "foo", 3);
    EXPECT_CALL(*m_mockSharedMem, rawData())
        .WillOnce(Return(m_compilerBuffer.data()));

    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, EmptyJsonInSharedMemoryAndNotEnoughMemory)
{
    test_utils::generateParams(m_params, "foo bar.c");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_FALSE), Return(true)));

    EXPECT_CALL(m_utils, createSharedMem(_, _))
        .WillOnce(Return(ByMove(std::move(m_uniqueSharedMem))));
    EXPECT_CALL(*m_mockSharedMem, first())
        .WillOnce(Return(false));

    EXPECT_CALL(*m_mockSharedMem, rawData())
        .WillOnce(Return(m_compilerBuffer.data()));
    EXPECT_CALL(*m_mockSharedMem, getSize())
        .WillRepeatedly(Return(0));

    EXPECT_THROW(m_compiler.doWork(m_params), std::runtime_error);
}

TEST_F(CompilerTest, EmptyJsonInSharedMemorySuccess)
{
    test_utils::generateParams(m_params, "foo bar.c");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_FALSE), Return(true)));

    EXPECT_CALL(m_utils, createSharedMem(_, _))
        .WillOnce(Return(ByMove(std::move(m_uniqueSharedMem))));
    EXPECT_CALL(*m_mockSharedMem, first())
        .WillOnce(Return(false));

    EXPECT_CALL(*m_mockSharedMem, rawData())
        .WillOnce(Return(m_compilerBuffer.data()));
    EXPECT_CALL(*m_mockSharedMem, getSize())
        .WillRepeatedly(Return(m_compilerBuffer.size()));

    m_compiler.doWork(m_params);
}

TEST_F(CompilerTest, EmptyJsonInSharedMemoryAndMultipleArgumentsSuccess)
{
    test_utils::generateParams(m_params, "foo -a -b foo.c");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_FALSE), Return(true)));

    EXPECT_CALL(m_utils, createSharedMem(_, _))
        .WillOnce(Return(ByMove(std::move(m_uniqueSharedMem))));
    EXPECT_CALL(*m_mockSharedMem, first())
        .WillOnce(Return(false));

    EXPECT_CALL(*m_mockSharedMem, rawData())
        .WillOnce(Return(m_compilerBuffer.data()));
    EXPECT_CALL(*m_mockSharedMem, getSize())
        .WillRepeatedly(Return(m_compilerBuffer.size()));

    m_compiler.doWork(m_params);
}

TEST_F(CompilerTest, JsonInSharedMemorySuccess)
{
    test_utils::generateParams(m_params, "foo bar.c");
    EXPECT_CALL(m_utils, getCwd(_))
        .WillOnce(Return(true));
    EXPECT_CALL(m_utils, getEnvVar(StrEq(Constants::GENCC_OPTIONS), _))
        .WillOnce(DoAll(SetArgReferee<1>(test_utils::JSON_BUILD_FALSE), Return(true)));

    EXPECT_CALL(m_utils, createSharedMem(_, _))
        .WillOnce(Return(ByMove(std::move(m_uniqueSharedMem))));
    EXPECT_CALL(*m_mockSharedMem, first())
        .WillOnce(Return(false));

    strncpy(m_compilerBuffer.data(), test_utils::JSON_DB_GOOD, strlen(test_utils::JSON_DB_GOOD));
    EXPECT_CALL(*m_mockSharedMem, rawData())
        .WillOnce(Return(m_compilerBuffer.data()));
    EXPECT_CALL(*m_mockSharedMem, getSize())
        .WillRepeatedly(Return(m_compilerBuffer.size()));

    m_compiler.doWork(m_params);
}
