#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include <optional>
#include <string_view>
#include <vector>

struct Arguments {
    Arguments() = delete;
    Arguments(std::string_view name, const std::vector<std::string_view> & args);

    std::string_view name;

private:
    void help();
};

#endif // ARGUMENTS_HPP
