/**
 * @file inspect.hpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Defines functions for abstracting system calls for viewing file information.
 * @version 1.0
 * @date 2022-05-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FSOP_INSPECT_HPP_INCLUDED
#define FSOP_INSPECT_HPP_INCLUDED

#include <variant>      // std::variant
#include <iomanip>      // std::put_time
#include <iostream>     // std::ostream, std::cout
#include <string_view>  // std::string_view
#include <system_error> // std::system_error

#include <cerrno>       // errno, errno macros
#include <cstring>      // std::strerror

#include <pwd.h>           // getpwuid
//#include <group.h>         // getgrgid
#include <sys/stat.h>      // stat, struct stat
#include <sys/types.h>     // 
// #include <sys/sysmacros.h> // major, minor

namespace fsop
{
    /**
     * @brief A variant over stat structures to hold inode
     *  information depending on the size requirement.
     *
     */
    using stat_info = std::variant<struct stat, struct stat64>;

    /**
     * @brief Inspect the inode of a file, via the stat system call.
     *
     * This function summarizes available data in the inode, as returned by the
     * stat system call and returns the data as a stat object.
     *
     * @param path Path to the file to inspect.
     * @param follow_symlinks If true, uses stat instead of lstat to resolve symbolic links.
     * @return {stat_info} Information related to the file,
     *                     in a structure large enough to hold it.
     */
    stat_info inspect_file(std::string_view path, bool follow_symlinks = false);

    /**
     * @brief Prints information received by a call to {inspect_file}
     *  to a given output stream. Displayed information is presented
     *  in a user friendly manner.
     *
     * @param os The output stream to write content to.
     * @param info The structure object containing information to display.
     * @return {std::ostream&} Reference to the output stream for cascading operations.
     */
    std::ostream& print_stat_info(std::ostream& os, const stat_info& info);
}

#endif // FSOP_INSPECT_HPP_INCLUDED