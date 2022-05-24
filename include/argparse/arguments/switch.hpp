#ifndef ARGPARSE_SWITCH_HPP_INCLUDED
#define ARGPARSE_SWITCH_HPP_INCLUDED

#include "argparse/common.hpp"

namespace argparse
{
    /**
     * @brief Represents a switch/flag argument, which has no arity and is used to store
     *      a boolean or a constant upon presence or absence.
     *
     */
    struct Switch : public Argument
    {
        /**
         * @brief Construct a new Switch object
         *
         * @tparam NameType Placeholder type for named arguments.
         * @tparam AliasType Placeholder type for named arguments.
         * @tparam DestinationType Placeholder type for named arguments.
         * @tparam DefaultValueType Placeholder type for named arguments.
         * @tparam DescriptionType Placeholder type for named arguments.
         * @tparam NegatedType Placeholder type for named arguments.
         *
         * @param name The name of the argument to display.
         * @param alias Alias for the argument (default="").
         * @param dest Destination name for the argument in the parsed table (default=name).
         * @param defaults Default value for the argument.
         * @param help Brief description about the argument (default="").
         * @param negated Treat flag presence as falsy.
         */
        template<
            typename NameType = std::nullptr_t,
            typename AliasType = std::nullptr_t,
            typename DestinationType = std::nullptr_t,
            typename DefaultValueType = std::nullptr_t,
            typename DescriptionType = std::nullptr_t,
            typename NegatedType = std::nullptr_t
        >
        Switch(
            NameType name = nullptr,
            AliasType alias = nullptr,
            DestinationType dest = nullptr,
            DefaultValueType defaults = nullptr,
            DescriptionType help = nullptr,
            NegatedType negated = nullptr
        );
    };
}

#endif // ARGPARSE_SWITCH_HPP_INCLUDED