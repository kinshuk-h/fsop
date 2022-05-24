#include <iostream>

#include "argparse.hpp"

using namespace argparse::arguments;

int main(int argc, const char** argv)
{
    // argparse::Parser parser { name = argv[0] };

    // parser.add();

    auto arg = argparse::Switch {
        name = "debug", alias = "d", //alias = "verbose"
        negated = true,
    };

    //auto args = parser.parse_args(argc, argv);

    //std::cout << args << "\n";
}