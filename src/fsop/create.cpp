/**
 * @file create.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file implementing the functions defined in fsop/create.hpp
 * @version 1.0
 * @date 2022-05-30
 *
 * @copyright Copyright (c) 2022
 */

#include "fsop/create.hpp"

#include <cerrno>       // errno, errno macros
#include <cstring>      // std::strerror

#include <system_error> // std::system_error

#include <fcntl.h>      // creat (Android)
#include <unistd.h>     // creat (MinGW), mode_t
#include <sys/stat.h>   // S_* constant macros, mknod (Android), chmod (Android)

void fsop::create_file(std::string_view path, mode_t permissions, bool overwrite)
{
    // Check if file previously exists
    struct stat inode_data;
    int status = stat(path.data(), &inode_data);
    if(status != -1)
    {
        if(not overwrite)
        {
            throw std::system_error
            (
                EEXIST, std::generic_category(),
                "create_file(): failed to create new regular file '" +
                std::string(path.data(), path.size()) + "'"
            );
        }
        else if((inode_data.st_mode & S_IFMT) != S_IFREG)
            if(unlink(path.data()) == -1)
                throw std::system_error
                (
                    errno, std::generic_category(),
                    "create_file(): unable to unlink previously existing file for overwrite"
                );
    }
    // Aliter: open(path.data(), O_CREAT | O_WRONLY | O_TRUNC, permissions);
    int fd = creat(path.data(), permissions);
    if(fd == -1 and (not overwrite or errno != EEXIST))
    {
        std::string error_desc = "create_file(): ";
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
    // Verify successful file creation.
    status = fstat(fd, &inode_data);
    if(status == -1)
    {
        close(fd);
        throw std::system_error
        (
            errno, std::generic_category(),
            "create_pipe(): failed to stat created file"
        );
    }
    // Fix permissions.
    if((inode_data.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) != permissions)
    {
        status = fchmod(fd, permissions);
        if(status == -1)
        {
            close(fd);
            throw std::system_error
            (
                errno, std::generic_category(),
                "create_file(): failed to change permissions"
            );
        }
    }
    // close the file, since creat also opens the file for writing.
    close(fd);
}

void fsop::create_pipe(std::string_view path, mode_t permissions, bool overwrite)
{
    // Check if file previously exists
    struct stat inode_data;
    int status = stat(path.data(), &inode_data);
    if(status != -1)
    {
        if(not overwrite)
        {
            throw std::system_error
            (
                EEXIST, std::generic_category(),
                "create_pipe(): failed to create new named pipe '" +
                std::string(path.data(), path.size()) + "'"
            );
        }
        else if((inode_data.st_mode & S_IFMT) != S_IFIFO)
            if(unlink(path.data()) == -1)
                throw std::system_error
                (
                    errno, std::generic_category(),
                    "create_pipe(): unable to unlink previously existing file for overwrite"
                );
    }
    status = mknod(path.data(), S_IFIFO | permissions, 0);
    if(status == -1 and (not overwrite or errno != EEXIST))
    {
        std::string error_desc = "create_pipe(): ";
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
    // Verify successful pipe creation.
    status = stat(path.data(), &inode_data);
    if(status == -1)
    {
        throw std::system_error
        (
            errno, std::generic_category(),
            "create_pipe(): failed to stat created pipe"
        );
    }
    // Fix permissions.
    if((inode_data.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) != permissions)
    {
        status = chmod(path.data(), permissions);
        if(status == -1)
        {
            throw std::system_error
            (
                errno, std::generic_category(),
                "create_pipe(): failed to change permissions"
            );
        }
    }
}