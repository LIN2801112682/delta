#include "neu/core.h"
#include "neu/delta_index_manager.h"
#include "neu/native_index_manager.h"
#include "utils/file_handler.hpp"
#include "utils/scope_exit.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>

static constexpr auto basic_file_path{"../resources/chr3.txt"};
static constexpr auto native_file_path{"../resources/query2.txt"};
static constexpr auto regex_file_path{"../resources/regex-query"};

int main()
{
    auto basic_str{load_first_line(basic_file_path)};
    std::transform(
        std::begin(basic_str),
        std::end(basic_str),
        std::begin(basic_str),
        tolower);

    auto native_str_vec{load_all_line(native_file_path)};
    std::for_each(
        std::begin(native_str_vec),
        std::end(native_str_vec),
        [](auto &native_str)
        {
            std::transform(
                std::begin(native_str),
                std::end(native_str),
                std::begin(native_str),
                tolower);
        });

    auto is_es_dlm = [](const neu::ch_t ch)
    {
        switch (ch)
        {
        case ' ':
        case '.':
        case '\'':
        case '\"':
        case '(':
        case ')':
            return true;
        default:
            return false;
        }
    };

    neu::delta_index_manager_t delta_index_manager{basic_str, is_es_dlm};
    neu::native_index_manager_t native_index_manager{is_es_dlm};
    neu::doc_id_t doc_id{0};
    std::for_each(
        std::cbegin(native_str_vec),
        std::cend(native_str_vec),
        [&basic_str,
         &delta_index_manager,
         &native_index_manager,
         &doc_id](const auto &native_str)
        {
            delta_index_manager.add_delta_index(doc_id, neu::extract_delta(basic_str, native_str));
            native_index_manager.add_native_index(doc_id, native_str);
            doc_id++;
        });

    auto regex_str_vec{load_all_line(regex_file_path)};
    std::for_each(
        std::begin(regex_str_vec),
        std::end(regex_str_vec),
        [](auto &regex_str)
        {
            std::transform(
                std::begin(regex_str),
                std::end(regex_str),
                std::begin(regex_str),
                tolower);
        });

    std::for_each(
        std::cbegin(regex_str_vec),
        std::cend(regex_str_vec),
        [&delta_index_manager,
         &native_index_manager](const auto &regex_str)
        {
#if 0
            auto begin_time{std::chrono::high_resolution_clock::now()};
            SCOPE_GUARD
            {
                auto end_time = std::chrono::high_resolution_clock::now();
                auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
                auto program_times = elapsed_time.count();
                std::cout << " time: " << program_times << " microseconds\n";
            };
#endif
            auto delta_result{delta_index_manager.regex_query(regex_str)};
#if 0
            std::cout << "delta_result_count: " << delta_result.size() << '\n';
#endif
#if 1
            auto native_result{native_index_manager.regex_query(regex_str)};
            if (delta_result != native_result)
            {
                std::cerr << "delta_result_count: " << delta_result.size() << '\n';
                std::cerr << "native_result_count: " << native_result.size() << '\n';
            }
#endif
        });
    std::cout << "index correct.\n";
    return 0;
}