#include "fsop_cli.hpp"     // Base header containing function declaration for 'inspect'

#include <iostream>         // std::cout, std::cerr

#include "fsop/inspect.hpp" // fsop::inspect_file, fsop::print_stat_info

int fsop_cli::inspect(const argparse::types::result_map& args, std::string_view program_name)
{
    auto paths  = std::any_cast<std::vector<std::string>>(args.at("path"));
    auto quiet  = std::any_cast<bool>                    (args.at("quiet"));

    for(const auto& path : paths)
    {
        try
        {
            if(not quiet)
                std::cout << program_name << ": trying to inspect '" << path << "' ... ";
            auto information = fsop::inspect_file(path);
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