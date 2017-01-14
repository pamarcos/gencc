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

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <vector>

namespace test_utils {

static constexpr char const* JSON_GOOD = "{\"foo\":true}";
static constexpr char const* JSON_BUILD_TRUE = "{\"build\":true}";
static constexpr char const* JSON_BUILD_FALSE = "{\"build\":false}";

void generateParams(std::vector<std::string>& vec, const std::string& str = "");
}

#endif // TEST_UTILS_H
