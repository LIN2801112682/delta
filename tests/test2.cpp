#include "neu/core.h"
#include "neu/delta_index_manager.h"
#include "utils/scope_exit.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>

static const std::string basic_file_path{"../resources/chr3.txt"};
static const std::string native_file_path{"../resources/query2.txt"};
static const std::string regex_file_path{"../resources/regex-query"};

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
        std::transform(std::begin(basic_str), std::end(basic_str), std::begin(basic_str), tolower);
#if 0
        std::cout << "basic_str: " << basic_str << '\n';
#endif
    }

    neu::delta_index_manager manager{basic_str};
    {
        std::ifstream native_ifs{native_file_path, std::ios::in};
        SCOPE_GUARD
        {
            native_ifs.close();
        };

        neu::str_t native_str{};
        neu::doc_id_t doc_id{neu::delta_index_manager::k_basic_doc_id};
        while (getline(native_ifs, native_str))
        {
            std::transform(std::begin(native_str), std::end(native_str), std::begin(native_str), tolower);
#if 0
            std::cout << "native_str: " << native_str << '\n';
#endif

            auto delta{neu::extract_delta(basic_str, native_str)};
            manager.add_delta_index(++doc_id, std::move(delta));
        }
    }

    std::vector<neu::str_t> regex_str_vec{};
    {
        std::ifstream regex_ifs{regex_file_path, std::ios::in};
        SCOPE_GUARD
        {
            regex_ifs.close();
        };

        neu::str_t regex_str{};
        while (getline(regex_ifs, regex_str))
        {
            std::transform(std::begin(regex_str), std::end(regex_str), std::begin(regex_str), tolower);
#if 0
            std::cout << "regex_str: " << regex_str << '\n';
#endif
            regex_str_vec.emplace_back(regex_str);
        }
    }

    for (const auto &regex_str : regex_str_vec)
    {
        {
            auto begin_time{std::chrono::high_resolution_clock::now()};
            SCOPE_GUARD
            {
                auto end_time = std::chrono::high_resolution_clock::now();
                auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
                auto program_times = elapsed_time.count();
                std::cout << " time: " << program_times << " microseconds\n";
            };

            auto result{manager.regex_query(regex_str)};
            std::cout << "  delta_result_count: " << result.size();
        }
    }

    return 0;
}