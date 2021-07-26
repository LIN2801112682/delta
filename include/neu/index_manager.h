#ifndef _NEU_INDEX_MANAGER_H_
#define _NEU_INDEX_MANAGER_H_

#include "neu/types.h"
#include "neu/core.h"

namespace neu
{
    using off_uset_t = std::unordered_set<offset_t>;
    using doc_id_umap_t = std::unordered_map<doc_id_t, off_uset_t>;
    using inverted_index_t = std::unordered_map<str_t, doc_id_umap_t>;

    static constexpr doc_id_t k_basic_doc_id{0};

    class index_manager
    {
    public:
        index_manager(const str_t &basic_str);
        void add_delta_invert_index(const doc_id_t doc_id, const delta_t &delta);
        void add_native_inverted_index(const doc_id_t doc_id, const str_t &native_str);
        doc_id_umap_t regex_query_delta_invert_index(const str_t &regex_str);
        doc_id_umap_t regex_query_native_invert_index(const str_t &regex_str);

    private:
        const str_t basic_str_;
        std::vector<str_t> basic_token_vec_{};
        inverted_index_t basic_inverted_index_{};
        inverted_index_t delta_inverted_index_{};
        inverted_index_t native_inverted_index_{};
    };
};

#endif