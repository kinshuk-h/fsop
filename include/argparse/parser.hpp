/**
 * @file parser.hpp
 * @author Kinshuk Vasisht (kinshuk.mcs21@cs.du.ac.in, RN: 19)
 * @brief Defines the parser class which performs argument parsing using registered arguments.
 * @version 1.0
 * @date 2022-05-28
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef ARGPARSE_PARSER_HPP_INCLUDED
#define ARGPARSE_PARSER_HPP_INCLUDED

#include <iostream>                // std::cout

#include "common.hpp"              // argparse::types, argparse::arguments
#include "arguments/argument.hpp"  // argparse::Argument
#include "arguments/switch.hpp"    // argparse::Switch

namespace argparse
{
    class Parser;

    /**
     * @brief Creates an argument type to hold subparsers as a mutually exclusive group.
     *
     *      Subparsers allow splitting of arguments and functionality across subcommands.
     *      Each subparsers is by itself a parser and is allowed to have its own set of argument objects.
     *
     *      The parent parser delegates the parsing of remaining options to the subparser whose name
     *      appears on the command line.
     */
    class SubparserSet : public Argument
    {
        std::unordered_map<std::string, std::shared_ptr<Parser>> _parsers;

    public:
        /**
         * @brief Construct a new Subparser Set object
         *
         * @tparam Parsers Types of parser objects.
         * @param parent Parent program parser for the subparsers.
         * @param dest Destination name for the value indicating the selected subparser's name in the parsed table.
         * @param parsers Parsers to add as subparsers.
         */
        template<typename... Parsers>
        SubparserSet(Parser& parent, std::string dest, Parsers&&... parsers)
        : Argument(arguments::name = dest, arguments::dest = dest)
        {
            ( _parsers.emplace(parsers.prog(), std::make_unique<Parser>(parsers.parent(parent))), ... );
            _required = true; _positional = true;
        }

        std::string usage     ()                          const noexcept override;
        std::string descriptor(unsigned tty_columns = 60) const noexcept override;
        /**
         * @brief Creates a copy of the argument as a unique_ptr for polymorphic usage.
         */
        std::unique_ptr<Argument> clone() const override
        {
            return std::make_unique<SubparserSet>(*this);
        }
        range::iterator parse_args(
            range::iterator begin, range::iterator end,
            types::result_map& values
        ) const override;
    };

    /**
     * @brief Represents an argument parser, which can parse argument values from arbitrary ranges
     * using provided argument objects describing parsing information.
     *
     * Objects of this type can hold argument objects (switches, optional & positional arguments) as
     * well as complex structures such as mutually exclusive groups and subparsers.
     *
     * The parser can then parse values for specified arguments from any range specified by
     * its start and end iterators.
     *
     * Further, the parser can generate a standard help description compiling descriptions of
     * various arguments together.
     */
    class Parser
    {
        std::string _name, /**< Name of the parser/program to display. */
                    _description, /**< Text to display before the argument help. */
                    _epilog; /**< Text to display after the argument help. */
        std::ostream& _out_stream; /**< Output stream to use for operations. */

        /** Hash Table (unordered_map) of optional arguments to be parsed. */
        std::unordered_map<std::string, std::shared_ptr<Argument>> _optionals;
        /** Vector of positional arguments to be parsed. */
        std::vector<std::unique_ptr<Argument>> _positionals;

        Parser* _parent = nullptr; /**< Parent parser for the parser. */

    public:
        /**
         * @brief Writes a formatted description to the given output stream.
         *
         * This function is for internal formatting of parser descriptions
         * (such as when generating the argument help).
         *
         * @param os The output stream to write the description to.
         * @param description The description to write.
         * @param tty_columns Maximum number of terminal columns/characters to span per line.
         * @return std::ostream& Reference to the output stream for cascading operations.
         */
        static std::ostream& write_description(
            std::ostream& os,
            std::string_view description,
            unsigned tty_columns = 60
		);

        /**
         * @brief Construct a new Parser object
         *
         * @tparam NameType Placeholder type for a named argument.
         * @tparam DescriptionType Placeholder type for a named argument.
         * @tparam EpilogType Placeholder type for a named argument.
         * @tparam OutputStreamType Placeholder type for a named argument.
         *
         * @param name Name of the parser to display (default = "program").
         * @param description Text to display before the argument help (default = "").
         * @param epilog Text to display after the argument help (default = "").
         * @param output_stream Stream to use for parser output (default = std::cout).
         */
        template<
            typename NameType = std::nullptr_t,
            typename DescriptionType = std::nullptr_t,
            typename EpilogType = std::nullptr_t,
            typename OutputStreamType = std::nullptr_t
        >
        Parser(
            NameType name = nullptr,
            DescriptionType description = nullptr,
            EpilogType epilog = nullptr,
            OutputStreamType output_stream = nullptr
        ) : _out_stream(
            pick_if<types::OutputStream>(
                types::OutputStream(std::cout),
                name, description, epilog, output_stream
            ).get()
        )
        {
            _name        = pick_if<types::Name>(
                arguments::defaults::default_name,
                name, description, epilog, output_stream
            ).get();
            if(_name.empty()) _name = "program";
            _description = pick_if<types::Description>(
                arguments::defaults::default_description,
                name, description, epilog, output_stream
            ).get();
            _epilog      = pick_if<types::Epilog>(
                arguments::defaults::default_epilog,
                name, description, epilog, output_stream
            ).get();

            add_argument(
                Switch {
                    arguments::name = "help", arguments::alias = "h",
                    arguments::help = "show this help message and exit"
                }
            );
        }

        /**
         * @brief Construct a new Parser object (move construct).
         * This constructor moves content off the rvalue reference, effectively
         * saving on allocations (only copying of pointers and other internal
         * references is performed).
         *
         * @param parser Temporary parser object to move content from.
         */
        Parser(Parser&& parser) = default;

        /**
         * @brief Construct a new Parser object (copy construct).
         * This constructor performs a deep copy of the given lvalue reference,
         * by creating copies of all arguments and other members, which performs
         * multiple allaocations as required.
         *
         * @param parser Parser object to copy content from.
         */
        Parser(const Parser& parser)
        : _name(parser._name), _description(parser._description),
          _epilog(parser._epilog), _out_stream(parser._out_stream),
          _parent(parser._parent)
        {
            for(const auto& positional : parser._positionals)
                _positionals.push_back(positional->clone());
            for(const auto& [ name, option ] : parser._optionals)
                _optionals.emplace(name, option->clone());
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

        /**
         * @brief Adds multiple arguments to the parser's table of arguments.
         *
         * @tparam Arguments Types for individual argument objects.
         * @param arguments List of arguments to add.
         * @return {Parser&} A reference to the current parser object, for chaining method calls.
         */
        template<typename... Arguments>
        Parser& add_arguments(Arguments&&... arguments)
        {
            (add_argument(std::forward<Arguments>(arguments)), ...);
            return *this;
        }

        /**
         * @brief Adds subparsers to the parser's table of arguments.
         *
         * Subparsers allow splitting of arguments and functionality across subcommands.
         * Each subparsers is by itself a parser and is allowed to have its own set of argument objects.
         *
         * @tparam Parsers Types for individual parser objects.
         * @param dest Destination name for the value indicating the name of the selected parser.
         * @param parsers List of parsers to add.
         * @return {Parser&} A reference to the current parser object, for chaining method calls.
         */
        template<typename... Parsers>
        Parser& add_subparsers(std::string dest, Parsers&&... parsers)
        {
            add_argument(
                SubparserSet{ *this, dest, std::forward<Parsers>(parsers)... }
            );
            return *this;
        }

        /**
         * @brief Parses arguments from a given range defined by start and end iterators.
         *
         *  This function performs the necessary parsing to bind values for arguments.
         *  The values corresponding to arguments is returned as an `unordered_map`
         *  mapping destination names to the `std::any` type for holding values of arbitrary types.
         *
         *  This function has a signature similar to those of arguments, which makes this overload
         *  useful for use with subparsers for parsing parent and child arguments.
         *
         * @param begin The iterator to the beginning of the range of arguments.
         * @param end The iterator to the end of the range of arguments.
         * @param values Result map to store argument values.
         *
         * @return {Argument::range::iterator} iterators to the start
         *      of the remaining sequence.
         *
         * @throws {parse_error} describes inconsistencies present in the range
         *      corresponding to argument requirements.
         */
        Argument::range::iterator parse_args(
            Argument::range::iterator begin, Argument::range::iterator end,
            types::result_map& values
        );
        /**
         * @brief Parses arguments from a given range defined by start and end iterators.
         *
         *  This function performs the necessary parsing to bind values for arguments.
         *  The values corresponding to arguments is returned as an `unordered_map`
         *  mapping destination names to the `std::any` type for holding values of arbitrary types.
         *
         *  This is an alternate variant of the parse_args function where the map is returned instead
         *  of the data being saved inside an lvalue argument.
         *
         * @tparam InputIter Type of the iterator of the collection. Must satisfy requirements for an input iterator.
         *
         * @param begin The iterator to the beginning of the range of arguments.
         * @param end The iterator to the end of the range of arguments.
         *
         * @return {types::result_map} map object containing values for parsed arguments.
         *
         * @throws {parse_error} describes inconsistencies present in the range
         *      corresponding to argument requirements.
         */
        template<typename InputIter>
        types::result_map parse_args(InputIter begin, InputIter end)
        {
            types::result_map values;
            Argument::range argv { begin, end };
            parse_args(argv.begin(), argv.end(), values);
            return values;
        }
        /**
         * @brief Parses a set of arguments from a given argument vector referred by the size
         *        and pointer to the first element of the vector.
         *
         *      This function is a wrapper over the general `parse_args` for arbitrary collections,
         *      for convenience to use with the argument vector parameters accepted by `main` in the C/C++ standard.
         *
         * @param argc the argument count: number of arguments to parse.
         * @param argv the argument vector: collection of arguments (strings).
         *
         * @return {types::result_map} map object containing values for parsed arguments.
         *
         * @throws {parse_error} describes inconsistencies present in the range
         *      corresponding to argument requirements.
         */
        types::result_map parse_args(int argc, const char** argv)
        {
            return parse_args(argv+1, argv+argc);
        }
        /**
         * @brief Parses a set of arguments from a given argument vector referred by the size
         *        and pointer to the first element of the vector.
         *
         *      This function is a wrapper over the general `parse_args` for arbitrary collections,
         *      for convenience to use with the argument vector parameters accepted by `main` in the C/C++ standard.
         *
         * @param argc the argument count: number of arguments to parse.
         * @param argv the argument vector: collection of arguments (strings).
         *
         * @return {types::result_map} map object containing values for parsed arguments.
         *
         * @throws {parse_error} describes inconsistencies present in the range
         *      corresponding to argument requirements.
         */
        types::result_map parse_args(int argc, char** argv)
        {
            return parse_args(argv+1, argv+argc);
        }

        /**
         * @brief Returns a concise help created using argument descriptors.
         *
         * @param tty_columns maximum number of terminal columns (characters) to span (default=60).
         *
         * @return {std::string} The generated help.
         */
        std::string help(unsigned tty_columns = 60) const noexcept;
        /**
         * @brief Returns a concise help created using argument descriptors.
         *
         * @param tty_columns maximum number of terminal columns (characters) to span (default=60).
         *
         * @return {std::string} The generated help.
         */
        std::string usage(unsigned tty_columns = 60) const noexcept;

        /** Name of the parser/program to display. */
        const std::string&  name       () const noexcept { return _name; }
        /** Name of the parser/program to display. */
        const std::string&  prog       () const noexcept { return _name; }
        /** Text to display before the argument help. */
        const std::string&  description() const noexcept { return _description; }
        /** Text to display after the argument help. */
        const std::string&  epilog     () const noexcept { return _epilog; }
        /** Output stream to use for operations. */
        std::ostream&       ostream    () const noexcept { return _parent != nullptr ? _parent->ostream() :  _out_stream; }
        /** Identifier of the parser (fully qualified name with the entire hierarchy) */
        std::string         identifier () const noexcept
        {
            return (_parent != nullptr ? (_parent->identifier() + " ").c_str() : "") + _name;
        }
        /** Parent parser for the parser. */
        auto                parent     () const noexcept { return _parent; }

        /** Sets the parent for the parser, for which the current object acts as a subcommand. */
        Parser& parent(Parser& _parent) noexcept
        { this->_parent = &_parent; return *this; }
    };
}

#endif // ARGPARSE_PARSER_HPP_DEFINED