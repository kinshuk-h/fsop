#include "fsop_cli.hpp"       // Base header containing function declaration for 'create'

#include <iostream>           // std::cout, std::cerr

#include "fsop/create.hpp"    // fsop::create_file, fsop::create_pipe
#include "fsop/utilities.hpp" // fsop::utils::to_permissions

int fsop_cli::create(const argparse::types::result_map& args, std::string_view program_name)
{
    auto perms     = std::any_cast<mode_t>     (args.at("perms")    );
    auto path      = std::any_cast<std::string>(args.at("path")     );
    auto type      = std::any_cast<std::string>(args.at("type")     );
    auto overwrite = std::any_cast<bool>       (args.at("overwrite"));
    auto quiet     = std::any_cast<bool>       (args.at("quiet")    );

    auto type_name = (type == "regular" ? "regular file" : "named pipe");

    try
    {
        if(not quiet)
            std::cout << program_name << ": creating '" << path << "' as a "
                      << type_name << " and with permissions '"
                      << fsop::utils::to_permissions(perms) << "' ...\n";

        if(type == "regular")
            fsop::create_file(path, perms, overwrite);
        else
            fsop::create_pipe(path, perms, overwrite);

        if(not quiet)
            std::cout << program_name << ": successfully created "
                      << type_name << " '" << path << "'.\n\n";
        return EXIT_SUCCESS;
    }
    catch(std::system_error& error)
    {
        std::cerr << program_name << ": error: " << error.what() << "\n";
        if(error.code().value() == EEXIST)
            std::cerr << std::setw(program_name.size()) << ' '
                      << "  (to overwrite existing file, use '--overwrite')\n";
        std::cerr << "\n";
        return EXIT_FAILURE;
    }
}