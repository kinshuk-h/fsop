#include <iostream>

#include "argparse.hpp"

using namespace argparse::arguments;

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
mode_t parse_permissions(std::string_view perms)
{
    mode_t permissions = 0;
    // Parse permissions given as a 3 digit octal value.
    if(perms[0] == '0')
    {
        if(perms.size() != 4)
            throw std::invalid_argument("parse_permissions(): invalid length for octal permissions");
        if(perms[1] < '0' or perms[1] > '7')
            throw std::invalid_argument("parse_permissions(): invalid digit in octal permissions");
        permissions |= (perms[1] - '0'); // Add permissions for user level.
        permissions <<= 3;               // Move bits to the left for the next level.
        if(perms[2] < '0' or perms[2] > '7')
            throw std::invalid_argument("parse_permissions(): invalid digit in octal permissions");
        permissions |= (perms[2] - '0'); // Add permissions for group level.
        permissions <<= 3;               // Move bits to the left for the next level.
        if(perms[3] < '0' or perms[3] > '7')
            throw std::invalid_argument("parse_permissions(): invalid digit in octal permissions");
        permissions |= (perms[3] - '0'); // Add permissions for other level.
    }
    // Parse permissions given as a combination of user scope and mode, joined using a +.
    else if(auto pos = perms.find('+'); pos != std::string_view::npos)
    {
        auto groups = perms.substr(0, pos);
        if(groups.empty()) groups = "u";

        perms = perms.substr(pos+1);
        mode_t perm_bits = 0;
        for (char perm : perms)
        {
            switch(perm)
            {
                case 'r': perm_bits |= 4; break;
                case 'w': perm_bits |= 2; break;
                case 'x': perm_bits |= 1; break;
                default:
                    throw std::invalid_argument
                    (
                        "parse_permissions(): invalid value for permission, "
                        "must be one of r,w or x."
                    );
            }
        }
        for(char grp : groups)
        {
            switch(grp)
            {
                case 'u': permissions |= (perm_bits << 6); break;
                case 'g': permissions |= (perm_bits << 3); break;
                case 'o': permissions |= perm_bits; break;
                default:
                    throw std::invalid_argument
                    (
                        "parse_permissions(): invalid value for permission group, "
                        "must be one of u,g or o."
                    );
            }
        }
    }
    // Parse permissions given as a 9-letter string containing either of r,w,x or -.
    else
    {
        if(perms.size() != 9)
            throw std::invalid_argument("parse_permissions(): invalid length for permissions");
        for(int i=0; i<9; ++i)
        {
            char expected_value;
            switch(i % 3)
            {
                case 0: expected_value = 'r'; break;
                case 1: expected_value = 'w'; break;
                case 2: expected_value = 'x'; break;
            }
            if(perms[i] == '-' or perms[i] == expected_value)
            {
                if(perms[i] != '-')
                {
                    // bit to indicate permission to grant (read, write or execute).
                    mode_t perm_bit = (1 << (2 - (i % 3)));
                    // permission group to set permission bit in (user, group or other).
                    mode_t perm_grp = (2 - (i / 3)) * 3;
                    // Set permission bit by shifting to appropriate permission group.
                    permissions |= (perm_bit << perm_grp);
                }
            }
            else
                throw std::invalid_argument
                (
                    "parse_permissions(): invalid value at position " +
                    std::to_string(i+1) + " (expected - or " +
                    std::to_string(expected_value) + ")"
                );
        }
    }
    return permissions;
}

// #include <bitset>

int main(int argc, const char** argv)
{
    argparse::Parser parser { name = argv[0] };
    // parser.add(
        
    // );
    auto arg = argparse::Switch
    {
        name = "debug", alias = "d", negated = true,
        help = "enable debug mode for systems, effectively enabling logging "
               "and display of other verbose information"
    };
    std::cout << "usage: " << arg.usage() << "\n";
    std::cout << arg.descriptor() << "\n";

    // std::string perms;
    // while(true)
    // {
    //     try
    //     {
    //         std::cout << "> "; std::cin >> perms;
    //         if(perms == "exit") return 0;
    //         std::cout << std::bitset<9>(parse_permissions(perms)) << "\n";
    //     }
    //     catch(std::exception& error)
    //     {
    //         std::cout << "error: " << error.what() << "\n";
    //     }
    // }

    //auto args = parser.parse_args(argc, argv);

    //std::cout << args << "\n";
}