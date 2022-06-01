#ifndef FSOP_UTILITIES_HPP_INCLUDED
#define FSOP_UTILITIES_HPP_INCLUDED

#include <string>      // std::to_string
#include <stdexcept>   // std::invalid_argument
#include <string_view> // std::string_view

#include <sys/stat.h>  // S_I* constants.
#include <sys/types.h> // mode_t

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

    static constexpr const char* filesize_units[7] = { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei" };

    // TODO: Add docs
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

    
    std::string current_directory();
    
    void change_directory(std::string_view path);
}

#endif // FSOP_UTILITIES_HPP_INCLUDED