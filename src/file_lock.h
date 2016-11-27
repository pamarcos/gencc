#ifndef FILE_LOCK_H
#define FILE_LOCK_H

#include <fstream>
#include <memory>
#include <string>

class FileLock {
public:
    FileLock(const std::string& filename);

    virtual void createFile() = 0;
    virtual void removeFile() = 0;
    virtual bool writeToFile(const std::string& from) = 0;
    virtual bool readFromFile(std::string& to) = 0;

    std::string m_filename;
};

class FileLockGuard {
public:
    FileLockGuard(std::unique_ptr<FileLock> lockFile);
    ~FileLockGuard();

    FileLock* getLockFile() const;
    void removeFile(bool remove);

private:
    std::unique_ptr<FileLock> m_lockFile;
    bool m_removeFile;
};

class FileLockImpl : public FileLock {
public:
    FileLockImpl(const std::string& filename);

    void createFile() override;
    void removeFile() override;
    bool writeToFile(const std::string& from) override;
    bool readFromFile(std::string& to) override;
};

#endif // FILE_LOCK_H
