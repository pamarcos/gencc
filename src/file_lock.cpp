#include "file_lock.h"

#include <fstream>

FileLock::FileLock(const std::__1::string& filename)
    : m_filename(filename)
{
}

FileLockGuard::FileLockGuard(std::unique_ptr<FileLock> lockFile)
    : m_lockFile(std::move(lockFile))
{
    m_lockFile->createFile();
}

FileLockGuard::~FileLockGuard()
{
    m_lockFile->removeFile();
}

FileLockImpl::FileLockImpl(const std::string& filename)
    : FileLock(filename)
{
}

void FileLockImpl::createFile()
{
    std::ofstream file(m_filename);
}

void FileLockImpl::removeFile()
{
    std::remove(m_filename.c_str());
}
