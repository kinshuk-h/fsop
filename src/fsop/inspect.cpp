#include "fsop/inspect.hpp"

auto fsop::inspect_file(std::string_view path) -> fsop::stat_info
{
    stat_info inode_data;

    int status = stat(path.data(), &std::get<struct stat>(inode_data));
    if(status == 0) return inode_data;

    // If the failure was due to overflow on the buffer provided to hold the data ...
    if(errno == EOVERFLOW)
    {
        // ... retry using a call to a stat function which returns data in a larger structure.
        status = stat64(path.data(), &std::get<struct stat64>(inode_data));
        if(status == 0) return inode_data;
    }

    // Throw an exception, indicating the reason for failure.
    std::string error_desc = "create_file(): ";
    switch(errno)
    {
        case EACCES:
            error_desc += (
                "missing search (execute) permissions on "
                "one of the directories in the path"
            );
            break;
        case ENAMETOOLONG:
            error_desc += "pathname is too long for the pathname buffer";
            break;
        case ENOENT:
            error_desc += "non-existent directory components in path";
            break;
        case ENOMEM:
            error_desc += "out of kernel memory";
            break;
        case ENOTDIR:
            error_desc += "one of the path components in the path is not a directory";
            break;
        case EPERM:
            error_desc += "restricted operation";
            break;
        case EROFS:
            error_desc += "specified directory component's filesystem is marked as read-only";
            break;
        case EINVAL:
            error_desc += "invalid characters in filename";
            break;
        default:
            error_desc += "unknown, code " + std::to_string(errno);
    }
    throw std::system_error(errno, std::generic_category(), error_desc.c_str());
}