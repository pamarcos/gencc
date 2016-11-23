#ifndef FILE_LOCK_H
#define FILE_LOCK_H

#include <memory>
#include <string>

class FileLock {
public:
    FileLock(const std::string& filename);

    virtual void createFile() = 0;
    virtual void removeFile() = 0;

    std::string m_filename;
};

class FileLockGuard {
public:
    FileLockGuard(std::unique_ptr<FileLock> lockFile);
    ~FileLockGuard();

private:
    std::unique_ptr<FileLock> m_lockFile;
};

class FileLockImpl : public FileLock {
public:
    FileLockImpl(const std::string& filename);

    void createFile() override;
    void removeFile() override;
};

#endif // FILE_LOCK_H
