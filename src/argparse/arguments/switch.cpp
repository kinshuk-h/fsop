#include "argparse/arguments/switch.hpp"

template<
    using NameType = std::nullptr_t,
    using AliasType = std::nullptr_t,
    using DestinationType = std::nullptr_t,
    using DefaultValueType = std::nullptr_t,
    using DescriptionType = std::nullptr_t,
    using NegatedType = std::nullptr_t
>
argparse::Switch::Switch(
    NameType name = nullptr,
    AliasType alias = nullptr,
    DestinationType dest = nullptr,
    DefaultValueType defaults = nullptr,
    DescriptionType help = nullptr,
    NegatedType negated = nullptr
) : argparse::Argument(name, alias, dest, defaults, help, negated)
{
    _negated = pick_if<types::Negated>(
        arguments::defaults::negated,
        name, alias, dest, defaults, help, negated
    )
    if(not _defaults.has_value())
        _defaults = _negated;
    _arity = 0;
}