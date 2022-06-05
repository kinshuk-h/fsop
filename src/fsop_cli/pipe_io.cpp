/**
 * @file pipe_io.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file containing implementation of the pipe_io
 *        function declared in 'fsop_cli.hpp'.
 * @version 1.0
 * @date 2022-06-05
 *
 * @copyright Copyright (c) 2022
 */

#include "fsop_cli.hpp"     // Base header containing function declaration for 'write'

#include <cstring>          // std::strerror

#include <iostream>         // std::cout, std::cerr

#include <unistd.h>         // fork, getpid
#include <sys/types.h>      // pid_t

#include "fsop/file.hpp"    // fsop::File::create_unnamed_pipe

int fsop_cli::pipe_io(const argparse::types::result_map& args, std::string_view program_name)
{
    auto quiet     = std::any_cast<bool>       (args.at("quiet")    );

    if(not quiet)
        std::cout << program_name << ": initializing I/O over unnamed pipe ... \n";

    auto [ reader, writer ] = fsop::File::create_unnamed_pipe();

    std::string buffer;

    // Initialize parent & child processes in the same process
    // group to demonstrate pipe usage.
    pid_t status = fork();
    pid_t pid = getpid();
    if(status == -1)
    {
        std::cerr << program_name << ": error: unable to create child process for I/O: ";
        switch(errno)
        {
            case EAGAIN: std::cerr << "system limit for threads exceeded"; break;
            case ENOMEM: std::cerr << "out of kernel memory"; break;
            case ENOSYS: std::cerr << "platform does not support forking"; break;
        }
        std::cerr << "\n\n";
        return EXIT_FAILURE;
    }
    else if(status == 0)
    {
        reader.close(); // Explicitly close reader end.

        if(not quiet)
            std::cout << program_name << ": [Child , PID " << std::setw(5)
                      << pid << "] writer ready. Enter data to STDIN, "
                      << "send <EOF> (^D) to exit ...\n";

        while(getline(std::cin, buffer))
        {
            try
            {
                auto nbytes_written = writer.write(buffer);
                if(not quiet)
                    std::cout << program_name << ": [Child , PID "
                              << std::setw(5) << pid << "] wrote "
                              << nbytes_written << " bytes\n";
            }
            catch(std::system_error& error)
            {
                std::cerr << program_name << ": error: " << error.what() << "\n";
            }
        }

        if(not quiet)
            std::cout << program_name << ": [Child , PID " << std::setw(5)
                      << pid << "] write complete, exiting ...\n";
    }
    else
    {
        writer.close(); // Explicitly close writer end.

        while(not (buffer = std::move(reader.read())).empty())
        {
            try
            {
                if(not quiet)
                    std::cout << program_name << ": [Parent, PID "
                              << std::setw(5) << pid << "] read "
                              << buffer.size() << " bytes\n";
                std::cout << buffer << "\n";
            }
            catch(std::system_error& error)
            {
                std::cerr << program_name << ": error: " << error.what() << "\n";
            }
        }

        if(not quiet)
            std::cout << program_name << ": [Parent, PID " << std::setw(5)
                      << pid << "] read complete, exiting ...\n\n";
    }

    return EXIT_SUCCESS;
}