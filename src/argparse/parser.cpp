#include "argparse/parser.hpp"

template<
    typename NameType = std::nullptr_t,
    typename DescriptionType = std::nullptr_t,
    typename EpilogType = std::nullptr_t
>
argparse::Parser::Parser(
    NameType name = nullptr,
    DescriptionType description = nullptr,
    EpilogType epilog = nullptr
)
{
    _name        = pick_if<types::Name>(arguments::defaults::name       , name, description, epilog).get();
    if(_name.empty()) name = "program";
    _description = pick_if<types::Name>(arguments::defaults::description, name, description, epilog).get();
    _epilog      = pick_if<types::Name>(arguments::defaults::epilog     , name, description, epilog).get();
}