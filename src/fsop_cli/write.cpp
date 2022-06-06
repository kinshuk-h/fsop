// TODO: Add docs, reduce use of cin and cout

#include "fsop_cli.hpp"         // Base header containing function declaration for 'write'

#include <iostream>             // std::cout, std::cerr
#include <iterator>             // std::istreambuf_iterator
#include <algorithm>            // std::max

#include <signal.h>             // signal, SIG* constants

#include "fsop/file.hpp"        // fsop::File
#include "fsop/utilities.hpp"   // fsop::utils::to_type

int fsop_cli::write(const argparse::types::result_map& args, std::string_view program_name)
{
    // Ignore SIGPIPE, and make the writer raise an error instead.
    signal(SIGPIPE, SIG_IGN);

    try
    {
        auto path    = std::any_cast<std::string>(args.at("path") );
        auto as_pipe = std::any_cast<bool>       (args.at("pipe") );
        auto quiet   = std::any_cast<bool>       (args.at("quiet"));
        auto lbuf    = std::any_cast<bool>       (args.at("line-buffered"));

        if(path.empty())
        {
            if(as_pipe) // I/O from an unnamed pipe.
            {
                return pipe_io(args, program_name);
            }
            else
            {
                std::cerr << program_name << ": error: specify either a file path, "
                          << "or use an unnamed pipe (via --pipe)\n\n";
                return EXIT_FAILURE;
            }
        }
        else // Write to a regular file or named pipe.
        {
            int open_options = O_WRONLY;
            if(std::any_cast<bool>(args.at("append")))
                open_options |= O_APPEND;
            if(std::any_cast<bool>(args.at("truncate")))
                open_options |= O_TRUNC;

            auto file = fsop::File::open_file(path, open_options);
            auto file_type = file.stat().st_mode & S_IFMT;
            auto type_name = fsop::utils::to_type(file_type);

            if(file_type == S_IFREG and as_pipe)
            {
                std::cerr << program_name << ": error: '" << path << "' is not a named pipe\n"
                          << std::setw(program_name.size()) << ' '
                          << "  (to auto-detect file type, avoid using the '--pipe' flag)\n\n";
                return EXIT_FAILURE;
            }
            else if(file_type == S_IFREG or file_type == S_IFIFO)
            {
                auto offsets      = std::any_cast<std::vector<off64_t>>(args.at("offset"));
                auto offset_bases = std::any_cast<std::vector<int>>    (args.at("offset-base"));

                size_t batch_counts = std::max(offset_bases.size(), offsets.size());

                if(batch_counts == 0)
                {
                    batch_counts = 1;
                    offsets     .push_back(0);
                    offset_bases.push_back(SEEK_CUR);
                }
                else if(std::any_cast<bool>(args.at("append")))
                {
                    std::cerr << program_name << ": warning: --append flag is set, so values with --offset and "
                              << "--offset-base will have no effect\n\n";
                }

                if(not quiet)
                    std::cout << program_name << ": writing content to " << type_name
                                << " '" << path << "' ...\n\n";

                for(size_t batch_index = 1; batch_index <= batch_counts; ++ batch_index)
                {
                    auto offset      = offsets.size()      >= batch_index
                        ? offsets[batch_index-1]      : 0;
                    auto offset_base = offset_bases.size() >= batch_index
                        ? offset_bases[batch_index-1] : SEEK_CUR;

                    if(not quiet)
                    {
                        std::cout << program_name << ": write batch #" << batch_index
                                  << ": write from offset " << offset
                                  << " relative to the ";
                        switch(offset_base)
                        {
                            case SEEK_SET: std::cout << "beginning\n"; break;
                            case SEEK_CUR: std::cout << "current position\n"; break;
                            case SEEK_END: std::cout << "end\n"; break;
                        }
                    }

                    if(not quiet)
                    {
                        std::cout << std::setw(program_name.size()) << ' '
                                  << "  Enter data to STDIN, ";
                        if (lbuf)
                            std::cout << "send '\\n' (RETURN) to finish ...\n";
                        else
                            std::cout << "send <EOF> (^D) to finish ...\n";
                    }

                    std::string content;
                    if (lbuf) getline(std::cin, content);
                    else
                        content.append
                        (
                            std::istreambuf_iterator<char>(std::cin),
                            std::istreambuf_iterator<char>()
                        );

                    try
                    {
                        auto nbytes_written = file.write(content, offset, offset_base);
                        if(not quiet)
                            std::cout << '\n' << program_name << ": wrote " << nbytes_written
                                      << " of " << content.size() << " bytes to the file";
                    }
                    catch(std::system_error& error)
                    {
                        std::cerr << program_name << ": write failure: " << error.what();
                    }

                    if(not quiet)
                        std::cout << "\n\n";
                }
            }
            else
            {
                std::cerr << program_name << ": error: unable to handle writing to a "
                            << type_name << " (file type not supported)\n\n";
                return EXIT_FAILURE;
            }
        }

        return EXIT_SUCCESS;
    }
    catch(std::system_error& error)
    {
        std::cerr << program_name << ": error: " << error.what() << "\n";
        if(error.code().value() == ENOENT)
            std::cerr << std::setw(program_name.size()) << ' '
                      << "  (run '" << program_name << " create --help' for "
                      << "help on how to create files/pipes)\n";
        std::cerr << "\n";
        return EXIT_FAILURE;
    }
}