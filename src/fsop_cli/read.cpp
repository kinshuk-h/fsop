/**
 * @file read.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file containing implementation of the read
 *        function declared in 'fsop_cli.hpp'.
 * @version 1.0
 * @date 2022-06-05
 *
 * @copyright Copyright (c) 2022
 */

#include "fsop_cli.hpp"         // Base header containing function declaration for 'read'

#include <iostream>             // std::cout, std::cerr
#include <algorithm>            // std::max

#include "fsop/file.hpp"        // fsop::File
#include "fsop/utilities.hpp"   // fsop::utils::to_type

int fsop_cli::read(const argparse::types::result_map& args, std::string_view program_name)
{
    try
    {
        auto path    = std::any_cast<std::string>(args.at("path") );
        auto as_pipe = std::any_cast<bool>       (args.at("pipe") );
        auto quiet   = std::any_cast<bool>       (args.at("quiet"));

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
        else // Read from a regular file or named pipe.
        {
            auto file = fsop::File::open_file(path, O_RDONLY);
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
                auto byte_counts  = std::any_cast<std::vector<size_t>> (args.at("byte-count"));

                size_t batch_counts = std::max(offset_bases.size(), byte_counts.size());
                batch_counts = std::max(batch_counts, offsets.size());

                if(batch_counts == 0)
                {
                    batch_counts = 1;
                    offsets     .push_back(0);
                    offset_bases.push_back(SEEK_CUR);
                    byte_counts .push_back(fsop::File::bytes_till_end);
                }

                if(not quiet)
                    std::cout << program_name << ": reading content from " << type_name
                                << " '" << path << "' ...\n\n";

                for(size_t batch_index = 1; batch_index <= batch_counts; ++ batch_index)
                {
                    auto offset      = offsets.size()      >= batch_index
                        ? offsets[batch_index-1]      : 0;
                    auto offset_base = offset_bases.size() >= batch_index
                        ? offset_bases[batch_index-1] : SEEK_CUR;
                    auto byte_count  = byte_counts.size()  >= batch_index
                        ? byte_counts[batch_index-1]  : fsop::File::bytes_till_end;

                    if(not quiet)
                    {
                        std::cout << program_name << ": read batch #" << batch_index
                                    << ": read ";
                        if(byte_count == fsop::File::bytes_till_end)
                            std::cout << "all bytes till the end ";
                        else
                            std::cout << byte_count << " bytes ";
                        std::cout << "from offset " << offset
                                    << " relative to the ";
                        switch(offset_base)
                        {
                            case SEEK_SET: std::cout << "beginning\n"; break;
                            case SEEK_CUR: std::cout << "current position\n"; break;
                            case SEEK_END: std::cout << "end\n"; break;
                        }
                    }

                    try
                    {
                        auto content = file.read(byte_count, offset, offset_base);
                        if(not quiet and content.empty()) std::cout << "<empty-file, no content>";
                        else std::cout << content;
                        if(not quiet and byte_count != fsop::File::bytes_till_end and content.size() < byte_count)
                            std::cerr << "(read only " << content.size() << " bytes from the file)\n";
                    }
                    catch(std::system_error& error)
                    {
                        std::cerr << program_name << ": read failure: " << error.what();
                    }

                    if(not quiet)
                        std::cout << "\n\n";
                }
            }
            else
            {
                std::cerr << program_name << ": error: unable to handle reading from a "
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