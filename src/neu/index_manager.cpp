#include "neu/index_manager.h"
#include <functional>
#include <sstream>
#include <regex>

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
    split_str(str_v_t str,  std::function<bool (const ch_t ch)> isSeparator)
    {
        offset_t begin{0}, end{0};
        bool is_find_begin{false};
        for (offset_t i{0}; i < str.size(); ++i)
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
                //return token, begin
            }
        }
    }

    index_manager::index_manager(str_v_t basic_str)
        : basic_str_{basic_str}
    {
        
        //add_general_inverted_index(basic_inverted_index_, k_basic_doc_id, basic_str_);
    }

    void
    index_manager::add_index(const doc_id_t doc_id, const delta_t &delta)
    {
        //auto merged_str{merge_str_by_delta(basic_str_, delta)};
        //add_general_inverted_index(delta_inverted_index_, doc_id, merged_str);
    }

    doc_id_umap_t
    index_manager::regex_query(str_v_t regex_str)
    {
        doc_id_umap_t result{};
        /*
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
        */
        return result;
    }
};