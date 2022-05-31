#include "fsop/create.hpp"

extern "C" {
   int mknod(const char *pathname, mode_t mode, dev_t dev);
}

void fsop::create_file(std::string_view path, mode_t permissions)
{
    // Check if file previously exists
    struct stat inode_data;
    int status = stat(path.data(), &inode_data);
    if(status != -1)
    {
         throw std::system_error
         (
              EEXIST, std::generic_category(),
              "create_file(): failed to create new regular file '" +
              std::string(path.data(), path.size()) + "'"
         );
    }
    int fd = creat(path.data(), permissions);
    if(fd == -1)
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
    status = fstat(fd, &inode_data);
    if(status == -1)
    {
        throw std::system_error
        (
            errno, std::generic_category(),
            "create_pipe(): failed to stat created file"
        );
    }
    // Fix permissions.
    if(inode_data.st_mode != permissions)
    {
        status = fchmod(fd, permissions);
        if(status == -1)
        {
            throw std::system_error
            (
                errno, std::generic_category(),
                "create_pipe(): failed to change permissions"
            );
        }
    }
    // close the file, since creat also opens the file for writing.
    close(fd);
}

void fsop::create_pipe(std::string_view path, mode_t permissions)
{
    int status = mknod(path.data(), S_IFIFO | permissions, 0);
    if(status == -1)
    {
        std::string error_desc = "create_file(): ";
        switch(errno)
        {
            // case EACCES:
            //     error_desc += "missing access permissions one of the directories in the path";
            //     break;
            // case EDQUOT:
            //     error_desc += "user disk block/inode quota exhausted";
            //     break;
            case EEXIST:
                error_desc += "file already exists in current working directory";
                break;
            // case EINVAL:
            //     error_desc += "invalid characters in filename";
            //     break;
            // case ENFILE:
            //     error_desc += "system quota for total open files exhausted";
            //     break;
            // case ENOENT:
            //     error_desc += "non-existent directory components in path";
            //     break;
            default:
                error_desc += "unknown, code " + std::to_string(errno) + ": ";
        }
        throw std::system_error(errno, std::generic_category(), error_desc.c_str());
    }
}