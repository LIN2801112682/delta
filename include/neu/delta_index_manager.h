#ifndef _NEU_DELTA_INDEX_MANAGER_H_
#define _NEU_DELTA_INDEX_MANAGER_H_

#include "neu/types.h"
#include "neu/core.h"

namespace neu
{
    class delta_index_manager_t
    {
    public:
        static constexpr doc_id_t k_basic_doc_id{0};
        delta_index_manager_t(str_v_t basic_str_v);
        void add_delta_index(const doc_id_t doc_id, delta_t &&delta);
        doc_id_umap_t regex_query(const str_t &regex_str);

    private:
        const str_t basic_str_;
        std::unordered_map<doc_id_t, delta_t> delta_umap_{};
        std::unordered_map<str_t, doc_id_umap_t> basic_inverted_index_{};
        std::unordered_map<str_t, doc_id_umap_t> delta_inverted_index_{};
    };
};

#endif