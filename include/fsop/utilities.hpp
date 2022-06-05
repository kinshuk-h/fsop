/**
 * @file utilities.hpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Defines utility functions for use with the fsop module's functions.
 * @version 1.0
 * @date 2022-05-28
 *
 * @copyright Copyright (c) 2022
 */

#ifndef FSOP_UTILITIES_HPP_INCLUDED
#define FSOP_UTILITIES_HPP_INCLUDED

#include <tuple>       // std::make_tuple
#include <string>      // std::to_string
#include <string_view> // std::string_view

/**
 * @brief Defines common utilities for use with filesystem operations.
 *
 */
namespace fsop::utils
{
    /**
     * @brief Parses a given permission string into an integral value with bits set as per given permissions.
     *
     *      This function allows parsing strings of the following forms:
     *      - Octal 3-digit number beginning with 0 and digits indicating set bits.
     *      - Combination of letters indicating permission groups (u,g,o for user, group and others), with 
     *      letters r,w & x for permissions, separated by +.
     *      - 9 letter string containing r,w,x or - values denoting read, write and execute permissions.
     *
     * @param perms The string containing permissions to be parsed.
     * @return mode_t The permission bits indicating requested permissions.
     */
    mode_t parse_permissions(std::string_view perms);

    /**
     * @brief Returns a parseable string representation of a given numerical permissions entry.
     *
     * @param permissions The numeric permissions (as a mode_t variable).
     * @return {std::string} The 9-letter string representation of the permissions.
     */
    std::string to_permissions(mode_t permissions);

    /**
     * @brief Parses the value of whence for offset movement.
     *
     * @param whence String containing the relative offset base.
     * @return int Numeric constant corresponding to the given whence.
     */
    int parse_seek_whence(std::string_view whence);

    /**
     * @brief Returns the name of the file type corresponding to
     *        the file type number stored in the inode.
     *
     * @param type Type number of the file, from the inode.
     * @return {std::string} Name of the type.
     */
    std::string to_type(mode_t type);

    /**
     * @brief IEC unit prefixes for file sizes, in order of increasing size by power of 2s.
     */
    static constexpr const char* filesize_units[7] = { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei" };

    /**
     * @brief Converts a given file size in bytes to a human readable form.
     *
     * This function performs successive division of the file size until a
     * normal value is obtained, and returns a tuple of the reduced size, the
     * rounded off fractional size component and the unit prefix to use.
     *
     * @tparam Integral Type representing an integral value.
     *
     * @param byte_size Integral file size, in bytes.
     * @return {auto} tuple of 3 values: the reduced size, the rounded off fractional
     *      component and the unit prefix.
     */
    template<
        typename Integral,
        typename = std::enable_if_t<std::is_integral_v<Integral>, std::nullptr_t>
    >
    auto to_human_readable_size(Integral byte_size)
    {
        unsigned index = 0; Integral power = 0;
        while((byte_size >> power) > 512) { power += 10; ++index; }

        Integral size = (byte_size >> power), fraction = (byte_size & ((1 << power) - 1));
        fraction = (fraction * 1000) >> power;

        int dgt = fraction % 10; fraction /= 10;
        if(dgt > 4) { ++fraction; }
        if(fraction > 99) { fraction = 0; ++size; }

        return std::make_tuple(size, fraction, filesize_units[index]);
    }

    /**
     * @brief Returns the current working directory of the process.
     * Internally invokes the `getcwd()` syscall.
     *
     * @return {std::string} The current working directory.
     * @throws {std::system_error} Describes the reason for failure.
     */
    std::string current_directory();
    /**
     * @brief Sets the current working directory to the path indicated
     * by the given argument. Internally invokes the `chdir` syscall.
     *
     * @param path New path to use as the current working directories.
     * @throws {std::system_error} Describes the reason for failure.
     */
    void change_directory(std::string_view path);
}

#endif // FSOP_UTILITIES_HPP_INCLUDED