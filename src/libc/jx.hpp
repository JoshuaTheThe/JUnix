
// J/UNIX C++ DEVELOPMENT KIT

#pragma once
#define PROCEDURAL extern "C"

extern "C"
{
        #include <stdio.h>
};

namespace JX
{

template<bool has_seek=true, bool has_write=true, bool has_read=true>
class BasicFile
{
public:
        int fd=-1;
        int flags=0;

        BasicFile(const char *path) { if ((this->fd = ::open((char *)path, flags)) < 0) { ::exit(1); } }
        BasicFile() {}

        int read(void *const data, size_t count)        { if (has_read)  return ::read (this->fd, data, count); else return -1; }
        int write(const void *const data, size_t count) { if (has_write) return ::write(this->fd, data, count); else return -1; }
        int lseek(int off, int whence=SEEK_SET)         { if (has_seek)  return ::lseek(this->fd, off, whence); else return -1; }
};

template<bool has_seek=true, bool has_write=true, bool has_read=true>
class File : public BasicFile<has_seek, has_write, has_read>
{
public:
        File(const char *path) : BasicFile<has_seek, has_write, has_read>(path) { }
        ~File() { ::close(this->fd); }
};

static BasicFile<false> stdin;
static BasicFile<false> stdout;

PROCEDURAL int init_rt(void)
{
        stdout = stdin = BasicFile<false>("/dev/serial");
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
