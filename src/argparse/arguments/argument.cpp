#include "argparse/arguments/argument.hpp"

std::ostream& argparse::Argument::write_description(
    std::ostream& os, std::string_view description,
    unsigned tty_columns, std::string::size_type consumed
)
{
    std::string::size_type spc_w = (tty_columns / 3), text_w = tty_columns - spc_w,
                           start = 0, end = std::min(text_w, description.size());
    while(start < description.size())
    {
        if(start > 0 or consumed > spc_w)
            os << '\n' << std::setw(spc_w) << ' ' << ' ';
        else
            os << std::setw(spc_w-consumed) << ' ' << ' ';

        // Ensure line break would not break a word midway.
        if(end < description.size())
            while(not std::isspace(description[end]) and description[end] != '-') end--;

        for(auto i=start; i<end; ++i) os.put(description[i]);
        start = end+1; end = std::min(end + text_w, description.size());
    }

    return os;
}