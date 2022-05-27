#ifndef ARGPARSE_ARGUMENT_HPP_INCLUDED
#define ARGPARSE_ARGUMENT_HPP_INCLUDED

#include <stdexcept> // std::invalid_argument

#include "../common.hpp"

namespace argparse
{
    /**
     * @brief Represents a base type to store argument information and data.
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

        using value_type = types::ArgValueType;

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
            _name = pick_if<types::Name>(
                arguments::defaults::default_name,
                name, alias, dest, defaults, help, arity
            ).get();
            if(_name.empty())
                throw std::invalid_argument("Argument::Argument(): missing value for name");
            _alias = pick_if<types::Alias>(
                arguments::defaults::default_alias,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
            _destination = pick_if<types::Destination>(
                arguments::defaults::default_dest,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
            if(_destination.empty()) _destination = _name;
            _defaults = pick_if<types::DefaultValue>(
                arguments::defaults::default_value,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
            _description = pick_if<types::Help>(
                arguments::defaults::default_help,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
            _arity = pick_if<types::Arity>(
                arguments::defaults::default_arity,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
            _choices = pick_if<types::Choices>(
                arguments::defaults::default_choices,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
            _transform = pick_if<types::Transform>(
                arguments::defaults::default_transform,
                name, alias, dest, defaults, help,
                arity, choices, transform, required
            ).get();
        }

        virtual std::string usage() const noexcept = 0;
        virtual std::string descriptor() const noexcept = 0;

        virtual ~Argument() {}

        // The name of the argument to display.
        const std::string& name         () const noexcept { return _name ; }
        // Alias for the argument (default="").
        const std::string& alias        () const noexcept { return _alias; }
        // Brief description about the argument (default="").
        const std::string& help         () const noexcept { return _description; }
        // Destination name for the argument in the parsed table (default=name).
        const std::string& destination  () const noexcept { return _destination; }
        // Number of values to be stored for the argument (default=1).
        int                arity        () const noexcept { return _arity; }
        // Default value for the argument.
        const value_type&  default_value() const noexcept { return _defaults; }
        // Whether the argument is to be enforced as required.
        bool               required     () const noexcept { return _required; }
        // Possible choices for the argument's values.
        const auto&        choices      () const noexcept { return _choices; }
        // Transformation to apply over argument values.
        const auto&        transform    () const noexcept { return _transform; }

        // Sets the name of the argument to display.
        Argument& name         (const std::string& _name       ) noexcept
        { this->_name = _name; return *this; }
        // Sets the alias for the argument.
        Argument& alias        (const std::string& _alias      ) noexcept
        { this->_alias = _alias; return *this; }
        // Sets the brief description about the argument.
        Argument& help         (const std::string& _description) noexcept
        { this->_description = _description; return *this; }
        // Sets the destination name for the argument in the parsed table.
        Argument& destination  (const std::string& _destination) noexcept
        { this->_destination = _destination; return *this; }
        // Sets the number of values to be stored for the argument.
        Argument& arity        (int                _arity      ) noexcept
        { this->_arity = _arity; if(_arity == 0) this->required(false); return *this; }
        // Sets the default value for the argument.
        Argument& default_value(const value_type&  _defaults   ) noexcept
        { this->_defaults = _defaults; return *this; }
        // Sets whether the argument is to be enforced as required.
        Argument& required     (bool               _required   ) noexcept
        { this->_required = _required; return *this; }
        // Sets the possible choices for the argument's values.
        Argument& choices      (const std::optional<std::vector<std::string_view>>& _choices) noexcept
        { this->_choices = _choices; return *this; }
        // Sets the transformation to apply over argument values.
        Argument& transform    (const std::optional<std::function<std::any(value_type)>>& _transform) noexcept
        { this->_transform = _transform; return *this; }

        // Whether the argument is a positional argument.
        const bool positional = false;
    protected:
        std::string _name, _alias, _description, _destination;
        int _arity = 0; value_type _defaults; bool _required;
        std::optional<std::vector<std::string_view>> _choices;
        std::optional<std::function<std::any(value_type)>> _transform;
    };
}

    // struct Argument
    // {
    //     using value_type = std::optional<std::variant<bool, std::string, std::vector<std::string>>>;
    //     enum class Type : unsigned char { 
    //         /** Treats argument as a boolean flag. */
    //         SWITCH, 
    //         /** Stores a single value into the argument. */
    //         SINGLE_VALUE,
    //         /** Stores multiple values into the argument, read until the next flag. */ 
    //         MULTIPLE_VALUES 
    //     };
        
    //     /** @brief Construct a new Argument object with some default properties.
    //      * 
    //      * @param name The argument's name.
    //      * @param type The argument's type. Can be `SWITCH`, `SINGLE_VALUE` or `MULTIPLE_VALUES`
    //      * @param desc The argument's description to use in a help menu.
    //      * @param abbr Short flag representation for the argument. Use `char(0)` for avoiding a short representation.
    //      * @param default_value Default value of the argument, if applicable.
    //      * @param positional Boolean value indicating if the argument is to be treated as positional.
    //      */
    //     Argument(std::string_view name, Type type, std::string_view desc = "", char abbr = '\0', 
    //              const value_type& default_value = std::nullopt, bool positional = false) 
    //     : _name(name), _type(type), _desc(desc), _abbr(abbr), _optional(default_value.has_value()), _positional(positional), _default(default_value)
    //     { if(_type == Type::SWITCH) { _default = false; _optional = true; _positional = false; } }
    //     /** @brief Construct a new Argument object */
    //     Argument() {}

    //     /** @brief Sets the argument's value.
    //      * 
    //      * @param value Current value for the argument.
    //      * @return Reference to the current object.
    //      */
    //     Argument& value      (const value_type& value) noexcept { _value    = value         ; return *this; }
    //     /** @brief Sets the argument's name.
    //      * 
    //      * @param name The name for the argument.
    //      * @return Reference to the current object.
    //      */
    //     Argument& name       (std::string_view name  ) noexcept { _name     = to_lower(name); return *this; }
    //     /** @brief Sets the argument's short flag representation.
    //      * 
    //      * @param abbr The short flag representation.
    //      * @return Reference to the current object.
    //      */
    //     Argument& flag       (const char& abbr       ) noexcept { _abbr     = abbr          ; return *this; }
    //     /** @brief Sets the argument's description.
    //      * 
    //      * @param desc The new description for the argument.
    //      * @return Reference to the current object.
    //      */
    //     Argument& description(std::string_view desc  ) noexcept { _desc     = desc          ; return *this; }
    //     /** @brief Sets the argument's default value.
    //      * This value is used when the current value is unset.
    //      * 
    //      * @param value The new default value.
    //      * @return Reference to the current object.
    //      */
    //     Argument& defaults   (const value_type& value) noexcept { _default  = value; _optional = true;                return *this; }
    //     /** @brief Sets whether the argument is optional. 
    //      * If the argument has a default value, then this remains true.
    //      * 
    //      * @param optional Whether to keep the argument as optional.
    //      * @return Reference to the current object.
    //      */
    //     Argument& optional   (const bool optional    ) noexcept { _optional = _default.has_value() ? true : optional; return *this; }
    //     /** @brief Sets whether the argument is positional.
    //      * For `SWITCH` arguments, setting this to true will change the type to `SINGLE_VALUE`.
    //      * 
    //      * @param positional Whether the argument is positional.
    //      * @return Reference to the current object.
    //      */
    //     Argument& positional (const bool positional  ) noexcept { _positional = positional; if(_type == Type::SWITCH) { _type = Type::SINGLE_VALUE; } return *this; }
    //     /** @brief Sets the argument's type. 
    //      * If the type is set to `SWITCH` then the argument is set as optional.
    //      * 
    //      * @param type The new type for the argument.
    //      * @return Reference to the current object.
    //      */
    //     Argument& type       (const Type& type       ) noexcept { _type = type; if(_type == Type::SWITCH) { defaults(false); _positional = false; }   return *this; }

    //     /** @brief Gets the argument's current value.
    //      * 
    //      * @return The argument's value if set or the default value.
    //      */
    //     value_type  value      () const noexcept { return _value.has_value() ? _value : _default; }
    //     /** @brief Gets the argument's default value.
    //      * 
    //      * @return The default value as set.
    //      */
    //     value_type  defaults   () const noexcept { return _default;    }
    //     /** @brief Gets the argument's name.
    //      * 
    //      * @return The argument's name.
    //      */
    //     std::string name       () const noexcept { return _name;       }
    //     /** @brief Gets the argument's description.
    //      * 
    //      * @return The argument's description.
    //      */
    //     std::string description() const noexcept { return _desc;       }
    //     /** @brief Gets the argument's short flag status. Defaults to `char(0)`
    //      * 
    //      * @return The argument's short flag representation.
    //      */
    //     char        flag       () const noexcept { return _abbr;       }
    //     /** @brief Gets the argument's type. Defaults to `SINGLE_VALUE`.
    //      * 
    //      * @return The argument's type, as defined in `Argument::Type`
    //      */
    //     Type        type       () const noexcept { return _type;       }
    //     /** @brief Gets the argument's status of being optional. Defaults to false.
    //      * 
    //      * @return `true` if the argument is optional.
    //      */
    //     bool        optional   () const noexcept { return _optional;   }
    //     /** @brief Gets the argument's status of being positional. Defaults to false.
    //      * 
    //      * @return `true` if the argument is positional.
    //      */
    //     bool        positional () const noexcept { return _positional; }

    //     /** @brief Returns a string descriptor of the argument, hinting its usage.
    //      * 
    //      * @return The descriptor string.
    //      */
    //     std::string descriptor() const noexcept 
    //     {  
    //         std::string name; 
    //         if(_positional) { name = _name; if(_type == Type::MULTIPLE_VALUES) name += "..."; }
    //         else 
    //         {
    //             if(_abbr != '\0') { name.push_back('-'); name.push_back(_abbr); }
    //             else { name = "--"; name += _name; }
    //             switch(_type) 
    //             {
    //                 case Type::SWITCH         : break;
    //                 case Type::SINGLE_VALUE   : name += " " + to_upper(_name); break;
    //                 case Type::MULTIPLE_VALUES: name += " " + to_upper(_name) + "..."; break;
    //             }
    //         }
    //         return _optional ? "["+name+"]" : "<"+name+">";
    //     }
    //     /** @brief Returns a list compatible representation of the argument's name.
    //      * 
    //      * @return The name suitable for listing the argument.
    //      */
    //     std::string list_name() const noexcept
    //     {
    //         std::string name; 
    //         if(_positional) { name = _name; if(_type == Type::MULTIPLE_VALUES) name += "..."; }
    //         else 
    //         {
    //             name = "--" + _name; if(_abbr != '\0') { std::string str = "-*, "; str[1] = _abbr; name = str+name; }
    //             switch(_type) 
    //             {
    //                 case Type::SWITCH         : break;
    //                 case Type::SINGLE_VALUE   :
    //                 case Type::MULTIPLE_VALUES: name += " " + to_upper(_name); break;
    //             }
    //         }
    //         return name;
    //     }

    // private:
    //     std::string _name; Type _type = Type::SINGLE_VALUE; std::string _desc; char _abbr = '\0';  
    //     bool _optional = false, _positional = false; value_type _value = std::nullopt, _default = std::nullopt;
    // };

#endif // ARGPARSE_ARGUMENT_HPP_INCLUDED