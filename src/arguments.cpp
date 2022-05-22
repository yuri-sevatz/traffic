#include <cstdlib>
#include <iostream>

#include "arguments.hpp"

Arguments::Arguments(std::string_view name, const std::vector<std::string_view> & args) :
    name(name)
{
    for (auto arg = args.begin(); arg != args.end(); ++arg) {
        if (*arg == "--help" || *arg == "-h") {
            help();
            std::quick_exit(EXIT_SUCCESS);
        } else {
            std::cerr << "invalid positional argument: \"" << *arg << "\", aborting" << std::endl;
            std::quick_exit(EXIT_FAILURE);
        }
    }
}

void Arguments::help() {
    std::cerr
        << "Usage: " << name << "[-h]\n"
        << '\n'
        << "Traffic Light Simulator\n"
        << '\n'
        << "Options:\n"
        << "  -h, --help            show this help message and exit\n"
        << '\n'
        << std::flush;
}
