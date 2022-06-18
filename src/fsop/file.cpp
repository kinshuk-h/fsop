/**
 * @file file.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file implementing the methods defined in fsop/file.hpp
 * @version 1.0
 * @date 2022-05-30
 *
 * @copyright Copyright (c) 2022
 */

#include "fsop/file.hpp"

#include <cerrno>       // errno, errno macros

#include <system_error> // std::system_error

#include <unistd.h>     // read, write, close, dup

fsop::File::File(const File& file_ref)
{
    _descriptor = dup(file_ref._descriptor);
    if(_descriptor == invalid_descriptor)
        throw std::system_error(errno, std::generic_category(), "failed to copy file descriptor");
}

fsop::File& fsop::File::operator=(const File& file_ref)
{
    _descriptor = dup(file_ref._descriptor);
    if(_descriptor == invalid_descriptor)
        throw std::system_error(errno, std::generic_category(), "failed to copy file descriptor");
    return *this;
}
fsop::File& fsop::File::operator=(File&& tmp_file)
{
    _descriptor = tmp_file._descriptor;
    tmp_file._descriptor = invalid_descriptor;
    return *this;
}

std::string fsop::File::read(size_t nbytes, off64_t offset, int whence)
{
    if(whence != SEEK_CUR or offset != 0)
    {
        auto new_offset = lseek64(_descriptor, offset, whence);
        if(new_offset == -1)
        {
            std::string error_desc = "read(): ";
            switch (errno)
            {
            case ESPIPE:
                error_desc += "seeking on a pipe/FIFO or socket is not allowed";
                break;
            default:
                break;
            }
            throw std::system_error(errno, std::generic_category(), error_desc);
        }
    }
    std::string data; ssize_t nbytes_read;
    if(nbytes != bytes_till_end)
    {
        data.resize(nbytes);
        nbytes_read = ::read(_descriptor, data.data(), nbytes);
        if(nbytes_read == 0)
        {
            throw std::system_error
            (
                errno, std::generic_category(),
                "read(): offset pointer at end of file (EOF)"
            );
        }
        else if(nbytes_read == -1)
        {
            throw std::system_error
            (
                errno, std::generic_category(),
                "read(): failed to read file content"
            );
        }
        else if(static_cast<size_t>(nbytes_read) < nbytes)
            data.resize(nbytes_read);
    }
    else
    {
        char buffer[1024];
        while((nbytes_read = ::read(_descriptor, buffer, sizeof(buffer))) )
        {
            if(nbytes_read == -1)
            {
                throw std::system_error
                (
                    errno, std::generic_category(),
                    "read(): failed to read file content"
                );
            }
            data.append(buffer, nbytes_read);
            if(static_cast<size_t>(nbytes_read) < sizeof(buffer)) break;
        }
    }
    return data;
}

size_t      fsop::File::write(std::string_view data, off64_t offset, int whence)
{
    if(whence != SEEK_CUR or offset != 0)
    {
        auto new_offset = lseek64(_descriptor, offset, whence);
        if(new_offset == -1)
        {
            std::string error_desc = "write(): ";
            switch (errno)
            {
            case ESPIPE:
                error_desc += "seeking on a pipe/FIFO or socket is not allowed";
                break;
            default:
                break;
            }
            throw std::system_error(errno, std::generic_category(), error_desc);
        }
    }
    auto nbytes_written = ::write(_descriptor, data.data(), data.size());
    if(nbytes_written == -1)
        throw std::system_error
        (
            errno, std::generic_category(),
            "write(): failed to write content to file"
        );
    else return nbytes_written;
}

struct stat64 fsop::File::stat()
{
    if(not _info_set)
    {
        if(fstat64(_descriptor, &_info) == -1)
        {
            throw std::system_error(errno, std::generic_category(), "stat(): failed to stat file");
        }
        _info_set = true;
    }
    return _info;
}

void fsop::File::close() noexcept
{
    if(_descriptor > 2)
    {
        ::close(_descriptor);
        _descriptor = invalid_descriptor;
    }
}