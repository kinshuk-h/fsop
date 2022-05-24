#ifndef ARGPARSE_PARSER_HPP_INCLUDED
#define ARGPARSE_PARSER_HPP_INCLUDED

#include "common.hpp"
#include "arguments/argument.hpp"

namespace argparse
{
    class Parser
    {
    private:
        std::string _name,
                    _description,
                    _epilog;

        // std::unordered_map<std::string, Argument> _optional;
        // std::vector<>;

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
        );

        /**
         * @brief Adds a new argument to the parser's table of arguments.
         *
         * 
         *
         * @param argument 
         * @return {Parser&} A reference to the current parser object, for chaining method calls.
         */
        Parser& add(const Argument& argument);

        template<typename... Arguments>
        Parser& add_subparsers(Arguments... parsers);

        template<typename InputIter>
        void parse_args(InputIter begin, InputIter end);
        void parse_args(int argc, const char** argv);
        void parse_args(int argc, char** argv);
    };
}

#endif // ARGPARSE_PARSER_HPP_DEFINED