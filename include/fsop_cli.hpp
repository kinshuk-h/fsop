#ifndef FSOP_CLI_HPP_INCLUDED
#define FSOP_CLI_HPP_INCLUDED

#include "argparse.hpp" // argparse::types::result_map

/**
 * @brief Defines functions performing operations offered by the CLI application.
 */
namespace fsop_cli
{
    /**
     * @brief Implements the create subcommand for the CLI application.
     *
     * @param args Parsed map of arguments.
     * @param program_name Name of the CLI program.
     * @return {int} Exit code for the program.
     */
    int create (const argparse::types::result_map& args, std::string_view program_name = "program");
    /**
     * @brief Implements the pipe_io subcommand for the CLI application (redirected to internally by read and write).
     *
     * @param args Parsed map of arguments.
     * @param program_name Name of the CLI program.
     * @return {int} Exit code for the program.
     */
    int pipe_io(const argparse::types::result_map& args, std::string_view program_name = "program");
    /**
     * @brief Implements the read subcommand for the CLI application.
     *
     * @param args Parsed map of arguments.
     * @param program_name Name of the CLI program.
     * @return {int} Exit code for the program.
     */
    int read   (const argparse::types::result_map& args, std::string_view program_name = "program");
    /**
     * @brief Implements the write subcommand for the CLI application.
     *
     * @param args Parsed map of arguments.
     * @param program_name Name of the CLI program.
     * @return {int} Exit code for the program.
     */
    int write  (const argparse::types::result_map& args, std::string_view program_name = "program");
    /**
     * @brief Implements the inspect subcommand for the CLI application.
     *
     * @param args Parsed map of arguments.
     * @param program_name Name of the CLI program.
     * @return {int} Exit code for the program.
     */
    int inspect(const argparse::types::result_map& args, std::string_view program_name = "program");
}

#endif // FSOP_CLI_HPP_INCLUDED