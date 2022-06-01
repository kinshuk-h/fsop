/**
 * @file argument.hpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Defines the base class for arguments, which hold argument description and parsing information.
 * @version 1.0
 * @date 2022-05-28
 *
 * @copyright Copyright (c) 2022
 */

#ifndef ARGPARSE_ARGUMENT_HPP_INCLUDED
#define ARGPARSE_ARGUMENT_HPP_INCLUDED

#include <stdexcept> // std::invalid_argument

#include "../common.hpp"

namespace argparse
{
    /**
     * @brief Represents a base type to store argument description and parsing information.
     *
     * Argument objects maintain information about command-line arguments for effective parsing.
     * Further, the provided data aids in providing a concise description to the user.
     *
     */
    struct Argument
    {
        // Special arity value: allows reading 0 or more values.
        static const int ZERO_OR_MORE = -1;
        // Special arity value: allows reading 1 or more values.
        static const int ONE_OR_MORE  = -2;

        // Type of the argument values, as a variant over common argument value types.
        using value_type = types::argument_value_type;
        // Type of the collection used for storing unparsed and unprocessed arguments.
        using range = std::vector<std::string_view>;

        /**
         * @brief Writes a formatted description to the given output stream.
         *
         * This function is for internal formatting of argument descriptions
         * (such as when generating the argument help).
         *
         * @param os The output stream to write the description to.
         * @param description The description to write.
         * @param tty_columns Maximum number of terminal columns/characters to span per line.
         * @param consumed Number of characters assumed to be present on the first line (consumed character count).
         * @return std::ostream& Reference to the output stream for cascading operations.
         */
        static std::ostream& write_description(
            std::ostream& os,
            std::string_view description,
            unsigned tty_columns = 60,
            std::string::size_type consumed = 0
		);

        /**
         * @brief Construct a new Argument object
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
        Argument(
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
        {
            this->name ( pick_if<types::Name>(
                arguments::defaults::default_name,
                name, alias, dest, defaults, help, arity
            ).get() );
            _alias = pick_if<types::Alias>(
                arguments::defaults::default_alias,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
            this->destination ( pick_if<types::Destination>(
                arguments::defaults::default_dest,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get() );
            this->default_value ( pick_if<types::DefaultValue>(
                arguments::defaults::default_value,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get() );
            _description = pick_if<types::Help>(
                arguments::defaults::default_help,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
            this->arity ( pick_if<types::Arity>(
                arguments::defaults::default_arity,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get() );
            this->choices( pick_if<types::Choices>(
                arguments::defaults::default_choices,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get() );

            if(_choices.has_value() and _defaults.has_value())
            {
                const auto& __choices = _choices.value();
                if(std::holds_alternative<std::string>(_defaults.value()))
                {
                    auto __default = std::get<std::string>(_defaults.value());
                    if(not std::binary_search(__choices.begin(), __choices.end(), __default))
                        throw std::invalid_argument
                        (
                            "Argument::Argument(): invalid default value for "
                            "argument, must be one of given choices"
                        );
                }
                else if(std::holds_alternative<std::vector<std::string>>(_defaults.value()))
                {
                    auto& __defaults = std::get<std::vector<std::string>>(_defaults.value());
                    if(not std::includes(__choices.begin(), __choices.end(), __defaults.begin(), __defaults.end()))
                        throw std::invalid_argument
                        (
                            "Argument::Argument(): invalid default value for "
                            "argument, must be one of given choices"
                        );
                }

            }

            _transform = pick_if<types::Transform>(
                arguments::defaults::default_transform,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
        }

        /**
         * @brief Describes the usage of the argument through a returned string.
         */
        virtual std::string usage() const noexcept = 0;
        /**
         * @brief Describes the argument along with the provided explanation.
         */
        virtual std::string descriptor(unsigned tty_columns = 60) const noexcept = 0;
        /**
         * @brief Creates a copy of the argument as a unique_ptr for polymorphic usage.
         */
        virtual std::unique_ptr<Argument> clone() const = 0;
        /**
         * @brief Parses arguments according to the argument specification from a range of argument values
         *      referred by their beginning and end iterators.
         *
         * @param begin iterator pointing to the beginning of the argument value range.
         * @param end iterator pointing to the end of the argument value range.
         * @param values map (hash table) to store parsed valued for the argument.
         *
         * @return {range::iterator} iterator pointing to the next argument value
         *  unused by the current argument.
         */
        virtual range::iterator parse_args(
            range::iterator begin, range::iterator end,
            types::result_map& values
        ) const = 0;

        virtual ~Argument() {}

        // The name of the argument to display.
        const std::string& name         () const noexcept { return _name ; }
        // Alias for the argument (default="").
        const std::string& alias        () const noexcept { return _alias; }
        // Brief description about the argument (default="").
        const std::string& help         () const noexcept { return _description; }
        // Destination name for the argument in the parsed table (default=name).
        const std::string& destination  () const noexcept { return _destination.empty() ? _name : _destination; }
        // Number of values to be stored for the argument (default=1).
        int                arity        () const noexcept { return _arity; }
        // Default value for the argument.
        std::any           default_value() const
        {
            if(_transform.has_value())
                return _transform.value()(_defaults.value());
            else
            {
                auto& __default = _defaults.value();
                if(std::holds_alternative<bool>(__default))
                    return std::get<0>(__default);
                else if(std::holds_alternative<std::string>(__default))
                    return std::get<1>(__default);
                else
                    return std::get<2>(__default);
            }
        }
        // Whether the argument is a positional argument.
        bool               positional   () const noexcept { return _positional; }
        // Whether the argument is to be enforced as required.
        bool               required     () const noexcept { return _required; }
        // Possible choices for the argument's values.
        const auto&        choices      () const noexcept { return _choices; }
        // Transformation to apply over argument values.
        const auto&        transform    () const noexcept { return _transform; }

        /**
         * @brief Sets the name of the argument to display.
         *
         * @param _name New value for the name of the argument to display.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& name         (std::string_view   _name       )
        {
            this->_name.assign(_name.data(), _name.size());
            if(this->_name.empty())
                throw std::invalid_argument("Argument::Argument(): missing value for name");
            return *this;
        }
        /**
         * @brief Sets the alias for the argument.
         *
         * @param _alias New value for the alias for the argument.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& alias        (std::string_view   _alias      ) noexcept
        { this->_alias.assign(_alias.data(), _alias.size()); return *this; }
        /**
         * @brief Sets the brief description about the argument.
         *
         * @param _description New value for the brief description about the argument.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& help         (std::string_view   _description) noexcept
        { this->_description.assign(_description.data(), _description.size()); return *this; }
        /**
         * @brief Sets the destination name for the argument in the parsed table.
         *
         * @param _destination New value for the destination name for the argument in the parsed table.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& destination  (std::string_view   _destination) noexcept
        {
            this->_destination.assign(_destination.data(), _destination.size());
            return *this;
        }
        /**
         * @brief Sets the number of values to be stored for the argument.
         *
         * @param _arity New value for the number of values to be stored for the argument.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& arity        (int                _arity      ) noexcept
        {
            this->_arity = _arity;
            if(_arity == 0) this->required(false);
            return *this;
        }
        /**
         * @brief Sets the default value for the argument.
         *
         * @param _defaults New value for the default value for the argument.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& default_value(const types::DefaultValue::value_type&  _defaults   ) noexcept
        {
            this->_defaults = _defaults;
            if(_defaults.has_value())
            {
                _required = false;
                if(std::holds_alternative<std::vector<std::string>>(_defaults.value()))
                {
                    auto& __defaults = std::get<2>(this->_defaults.value());
                    std::sort(__defaults.begin(), __defaults.end());
                }
            }
            return *this;
        }
        /**
         * @brief Sets whether the argument is to be enforced as required.
         *
         * @param _required New value for whether the argument is to be enforced as required.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& required     (bool               _required   ) noexcept
        {
            this->_required = _required;
            if(not _defaults.has_value()) this->_required = true;
            return *this;
        }
        /**
         * @brief Sets the possible choices for the argument's values.
         *
         * @param _choices New value for the possible choices for the argument's values.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& choices      (const types::Choices::value_type& _choices) noexcept
        {
            this->_choices = _choices;
            if(this->_choices.has_value())
            {
                auto& __choices = this->_choices.value();
                std::sort(__choices.begin(), __choices.end());
            }
            return *this;
        }
        /**
         * @brief Sets the transformation to apply over argument values.
         *
         * @param _transform New value for the transformation to apply over argument values.
         * @returns {Argument&} Reference to the current object for chaining operations.
         */
        Argument& transform    (const types::Transform::value_type& _transform) noexcept
        { this->_transform = _transform; return *this; }

    protected:
        std::string _name, _alias, _description, _destination;
        int _arity = 0; bool _required, _positional = false;
        types::DefaultValue::value_type _defaults;
        types::Choices::value_type _choices;
        types::Transform::value_type _transform;
    };

    inline const int Argument::ZERO_OR_MORE;
    inline const int Argument::ONE_OR_MORE ;
}

#endif // ARGPARSE_ARGUMENT_HPP_INCLUDED