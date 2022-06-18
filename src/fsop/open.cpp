/**
 * @file open.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file for implementation of static functions for opening files and pipes defined in fsop/file.hpp
 * @version 1.0
 * @date 2022-05-28
 *
 * @copyright Copyright (c) 2022
 */

#include "fsop/file.hpp"

#include <cerrno>       // errno, errno macros

#include <system_error> // std::system_error

#include <fcntl.h>      // open
#include <unistd.h>     // pipe

fsop::File fsop::File::open_file(std::string_view path, int open_options)
{
    // Check if file previously exists
    struct stat64 inode_data;
    int status = stat64(path.data(), &inode_data);
    if(status == -1)
    {
         throw std::system_error
         (
              ENOENT, std::generic_category(),
              "open_file(): failed to open file (possibly non-existent) '" +
              std::string(path.data(), path.size()) + "'"
         );
    }
    int fd = open(path.data(), open_options);
    if(fd == -1)
    {
        std::string error_desc = "open_file(): ";
        switch(errno)
        {
            case ENAMETOOLONG:
                error_desc += "pathname is too long for the pathname buffer";
                break;
            case EPERM:
                error_desc += "restricted operation";
                break;
            case EROFS:
                error_desc += "specified directory component's filesystem is marked as read-only";
                break;
            case EACCES:
                error_desc += "missing access permissions on one of the directories in the path";
                break;
            case EDQUOT:
                error_desc += "user disk block/inode quota exhausted";
                break;
            case EEXIST:
                error_desc += "file already exists in current working directory";
                break;
            case EINVAL:
                error_desc += "invalid characters in filename";
                break;
            case ENFILE:
                error_desc += "system quota for total open files exhausted";
                break;
            case ENOENT:
                error_desc += "non-existent directory components in path";
                break;
            default:
                error_desc += "unknown, code " + std::to_string(errno);
        }
        throw std::system_error(errno, std::generic_category(), error_desc.c_str());
    }
    return { fd, inode_data };
}

std::pair<fsop::File, fsop::File> fsop::File::create_unnamed_pipe()
{
    int pipefd[2];
    int status = pipe(pipefd);

    if(status == -1)
    {
        std::string error_desc = "create_unnamed_pipe(): ";
        switch(errno)
        {
            case EMFILE:
                error_desc += (
                    "process limit for open file "
                    "descriptors exhausted (FD table full)"
                );
                break;
            case ENFILE:
                error_desc += (
                    "system quota for open files or user "
                    "limit for memory allocatable for pipes reached"
                );
                break;
        }
        throw std::system_error
        ( errno, std::generic_category(), error_desc );
    }

    return std::make_pair(File{ pipefd[0] }, File{ pipefd[1] });
}