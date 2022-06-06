/**
 * @file file.hpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Defines a structure for abstracting I/O operations via system calls.
 * @version 1.0
 * @date 2022-06-03
 *
 * @copyright Copyright (c) 2022
 */

#ifndef FSOP_FILE_HPP_INCLUDED
#define FSOP_FILE_HPP_INCLUDED

#include <cstddef>     // std::size_t

#include <string>      // std::string
#include <utility>     // std::pair
#include <string_view> // std::string_view

#include <fcntl.h>     // O_* constants
#include <sys/stat.h>  // stat64
#include <sys/types.h> // off64_t

namespace fsop
{
    /**
     * @brief Defines a structure for abstracting I/O operations via system calls.
     *
     * File objects work over a file descriptor referencing a global file table entry maintaining
     * I/O mode and offset. They follow the RAII idiom and perform auto-closing of the file table entry
     * when the corresponding object runs out of scope. Descriptor ownership can however be transferred
     * to other objects through move construction or move assignment.
     */
    struct File
    {
        /** Constant to represent an invalid file descriptor. */
        static constexpr int    invalid_descriptor = -1;
        /** Constant to specify infinite byte count (read until end of file) */
        static constexpr size_t bytes_till_end     = -1ULL;

        /**
         * @brief Construct a new File object
         *
         * @param descriptor File descriptor to maintain to refer a global file table entry
         *                   corresponding to an open file.
         */
        File(int descriptor) : _descriptor(descriptor) {}
        /**
         * @brief Construct a new File object
         *
         * @param descriptor File descriptor to maintain to refer a global file table entry
         *                   corresponding to an open file.
         * @param inode_info File data as available from a call to stat.
         */
        File(int descriptor, struct stat64 inode_info)
        : _descriptor(descriptor), _info(inode_info), _info_set(true) {}
        /**
         * @brief Construct a new File object
         *
         * @param file_ref File object reference maintaining a file descriptor.
         *                 The descriptor is duped to increase the effective reference count
         *                 of the global file table entry.
         */
        File(const File& file_ref);
        /**
         * @brief Construct a new File object
         *
         * @param tmp_file Temporary file object holding an active file descriptor.
         *                 State is transferred to the current object without affecting
         *                 reference counts of the file table entry.
         */
        File(File&& tmp_file)
        : _descriptor(tmp_file._descriptor), _info(tmp_file._info),
          _info_set(tmp_file._info_set)
        {
            tmp_file._descriptor = invalid_descriptor;
            tmp_file._info_set = false;
        }
        /**
         * @brief Destroy the File object, after closing the associated file descriptor.
         */
        ~File()
        {
            this->close();
        }

        /**
         * @brief Copy assignment: Copies (dupes) the file object's descriptor to the current object.
         *
         * @param file_ref File object reference maintaining a file descriptor.
         *                 The descriptor is duped to increase the effective reference count
         *                 of the global file table entry.
         * @return {File&} Reference to the current object, for chaining operations.
         */
        File& operator=(const File& file_ref);
        /**
         * @brief Move assignment: Moves the file descriptor, effectively transferring ownership.
         * 
         * @param tmp_file Temporary file object holding an active file descriptor.
         *                 State is transferred to the current object without affecting
         *                 reference counts of the file table entry.
         * @return {File&} Reference to the current object, for chaining operations.
         */
        File& operator=(File&& tmp_file);

        /**
         * @brief Reads data as a sequence of characters from the file referenced by the object.
         *        Bytes are read as-is, so some bytes may not be valid printable characters,
         *        depending on the nature of the file content.
         *
         * @param nbytes Number of bytes to read, defaulting to the index of the last byte of the file.
         * @param offset Byte offset to begin reading content from (default=0).
         * @param whence Offset base for movement reference along the file, can be one of
         *               SEEK_SET, SEEK_CUR or SEEK_END (default=SEEK_CUR).
         *
         * @return {std::string} A string containing the read content.
         */
        std::string read(size_t nbytes = bytes_till_end, off64_t offset = 0, int whence = SEEK_CUR);
        /**
         * @brief Writes byte data to the file referenced by the object,
         *        optionally at the specified offset location.
         *
         * @param data Data to write, can be a std::string or a c-string (any type convertible to std::string_view).
         * @param offset Byte offset to begin writing content to (default=0).
         * @param whence Offset base for movement reference along the file, can be one of
         *               SEEK_SET, SEEK_CUR or SEEK_END (default=SEEK_CUR).
         * @return {size_t} Number of bytes actually written to the file.
         */
        size_t      write(std::string_view data, off64_t offset = 0, int whence = SEEK_CUR);

        /**
         * @brief Closes the file referenced by the internal descriptor,
         * thereby moving it to an invalid state for future read and write operations.
         *
         */
        void close() noexcept;

        /**
         * @brief Returns information about the file as available in the file's inode.
         * Internally calls fstat and caches the result.
         *
         * @return {struct stat64} Object describing the file information, possibly cached.
         */
        struct stat64 stat();

        /**
         * @brief Invalidates the cached file information, forcing a stat call.
         */
        void invalidate_cache() { _info_set = false; }

        /**
         * @brief Opens a file for performing I/O.
         *
         * @param path Path to the file to open.
         * @param open_options Options to specify how to open the file.
         * @return {File} The reference to the file descriptor, wrapped in an auto-closeable File object.
         */
        static File open_file(std::string_view path, int open_options = O_RDONLY);

        /**
         * @brief Creates a temporary, process-specific, unnamed pipe for performing I/O.
         *
         * Unnamed pipes allow for sequential streaming of data in a first-in,
         * first-out manner, from a writer to a reader. This is useful for communication
         * between processes.
         *
         * @return {std::pair<File, File>} Pair of File instances corresponding to the reader and writer respectively.
         */
        static std::pair<File, File> create_unnamed_pipe();

        /** Implicit casting operator: determines truthness based on the descriptor. */
        operator bool() { return _descriptor != invalid_descriptor; }
    private:
        /** Internal descriptor maintained by the object. */
        int _descriptor = -1;
        /** Cached file information (inode data). */
        struct stat64 _info;
        /** Indicates whether the file data was cached */
        bool _info_set = false;
    };

    inline constexpr int File::invalid_descriptor;
    inline constexpr size_t File::bytes_till_end;
}

#endif // FSOP_FILE_HPP_INCLUDED