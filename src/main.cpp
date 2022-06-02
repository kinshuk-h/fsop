#include <iostream>

#include "argparse.hpp"

// #include "fsop/read.hpp"
// #include "fsop/write.hpp"
#include "fsop/create.hpp"
#include "fsop/inspect.hpp"
#include "fsop/utilities.hpp"

using namespace std::string_literals;
using namespace argparse::arguments;

int main(int argc, const char** argv)
{
    argparse::Parser parser {
        name = argv[0],
        description = (
            "demonstrate filesystem operations "
            "by using various system calls"
        )
    };

    parser.add_argument(
        argparse::Optional
        {
            name = "directory", alias = "d",
            help = "working directory for the process to use",
            default_value = ""s
        }
    );

    // Parser to handle the create subcommand.
    argparse::Parser create_parser {
        name = "create",
        description = "create regular files or named pipes"
    };
    create_parser.add_arguments(
        argparse::Positional
        {
            name = "path",
            help = (
                "path where the file needs to be created. "
                "All directory components are assumed to exist apriori."
            )
        },
        argparse::Optional
        {
            name = "type", alias = "t", help = "type of file to create",
            choices = std::vector <std::string_view> { "regular", "pipe" },
            default_value = "regular"s
        },
        argparse::Optional
        {
            name = "perms", alias = "p", default_value = "rw-rw-rw-"s,
            help = "permissions to set for the file",
            transform = [](const argparse::Optional::value_type& value) {
                return fsop::utils::parse_permissions(std::get<1>(value));
            }
        }
    );

    // Parser to handle the read subcommand.
    argparse::Parser read_parser {
        name = "read",
        description = "read content off regular files or pipes (named/unnamed)"
    };
    read_parser.add_arguments(
        argparse::Positional
        {
            name = "path", required = false,
            help = "path of the file to read"
        },
        argparse::Optional
        {
            name = "type", alias = "t", help = "type of file being read (overridden if file name given)",
            choices = std::vector <std::string_view> { "regular", "pipe" },
            default_value = "regular"s
        },
        argparse::Optional
        {
            name = "perms", alias = "p", default_value = "0000"s,
            help = "permissions to set for the file prior to read (using chmod)",
            transform = [](const argparse::Optional::value_type& value) {
                return fsop::utils::parse_permissions(std::get<1>(value));
            }
        },
        argparse::Optional
        {
            name = "offset", alias = "b", default_value = "0"s,
            help = "read content from specified byte offset",
            transform = argparse::transforms::to_integral<off_t>
        },
        argparse::Optional
        {
            name = "offset-base", alias = "B", default_value = "SET"s,
            help = "offset base to start offset movement from",
            choices = std::vector<std::string_view> { "SET", "CUR", "END" },
            transform = [](const argparse::Optional::value_type& value) {
                auto valstr = std::get<1>(value);
                if(valstr == "SET") return SEEK_SET;
                else if(valstr == "CUR") return SEEK_CUR;
                else return SEEK_END;
            }
        },
        argparse::Optional
        {
            name = "byte-count", alias = "c", default_value = "-1"s,
            help = "number of bytes to read",
            transform = argparse::transforms::to_integral<off_t>,
        }
    );

    // Parser to handle the write subcommand.
    argparse::Parser write_parser {
        name = "write",
        description = "write content to regular files or pipes (named/unnamed)"
    };
    write_parser.add_arguments(
        argparse::Positional
        {
            name = "path", required = false,
            help = "path of the file to write"
        },
        argparse::Optional
        {
            name = "type", alias = "t", help = "type of file being read (overridden if file name given)",
            choices = std::vector <std::string_view> { "regular", "pipe" },
            default_value = "regular"s
        },
        argparse::Optional
        {
            name = "perms", alias = "p", default_value = "0000"s,
            help = "permissions to set for the file prior to write (using chmod)",
            transform = [](const argparse::Optional::value_type& value) {
                return fsop::utils::parse_permissions(std::get<1>(value));
            }
        },
        argparse::Optional
        {
            name = "offset", alias = "b", default_value = "0"s,
            help = "read content from specified byte offset",
            transform = argparse::transforms::to_integral<off_t>
        },
        argparse::Optional
        {
            name = "offset-base", alias = "B", default_value = "SET"s,
            help = "offset base to start offset movement from",
            choices = std::vector<std::string_view> { "SET", "CUR", "END" },
            transform = [](const argparse::Optional::value_type& value) {
                auto valstr = std::get<1>(value);
                if(valstr == "SET") return SEEK_SET;
                else if(valstr == "CUR") return SEEK_CUR;
                else return SEEK_END;
            }
        },
        argparse::Switch
        {
            name = "append", alias = "A",
            help = "open file in append mode (all write operations start from the end)"
        }
    );

    // Parser to handle the inspect subcommand.
    argparse::Parser inspect_parser {
        name = "inspect",
        description = "inspect and retrieve information about files"
    };
    inspect_parser.add_arguments(
        argparse::Positional
        {
            name = "path", arity = argparse::Argument::ONE_OR_MORE,
            help = "paths of files to inspect"
        }
    );

    // Register different subparsers for actions.
    parser.add_subparsers(
        "action", create_parser, read_parser, write_parser, inspect_parser
    );

    try
    {
        auto args = parser.parse_args(argc, argv);

        // for(const auto& [ name, value ] : args)
        // {
        //     std::cout << "args[" << name << "] = " << value.type().name() << "\n";
        // }
        auto action  = std::any_cast<std::string>(args["action"]);
        auto workdir = std::any_cast<std::string>(args["directory"]);

        if(not workdir.empty())
            fsop::utils::change_directory(workdir);

        std::cout << parser.prog() << ": current working directory: "
                  << fsop::utils::current_directory() << "\n\n";

        if(action == "create")
        {
            auto perms = std::any_cast<mode_t>     (args["perms"]);
            auto path  = std::any_cast<std::string>(args["path"]);
            auto type  = std::any_cast<std::string>(args["type"]);
            auto type_name = (type == "regular" ? "regular file" : "named pipe");

            std::cout << parser.prog() << ": creating '" << path << "' as a " << type_name
                      << " and with permissions '" << fsop::utils::to_permissions(perms) << "' ...\n";
            if(type == "regular") fsop::create_file(path, perms);
            else                  fsop::create_pipe(path, perms);
            std::cout << parser.prog() << ": successfully created "
                      << type_name << " '" << path << "'.\n\n";
            return EXIT_SUCCESS;
        }

        else if(action == "inspect")
        {
            auto paths  = std::any_cast<std::vector<std::string>>(args["path"]);
            for(const auto& path : paths)
            {
                try
                {
                    std::cout << parser.prog() << ": trying to inspect '" << path << "' ... ";
                    auto information = fsop::inspect_file(path); std::cout << "done\n";
                    fsop::print_stat_info(std::cout, information) << '\n';
                }
                catch(std::system_error& error)
                {
                    std::cout << "error\n";
                    std::cerr << parser.prog() << ": error: " << error.what() << "\n\n";
                    return EXIT_FAILURE;
                }
            }
        }
    }
    catch(std::exception& error)
    {
        std::cerr << parser.prog() << ": error: " << error.what() << "\n\n";
        return EXIT_FAILURE;
    }
}