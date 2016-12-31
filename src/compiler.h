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

#ifndef COMPILER_H
#define COMPILER_H

#include "common.h"

class Compiler : public Common, public GenccWorker {
public:
    Compiler(GenccOptions* options, Helper* helper);
    void doWork(const std::vector<std::string>& params) override;

protected:
    void writeCompilationDb() const;
    void fallback(unsigned retries) const;

    std::string m_directory;
    std::string m_command;
    std::string m_file;
};

#endif // COMPILER_H
