#include "fsop/inspect.hpp"
#include "fsop/utilities.hpp"

#include <pwd.h>           // struct passwd, getpwuid
#include <grp.h>           // struct group, getgrgid
#include <sys/sysmacros.h> // major, minor

auto fsop::inspect_file(std::string_view path) -> fsop::stat_info
{
    stat_info inode_data;

    // Use lstat*() instead of stat(0 to prevent resolution of symbolic links.
    int status = lstat(path.data(), &std::get<struct stat>(inode_data));
    if(status == 0) return inode_data;

    // If the failure was due to overflow on the buffer provided to hold the data ...
    if(errno == EOVERFLOW)
    {
        // ... retry using a call to a stat function which returns data in a larger structure.
        status = lstat64(path.data(), &std::get<struct stat64>(inode_data));
        if(status == 0) return inode_data;
    }

    // Throw an exception, indicating the reason for failure.
    std::string error_desc = "inspect_file(): ";
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
    const std::string::size_type spc_w = 30;

    struct passwd* pwd; struct group* grp;
    struct stat64 fileinfo;
    if(std::holds_alternative<struct stat>(info))
    {
        auto _fileinfo = std::get<struct stat>(info);
        fileinfo.st_dev     = _fileinfo.st_dev;
        fileinfo.st_ino     = _fileinfo.st_ino;
        fileinfo.st_mode    = _fileinfo.st_mode;
        fileinfo.st_nlink   = _fileinfo.st_nlink;
        fileinfo.st_uid     = _fileinfo.st_uid;
        fileinfo.st_gid     = _fileinfo.st_gid;
        fileinfo.st_rdev    = _fileinfo.st_rdev;
        fileinfo.st_size    = _fileinfo.st_size;
        fileinfo.st_atime   = _fileinfo.st_atime;
        fileinfo.st_mtime   = _fileinfo.st_mtime;
        fileinfo.st_ctime   = _fileinfo.st_ctime;
        fileinfo.st_blksize = _fileinfo.st_blksize;
        fileinfo.st_blocks  = _fileinfo.st_blocks;
    }
    else fileinfo = std::get<struct stat64>(info);

    os << std::setw(spc_w) << "Inode number"             << " │ " << fileinfo.st_ino << '\n';
    os << std::setw(spc_w) << "Inode link count"         << " │ " << fileinfo.st_nlink << '\n';

    os << std::setw(spc_w) << "Device identifier"        << " │ " << std::hex << std::uppercase
       << std::showbase << fileinfo.st_dev  << " (major/class = " << major(fileinfo.st_dev)
       << ", minor/instance = " << minor(fileinfo.st_dev) << std::dec << ")\n";

    os << std::setw(spc_w) << "File owner (user)"        << " │ ";
    if((pwd = getpwuid(fileinfo.st_uid))) { os << pwd->pw_name << ", "; }
    os << "UID " << fileinfo.st_uid << '\n';
    os << std::setw(spc_w) << "File owner group"         << " │ ";
    if((grp = getgrgid(fileinfo.st_gid))) { os << grp->gr_name << ", "; }
    os << "GID " << fileinfo.st_gid << '\n';

    char file_type = '-';
    os << std::setw(spc_w) << "File type"                << " │ ";
    switch(fileinfo.st_mode & S_IFMT)
    {
        case S_IFBLK : os << "Block device";      file_type = 'b'; break;
        case S_IFCHR : os << "Character device";  file_type = 'c'; break;
        case S_IFDIR : os << "Directory";         file_type = 'd'; break;
        case S_IFIFO : os << "Named pipe / FIFO"; file_type = 'p'; break;
        case S_IFLNK : os << "Symbolic link";     file_type = 'l'; break;
        case S_IFREG : os << "Regular file";      file_type = '-'; break;
        case S_IFSOCK: os << "Socket";            file_type = 's'; break;
    }
    os << '\n';
    mode_t perms = fileinfo.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    os << std::setw(spc_w) << "Access Permissions"       << " │ " << file_type
       << utils::to_permissions(perms) << " (" << std::oct << perms << std::dec << ")\n";

    auto [ f_sz  , f_sz_frc  , f_sz_u   ] = utils::to_human_readable_size(fileinfo.st_size);
    os << std::setw(spc_w) << "File size"                << " │ " << f_sz  ;
    if(f_sz_frc   > 0) { os << '.' << std::setw(2) << std::setfill('0') << f_sz_frc   << std::setfill(' '); }
    os << ' ' << f_sz_u   << "B (" << fileinfo.st_size     << " B)\n";

    auto [ blk_sz, blk_sz_frc, blk_sz_u ] = utils::to_human_readable_size(fileinfo.st_blksize);
    os << std::setw(spc_w) << "Preferred I/O block size" << " │ " << blk_sz;
    if(blk_sz_frc > 0) { os << '.' << std::setw(2) << std::setfill('0') << blk_sz_frc << std::setfill(' '); }
    os << ' ' << blk_sz_u << "B (" << fileinfo.st_blksize  << " B)\n";

    os << std::setw(spc_w) << "# of blocks allocated"    << " │ "
       << ( (fileinfo.st_blocks * 512) / fileinfo.st_blksize ) << " blks. / "
       << fileinfo.st_blocks << " bu (1 bu = 512 B)\n";

    os << std::setw(spc_w) << "Last file access"         << " │ "
       << std::put_time(std::localtime(&fileinfo.st_atime), "%a, %b %e, %Y, at %H:%M:%S %Z") << '\n';
    os << std::setw(spc_w) << "Last status change"       << " │ "
       << std::put_time(std::localtime(&fileinfo.st_ctime), "%a, %b %e, %Y, at %H:%M:%S %Z") << '\n';
    os << std::setw(spc_w) << "Last file modification"   << " │ "
       << std::put_time(std::localtime(&fileinfo.st_mtime), "%a, %b %e, %Y, at %H:%M:%S %Z") << '\n';

    return os;
}