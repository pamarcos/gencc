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

    void log(const char* fmt, ...)
    {
        if (m_enable) {
            va_list vl;
            va_start(vl, fmt);
            vprintf(fmt, vl);
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
