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

#include "test_utils.h"
#include "gtest/gtest.h"

namespace utils {

void generateParams(std::vector<std::string>& vec, const std::string& str)
{
    vec.clear();
    if (str.empty()) {
        return;
    }
    size_t lastPos = 0;
    while (lastPos != std::string::npos) {
        size_t pos = str.find(' ', lastPos);
        vec.emplace_back(str.substr(lastPos, pos - lastPos));
        lastPos = (pos != std::string::npos) ? pos + 1 : pos;
    }
}

TEST(Utils, generateParams)
{
    std::vector<std::string> vec;
    generateParams(vec, "onetwo threefour fivesix seveneight nineten");
    std::vector<std::string> checkParams = { "onetwo", "threefour", "fivesix", "seveneight", "nineten" };
    EXPECT_EQ(vec, checkParams);
}
}
