#ifndef FSOP_CREATE_HPP_INCLUDED
#define FSOP_CREATE_HPP_INCLUDED

#include <string_view>  // std::string_view

#include <sys/types.h>  // mode_t

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
     * @param overwrite If true, recreates the file, effectively truncating previous content.
     *
     * @throws {std::system_error} reason for failure of operation.
     */
    void create_file(std::string_view path, mode_t permissions, bool overwrite = false);

    /**
     * @brief Creates a new named pipe (FIFO file) in the filesystem.
     *
     * For successful operation, ensure that the file does not previously exist, all directories along the path exist,
     * there is ample resources for inode and/or disk block allocation and appropriate permissions along all directory
     * components of the path are available.
     *
     * @param path The path at which the file must be created.
     * @param permissions The permissions to associate with the file.
     * @param overwrite If true, recreates the file, effectively truncating previous content.
     *
     * @throws {std::system_error} reason for failure of operation.
     */
    void create_pipe(std::string_view path, mode_t permissions, bool overwrite = false);
}

#endif // FSOP_CREATE_HPP_INCLUDED