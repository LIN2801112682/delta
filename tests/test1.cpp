#include "neu/core.h"
#include "utils/scope_exit.hpp"
#include <string>
#include <iostream>
#include <fstream>

static const std::string basic_file_path{"../resources/chr0716.txt"};
static const std::string native_file_path{"../resources/query.txt"};

int main()
{
    std::string basic_str{};
    {
        std::ifstream basic_ifs{basic_file_path, std::ios::in};
        SCOPE_GUARD
        {
            basic_ifs.close();
        };

        getline(basic_ifs, basic_str);
#if 0
        std::cout << "basic_str: " << basic_str << '\n';
#endif
    }

    {
        std::ifstream log_ifs{native_file_path, std::ios::in};
        SCOPE_GUARD
        {
            log_ifs.close();
        };

        std::string log_str{};
        while (getline(log_ifs, log_str))
        {
#if 0
            std::cout << "log_str: " << log_str << '\n';
#endif

            auto node_stack{neu::backtracking_path(basic_str, log_str)};
            auto merged_str{neu::merge_str_by_node_stack(basic_str, std::move(node_stack))};
#if 0
            std::cout << "merged_str: " << merged_str << '\n';
#endif
            if (merged_str == log_str)
            {
                std::cout << "merge correct\n";
            }
            else
            {
                std::cout << "merge error\n";
            }
        }
    }

    return 0;
}