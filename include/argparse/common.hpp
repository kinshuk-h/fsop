#ifndef ARGPARSE_COMMON_HPP_INCLUDED
#define ARGPARSE_COMMON_HPP_INCLUDED

#include "utilities.hpp"

#include <string>      // std::string
#include <vector>      // std::vector
#include <variant>     // std::variant
#include <optional>    // std::optional
#include <string_view> // std::string_view

namespace argparse
{
    /** @brief Aliases for strong types for arguments. */
    namespace types
    {
        using ArgValueType = std::optional<std::variant<bool, std::string, std::vector<std::string>>>;
        using Name         = NamedType<std::string_view, struct NameTag>;
        using Help         = NamedType<std::string_view, struct HelpTag>;
        using Alias        = NamedType<std::string_view, struct AliasTag>;
        using Destination  = NamedType<std::string_view, struct DestinationTag>;
        using DefaultValue = NamedType<ArgValueType,     struct DefaultValueTag>;
        using Negated      = NamedType<bool,             struct NegatedTag>;
        using Description  = NamedType<std::string_view, struct DescriptionTag>;
        using Epilog       = NamedType<std::string_view, struct EpilogTag>;
    }

    /** @brief Argument variables for use as named arguments. */
    inline namespace arguments
    {
        // Name to display.
        types::Name::Argument name;
        // Help description for argument.
        types::Help::Argument help;
        // Alias(es) for the argument.
        types::Alias::Argument alias;
        // Destination name for the argument in the parsed table.
        types::Destination::Argument dest;
        // Default value for the argument.
        types::DefaultValue::Argument default_value;
        // Whether to treat flag presence as falsy.
        types::Negated::Argument negated;
        // Text to display before the argument help.
        types::Description::Argument description;
        // Text to display after the argument help.
        types::Epilog::Argument epilog;

        /** Default values for arguments. */
        namespace defaults
        {
            constexpr auto default_name        = types::Name("");
            constexpr auto default_help        = types::Help("");
            constexpr auto default_alias       = types::Alias("");
            constexpr auto default_dest        = types::Destination("");
            constexpr auto default_negated     = types::Negated(false);
            constexpr auto default_description = types::Description("");
            constexpr auto default_epilog      = types::Epilog("");

            auto default_value       = types::DefaultValue(std::nullopt);
        }
    }
}

#endif // ARGPARSE_COMMON_HPP_INCLUDED