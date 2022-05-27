#ifndef ARGPARSE_PARSER_HPP_INCLUDED
#define ARGPARSE_PARSER_HPP_INCLUDED

#include <memory> // std::unique_ptr

#include "common.hpp"
#include "arguments/argument.hpp"
#include "arguments/positional.hpp"

namespace argparse
{
    class Parser
    {
    private:
        std::string _name,
                    _description,
                    _epilog;

        std::unordered_map<std::string, std::shared_ptr<Argument>> _optionals;
        std::vector<std::unique_ptr<Argument>> _positionals;

    public:
        /**
         * @brief Construct a new Parser object
         *
         * @tparam DescriptionType Placeholder type for a named argument.
         * @tparam EpilogType Placeholder type for a named argument.
         *
         * @param name Name of the parser to display (default = "program").
         * @param description Text to display before the argument help (default = "").
         * @param epilog Text to display after the argument help (default = "").
         */
        template<
            typename NameType = std::nullptr_t,
            typename DescriptionType = std::nullptr_t,
            typename EpilogType = std::nullptr_t
        >
        Parser(
            NameType name = nullptr,
            DescriptionType description = nullptr,
            EpilogType epilog = nullptr
        )
        {
            _name        = pick_if<types::Name>(
                arguments::defaults::default_name,
                name, description, epilog
            ).get();
            if(_name.empty()) _name = "program";
            _description = pick_if<types::Description>(
                arguments::defaults::default_description,
                name, description, epilog
            ).get();
            _epilog      = pick_if<types::Epilog>(
                arguments::defaults::default_epilog,
                name, description, epilog
            ).get();
        }

        /**
         * @brief Adds a new argument to the parser's table of arguments.
         *
         * The argument parser can then parse a given argument vector for matching arguments
         * and return a mapping of processed values.
         *
         * @param argument The argument to add to the parser table.
         * @return {Parser&} A reference to the current parser object, for chaining method calls.
         */
        Parser& add_argument(Argument&& argument);

        template<typename... Arguments>
        Parser& add_subparsers(Arguments&&... parsers)
        {
            return *this;
        }

        template<typename InputIter>
        void parse_args(InputIter begin, InputIter end)
        {

        }
        void parse_args(int argc, const char** argv);
        void parse_args(int argc, char** argv);

        /**
         * @brief Returns a concise help created using argument descriptors.
         *
         * @return {std::string} The generated help.
         */
        std::string help() const noexcept;
    };
}

#endif // ARGPARSE_PARSER_HPP_DEFINED