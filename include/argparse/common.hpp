#ifndef ARGPARSE_COMMON_HPP_INCLUDED
#define ARGPARSE_COMMON_HPP_INCLUDED

#include "utilities.hpp"

#include <any>         // std::any
#include <string>      // std::string
#include <vector>      // std::vector
#include <variant>     // std::variant
#include <optional>    // std::optional
#include <functional>  // std::function
#include <string_view> // std::string_view

namespace argparse
{
    /** @brief Aliases for strong types for arguments. */
    namespace types
    {
        using ArgValueType = std::optional<std::variant<bool, std::string, std::vector<std::string>>>;
        using Name         = NamedType<std::string_view,                                     struct NameTag>;
        using Help         = NamedType<std::string_view,                                     struct HelpTag>;
        using Alias        = NamedType<std::string_view,                                     struct AliasTag>;
        using Destination  = NamedType<std::string_view,                                     struct DestinationTag>;
        using Arity        = NamedType<int,                                                  struct ArityTag>;
        using DefaultValue = NamedType<ArgValueType,                                         struct DefaultValueTag>;
        using Choices      = NamedType<std::optional<std::vector<std::string_view>>,         struct ChoicesTag>;
        using Transform    = NamedType<std::optional<std::function<std::any(ArgValueType)>>, struct TransformTag>;
        using Required     = NamedType<bool,                                                 struct RequiredTag>;
        using Negated      = NamedType<bool,                                                 struct NegatedTag>;
        using Description  = NamedType<std::string_view,                                     struct DescriptionTag>;
        using Epilog       = NamedType<std::string_view,                                     struct EpilogTag>;
    }

    /** @brief Argument variables for use as named arguments. */
    inline namespace arguments
    {
        // Name to display.
        inline types::Name::Argument name;
        // Help description for argument.
        inline types::Help::Argument help;
        // Alias(es) for the argument.
        inline types::Alias::Argument alias;
        // Destination name for the argument in the parsed table.
        inline types::Destination::Argument dest;
        // Default value for the argument.
        inline types::DefaultValue::Argument default_value;
        // Arity for the argument (number of values to store).
        inline types::Arity::Argument arity;
        // Domain of possible choices for the argument.
        inline types::Choices::Argument choices;
        // Transformation function to apply over argument values.
        inline types::Transform::Argument transform;
        // Whether the argument is to be marked as required.
        inline types::Required::Argument required;
        // Whether to treat flag presence as falsy.
        inline types::Negated::Argument negated;
        // Text to display before the argument help.
        inline types::Description::Argument description;
        // Text to display after the argument help.
        inline types::Epilog::Argument epilog;

        /** Default values for arguments. */
        namespace defaults
        {
            inline constexpr auto default_name        = types::Name("");
            inline constexpr auto default_help        = types::Help("");
            inline constexpr auto default_alias       = types::Alias("");
            inline constexpr auto default_dest        = types::Destination("");
            inline constexpr auto default_arity       = types::Arity(1);
            inline constexpr auto default_negated     = types::Negated(false);
            inline constexpr auto default_description = types::Description("");
            inline constexpr auto default_epilog      = types::Epilog("");

            inline auto default_transform = types::Transform(std::nullopt);
            inline auto default_choices   = types::Choices(std::nullopt);
            inline auto default_value     = types::DefaultValue(std::nullopt);
        }
    }
}

#endif // ARGPARSE_COMMON_HPP_INCLUDED