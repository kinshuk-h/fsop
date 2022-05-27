#include "argparse/arguments/switch.hpp"

#include <cctype>       // std::isalpha
#include <algorithm>    // std::min, std::swap

std::string argparse::Switch::usage() const noexcept
{
    if(not _alias.empty() and _alias.size() < _name.size())
        return (_alias.size() == 1 ? "[-" : "[--") + _alias + "]";
    return (_name.size() == 1 ? "[-" : "[--") + _name + "]";
}

std::string argparse::Switch::descriptor() const noexcept
{
    std::string_view nm1 = _name, nm2 = _alias;
    if(nm1.size() > nm2.size()) std::swap(nm1, nm2);

    std::string _descriptor = "\t";
    if(not nm1.empty())
    {
        _descriptor += (nm1.size() == 1 ? "-" : "--");
        _descriptor += nm1; _descriptor += " / ";
    }
    _descriptor += (nm2.size() == 1 ? "-" : "--");
    _descriptor += nm2; _descriptor += "\t\t";

    size_t start = 0, end = std::min(40ULL, _description.size());
    while(start < _description.size())
    {
        while(std::isalnum(_description[end])) end--;
        _descriptor.append(_description, start, end-start+1);
        _descriptor += "\n\t\t\t\t";
        start = end+1; end = std::min(end+40, _description.size());
    }

    return _descriptor;
}