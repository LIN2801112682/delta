#include "neu/core.h"
#include "utils/file_handler.hpp"
#include <iostream>

static constexpr auto basic_file_path{"../resources/chr3.txt"};
static constexpr auto native_file_path{"../resources/query2.txt"};

int main()
{
    auto basic_str{load_first_line(basic_file_path)};

    auto native_str_vec{load_all_line(native_file_path)};

    for (const auto &native_str : native_str_vec)
    {
        auto delta{neu::extract_delta(basic_str, native_str)};
        auto merged_str{neu::merge_str(basic_str, delta)};
        if (native_str != merged_str)
        {
            std::cerr << "basic_str: " << basic_str << '\n'
                      << "native_str: " << native_str << '\n'
                      << "merged_str: " << merged_str << '\n';
        }
    }

    std::cout << "merge correct.\n";
    return 0;
}