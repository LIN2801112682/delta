#ifndef _UTILS_FILE_HANDLER_HPP_
#define _UTILS_FILE_HANDLER_HPP_

#include "utils/scope_exit.hpp"
#include <fstream>
#include <string>
#include <vector>

std::string
load_first_line(std::string_view path)
{
    std::ifstream ifs{path, std::ios::in};
    SCOPE_GUARD
    {
        ifs.close();
    };

    std::string str{};
    getline(ifs, str);
    return str;
}

std::vector<std::string>
load_all_line(std::string_view path)
{
    std::ifstream ifs{path, std::ios::in};
    SCOPE_GUARD
    {
        ifs.close();
    };

    std::vector<std::string> all_str{};
    std::string str{};
    while (getline(ifs, str))
    {
        all_str.emplace_back(std::move(str));
    }
    return all_str;
}

#endif