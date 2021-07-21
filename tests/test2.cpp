#include "neu/core.h"
#include "neu/node.h"
#include "neu/index_manager.h"
#include "utils/scope_exit.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>

#define DEBUG_PRINT 0

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
    }
#if DEBUG_PRINT
    std::cout << "basic_str: " << basic_str << '\n';
#endif

    std::vector<std::vector<neu::node>> delta_vec;
    {
        std::ifstream log_ifs{log_file_path, std::ios::in};
        SCOPE_GUARD
        {
            log_ifs.close();
        };

        std::string log_str{};
        while (getline(log_ifs, log_str))
        {
#if DEBUG_PRINT
            std::cout << "log_str: " << log_str << '\n';
#endif
            auto delta{neu::backtracking_path(basic_str, log_str)};
            std::vector<neu::node> vdelta{};
            while (!delta.empty())
            {
                vdelta.emplace_back(delta.top());
                delta.pop();
            }
            delta_vec.emplace_back(vdelta);
        }
    }

    auto init_begin_time = std::chrono::high_resolution_clock::now();
    neu::index_manager manager{basic_str};
    for (const auto &delta : delta_vec)
    {
        manager.push_delta(delta);
    }
    auto init_end_time = std::chrono::high_resolution_clock::now();
    auto init_elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(init_end_time - init_begin_time);
    auto init_program_times = init_elapsed_time.count();
    std::cout << "Init sum time: " << init_program_times << '\n';

    std::vector<std::string> regex_str_vec{};
    {
        std::ifstream regex_ifs{regex_file_path, std::ios::in};
        SCOPE_GUARD
        {
            regex_ifs.close();
        };

        std::string regex_str{};
        while (getline(regex_ifs, regex_str))
        {
            regex_str_vec.emplace_back(regex_str);
        }
    }

    auto begin_time = std::chrono::high_resolution_clock::now();
    for (const auto &regex_str : regex_str_vec)
    {
#if DEBUG_PRINT
        std::cout << "regex_str: " << regex_str << '\n';
#endif
        auto begin_time = std::chrono::high_resolution_clock::now();
        auto result{manager.regex_query(regex_str)};
        int sum{0};
        for (const auto &offset_set : result)
        {
            sum += offset_set.size();
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
        auto program_times = elapsed_time.count();
        std::cout << "  Regex query time: " << program_times
                  << ", query sum: " << sum << '\n';
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Regex query sum time: " << program_times << '\n';

    return 0;
}