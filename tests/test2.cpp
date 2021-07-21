#include "neu/core.h"
#include "neu/node.h"
#include "neu/index_manager.h"
#include "utils/scope_exit.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>

#define DEBUG_PRINT 0
#define TEST_MERGE 0

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

    neu::index_manager manager{basic_str};
    neu::index_manager::doc_id_type doc_id{1};
    for (const auto &delta : delta_vec)
    {
        manager.push_token_by_id_and_delta(doc_id, delta);
        ++doc_id;
    }

    return 0;
}