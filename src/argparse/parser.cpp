#include "argparse/parser.hpp"

std::string argparse::SubparserSet::usage() const noexcept
{
    std::string choices;
    for(const auto& [ name, _ ] : _parsers)
    { choices.push_back(','); choices += name; }
    if(not choices.empty()) { choices[0] = '{'; choices.push_back('}'); };
    return choices + " ...";
}

std::string argparse::SubparserSet::descriptor(int tty_column_count) const noexcept
{
    std::stringstream _descriptor; std::string choices;
    for(const auto& [ name, _ ] : _parsers)
    { choices.push_back(','); choices += name; }
    if(not choices.empty()) { choices[0] = '{'; choices.push_back('}'); };

    std::string::size_type spc_w = (tty_column_count / 3), text_w = tty_column_count - spc_w;
    _descriptor << "  " << choices << std::left;
    for(const auto& [ name, parser ] : _parsers)
    {
        _descriptor << "\n    " << name;
        utils::write_description(
            _descriptor, parser->description(),
            tty_column_count, name.size()+4
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
        std::string choice_list; auto it = _parsers.begin();
        choice_list.append(it->first); ++it;
        for(; it != _parsers.end(); ++it)
            choice_list += ", " + it->first;
        throw std::invalid_argument
        (
            "parse_args(): invalid choice for " + _destination +
            " (choose from " + choice_list + ")"
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

std::string argparse::Parser::help(int tty_column_count) const noexcept
{
    std::string usage = "usage: " + identifier(), positional_args, optional_args;

    for(const auto&[ name, option ] : _optionals)
    {
        if(option->name() != name) continue;
        if(optional_args.empty())
            optional_args = "\n\noptional arguments:";
        optional_args.push_back('\n');
        optional_args += option->descriptor(tty_column_count);
        usage += " " + option->usage();
    }
    for(const auto& positional : _positionals)
    {
        if(positional_args.empty())
            positional_args = "\n\npositional arguments:";
        positional_args.push_back('\n');
        positional_args += positional->descriptor(tty_column_count);
        usage += " " + positional->usage();
    }

    return usage + (not _description.empty() ? "\n\n" : "") +
        _description + positional_args + optional_args +
        (not _epilog.empty() ? "\n\n" : "") + _epilog;
}