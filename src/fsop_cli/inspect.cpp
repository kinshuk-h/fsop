/**
 * @file inspect.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file containing implementation of the inspect
 *        function declared in 'fsop_cli.hpp'.
 * @version 1.0
 * @date 2022-06-05
 *
 * @copyright Copyright (c) 2022
 */

#include "fsop_cli.hpp"     // Base header containing function declaration for 'inspect'

#include <iostream>         // std::cout, std::cerr

#include "fsop/inspect.hpp" // fsop::inspect_file, fsop::print_stat_info

int fsop_cli::inspect(const argparse::types::result_map& args, std::string_view program_name)
{
    auto paths            = std::any_cast<std::vector<std::string>>(args.at("path"));
    auto quiet            = std::any_cast<bool>                    (args.at("quiet"));
    auto follow_symlinks  = std::any_cast<bool>                    (args.at("follow-symlinks"));

    for(const auto& path : paths)
    {
        try
        {
            if(not quiet)
                std::cout << program_name << ": trying to inspect '" << path << "' ... ";
            auto information = fsop::inspect_file(path, follow_symlinks);
            if(not quiet)
                std::cout << "done\n";
            fsop::print_stat_info(std::cout, information) << '\n';
        }
        catch(std::system_error& error)
        {
            if(not quiet) std::cout << "error\n";
            std::cerr << program_name << ": error: " << error.what() << "\n\n";
        }
    }
    return EXIT_SUCCESS;
}