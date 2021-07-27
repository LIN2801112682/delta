#ifndef _NEU_INDEX_MANAGER_H_
#define _NEU_INDEX_MANAGER_H_

#include "neu/types.h"
#include "neu/core.h"

namespace neu
{
    using off_uset_t = std::unordered_set<offset_t>;
    using doc_id_umap_t = std::unordered_map<doc_id_t, off_uset_t>;

    class index_manager
    {
    public:
        static constexpr doc_id_t k_basic_doc_id{0};
        index_manager(str_v_t basic_str);
        void add_delta_index(const doc_id_t doc_id, delta_t &&delta);
        doc_id_umap_t regex_query(const str_t &regex_str);

    private:
        const str_t basic_str_;
        std::unordered_map<str_t, doc_id_umap_t> basic_inverted_index_{};
        std::unordered_map<str_t, doc_id_umap_t> delta_inverted_index_{};
        std::unordered_map<doc_id_t, delta_t> delta_umap_{};
    };
};

#endif