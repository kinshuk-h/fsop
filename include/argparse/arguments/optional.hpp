/**
 * @file optional.hpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Defines an argument subtype for optional arguments.
 * @version 1.0
 * @date 2022-05-28
 *
 * @copyright Copyright (c) 2022
 */
#ifndef ARGPARSE_OPTIONAL_HPP_INCLUDED
#define ARGPARSE_OPTIONAL_HPP_INCLUDED

#include "argparse/arguments/argument.hpp"

namespace argparse
{
    /**
     * @brief Represents an optional argument, which can have arbitrary arity
     *        and is specified with a flag.
     *
     */
    struct Optional : public Argument
    {
        /**
         * @brief Construct a new Optional object
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
        Optional(
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
            this->required ( pick_if<types::Required>(
                types::Required(false),
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get() );
        }

        std::string usage() const noexcept override;
        std::string descriptor(unsigned tty_columns = 60) const noexcept override;
        /**
         * @brief Creates a copy of the argument as a unique_ptr for polymorphic usage.
         */
        std::unique_ptr<Argument> clone() const override
        {
            return std::make_unique<Optional>(*this);
        }
        range::iterator parse_args(
            range::iterator begin, range::iterator end,
            types::result_map& values
        ) const override;
    };
}

#endif // ARGPARSE_OPTIONAL_HPP_INCLUDED