/**
 * @file positional.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file implementing the methods defined in argparse/arguments/positional.hpp
 * @version 1.0
 * @date 2022-05-30
 *
 * @copyright Copyright (c) 2022
 */

#include "argparse/arguments/positional.hpp"

std::string argparse::Positional::usage() const noexcept
{
    auto start   = (_arity == Argument::ZERO_OR_MORE or not _required ? "[" : "");
    auto end     = (_arity == Argument::ZERO_OR_MORE or not _required ? "]" : "");

    auto content = _choices.has_value() ? "{"+utils::join(_choices.value(), ",")+"}" : _name;
    if     (_arity > 1) { content += " x" + std::to_string(_arity); }
    else if(_arity < 0) { content += "..."; }

    return start + content + end;
}

std::string argparse::Positional::descriptor(unsigned tty_columns) const noexcept
{
    auto content = _choices.has_value() ? "{"+utils::join(_choices.value(), ",")+"}" : _name;
    if     (_arity > 1) { content += " x" + std::to_string(_arity); }
    else if(_arity < 0) { content += "..."; }

    std::stringstream _descriptor;
    this->write_description(
         _descriptor << "  " << content, _description,
         tty_columns, content.size() + 2
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
        if(arg_it == end or (not arg_it->empty() and (*arg_it)[0] == '-' and not is_numeric(*arg_it)))
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
                    throw std::invalid_argument
                    (
                        "parse_args(): invalid choice for " + _name +
                        " (choose from " + utils::join(__choices, ", ") + ")"
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
        while(arg_it != end and (arg_it->empty() or (*arg_it)[0] != '-' or is_numeric(*arg_it)))
        {
            auto arg = *arg_it; arg_it = std::next(arg_it);
            if(_choices.has_value())
            {
                auto& __choices = _choices.value();
                if(not std::binary_search(__choices.begin(), __choices.end(), arg))
                {
                    throw std::invalid_argument
                    (
                        "parse_args(): invalid choice for " + _name +
                        " (choose from " + utils::join(__choices, ", ") + ")"
                    );
                }
            }
            argvals.emplace_back(arg.data(), arg.size());
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