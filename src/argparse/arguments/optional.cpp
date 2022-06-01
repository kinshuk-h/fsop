#include "argparse/arguments/optional.hpp"

std::string argparse::Optional::usage() const noexcept
{
    auto start = (_arity == Argument::ZERO_OR_MORE or not _required ? "[" : "");
    auto end   = (_arity == Argument::ZERO_OR_MORE or not _required ? "]" : "");

    std::string_view min_nm = (not _alias.empty() and _alias.size() < _name.size()) ? _alias : _name;
    std::string name = (min_nm.size() == 1 ? "-" : "--"), content; name += min_nm;

    if(_choices.has_value()) content = "{"+utils::join(_choices.value(), ",")+"}";
    else std::transform(_name.begin(), _name.end(), std::back_inserter(content), ::toupper);

    if     (_arity > 1) { content += " x" + std::to_string(_arity); }
    else if(_arity < 0) { content += "..."; }
    return start + name + " " + content + end;
}

std::string argparse::Optional::descriptor(unsigned tty_columns) const noexcept
{
    std::string content; std::string_view nm1 = _name, nm2 = _alias;
    if(nm1.size() > nm2.size()) std::swap(nm1, nm2);

    if(_choices.has_value()) content = "{"+utils::join(_choices.value(), ",")+"}";
    else std::transform(_name.begin(), _name.end(), std::back_inserter(content), ::toupper);

    if     (_arity > 1) { content += " x" + std::to_string(_arity); }
    else if(_arity < 0) { content += "..."; }

    std::stringstream _descriptor; _descriptor << "  ";
    if(not nm1.empty())
    {
        _descriptor << (nm1.size() == 1 ? "-" : "--");
        _descriptor << nm1 << ' ' << content << ", ";
    }
    _descriptor << (nm2.size() == 1 ? "-" : "--");
    _descriptor << nm2 << ' ' << content << "";

    this->write_description(
         _descriptor, _description,
         tty_columns, _descriptor.tellp()
    );
    return _descriptor.str();
}

argparse::Argument::range::iterator argparse::Optional::parse_args(
    range::iterator begin, range::iterator end,
    types::result_map& values
) const
{
    auto arg_it = std::next(begin);

    auto assign_pos = begin->find('=');
    if(_arity == 1 and assign_pos != std::string_view::npos)
    {
        auto arg = begin->substr(assign_pos + 1);
        if(_choices.has_value())
        {
            auto& __choices = _choices.value();
            if(not std::binary_search(__choices.begin(), __choices.end(), arg))
            {
                throw std::invalid_argument
                (
                    "parse_args(): invalid choice for --" + _name +
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
    else if(_arity == 1)
    {
        if(arg_it == end or (not arg_it->empty() and (*arg_it)[0] == '-'))
        {
            throw std::invalid_argument
            ( "parse_args(): expected one argument for --" + _name );
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
                        "parse_args(): invalid choice for --" + _name +
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
        while(arg_it != end and (arg_it->empty() or (*arg_it)[0] != '-'))
        {
            auto arg = *arg_it; arg_it = std::next(arg_it);
            if(_choices.has_value())
            {
                auto& __choices = _choices.value();
                if(not std::binary_search(__choices.begin(), __choices.end(), arg))
                {
                    throw std::invalid_argument
                    (
                        "parse_args(): invalid choice for --" + _name +
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
                " arguments for --" + _name + " (got " + std::to_string(argvals.size()) + ")"
            );
        }
        else if(_arity < 0 and argvals.size() < static_cast<size_t>(-_arity-1))
        {
            throw std::invalid_argument
            (
                "parse_args(): expected at least " + std::to_string(-_arity-1) +
                " arguments for --" + _name + " (got " + std::to_string(argvals.size()) + ")"
            );
        }
        // Add vector of arguments to the values map.
        if(_transform.has_value())
            values[destination()] = _transform.value()(argvals);
        else values[destination()] = argvals;
    }

    return arg_it;
}