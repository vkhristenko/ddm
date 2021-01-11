#ifndef FileInterfaces_hpp
#define FileInterfaces_hpp

#include <inttypes.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

namespace ddm {

struct Seekable {
    virtual void SeekTo(int64_t const pos) = 0;
    virtual int64_t Pos() const = 0;
};

struct Closeable {
    virtual void Close() = 0;
};

struct Reservable {
    /*
    virtual void Reserve(int64_t const nbytes) = 0;
    virtual void Reserve(int64_t const pos, int64_t const nbytes) = 0;
    */
};

class SourceInterface : public Seekable, public Closeable {
public:
    explicit SourceInterface(std::string const& path) : path_{path} {}

    virtual ~SourceInterface() {}

    virtual void Read(int64_t const nbytes, void *data) = 0;

    virtual void ReadAt(int64_t const pos, int64_t const nbytes, void *data) = 0;

protected:
    std::string path_;
};

class SinkInterface : public Seekable, public Closeable , public Reservable {
public:
    explicit SinkInterface(std::string const& path) : path_{path} {}

    virtual ~SinkInterface() {}

    virtual void Write(int64_t const nbytes, void const* data) = 0;

    virtual void WriteAt(int64_t const pos, int64_t const nbytes, void const *data) = 0;

protected:
    std::string path_;
};

class OutputFile : public SinkInterface {
public:
    explicit OutputFile(std::string const& path)
        : SinkInterface{path}
        , fd_{open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 
            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)}
    {}

    ~OutputFile() override { this->Close(); }

    // FIXME should be the same for both interfaces
    void SeekTo(int64_t const pos) override { lseek(fd_, pos, SEEK_SET); }
    int64_t Pos() const override { return lseek(fd_, 0, SEEK_CUR); }
    
    void Write(int64_t nbytes, void const* data) override { write(fd_, (void*)data, nbytes); }

    void WriteAt(int64_t const pos, int64_t const nbytes, void const* data) override {
        this->SeekTo(pos);
        this->Write(nbytes, data);
    }

    /*
    void Reserve(int64_t const nbytes) override {
        std::vector<uint8_t> buffer(nbytes, 0);
        this->Write(nbytes, buffer.data());
    }

    void Reserve(int64_t const pos, int64_t const nbytes) override {
        std::vector<uint8_t> buffer(nbytes, 0);
        this->WriteAt(pos, nbytes, buffer.data());
    }*/
  
protected:
    void Close() override { close(fd_); }

protected:
    int fd_;
};

class InputFile : public SourceInterface {
public:
    explicit InputFile(std::string const& path) 
        : SourceInterface{path}
        , fd_{open(path_.c_str(), O_RDONLY, 0)}
    {}

    ~InputFile() override { this->Close(); }

    void SeekTo(int64_t const pos) override { lseek(fd_, pos, SEEK_SET); }
    int64_t Pos() const override { return lseek(fd_, 0, SEEK_CUR); }

    void Read(int64_t const nbytes, void *data) override { read(fd_, data, nbytes); }

    void ReadAt(int64_t const pos, int64_t const nbytes, void *data) override {
        this->SeekTo(pos);
        this->Read(nbytes, data);
    }

protected:
    void Close() override { close(fd_); }

protected:
    int fd_;
};

} // namespace ddm

#endif
