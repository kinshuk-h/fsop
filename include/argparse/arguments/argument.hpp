#ifndef ARGPARSE_ARGUMENT_HPP_INCLUDED
#define ARGPARSE_ARGUMENT_HPP_INCLUDED

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
         *
         * @param name The name of the argument to display.
         * @param alias Alias for the argument (default="").
         * @param dest Destination name for the argument in the parsed table (default=name).
         * @param defaults Default value for the argument.
         * @param help Brief description about the argument (default="").
         * @param arity Number of values to be stored for the argument (default=1).
         */
        template<
            typename NameType = std::nullptr_t,
            typename AliasType = std::nullptr_t,
            typename DestinationType = std::nullptr_t,
            typename DefaultValueType = std::nullptr_t,
            typename DescriptionType = std::nullptr_t,
            typename ArityType = std::nullptr_t
        >
        Argument(
            NameType name = nullptr,
            AliasType alias = nullptr,
            DestinationType dest = nullptr,
            DefaultValueType defaults = nullptr,
            DescriptionType help = nullptr,
            ArityType arity = nullptr
        );



    private:
        std::string _name, _alias, _description, _destination;
        int _arity = 0;
        value_type _defaults;
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