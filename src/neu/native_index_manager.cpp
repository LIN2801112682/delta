#include "neu/native_index_manager.h"
#include "utils/split_str.hpp"
#include <iostream>
#include <regex>

namespace neu
{
    void
    native_index_manager_t::add_native_index(const doc_id_t doc_id, str_v_t native_str_v)
    {
        split_str_t split_str{native_str_v, is_es_dlm};
        while (true)
        {
            auto [has_next, token, offset]{split_str.get_next()};
            if (!has_next)
            {
                break;
            }
            auto &doc_id_umap{native_inverted_index_[token]};
            auto &offset_uset{doc_id_umap[doc_id]};
            offset_uset.emplace(offset);
        }
    }

    doc_id_umap_t
    native_index_manager_t::regex_query(const str_t &regex_str)
    {
        doc_id_umap_t result{};
        std::regex pattern{regex_str};
        std::smatch regex_result;

        for (const auto &[token, doc_id_umap] : native_inverted_index_)
        {
            if (regex_match(token, regex_result, pattern))
            {
                for (const auto &[doc_id, offset_uset] : doc_id_umap)
                {
                    for (const auto &offset : offset_uset)
                    {
                        result[doc_id].emplace(offset);
                    }
                }
            }
        }
        return result;
    }
};