#ifndef _NEU_TYPES_H_
#define _NEU_TYPES_H_

#include <string>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>

namespace neu
{
    using ch_t = char;                            //ch = character
    using str_t = std::basic_string<ch_t>;        //str = string
    using str_v_t = std::basic_string_view<ch_t>; //v = view
    using doc_t = std::vector<str_t>;             //doc = document
    using col_t = std::vector<doc_t>;             //col = collection
    using doc_id_t = col_t::size_type;
    using offset_t = str_t::size_type;

    enum class node_type_enum
    {
        insert = 1,
        deletE,
        replace,
    };

    struct node_t
    {
        str_t content_;
        offset_t low_;
        offset_t high_;
        offset_t native_str_offset_;
        node_type_enum type_;
    };

    using delta_t = std::vector<node_t>;
    using node_stack_t = std::stack<node_t>;

    using off_uset_t = std::unordered_set<offset_t>;
    using doc_id_umap_t = std::unordered_map<doc_id_t, off_uset_t>;
    using inverted_index_t = std::unordered_map<str_t, doc_id_umap_t>;
};

#endif