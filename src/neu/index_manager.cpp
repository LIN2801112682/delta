#include "neu/index_manager.h"
#include "neu/split_str.hpp"
#include <regex>
#include <iostream>

namespace neu
{
    auto
    is_separator(const ch_t ch)
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

    index_manager::index_manager(str_v_t basic_str)
        : basic_str_{basic_str}
    {
        split_str_t split_str{basic_str, is_separator};
        while (split_str.has_next())
        {
            auto [token, offset]{split_str.get_next()};
            basic_token_vec_.emplace_back(token);
            auto &doc_id_umap{basic_inverted_index_[token]};
            auto &offset_uset{doc_id_umap[k_basic_doc_id]};
            offset_uset.emplace(offset);
        }
    }

    void
    index_manager::add_index(const doc_id_t doc_id, const delta_t &delta)
    {
        // todo
    }

    doc_id_umap_t
    index_manager::regex_query(const str_t &regex_str)
    {
        doc_id_umap_t result{};
        std::regex pattern{regex_str};
        std::smatch regex_result;
        for (const auto &[token, doc_id_umap] : basic_inverted_index_)
        {
            if (regex_match(token, regex_result, pattern))
            {
                for (const auto &[doc_id, offset_uset] : doc_id_umap)
                {
                    // todo
                    result[doc_id] = offset_uset;
                }
            }
        }
        for (const auto &[token, doc_id_umap] : delta_inverted_index_)
        {
            if (regex_match(token, regex_result, pattern))
            {
                for (const auto &[doc_id, offset_uset] : doc_id_umap)
                {
                    result[doc_id] = offset_uset;
                }
            }
        }
        return result;
    }
};