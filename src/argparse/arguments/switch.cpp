/**
 * @file switch.cpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Source file implementing the methods defined in argparse/arguments/switch.hpp
 * @version 1.0
 * @date 2022-05-30
 *
 * @copyright Copyright (c) 2022
 */

#include "argparse/arguments/switch.hpp"

std::string argparse::Switch::usage() const noexcept
{
    if(not _alias.empty() and _alias.size() < _name.size())
        return (_alias.size() == 1 ? "[-" : "[--") + _alias + "]";
    return (_name.size() == 1 ? "[-" : "[--") + _name + "]";
}

std::string argparse::Switch::descriptor(unsigned tty_columns) const noexcept
{
    std::string_view nm1 = _name, nm2 = _alias;
    if(nm1.size() > nm2.size()) std::swap(nm1, nm2);

    std::stringstream _descriptor; _descriptor << "  ";
    if(not nm1.empty())
    {
        _descriptor << (nm1.size() == 1 ? "-" : "--");
        _descriptor << nm1 << ", ";
    }
    _descriptor << (nm2.size() == 1 ? "-" : "--");
    _descriptor << nm2;

    this->write_description(
         _descriptor, _description,
         tty_columns, _descriptor.tellp()
    );
    return _descriptor.str();
}

argparse::Argument::range::iterator argparse::Switch::parse_args(
    range::iterator begin, [[maybe_unused]] range::iterator end,
    types::result_map& values
) const
{
    if(_transform.has_value())
        values[destination()] = _transform.value()(true);
    else values[destination()] = true;
    return std::next(begin);
}