#ifndef _NEU_NATIVE_INDEX_MANAGER_H_
#define _NEU_NATIVE_INDEX_MANAGER_H_

#include "neu/types.h"
#include "neu/core.h"

namespace neu
{
    class native_index_manager_t
    {
    public:
        void add_native_index(const doc_id_t doc_id, str_v_t native_str_v);
        doc_id_umap_t regex_query(const str_t &regex_str);

    private:
        std::unordered_map<str_t, doc_id_umap_t> native_inverted_index_{};
    };
};

#endif