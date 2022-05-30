#include "argparse/parser.hpp"

/**
 * @brief Checks if a given string encapsulates a numeric value.
 *
 * @param value The string to check.
 */
bool is_numeric(std::string_view value)
{
    return value.empty() or (
        (value[0] == '-' or std::isdigit(value[0])) and
        std::all_of(std::next(value.begin()), value.end(), ::isdigit)
    );
}

// Selects the first element of a pair object.
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
        if(not arg_value.empty() and arg_value[0] == '-' && not is_numeric(arg_value))
        {
            arg_value = arg_value.substr(1);
            if(not arg_value.empty() and arg_value[0] == '-')
            {
                arg_value = arg_value.substr(1);
                auto arg_name = std::string { arg_value.data(), arg_value.size() };
                if(auto option = _optionals.find(arg_name); option != _optionals.end())
                {
                    const auto& argument = option->second;
                    arg_iter = argument->parse_args(arg_iter, end, values);

                    // Short-circuit when --help flag present.
                    if(option->second->name() == "help")
                    {
                        ostream() << help() << "\n\n";
                        std::exit(EXIT_SUCCESS); return arg_iter;
                    }
                }
                else
                {
                    throw std::invalid_argument
                    ( "parse_args(): invalid option: '--" +  arg_name + "'" );
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
                            throw std::invalid_argument
                            (
                                "parse_args(): optional argument with non-zero arity can"
                                "only be present at the end of the short flag list"
                            );
                        }
                        else if(i == arg_value.size()-1)
                            arg_iter = argument->parse_args(arg_iter, end, values);
                        else if(argument->arity() == 0)
                            argument->parse_args(arg_iter, end, values);

                        // Short-circuit when --help flag present.
                        if(option->second->name() == "help")
                        {
                            ostream() << help() << "\n\n";
                            std::exit(EXIT_SUCCESS); return arg_iter;
                        }
                    }
                    else
                    {
                        throw std::invalid_argument
                        ( "parse_args(): invalid option: '-" + abbreviation + "'" );
                    }
                }
            }
        }
        else if(positional_iter != _positionals.end())
        {
            arg_iter = (*positional_iter)->parse_args(arg_iter, end, values);
            ++ positional_iter;
        }
        // Silently ignore arguments without a destination.
        else arg_iter = std::next(arg_iter);
    }
    while(positional_iter != _positionals.end())
    {
        if((*positional_iter)->required())
            throw std::invalid_argument
            (
                "parse_args(): missing value for required argument: " +
                (*positional_iter)->name()
            );
        else
            values[(*positional_iter)->destination()] = (*positional_iter)->default_value();
    }
    for(const auto& [ _, option ] : _optionals)
    {
        if(values.find(option->destination()) == values.end())
        {
            if(option->required())
                throw std::invalid_argument
                (
                    "parse_args(): missing value for required argument: --" +
                    option->name()
                );
            else
            {
                values[option->destination()] = option->default_value();
            }
        }
    }
    return arg_iter;
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

    content << (not _description.empty() ? "\n\n" : "")
            << _description
            << positional_args << optional_args
            << (not _epilog.empty() ? "\n\n" : "")
            << _epilog;

    return content.str();
}