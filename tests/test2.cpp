#include "neu/core.h"
#include "neu/index_manager.h"
#include "utils/scope_exit.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>

static const std::string basic_file_path{"../resources/chr3.txt"};
static const std::string log_file_path{"../resources/query2.txt"};
static const std::string regex_file_path{"../resources/regex-query"};

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
        std::transform(std::begin(basic_str), std::end(basic_str), std::begin(basic_str), tolower);
#if 0
        std::cout << "basic_str: " << basic_str << '\n';
#endif
    }

    neu::index_manager manager{basic_str};
    {
        std::ifstream log_ifs{log_file_path, std::ios::in};
        SCOPE_GUARD
        {
            log_ifs.close();
        };

        std::string log_str{};
        neu::doc_id_type doc_id{1}; // from 1;
        while (getline(log_ifs, log_str))
        {
            std::transform(std::begin(log_str), std::end(log_str), std::begin(log_str), tolower);
#if 0
            std::cout << "log_str: " << log_str << '\n';
#endif

            auto node_stack{neu::backtracking_path(basic_str, log_str)};

            std::vector<neu::node> delta{};
            while (!node_stack.empty())
            {
                auto &n{node_stack.top()};
                node_stack.pop();
                delta.push_back(n);
            }

            manager.add_delta(doc_id, delta);
            ++doc_id;
        }
    }


    {
        std::ifstream regex_ifs{regex_file_path, std::ios::in};
        SCOPE_GUARD
        {
            regex_ifs.close();
        };

        auto begin_time_1 = std::chrono::high_resolution_clock::now();
        std::string regex_str{};
        while (getline(regex_ifs, regex_str))
        {
            std::transform(std::begin(regex_str), std::end(regex_str), std::begin(regex_str), tolower);
#if 0
            std::cout << "regex_str: " << regex_str << '\n';
#endif

            auto begin_time = std::chrono::high_resolution_clock::now();
            auto result{manager.regex_query(regex_str)};
            auto result_count{result.size()};
            //std::cout << "result_count: " << result_count << '\n';
            auto end_time = std::chrono::high_resolution_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
            auto program_times = elapsed_time.count();
            std::cout << "  Location time: " << program_times << " microseconds"
                      << " result_count: " << result_count << '\n';
        }
        auto end_time_1 = std::chrono::high_resolution_clock::now();
        auto elapsed_time_1 = std::chrono::duration_cast<std::chrono::microseconds>(end_time_1 - begin_time_1);
        auto program_times_1 = elapsed_time_1.count();
        std::cout << "Location time: " << program_times_1 << " microseconds\n";
    }

    return 0;
}