#ifndef _NEU_INDEX_MANAGER_H_
#define _NEU_INDEX_MANAGER_H_

#include "neu/node.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <tuple>

namespace neu
{
    class index_manager
    {
    public:
        using doc_id_type = std::vector<std::string>::size_type;
        using offset_type = std::string::size_type;
        using basic_inverted_index_type = std::unordered_map<std::string, std::unordered_set<offset_type>>;
        using inverted_index_type = std::unordered_map<std::string, std::unordered_map<doc_id_type, std::unordered_set<offset_type>>>;
        index_manager(const std::string &basic_str);
        void push_token_by_id_and_delta(const doc_id_type doc_id, const std::vector<node> &delta);
        std::vector<std::unordered_set<offset_type>> regex_query(const std::string &regex_str);

    private:
        const std::string basic_str_;
        const basic_inverted_index_type basic_inverted_index_;
        inverted_index_type inverted_index_{};
    };
};

#endif