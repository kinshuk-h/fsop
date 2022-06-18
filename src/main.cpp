/**
 * @file main.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Driver source code for the CLI application.
 * @version 1.0
 * @date 2022-06-05
 *
 * @copyright Copyright (c) 2022
 */

#include <iostream>     // std::cout, std::cin

#include "argparse.hpp" // Module for argument parsing.
#include "fsop_cli.hpp" // Functions implementing CLI actions.

#include "fsop/utilities.hpp"   // fsop::utils::*

using namespace std::string_literals;
using namespace argparse::arguments;

int main(int argc, const char** argv)
{
    auto to_seek_whences = [](const argparse::Optional::value_type& value) {
        const auto& _values = std::get<2>(value);
        std::vector<int> whences; whences.reserve(_values.size());
        for(const auto& valstr : _values)
            whences.push_back(fsop::utils::parse_seek_whence(valstr));
        return whences;
    };

    argparse::Parser parser {
        name = argv[0],
        description = (
            "demonstrate filesystem operations "
            "by using various system calls"
        ),
        epilog = (
            "to view information about how to use "
            "actions, use <action> --help"
        )
    };

    parser.add_arguments(
        argparse::Switch
        {
            name = "quiet", alias = "q",
            help = "suppress informational and logging messages on STDOUT"
        },
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
        description = "create regular files or named pipes",
        epilog = (
            "create allows creation of regular files and named pipes\n\n"
            "permissions can be either as an octal number, a 9-letter string containing the letters r, w, x or - for "
            "indicating read, write, execute or no permission for the corresponding permission group, or the string "
            "separating permission group letters (u, g and o) with permissions (r, w or x) with a + symbol (e.g.: "
            "ug+rw to grant read and write permission to user and the group)\n\n"
            "the overwrite mode works by unlinking the file and recreating it, so hard links might be lost as a result"
        )
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
        argparse::Switch
        {
            name = "overwrite", alias = "o",
            help = "if file exists, truncate file contents (effectively recreating the file)"
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
        description = "read content off regular files or pipes (named/unnamed)",
        epilog = (
            "read allows reading arbitrary bytes of data from a regular file or named pipe\n\n"
            "read performs read operations in batches, where each batch can include a start offset, a whence "
            "value and bytes to be read\n\n"
            "bytes to read can be -1 to specify reading till the end of the file."
        )
    };
    read_parser.add_arguments(
        argparse::Positional
        {
            name = "path", required = false,
            help = "path of the file to read"
        },
        argparse::Switch
        {
            name = "pipe", alias = "p",
            help = "read (and write?) content from a pipe (possibly unnamed)",
        },
        argparse::Optional
        {
            name = "offset", alias = "b", arity = argparse::Argument::ZERO_OR_MORE,
            help = "read content from specified byte offset",
            transform = argparse::transforms::to_integral<off64_t>
        },
        argparse::Optional
        {
            name = "offset-base", alias = "B", arity = argparse::Argument::ZERO_OR_MORE,
            help = "offset base to start offset movement from",
            choices = std::vector<std::string_view> { "SET", "CUR", "END" },
            transform = to_seek_whences
        },
        argparse::Optional
        {
            name = "byte-count", alias = "c",
            arity = argparse::Argument::ZERO_OR_MORE,
            help = "number of bytes to read",
            transform = argparse::transforms::to_integral<size_t>,
        }
    );

    // Parser to handle the write subcommand.
    argparse::Parser write_parser {
        name = "write",
        description = "write content to regular files or pipes (named/unnamed)",
        epilog = (
            "write allows writing arbitrary bytes of data to a regular file or named pipe\n\n"
            "write performs write operations in batches, where each batch can include a start offset and a "
            "whence value\n\n"
            "Data to be written is read from STDIN, and can be entered in "
            "line-buffered (--lbuf) or EOF (normal) mode."
        )
    };
    write_parser.add_arguments(
        argparse::Positional
        {
            name = "path", required = false,
            help = "path of the file to write"
        },
        argparse::Switch
        {
            name = "pipe", alias = "p",
            help = "write (and read?) content to a pipe (possibly unnamed)",
        },
        argparse::Switch
        {
            name = "line-buffered", alias = "lbuf",
            help = "read STDIN in line-buffered manner",
        },
        argparse::Optional
        {
            name = "offset", alias = "b", arity = argparse::Argument::ZERO_OR_MORE,
            help = "read content from specified byte offset",
            transform = argparse::transforms::to_integral<off64_t>
        },
        argparse::Optional
        {
            name = "offset-base", alias = "B", arity = argparse::Argument::ZERO_OR_MORE,
            help = "offset base to start offset movement from",
            choices = std::vector<std::string_view> { "SET", "CUR", "END" },
            transform = to_seek_whences
        },
        argparse::Switch
        {
            name = "append", alias = "A",
            help = "open file in append mode (all write operations start from the end)"
        },
        argparse::Switch
        {
            name = "truncate", alias = "t",
            help = "truncate file contents prior to writing"
        }
    );

    // Parser to handle the inspect subcommand.
    argparse::Parser inspect_parser {
        name = "inspect",
        description = "inspect and retrieve information about files",
    };
    inspect_parser.add_arguments(
        argparse::Positional
        {
            name = "path", arity = argparse::Argument::ONE_OR_MORE,
            help = "paths of files to inspect"
        },
        argparse::Switch
        {
            name = "follow-symlinks", alias = "l",
            help = "resolve symbolic links and inspect the resolved path instead"
        }
    );

    // Register different subparsers for actions.
    parser.add_subparsers(
        "action",
        create_parser, read_parser,
        write_parser, inspect_parser
    );

    try
    {
        auto args = parser.parse_args(argc, argv);

        auto action  = std::any_cast<std::string>(args.at("action"));
        auto workdir = std::any_cast<std::string>(args.at("directory"));
        auto quiet   = std::any_cast<bool>       (args.at("quiet"));

        if(not workdir.empty())
            fsop::utils::change_directory(workdir);

        if(not quiet)
            std::cout << parser.prog() << ": current working directory: "
                      << fsop::utils::current_directory() << "\n\n";

        if     (action == "create" )
            return fsop_cli::create (args, parser.prog());
        else if(action == "read"   )
            return fsop_cli::read   (args, parser.prog());
        else if(action == "write"  )
            return fsop_cli::write  (args, parser.prog());
        else if(action == "inspect")
            return fsop_cli::inspect(args, parser.prog());
    }
    catch(argparse::parse_error& error)
    {
        std::cerr << error.parser_usage() << '\n';
        std::cerr << "error: " << error.what() << "\n\n";
        return EXIT_FAILURE;
    }
    catch(std::exception& error)
    {
        std::cerr << parser.prog() << ": error: " << error.what() << "\n\n";
        return EXIT_FAILURE;
    }
}