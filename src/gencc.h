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

#ifndef GENCC_H
#define GENCC_H

#include "common.h"
#include <memory>
#include <string>
#include <vector>

class Gencc final : public Common {
public:
    Gencc(Helper* helper);
    int init(std::vector<std::string>& params);

    void setWorker(std::unique_ptr<GenccWorker>& worker);

private:
    bool parseArgs(std::vector<std::string>& params);
    void help();
    std::unique_ptr<GenccWorker> m_worker;
    GenccOptions m_genccOptions;
};

#endif // GENCC_H
