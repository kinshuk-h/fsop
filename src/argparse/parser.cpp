/**
 * @file parser.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file implementing the methods defined in argparse/parser.hpp
 * @version 1.0
 * @date 2022-05-30
 *
 * @copyright Copyright (c) 2022
 */

#include "argparse/parser.hpp"

/** Selects the first element of a pair object. */
constexpr auto select_first = [](const auto& x) -> auto& { return x.first; };

std::string argparse::SubparserSet::usage() const noexcept
{ return "{" + utils::join(_parsers, ",", select_first) + "} ..."; }

std::string argparse::SubparserSet::descriptor(unsigned tty_columns) const noexcept
{
    auto choices = utils::join(_parsers, ",", select_first);

    std::stringstream _descriptor;
    _descriptor << "  {" << choices << "}" << std::left;

    for(const auto& [ name, parser ] : _parsers)
    {
        _descriptor << "\n    " << name;
        this->write_description(
            _descriptor, parser->description(),
            tty_columns, name.size() + 4
        );
    }

    return _descriptor.str();
}

argparse::Argument::range::iterator argparse::SubparserSet::parse_args(
    range::iterator begin, range::iterator end,
    types::result_map& values
) const
{
    std::string arg { begin->data(), begin->size() };
    if(auto parser_it = _parsers.find(arg); parser_it != _parsers.end())
    {
        values[destination()] = parser_it->first;
        return parser_it->second->parse_args(std::next(begin), end, values);
    }
    else
    {
        throw std::invalid_argument
        (
            "parse_args(): invalid choice for " + _destination +
            " (choose from " + utils::join(_parsers, ",", select_first) + ")"
        );
    }
}

std::ostream& argparse::Parser::write_description(
    std::ostream& os, std::string_view description, unsigned tty_columns
)
{
    std::string_view::size_type pos, prev_pos = description.find_first_not_of("\n", 0),
                                start, end, text_w = tty_columns;
    while(prev_pos != std::string_view::npos)
    {
        pos = description.find('\n', prev_pos+1);

        auto sub_description = description.substr(prev_pos, pos-prev_pos);

        start = 0; end = std::min(text_w, sub_description.size());
        while(start < sub_description.size())
        {
            if(start > 0) os << '\n';

            // Ensure line break would not break a word midway.
            if(end < sub_description.size())
                while(not std::isspace(sub_description[end]) and sub_description[end] != '-') end--;

            for(auto i=start; i<end; ++i) os.put(sub_description[i]);
            start = end+1; end = std::min(end + text_w, sub_description.size());
        }

        prev_pos = description.find_first_not_of("\n", pos);
        if(prev_pos != std::string_view::npos) os << "\n\n";
    }

    return os;
}

argparse::Parser& argparse::Parser::add_argument(Argument&& argument)
{
    if(argument.positional())
        _positionals.push_back(argument.clone());
    else
    {
        std::shared_ptr<Argument> arg_ptr = argument.clone();
        _optionals[argument.name()] = arg_ptr;
        if(not argument.alias().empty())
            _optionals[argument.alias()] = arg_ptr;
    }

    return *this;
}

auto argparse::Parser::parse_args(
    Argument::range::iterator begin, Argument::range::iterator end,
    types::result_map& values
) -> Argument::range::iterator
{
    auto positional_iter = _positionals.begin();
    auto arg_iter = begin;

    for(; arg_iter != end;)
    {
        auto arg_value = *arg_iter;
        if(not arg_value.empty() and arg_value[0] == '-' and not utils::is_numeric(arg_value))
        {
            arg_value = arg_value.substr(1);
            if(not arg_value.empty() and arg_value[0] == '-')
            {
                arg_value = arg_value.substr(1);
                auto arg_name = std::string { arg_value.data(), arg_value.size() };
                if(auto option = _optionals.find(arg_name); option != _optionals.end())
                {
                    const auto& argument = option->second;
                    try
                    {
                        arg_iter = argument->parse_args(arg_iter, end, values);
                    }
                    catch(std::invalid_argument& error)
                    {
                        throw argparse::parse_error(error.what(), usage());
                    }

                    // Short-circuit when --help flag present.
                    if(option->second->name() == "help")
                    {
                        ostream() << help() << "\n\n";
                        std::exit(EXIT_SUCCESS); return arg_iter;
                    }
                }
                else
                {
                    throw argparse::parse_error
                    ( "parse_args(): invalid option: '--" +  arg_name + "'", usage() );
                }
            }
            else
            {
                for(size_t i=0; i<arg_value.size(); ++i)
                {
                    auto abbreviation = std::string(1, arg_value[i]);
                    if(auto option = _optionals.find(abbreviation); option != _optionals.end())
                    {
                        const auto& argument = option->second;
                        if(argument->arity() != 0 and i < arg_value.size()-1)
                        {
                            throw argparse::parse_error
                            (
                                "parse_args(): optional argument with non-zero arity can"
                                "only be present at the end of the short flag list", usage()
                            );
                        }
                        else if(i == arg_value.size()-1)
                            try
                            {
                                arg_iter = argument->parse_args(arg_iter, end, values);
                            }
                            catch(std::invalid_argument& error)
                            {
                                throw argparse::parse_error(error.what(), usage());
                            }
                        else if(argument->arity() == 0)
                            try
                            {
                                argument->parse_args(arg_iter, end, values);
                            }
                            catch(std::invalid_argument& error)
                            {
                                throw argparse::parse_error(error.what(), usage());
                            }

                        // Short-circuit when --help flag present.
                        if(option->second->name() == "help")
                        {
                            ostream() << help() << "\n\n";
                            std::exit(EXIT_SUCCESS); return arg_iter;
                        }
                    }
                    else
                    {
                        throw argparse::parse_error
                        ( "parse_args(): invalid option: '-" + abbreviation + "'", usage() );
                    }
                }
            }
        }
        else if(positional_iter != _positionals.end())
        {
            try
            {
                arg_iter = (*positional_iter)->parse_args(arg_iter, end, values);
            }
            catch(std::invalid_argument& error)
            {
                throw argparse::parse_error(error.what(), usage());
            }
            ++ positional_iter;
        }
        // Silently ignore arguments without a destination.
        else arg_iter = std::next(arg_iter);
    }
    while(positional_iter != _positionals.end())
    {
        if((*positional_iter)->required())
            throw argparse::parse_error
            (
                "parse_args(): missing value for required argument: " +
                (*positional_iter)->name(), usage()
            );
        else
            values[(*positional_iter)->destination()] = (*positional_iter)->default_value();
        ++ positional_iter;
    }
    for(const auto& [ _, option ] : _optionals)
    {
        if(values.find(option->destination()) == values.end())
        {
            if(option->required())
                throw argparse::parse_error
                (
                    "parse_args(): missing value for required argument: --" +
                    option->name(), usage()
                );
            else
            {
                values[option->destination()] = option->default_value();
            }
        }
    }
    return arg_iter;
}

std::string argparse::Parser::usage(unsigned tty_columns) const noexcept
{
    std::ostringstream content; content << "usage: " + identifier();
    std::string::size_type cursor = 0, spc_w = content.tellp(),
                           text_w = tty_columns - spc_w;

    for(const auto&[ name, option ] : _optionals)
    {
        if(option->name() != name) continue;

        auto arg_usage = option->usage();
        if(cursor > 0 and (cursor + arg_usage.size() + 1) > text_w)
        {
            content << '\n' << std::setw(spc_w) << ' '
                    << ' ' << arg_usage;
            cursor = 1 + arg_usage.size();
        }
        else
        {
            content << ' ' << arg_usage;
            cursor += 1 + arg_usage.size();
        }
    }
    for(const auto& positional : _positionals)
    {
        auto arg_usage = positional->usage();
        if(cursor > 0 and (cursor +  + 1) > text_w)
        {
            content << '\n' << std::setw(spc_w) << ' '
                    << ' ' << arg_usage;
            cursor = 1 + arg_usage.size();
        }
        else
        {
            content << ' ' << arg_usage;
            cursor += 1 + arg_usage.size();
        }
    }

    return content.str();
}

std::string argparse::Parser::help(unsigned tty_columns) const noexcept
{
    std::ostringstream content; content << "usage: " + identifier();
    std::string::size_type cursor = 0, spc_w = content.tellp(),
                           text_w = tty_columns - spc_w;
    std::string positional_args, optional_args;

    for(const auto&[ name, option ] : _optionals)
    {
        if(option->name() != name) continue;
        if(optional_args.empty())
            optional_args = "\n\noptional arguments:";
        optional_args.push_back('\n');
        optional_args += option->descriptor(tty_columns);

        auto arg_usage = option->usage();
        if(cursor > 0 and (cursor + arg_usage.size() + 1) > text_w)
        {
            content << '\n' << std::setw(spc_w) << ' '
                    << ' ' << arg_usage;
            cursor = 1 + arg_usage.size();
        }
        else
        {
            content << ' ' << arg_usage;
            cursor += 1 + arg_usage.size();
        }
    }
    for(const auto& positional : _positionals)
    {
        if(positional_args.empty())
            positional_args = "\n\npositional arguments:";
        positional_args.push_back('\n');
        positional_args += positional->descriptor(tty_columns);

        auto arg_usage = positional->usage();
        if(cursor > 0 and (cursor +  + 1) > text_w)
        {
            content << '\n' << std::setw(spc_w) << ' '
                    << ' ' << arg_usage;
            cursor = 1 + arg_usage.size();
        }
        else
        {
            content << ' ' << arg_usage;
            cursor += 1 + arg_usage.size();
        }
    }

    content << (not _description.empty() ? "\n\n" : "");
    this->write_description(content, _description, tty_columns)
        << positional_args << optional_args
        << (not _epilog.empty() ? "\n\n" : "");
    this->write_description(content, _epilog, tty_columns);

    return content.str();
}