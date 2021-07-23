#ifndef _NEU_INDEX_MANAGER_H_
#define _NEU_INDEX_MANAGER_H_

#include "neu/core.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <tuple>

namespace neu
{
    using doc_id_type = std::size_t;
    using offset_type = std::string::size_type;
    using offset_uset_type = std::unordered_set<offset_type>;
    using basic_inverted_index_type = std::unordered_map<std::string, offset_uset_type>;
    using inverted_index_type = std::unordered_map<std::string, std::unordered_map<doc_id_type, offset_uset_type>>;

    class index_manager
    {
    public:
        index_manager(const std::string &basic_str);
        void add_delta(const doc_id_type doc_id, const delta_type &delta);
        std::unordered_map<doc_id_type, offset_uset_type> regex_query(const std::string &regex_str);

    private:
        const std::string basic_str_;
        const basic_inverted_index_type basic_inverted_index_;
        std::unordered_map<doc_id_type, delta_type> delta_umap_{};
        inverted_index_type inverted_index_{};
    };
};

#endif