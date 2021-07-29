#include "neu/core.h"
#include "utils/scope_exit.hpp"
#include <string>
#include <iostream>
#include <fstream>

static const std::string basic_file_path{"../resources/chr3.txt"};
static const std::string native_file_path{"../resources/query2.txt"};

int main()
{
    neu::str_t basic_str{};
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
        std::ifstream native_ifs{native_file_path, std::ios::in};
        SCOPE_GUARD
        {
            native_ifs.close();
        };

        neu::str_t native_str{};
        while (getline(native_ifs, native_str))
        {
#if 0
            std::cout << "native_str: " << native_str << '\n';
#endif

            auto delta{neu::extract_delta(basic_str, native_str)};
            auto merged_str{neu::merge_str(basic_str, delta)};
#if 0
            std::cout << "merged_str: " << merged_str << '\n';
#endif
            if (merged_str == native_str)
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