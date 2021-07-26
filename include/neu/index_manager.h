#ifndef _NEU_INDEX_MANAGER_H_
#define _NEU_INDEX_MANAGER_H_

#include "neu/types.h"
#include "neu/core.h"

namespace neu
{

    class index_manager
    {
    public:
        static constexpr doc_id_t k_basic_doc_id{0};
        index_manager(str_v_t basic_str);
        void add_index(const doc_id_t doc_id, const delta_t &delta);
        doc_id_umap_t regex_query(const str_t &regex_str);

    private:
        const str_t basic_str_;
        std::vector<str_t> basic_token_vec_{};
        inverted_index_t basic_inverted_index_{};
        inverted_index_t delta_inverted_index_{};
    };
};

#endif