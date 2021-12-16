#include "neu/core.h"
#include "utils/file_handler.hpp"
#include <iostream>

static constexpr auto basic_file_path{"../resources/chr3.txt"};
static constexpr auto native_file_path{"../resources/query2.txt"};
static constexpr auto save_file_path{"../resources/save.txt"};

int main()
{
    auto basic_str{load_first_line(basic_file_path)};

    auto native_str_vec{load_all_line(native_file_path)};

    std::ofstream ofs{save_file_path, std::ios::out};
    SCOPE_GUARD
    {
        ofs.close();
    };

    for (const auto &native_str : native_str_vec)
    {
        auto delta{neu::extract_delta(basic_str, native_str)};
        auto save_str{neu::serialize(delta)};
        ofs << save_str << '\n';
    }
    std::cout << "save suc.\n";

    auto save_str_vec{load_all_line(save_file_path)};
    std::cout << "load suc.\n";

    if (native_str_vec.size() != save_str_vec.size())
    {
        std::cerr << "native_size: " << native_str_vec.size() << ' '
                  << "save_size: " << save_str_vec.size() << '\n';
    }

    int i = 0;
    for (const auto &native_str : native_str_vec)
    {
        auto delta{neu::deserialize(save_str_vec[i++])};
        auto merged_str{neu::merge_str(basic_str, delta)};
        if (native_str != merged_str)
        {
            std::cerr << "basic_str: " << basic_str << '\n'
                      << "native_str: " << native_str << '\n'
                      << "merged_str: " << merged_str << '\n';
            break;
        }
    }

    std::cout << "merge correct.\n";
    return 0;
}