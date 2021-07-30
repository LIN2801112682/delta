#include "neu/core.h"
#include "utils/file_handler.hpp"
#include <iostream>
#include <string>

static const std::string basic_file_path{"../resources/chr3.txt"};
static const std::string native_file_path{"../resources/query2.txt"};

int main()
{
    auto basic_str{load_first_line(basic_file_path)};
#if 1
    std::cout << "basic_str: " << basic_str << '\n';
#endif

    auto native_str_vec{load_all_line(native_file_path)};
    for (const auto &native_str : native_str_vec)
    {
#if 1
        std::cout << "native_str: " << native_str << '\n';
#endif
        auto delta{neu::extract_delta(basic_str, native_str)};
        auto merged_str{neu::merge_str(basic_str, delta)};
#if 1
        std::cout << "merged_str: " << merged_str << '\n';
#endif
        if (native_str != merged_str)
        {
            std::cerr << "merge error\n";
            assert(false);
        }
    }
    return 0;
}