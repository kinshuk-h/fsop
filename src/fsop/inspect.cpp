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

std::ostream& fsop::print_stat_info(std::ostream& os, const fsop::stat_info& info)
{
    
    struct passwd *pwd;

if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
    printf(" %-8.8s", pwd->pw_name);
else
    printf(" %-8d", statbuf.st_uid);
    struct group *grp;
if ((grp = getgrgid(statbuf.st_gid)) != NULL)
    printf(" %-8.8s", grp->gr_name);
else
    printf(" %-8d", statbuf.st_gid);
    
    printf("ID of containing device:  [%jx,%jx]\n",
                   (uintmax_t) major(sb.st_dev),
                   (uintmax_t) minor(sb.st_dev));

           printf("File type:                ");

           switch (sb.st_mode & S_IFMT) {
           case S_IFBLK:  printf("block device\n");            break;
           case S_IFCHR:  printf("character device\n");        break;
           case S_IFDIR:  printf("directory\n");               break;
           case S_IFIFO:  printf("FIFO/pipe\n");               break;
           case S_IFLNK:  printf("symlink\n");                 break;
           case S_IFREG:  printf("regular file\n");            break;
           case S_IFSOCK: printf("socket\n");                  break;
           default:       printf("unknown?\n");                break;
           }

           printf("I-node number:            %ju\n", (uintmax_t) sb.st_ino);

           printf("Mode:                     %jo (octal)\n",
                   (uintmax_t) sb.st_mode);

           printf("Link count:               %ju\n", (uintmax_t) sb.st_nlink);
           printf("Ownership:                UID=%ju   GID=%ju\n",
                   (uintmax_t) sb.st_uid, (uintmax_t) sb.st_gid);

           printf("Preferred I/O block size: %jd bytes\n",
                   (intmax_t) sb.st_blksize);
           printf("File size:                %jd bytes\n",
                   (intmax_t) sb.st_size);
           printf("Blocks allocated:         %jd\n",
                   (intmax_t) sb.st_blocks);

           printf("Last status change:       %s", ctime(&sb.st_ctime));
           printf("Last file access:         %s", ctime(&sb.st_atime));
           printf("Last file modification:   %s", ctime(&sb.st_mtime));
}