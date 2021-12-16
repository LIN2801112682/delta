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
        auto deserialize_delta{neu::deserialize(save_str)};
        auto merged_str{neu::merge_str(basic_str, deserialize_delta)};
        if (native_str != merged_str)
        {
            std::cerr << "delta: " << '\n';

            break;
        }
    }

    std::cout << "merge correct.\n";
    return 0;
}