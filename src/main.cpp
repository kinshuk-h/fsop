#include <iostream>

#include "argparse.hpp"

#include "fsop/create.hpp"
#include "fsop/utilities.hpp"

using namespace argparse::arguments;

#include <bitset>

int main(int argc, const char** argv)
{
    argparse::Parser parser {
        name = argv[0],
        description = (
            "demonstrate filesystem operations "
            "by using various system calls"
        )
    };

    // Parser to handle the create subcommand.
    argparse::Parser create_parser {
        name = "create",
        description = "create regular files or named pipes"
    };
    create_parser.add_arguments(
        argparse::Switch
        {
            name = "debug", alias = "d", negated = true,
            help = (
                "enable debug mode for systems, effectively enabling logging "
                "and display of other verbose information"
            )
        },
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
            default_value = "regular"
        },
        argparse::Optional
        {
            name = "perms", alias = "p", default_value = "rw-rw-rw-",
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
            default_value = "regular"
        },
        argparse::Optional
        {
            name = "perms", alias = "p", default_value = "0000",
            help = "permissions to set for the file prior to read (using chmod)",
            transform = [](const argparse::Optional::value_type& value) {
                return fsop::utils::parse_permissions(std::get<1>(value));
            }
        },
        argparse::Optional
        {
            name = "offset", alias = "b", default_value = "0",
            help = "read content from specified byte offset",
            transform = argparse::transforms::to_integral<off_t>
        },
        argparse::Optional
        {
            name = "offset-base", alias = "B", default_value = "SET",
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
            name = "byte-count", alias = "c", default_value = "-1",
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
            default_value = "regular"
        },
        argparse::Optional
        {
            name = "perms", alias = "p", default_value = "0000",
            help = "permissions to set for the file prior to write (using chmod)",
            transform = [](const argparse::Optional::value_type& value) {
                return fsop::utils::parse_permissions(std::get<1>(value));
            }
        },
        argparse::Optional
        {
            name = "offset", alias = "b", default_value = "0",
            help = "read content from specified byte offset",
            transform = argparse::transforms::to_integral<off_t>
        },
        argparse::Optional
        {
            name = "offset-base", alias = "B", default_value = "SET",
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

    // Register different subparsers for actions.
    parser.add_subparsers("action", create_parser, read_parser, write_parser);

    try
    {
        auto args = parser.parse_args(argc, argv);

        // for(const auto& [ name, value ] : args)
        // {
        //     std::cout << "args[" << name << "] = " << value.type().name() << "\n";
        // }
        auto action = std::any_cast<std::string>(args["action"]);
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
                      << type_name << " '" << path << "'.\n";
            return EXIT_SUCCESS;
        }
    }
    catch(std::exception& error)
    {
        std::cerr << argv[0] << ": " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    // std::cout << creat("f1.txt", 0764) << "\n";
    // std::cout << chmod("f1.txt", 0764) << "\n";

    //std::cout << args << "\n";
}