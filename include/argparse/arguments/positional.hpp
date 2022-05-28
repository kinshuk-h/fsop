/**
 * @file positional.hpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Defines an argument subtype for positional arguments.
 * @version 1.0
 * @date 2022-05-28
 *
 * @copyright Copyright (c) 2022
 */

#ifndef ARGPARSE_POSITIONAL_HPP_INCLUDED
#define ARGPARSE_POSITIONAL_HPP_INCLUDED

#include "argparse/arguments/argument.hpp"

namespace argparse
{
    /**
     * @brief Represents a positional argument, which must be specified in the exact
     *        position it is declared in.
     *
     */
    struct Positional : public Argument
    {
        /**
         * @brief Construct a new Positional object
         *
         * @tparam NameType Placeholder type for named arguments.
         * @tparam AliasType Placeholder type for named arguments.
         * @tparam DestinationType Placeholder type for named arguments.
         * @tparam DefaultValueType Placeholder type for named arguments.
         * @tparam DescriptionType Placeholder type for named arguments.
         * @tparam ArityType Placeholder type for named arguments.
         * @tparam ChoicesType Placeholder type for named arguments.
         * @tparam TransformType Placeholder type for named arguments.
         * @tparam RequiredType Placeholder type for named arguments.
         *
         * @param name The name of the argument to display.
         * @param alias Alias for the argument (default="").
         * @param dest Destination name for the argument in the parsed table (default=name).
         * @param defaults Default value for the argument.
         * @param help Brief description about the argument (default="").
         * @param arity Number of values to be stored for the argument (default=1).
         * @param choices Possible choices for the argument's values.
         * @param transform Transformation to apply over argument values.
         * @param required Whether the option is to be enforced as required.
         */
        template<
            typename NameType = std::nullptr_t,
            typename AliasType = std::nullptr_t,
            typename DestinationType = std::nullptr_t,
            typename DefaultValueType = std::nullptr_t,
            typename DescriptionType = std::nullptr_t,
            typename ArityType = std::nullptr_t,
            typename ChoicesType = std::nullptr_t,
            typename TransformType = std::nullptr_t,
            typename RequiredType = std::nullptr_t
        >
        Positional(
            NameType name = nullptr,
            AliasType alias = nullptr,
            DestinationType dest = nullptr,
            DefaultValueType defaults = nullptr,
            DescriptionType help = nullptr,
            ArityType arity = nullptr,
            ChoicesType choices = nullptr,
            TransformType transform = nullptr,
            RequiredType required = nullptr
        )
        : argparse::Argument(
            name, alias, dest, defaults, help,
            arity, choices, transform, required
        )
        {
            _positional = true;
            this->required ( pick_if<types::Required>(
                types::Required(true),
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get() );
        }

        std::string usage() const noexcept override;
        std::string descriptor(int tty_column_count = 60) const noexcept override;
        /**
         * @brief Creates a copy of the argument as a unique_ptr for polymorphic usage.
         */
        std::unique_ptr<Argument> clone() const override
        {
            return std::make_unique<Positional>(*this);
        }
        range::iterator parse_args(
            range::iterator begin, range::iterator end,
            types::result_map& values
        ) const override;
    };
}

#endif // ARGPARSE_POSITIONAL_HPP_INCLUDED