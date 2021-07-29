#ifndef _NEU_TYPES_H_
#define _NEU_TYPES_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include <functional>

namespace neu
{
    using ch_t = char;                            //ch = character
    using str_t = std::basic_string<ch_t>;        //str = string
    using str_v_t = std::basic_string_view<ch_t>; //v = view
    using doc_t = std::vector<str_t>;             //doc = document
    using col_t = std::vector<doc_t>;             //col = collection
    using doc_id_t = col_t::size_type;
    // bound problem is so complex that using signed type
    // using offset_t = str_t::size_type;
    using offset_t = std::make_signed_t<str_t::size_type>;
    using off_uset_t = std::unordered_set<offset_t>;
    using doc_id_umap_t = std::unordered_map<doc_id_t, off_uset_t>;

    enum class node_type_enum
    {
        insert = 0,
        deletE,
        replace,
    };

    struct node_t
    {
        str_t content_;
        offset_t low_;
        offset_t high_;
        offset_t native_right_left_offset_;
        node_type_enum type_;
    };

    using delta_t = std::vector<node_t>;

    using check_dlm_func_t = std::function<bool(const ch_t &ch)>;
};


#endif