#include <stdexcept> // std::invalid_argument

#include "argparse/arguments/argument.hpp"

template<
    using NameType = std::nullptr_t,
    using AliasType = std::nullptr_t,
    using DestinationType = std::nullptr_t,
    using DefaultValueType = std::nullptr_t,
    using DescriptionType = std::nullptr_t,
    using ArityType = std::nullptr_t
>
argparse::Argument::Argument(
    NameType name = nullptr,
    AliasType alias = nullptr,
    DestinationType dest = nullptr,
    DefaultValueType defaults = nullptr,
    DescriptionType help = nullptr,
    ArityType arity = nullptr
)
{
    _name = pick_if<types::Name>(
        arguments::defaults::name,
        name, alias, dest, defaults, help, arity
    ).get();
    if(_name.empty())
        throw std::invalid_argument("Argument::Argument(): missing value for name");
    _alias = pick_if<types::Alias>(
        arguments::defaults::alias,
        name, alias, dest, defaults, help, arity
    ).get();
    _destination = pick_if<types::Destination>(
        arguments::defaults::dest,
        name, alias, dest, defaults, help, arity
    ).get();
    _defaults = pick_if<types::DefaultValue>(
        arguments::defaults::defaults,
        name, alias, dest, defaults, help, arity
    ).get();
    _description = pick_if<types::Help>(
        arguments::defaults::help,
        name, alias, dest, defaults, help, arity
    ).get();
    _arity = pick_if<types::Arity>(
        arguments::defaults::arity,
        name, alias, dest, defaults, help, arity
    ).get();
}