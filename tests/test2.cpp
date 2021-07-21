#include "neu/core.h"
#include "neu/node.h"
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

    int log_size{0};
    {
        std::ifstream log_ifs{log_file_path, std::ios::in};
        SCOPE_GUARD
        {
            log_ifs.close();
        };
        std::string log_str{};
        while (getline(log_ifs, log_str))
        {
            ++log_size;
        }
    }
#if DEBUG_PRINT
    std::cout << "log_size: " << log_size << '\n';
#endif

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
#if TEST_MERGE
        auto merged_str = neu::merge_str(basic_str, std::move(delta));
#if DEBUG_PRINT
        std::cout << "merged_str: " << merged_str << '\n';
#endif
        assert(merged_str == log_str);
        std::cout << "merge correct\n";
#else

#endif
    }
    return 0;
}