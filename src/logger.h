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

#ifndef LOGGER_H
#define LOGGER_H

#include <cstdarg>
#include <cstdio>

class Logger {
public:
    static Logger& getInstance()
    {
        static Logger logger;
        return logger;
    }

    void log(const char* fmt, ...) const
    {
        if (m_enable) {
            va_list vl;
            va_start(vl, fmt);
            vprintf(fmt, vl);
            va_end(vl);
        }
    }

    bool isEnabled() const
    {
        return m_enable;
    }

    void enable()
    {
        m_enable = true;
    }

    void disable()
    {
        m_enable = false;
    }

private:
    Logger()
        : m_enable(true)
    {
    }

    bool m_enable;
};

#define LOG(...) Logger::getInstance().log(__VA_ARGS__)

#endif // LOGGER_H
