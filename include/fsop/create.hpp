#ifndef FSOP_CREATE_HPP_INCLUDED
#define FSOP_CREATE_HPP_INCLUDED

#include <string_view>  // std::string_view
#include <system_error> // std::system_error

#include <cerrno>    // errno, strerror
#include <unistd.h>  // creat, mode_t
#include <fcntl.h>

namespace fsop
{
    /**
     * @brief Creates a new regular file in the filesystem.
     *
     * For successful operation, ensure that the file does not previously exist, all directories along the path exist,
     * there is ample resources for inode and/or disk block allocation and appropriate permissions along all directory
     * components of the path are available.
     *
     * @param path The path at which the file must be created.
     * @param permissions The permissions to associate with the file.
     *
     * @throws {std::system_error} reason for failure of operation.
     */
    void create_file(std::string_view path, mode_t permissions);

    /**
     * @brief Creates a new named pipe (FIFO file) in the filesystem.
     *
     * For successful operation, ensure that the file does not previously exist, all directories along the path exist,
     * there is ample resources for inode and/or disk block allocation and appropriate permissions along all directory
     * components of the path are available.
     *
     * @param path The path at which the file must be created.
     * @param permissions The permissions to associate with the file.
     *
     * @throws {std::system_error} reason for failure of operation.
     */
    void create_pipe(std::string_view path, mode_t permissions);
}

#endif // FSOP_CREATE_HPP_INCLUDED