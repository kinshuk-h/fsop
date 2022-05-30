#include "argparse/arguments/positional.hpp"

#include <iostream>

std::string argparse::Positional::usage() const noexcept
{
    auto start = (_arity == Argument::ZERO_OR_MORE or not _required ? "[" : "");
    auto end   = (_arity == Argument::ZERO_OR_MORE or not _required ? "]" : "");
    std::string content, choices;
    if(_choices.has_value())
    {
        for(const auto& choice : _choices.value())
        { choices.push_back(','); choices += choice; }
        if(not choices.empty()) { choices[0] = '{'; choices.push_back('}'); }
    }
    content = not choices.empty() ? choices : _name;
    if(_arity > 1) { content += " x" + std::to_string(_arity); }
    else if(_arity < 0) { content += "..."; }
    return start + content + end;
}

std::string argparse::Positional::descriptor(int tty_column_count) const noexcept
{
    std::string::size_type spc_w = (tty_column_count / 3), text_w = tty_column_count - spc_w;

    std::stringstream _descriptor; _descriptor << "  ";
    std::string content, choices;

    if(_choices.has_value())
    {
        for(const auto& choice : _choices.value())
        { choices.push_back(','); choices += choice; }
        if(not choices.empty()) { choices[0] = '{'; choices.push_back('}'); }
    }

    content += not choices.empty() ? choices : _name;

    if(_arity > 1) { content += " x" + std::to_string(_arity); }
    else if(_arity < 0) { content += "..."; }

    utils::write_description(
         _descriptor << content, _description,
         tty_column_count, _descriptor.str().size()
    );
    return _descriptor.str();
}

argparse::Argument::range::iterator argparse::Positional::parse_args(
    range::iterator begin, range::iterator end,
    types::result_map& values
) const
{
    auto arg_it = begin;
    if(_arity == 1)
    {
        if(arg_it == end or (not arg_it->empty() and (*arg_it)[0] == '-'))
        {
            throw std::invalid_argument
            ( "parse_args(): expected one argument for " + _name );
        }
        else
        {
            auto arg = *arg_it; arg_it = std::next(arg_it);
            if(_choices.has_value())
            {
                auto& __choices = _choices.value();
                if(not std::binary_search(__choices.begin(), __choices.end(), arg))
                {
                    std::string choice_list { __choices[0].data(), __choices[0].size()  };
                    for(size_t i=1; i<__choices.size(); ++i)
                    {
                        choice_list += ", ";
                        choice_list.append(__choices[i].data(), __choices[i].size());
                    }
                    throw std::invalid_argument
                    (
                        "parse_args(): invalid choice for " + _name +
                        " (choose from " + choice_list + ")"
                    );
                }
            }
            std::string argval { arg.data(), arg.size() };
            // Add argument value to the values map.
            if(_transform.has_value())
                values[destination()] = _transform.value()(argval);
            else values[destination()] = argval;
        }
    }
    else
    {
        std::vector<std::string> argvals;
        // Read off arguments until the next optional flag or the end of the range.
        while(arg_it != end and (arg_it->empty() or (*arg_it)[0] != '-'))
        {
            auto arg = *arg_it; arg_it = std::next(arg_it);
            if(_choices.has_value())
            {
                auto& __choices = _choices.value();
                if(not std::binary_search(__choices.begin(), __choices.end(), arg))
                {
                    std::string choice_list { __choices[0].data(), __choices[0].size()  };
                    for(size_t i=1; i<__choices.size(); ++i)
                    {
                        choice_list += ", ";
                        choice_list.append(__choices[i].data(), __choices[i].size());
                    }
                    throw std::invalid_argument
                    (
                        "parse_args(): invalid choice for " + _name +
                        " (choose from " + choice_list + ")"
                    );
                }
            }
        }
        if(_arity > 1 and argvals.size() != static_cast<size_t>(_arity))
        {
            throw std::invalid_argument
            (
                "parse_args(): expected " + std::to_string(_arity) +
                " arguments for " + _name + " (got " + std::to_string(argvals.size()) + ")"
            );
        }
        else if(_arity < 0 and argvals.size() < static_cast<size_t>(-_arity-1))
        {
            throw std::invalid_argument
            (
                "parse_args(): expected at least " + std::to_string(-_arity-1) +
                " arguments for " + _name + " (got " + std::to_string(argvals.size()) + ")"
            );
        }
        // Add vector of arguments to the values map.
        if(_transform.has_value())
            values[destination()] = _transform.value()(argvals);
        else values[destination()] = argvals;
    }
    return arg_it;
}