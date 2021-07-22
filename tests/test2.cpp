#include "neu/core.h"
#include "neu/index_manager.h"
#include "utils/scope_exit.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

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

            manager.push_delta(delta);
        }
    }

    {
        std::ifstream regex_ifs{regex_file_path, std::ios::in};
        SCOPE_GUARD
        {
            regex_ifs.close();
        };

        std::string regex_str{};
        while (getline(regex_ifs, regex_str))
        {
            std::transform(std::begin(regex_str), std::end(regex_str), std::begin(regex_str), tolower);
#if 0
            std::cout << "regex_str: " << regex_str << '\n';
#endif

            auto result{manager.regex_query(regex_str)};
            auto result_count{0};
            for (const auto &offset_uset : result)
            {
                if (!offset_uset.empty())
                {
                    ++result_count;
                }
            }
            std::cout << "result_count: " << result_count << '\n';
        }
    }

    return 0;
}