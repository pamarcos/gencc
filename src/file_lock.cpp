#include "file_lock.h"

#include <fstream>

FileLock::FileLock(const std::string& filename)
    : m_filename(filename)
{
}

FileLockGuard::FileLockGuard(std::unique_ptr<FileLock> lockFile)
    : m_lockFile(std::move(lockFile))
    , m_removeFile(true)
{
    m_lockFile->createFile();
}

FileLockGuard::~FileLockGuard()
{
    if (m_removeFile) {
        m_lockFile->removeFile();
    }
}

FileLock* FileLockGuard::getLockFile() const
{
    return m_lockFile.get();
}

void FileLockGuard::removeFile(bool remove)
{
    m_removeFile = remove;
}

FileLockImpl::FileLockImpl(const std::string& filename)
    : FileLock(filename)
{
}

void FileLockImpl::createFile()
{
    std::ofstream ofs(m_filename);
}

void FileLockImpl::removeFile()
{
    std::remove(m_filename.c_str());
}

bool FileLockImpl::writeToFile(const std::string& from)
{
    std::ofstream ofs(m_filename, std::ios::app);
    if (ofs.good()) {
        ofs << from << '\n';
        ofs.flush();
        return true;
    }
    return false;
}

bool FileLockImpl::readFromFile(std::string& to)
{
    std::ifstream ifs(m_filename);
    if (ifs.good()) {
        ifs.seekg(0);
        std::string line;
        while (!ifs.eof()) {
            std::getline(ifs, line);
            to += line;
        }
        return true;
    }
    return false;
}
