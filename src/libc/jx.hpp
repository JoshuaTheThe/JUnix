
// J/UNIX C++ DEVELOPMENT KIT

#pragma once
#define PROCEDURAL extern "C"

extern "C"
{
        #include <stdio.h>
};

namespace JX
{

template<typename T, bool has_seek=true, bool has_write=true, bool has_read=true>
class BasicFile
{
public:
        int fd=-1;
        int mode=0,flags=0;

        BasicFile(const char *path) { this->fd = ::open3((char *)path, flags, mode); }
        BasicFile() {}

        size_t read(T *const data, size_t count)        { if (has_read)  return ::read (this->fd, data, count * sizeof(*data)); else return -1; }
        size_t write(const T *const data, size_t count) { if (has_write) return ::write(this->fd, data, count * sizeof(*data)); else return -1; }
        int    lseek(int off, int whence=SEEK_SET)      { if (has_seek)  return ::lseek(this->fd, off, whence);                 else return -1; }
};

template<typename T, bool has_seek=true, bool has_write=true, bool has_read=true>
class File : public BasicFile<T, has_seek, has_write, has_read>
{
public:
        File(const char *path) : BasicFile<T>(path) { }
        ~File() { ::close(this->fd); }
};

BasicFile<char, false> stdin;
BasicFile<char, false> stdout;
BasicFile<char, false> stderr;

PROCEDURAL int init_rt(void)
{
        stdin  = BasicFile<char, false>("/dev/serial");
        stdout = BasicFile<char, false>("/dev/serial");
        stderr = BasicFile<char, false>("/dev/serial");
        return 0;
}

PROCEDURAL int init_memory(void)
{
        return 0;
}

PROCEDURAL int shutdown(void)
{
        return 0;
}

};
