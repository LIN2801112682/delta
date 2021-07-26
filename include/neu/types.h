#ifndef _NEU_TYPES_H_
#define _NEU_TYPES_H_

#include <string>
#include <vector>
#include <limits>
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
    /*
    if (offset_t == signed) offset >= 0
    if (offset_t == unsigned) offset != offset_t::max
    */
    using offset_t = long; // bound problem is so complex that using signed type
    /*
    using offset_t = str_t::size_type;
    static const offset_t k_offset_minus_num = (std::numeric_limits<offset_t>::max)();
    */

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
        offset_t native_left_right_offset_;
        node_type_enum type_;
    };

    using delta_t = std::vector<node_t>;
    using node_stack_t = std::stack<node_t>;
};

#endif