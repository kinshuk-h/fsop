#include "argparse/parser.hpp"

argparse::Parser& argparse::Parser::add_argument(Argument&& argument)
{
    if(argument.positional)
        _positionals.emplace_back(&argument);
    // else
    // {
    //     auto arg_ptr = std::make_shared<Argument>(argument);
    //     _optionals[argument.name()] = arg_ptr;
    //     if(not argument.alias().empty())
    //         _optionals[argument.alias()] = arg_ptr;
    // }

    return *this;
}

std::string argparse::Parser::help() const noexcept
{
    std::string usage, positional_args, optional_args;



    return usage + positional_args + optional_args;
}