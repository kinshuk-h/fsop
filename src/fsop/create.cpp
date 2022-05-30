#include "fsop/create.hpp"

extern "C" {
   int mknod(const char *pathname, mode_t mode, dev_t dev);
}

void fsop::create_file(std::string_view path, mode_t permissions)
{
    int fd = creat(path.data(), permissions);
    if(fd == -1)
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
            // case EEXIST:
            //     error_desc += "file already exists in current working directory";
            //     break;
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
                error_desc += "unknown, code " + std::to_string(errno) + ": " + strerror(errno);
        }
        throw std::system_error(errno, std::generic_category(), error_desc.c_str());
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
//             //     break;
//             // case EDQUOT:
//             //     error_desc += "user disk block/inode quota exhausted";
//             //     break;
//             // case EEXIST:
//             //     error_desc += "file already exists in current working directory";
//             //     break;
//             // case EINVAL:
//             //     error_desc += "invalid characters in filename";
//             //     break;
//             // case ENFILE:
//             //     error_desc += "system quota for total open files exhausted";
//             //     break;
//             // case ENOENT:
//             //     error_desc += "non-existent directory components in path";
//             //     break;
            default:
                error_desc += "unknown, code " + std::to_string(errno) + ": " + strerror(errno);
        }
        throw std::system_error(errno, std::generic_category(), error_desc.c_str());
    }
}