#include "neu/index_manager.h"

#include "neu/core.h"

#include <sstream>
#include <regex>
#include <string_view>
#include <algorithm>

namespace neu
{
    auto
    isSeparator(const ch_t ch)
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
    }

    void
    add_general_inverted_index(inverted_index_t &inverted_index, const doc_id_t doc_id, const str_t &str)
    {
        position_t position{0};
        str_idx_t begin, end;
        bool is_find_begin{false};
        for (str_idx_t i{0}; i < str.size(); ++i)
        {
            const ch_t &ch{str[i]};
            if (!isSeparator(ch))
            {
                if (!is_find_begin)
                {
                    is_find_begin = true;
                    begin = i;
                }
                end = i;
            }
            if (isSeparator(ch) || i == str.size() - 1 && is_find_begin)
            {
                is_find_begin = false;
                str_t token{str.substr(begin, end - begin + 1)};

                auto &doc_id_map{inverted_index[token]};
                auto &position_offset_vec{doc_id_map[doc_id]};

                position_offset_vec.emplace_back(
                    position_offset_t{
                        position++,
                        offset_t{
                            begin,
                            end,
                        }});
            }
        }
    }

    index_manager::index_manager(const str_t &basic_str)
        : basic_str_{basic_str}
    {
        add_general_inverted_index(basic_inverted_index_, k_basic_doc_id, basic_str_);
    }

    void
    index_manager::add_delta_invert_index(const doc_id_t doc_id, const delta_type &delta)
    {
        auto merged_str{merge_str_by_delta(basic_str_, delta)};
        add_general_inverted_index(delta_inverted_index_, doc_id, merged_str);
    }

    void
    index_manager::add_native_inverted_index(const doc_id_t doc_id, const str_t &native_str)
    {
        add_general_inverted_index(native_inverted_index_, doc_id, native_str);
    }

    auto
    regex_query_general_invert_index(const inverted_index_t &inverted_index, const str_t &regex_str)
    {
        doc_id_uset_t result{};
        std::regex pattern{regex_str};
        std::smatch regex_result;
        for (const auto &[token, doc_id_umap] : inverted_index)
        {
            if (regex_match(token, regex_result, pattern))
            {
                for (const auto &[doc_id, _] : doc_id_umap)
                {
                    result.emplace(doc_id);
                }
            }
        }
        return result;
    }

    doc_id_uset_t
    index_manager::regex_query_delta_invert_index(const str_t &regex_str)
    {
        return regex_query_general_invert_index(delta_inverted_index_, regex_str);
    }

    doc_id_uset_t
    index_manager::regex_query_native_invert_index(const str_t &regex_str)
    {
        return regex_query_general_invert_index(native_inverted_index_, regex_str);
    }
};