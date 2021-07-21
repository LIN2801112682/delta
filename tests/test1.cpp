#include "neu/core.h"
#include "neu/node.h"
#include "utils/scope_exit.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>

#define DEBUG_PRINT 1
#define TEST_MERGE 1

static const std::string basic_file_path{"../resources/chr0716.txt"};
static const std::string log_file_path{"../resources/query.txt"};
static const std::string delta_file_path{"../resources/index0716.dlt"};
static constexpr int k_byte_to_bits{8};
static constexpr int k_size_bytes{2};
static constexpr int k_int_bytes{3};

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

    std::ofstream delta_ofs{delta_file_path, std::ios::out | std::ios::binary};
    SCOPE_GUARD
    {
        delta_ofs.close();
    };

    std::bitset<k_size_bytes * k_byte_to_bits> log_size_bs(log_size);
    delta_ofs.write((char *)&log_size_bs, k_size_bytes);

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
        assert(merged_str == log_str);
        std::cout << "merge correct\n";
#endif
#else

        int delta_size{static_cast<int>(delta.size())};
        std::bitset<k_size_bytes * k_byte_to_bits> delta_size_bs(delta_size);
        delta_ofs.write((char *)&delta_size_bs, k_size_bytes);

        while (!delta.empty())
        {
            auto d{delta.top()};
            delta.pop();
#if DEBUG_PRINT
            std::cout << "node: "
                      << "low: " << d.low_ << " high: " << d.high_
                      << " type: " << static_cast<int>(d.type_)
                      << " content: " << d.content_ << '\n';
#endif
            std::bitset<k_int_bytes * k_byte_to_bits> b(d.low_);
            b <<= 11; // todo: tokenize
            std::bitset<k_int_bytes * k_byte_to_bits> hb(d.high_);
            b ^= hb;
            b <<= 2; // todo: tokenize
            std::bitset<k_int_bytes * k_byte_to_bits> tb(static_cast<int>(d.type_));
            b ^= tb;
            delta_ofs.write((char *)&b, k_int_bytes);

            int content_size{static_cast<int>(d.content_.size())};
            std::bitset<k_size_bytes * k_byte_to_bits> content_size_bs(content_size);
            delta_ofs.write((char *)&content_size_bs, k_size_bytes);
            delta_ofs.write((char *)d.content_.c_str(), content_size);
        }
#endif
    }
}