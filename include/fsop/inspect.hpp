#ifndef FSOP_INSPECT_HPP_INCLUDED
#define FSOP_INSPECT_HPP_INCLUDED

#include <variant>      // std::variant
#include <string_view>  // std::string_view
#include <system_error> // std::system_error

#include <cerrno>       // errno, errno macros
#include <cstring>      // std::strerror

#include <sys/stat.h>   // stat, struct stat
#include <sys/types.h>  // 

namespace fsop
{
    // A variant over stat structures to hold inode information depending on the size requirement.
    using stat_info = std::variant<struct stat, struct stat64>;

    /**
     * @brief Inspect the inode of a file, via the stat system call.
     *
     * This function summarizes available data in the inode, as returned by the
     * stat system call and returns the data as a stat object.
     *
     * @param path Path to the file to inspect.
     * @return {stat_info} Information related to the file,
     *                     in a structure large enough to hold it.
     */
    stat_info inspect_file(std::string_view path);
}

#endif // FSOP_INSPECT_HPP_INCLUDED