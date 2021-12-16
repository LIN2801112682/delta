#include "neu/core.h"
#include "utils/file_handler.hpp"
#include <iostream>
#include <sstream>

static constexpr auto basic_file_path{"../resources/chr3.txt"};
static constexpr auto native_file_path{"../resources/query2.txt"};
static constexpr auto save_file_path{"../resources/save.txt"};

int main()
{
    auto basic_str{load_first_line(basic_file_path)};

    auto native_str_vec{load_all_line(native_file_path)};

    std::ofstream ofs{save_file_path, std::ios::out};
    for (const auto &native_str : native_str_vec)
    {
        auto delta{neu::extract_delta(basic_str, native_str)};
        ofs << delta;
    }
    ofs.close();

    int i{0};
    std::ifstream ifs{save_file_path, std::ios::in};
    while (ifs.peek() != EOF)
    {
        neu::delta_t delta{};
        ifs >> delta;
        auto merged_str{neu::merge_str(basic_str, delta)};
        const auto &native_str{native_str_vec[i++]};
        if (native_str != merged_str)
        {
            std::cerr << "basic_str: " << basic_str << '\n'
                      << "native_str: " << native_str << '\n'
                      << "merged_str: " << merged_str << '\n';
            break;
        }
    }
    ifs.close();

    if (i != native_str_vec.size())
    {
        std::cerr << "i: " << i
                  << "size: " << native_str_vec.size()
                  << '\n';
    }
    else
    {
        std::cout << "save and load correct, i: " << i << '\n';
    }

    return 0;
}