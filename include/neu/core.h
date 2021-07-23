#ifndef _NEU_CORE_H_
#define _NEU_CORE_H_

#include "neu/node.h"
#include <string_view>
#include <stack>
#include <vector>

namespace neu
{
    using delta_type = std::vector<node>;

    std::stack<node> backtracking_path(std::string_view basic_str, std::string_view log_str);
    std::string merge_str_by_node_stack(std::string_view basic_str, std::stack<node> &&node_stack);
    std::string merge_str_by_delta(std::string_view basic_str, const delta_type &delta);
};

#endif