#ifndef _NEU_INDEX_MANAGER_H_
#define _NEU_INDEX_MANAGER_H_

#include "neu/core.h"
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <tuple>

namespace neu
{
    using str_t = std::string; //str = string
    using ch_t = str_t::value_type; // ch = char
    using doc_t = std::vector<str_t>; //doc = document
    using col_t = std::vector<doc_t>; // col = collection
    using doc_id_t = col_t::size_type;
    using position_t = doc_t::size_type;
    using str_idx_t = str_t::size_type; 

    struct offset_t
    {
        str_idx_t begin;
        str_idx_t end;
    };

    struct position_offset_t
    {
        position_t position;
        offset_t offset;
    };
    using position_offset_vec_t = std::vector<position_offset_t>;
    using doc_id_umap_t = std::unordered_map<doc_id_t, position_offset_vec_t>;
    using inverted_index_t = std::unordered_map<str_t, doc_id_umap_t>;

    using doc_id_uset_t = std::unordered_set<doc_id_t>;

    static constexpr doc_id_t k_basic_doc_id{0};

    class index_manager
    {
    public:
        index_manager(const str_t &basic_str);
        void add_delta_invert_index(const doc_id_t doc_id, const delta_type &delta);
        void add_native_inverted_index(const doc_id_t doc_id, const str_t &native_str);
        doc_id_uset_t regex_query_delta_invert_index(const str_t &regex_str);
        doc_id_uset_t regex_query_native_invert_index(const str_t &regex_str);

    private:
        const str_t basic_str_;
        inverted_index_t basic_inverted_index_{};
        inverted_index_t delta_inverted_index_{};
        inverted_index_t native_inverted_index_{};
    };
};

#endif