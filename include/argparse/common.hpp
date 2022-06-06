/**
 * @file common.hpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Defines common constants and header includes for use throughout the argparse module.
 * @version 1.0
 * @date 2022-05-28
 *
 * @copyright Copyright (c) 2022
 */

#ifndef ARGPARSE_COMMON_HPP_INCLUDED
#define ARGPARSE_COMMON_HPP_INCLUDED

#include "utilities.hpp"

// Standardized C-Header Units
#include <cctype>       // std::isalpha, std::toupper
#include <cstdlib>      // std::exit

// C++ Header Units
#include <any>           // std::any
#include <string>        // std::string
#include <vector>        // std::vector
#include <memory>        // std::unique_ptr
#include <ostream>       // std::ostream
#include <sstream>       // std::stringstream
#include <iomanip>       // std::setw
#include <variant>       // std::variant
#include <optional>      // std::optional
#include <algorithm>     // std::min, std::swap, std::transform
#include <functional>    // std::function
#include <string_view>   // std::string_view
#include <unordered_map> // std::unordered_map

/**
 * @brief Exposes components for parsing command-line arguments.
 *
 * This namespace exposes classes for the parser and arguments and their subtypes.
 * Further it provides constants for emulating named arguments across the constructors
 * of the respective classes.
 */
namespace argparse
{
    /**
     * @brief Exception subtype to convey parse errors.
     */
    struct parse_error : public std::exception
    {
        /**
         * @brief Construct a new parse error object
         *
         * @param message Message explaining the reason of failure.
         * @param usage Usage string from the parser.
         */
        parse_error(std::string_view message, std::string_view usage)
        : std::exception(), _parser_usage(usage), _message(message) {}

        /**
         * @brief Returns the parser usage associated with the parser which raised the error.
         *
         * @return const std::string& String describing the usage.
         */
        const std::string& parser_usage() const noexcept { return _parser_usage; }

        /**
         * @brief Explains the reason for failure.
         *
         * @return const char* The reason fo failure.
         */
        virtual const char* what() const noexcept override { return _message.c_str(); }
    private:
        std::string _parser_usage, _message;
    };

    /**
     * @brief Aliases for strong types for arguments.
     *
     * This namespace defines type aliases for strong types to be used for parameters of the
     * argparse classes' constructors.
    */
    namespace types
    {
        /** General type for untransformed argument values. */
        using argument_value_type = std::variant<bool, std::string, std::vector<std::string>>;
        /** General type for the argument value transformation functions. */
        using transform_function  = std::function<std::any(const argument_value_type&)>;
        /** Type of the map holding the parsed argument values. */
        using result_map          = std::unordered_map<std::string, std::any>;

        /** Type for name of the program and argument. */
        using Name         = NamedType<std::string_view,                             struct NameTag>;
        /** Type for help description of the arguments. */
        using Help         = NamedType<std::string_view,                             struct HelpTag>;
        /** Type for alias name of the arguments. */
        using Alias        = NamedType<std::string_view,                             struct AliasTag>;
        /** Type for destination name for the argument. */
        using Destination  = NamedType<std::string_view,                             struct DestinationTag>;
        /** Type for arity of the argument (numeric value). */
        using Arity        = NamedType<int,                                          struct ArityTag>;
        /** Type for default value of the argument (either a bool, string or vector). */
        using DefaultValue = NamedType<std::optional<argument_value_type>,           struct DefaultValueTag>;
        /** Type for possible choices for the argument. */
        using Choices      = NamedType<std::optional<std::vector<std::string_view>>, struct ChoicesTag>;
        /** Type for transformation function for the argument. */
        using Transform    = NamedType<std::optional<transform_function>,            struct TransformTag>;
        /** Type for the required status for the argument. */
        using Required     = NamedType<bool,                                         struct RequiredTag>;
        /** Type for the negated status for the argument. */
        using Negated      = NamedType<bool,                                         struct NegatedTag>;
        /** Type for description of the parser/program. */
        using Description  = NamedType<std::string_view,                             struct DescriptionTag>;
        /** Type for epilog description of the parser/program. */
        using Epilog       = NamedType<std::string_view,                             struct EpilogTag>;
        /** Type for output stream of the parser/program. */
        using OutputStream = NamedType<std::ostream&,                                struct OutputStreamTag>;
    }

    /** @brief Argument variables for use as named arguments. */
    inline namespace arguments
    {
        /** Name to display. */
        inline types::Name::Argument name;
        /** Help description for argument. */
        inline types::Help::Argument help;
        /** Alias(es) for the argument. */
        inline types::Alias::Argument alias;
        /** Destination name for the argument in the parsed table. */
        inline types::Destination::Argument dest;
        /** Default value for the argument. */
        inline types::DefaultValue::Argument default_value;
        /** Arity for the argument (number of values to store). */
        inline types::Arity::Argument arity;
        /** Domain of possible choices for the argument. */
        inline types::Choices::Argument choices;
        /** Transformation function to apply over argument values. */
        inline types::Transform::Argument transform;
        /** Whether the argument is to be marked as required. */
        inline types::Required::Argument required;
        /** Whether to treat flag presence as falsy. */
        inline types::Negated::Argument negated;
        /** Text to display before the argument help. */
        inline types::Description::Argument description;
        /** Text to display after the argument help. */
        inline types::Epilog::Argument epilog;
        /** Stream to display help and usage information in. */
        inline types::OutputStream::Argument output_stream;

        /** Default values for arguments. */
        namespace defaults
        {
            /** Default value for the name argument. */
            inline constexpr auto default_name        = types::Name("");
            /** Default value for the help argument. */
            inline constexpr auto default_help        = types::Help("");
            /** Default value for the alias argument. */
            inline constexpr auto default_alias       = types::Alias("");
            /** Default value for the destination argument. */
            inline constexpr auto default_dest        = types::Destination("");
            /** Default value for the arity argument. */
            inline constexpr auto default_arity       = types::Arity(1);
            /** Default value for the negated argument. */
            inline constexpr auto default_negated     = types::Negated(false);
            /** Default value for the description argument. */
            inline constexpr auto default_description = types::Description("");
            /** Default value for the epilog argument. */
            inline constexpr auto default_epilog      = types::Epilog("");

            /** Default value for the transform argument. */
            inline auto default_transform = types::Transform(std::nullopt);
            /** Default value for the choices argument. */
            inline auto default_choices   = types::Choices(std::nullopt);
            /** Default value for the default value argument. */
            inline auto default_value     = types::DefaultValue(std::nullopt);
        }
    }

    /** @brief Transformation functions for common types. */
    inline namespace transforms
    {
        /**
         * @brief Maps the variant value to an integral type.
         *
         * @tparam Integral Integral type to map to.
         * @param value Variant value to map from.
         * @return Integral The integral value post mapping.
         */
        template<typename Integral, typename = std::enable_if_t<std::is_integral_v<Integral>, std::nullptr_t>>
        std::vector<Integral> to_integral(const types::argument_value_type& value)
        {
            std::vector<Integral> numvals;
            switch (value.index())
            {
            case 1:
                if constexpr (std::is_signed_v<Integral>)
                {
                    auto numval = std::stoll(std::get<1>(value));
                    numvals.push_back(static_cast<Integral>(numval));
                }
                else
                {
                    auto numval = std::stoull(std::get<1>(value));
                    numvals.push_back(static_cast<Integral>(numval));
                }
                break;
            case 2:
                const auto& strvals = std::get<2>(value);
                numvals.reserve(strvals.size());
                for(const auto& strval : strvals)
                {
                    if constexpr (std::is_signed_v<Integral>)
                    {
                        auto numval = std::stoll(strval);
                        numvals.push_back(static_cast<Integral>(numval));
                    }
                    else
                    {
                        auto numval = std::stoull(strval);
                        numvals.push_back(static_cast<Integral>(numval));
                    }
                }
            }
            return numvals;
        }
    }

    /** @brief Utility functions for use across the module. */
    inline namespace utils
    {
        /**
         * @brief Joins elements of a collection together to a single range.
         *
         * @tparam Range Type of the range. Must provide access to begin and end iterators.
         * @param range The range/collection of elements to join. Elements must be writable
         *              to streams (provide and overload for operator<<)
         * @param glue The string to use to join the content.
         * @return {std::string} The concatenated string.
         */
        template<typename Range>
        std::string join(const Range& range, std::string_view glue = " ")
        {
            std::ostringstream oss; auto it = std::begin(range);
            if(it != std::end(range))
                { oss << *it; it = std::next(it); }
            for(; it != std::end(range); it = std::next(it))
                oss << glue << *it;
            return oss.str();
        }

        /**
         * @brief Joins elements of a collection together to a single range.
         *
         * @tparam Range Type of the range. Must provide access to begin and end iterators.
         * @tparam TransformFunction Transformation function for mapping range elements.
         *
         * @param range The range/collection of elements to join. Elements must be writable
         *              to streams (provide and overload for operator<<)
         * @param glue The string to use to join the content.
         * @param transform Unary operation to apply over elements prior to join.
         * @return {std::string} The concatenated string.
         */
        template<typename Range, typename TransformFunction>
        std::string join(const Range& range, std::string_view glue, TransformFunction transform)
        {
            std::ostringstream oss; auto it = std::begin(range);
            if(it != std::end(range))
                { oss << transform(*it); it = std::next(it); }
            for(; it != std::end(range); it = std::next(it))
                oss << glue << transform(*it);
            return oss.str();
        }

        /**
         * @brief Checks if a given string encapsulates a numeric integral value.
         *
         * @param value The string to check.
         */
        inline bool is_numeric(std::string_view value)
        {
            return value.empty() or (
                (value[0] == '-' or value[0] == '+' or std::isdigit(value[0])) and
                std::all_of(std::next(value.begin()), value.end(), ::isdigit)
            );
        }
    }
}

#endif // ARGPARSE_COMMON_HPP_INCLUDED