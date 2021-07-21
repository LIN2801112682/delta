#ifndef _NEU_CORE_H_
#define _NEU_CORE_H_

#include "neu/node.h"
#include <string_view>
#include <stack>
#include <tuple>
#include <vector>

namespace neu
{
    std::stack<node> backtracking_path(std::string_view basic_str, std::string_view log_str);
    std::string merge_str(std::string_view basic_str, std::stack<node> &&delta);
    std::tuple<std::string, std::vector<node>::size_type, std::vector<node>::size_type> partial_merge_str(std::string_view basic_str, const std::vector<node> &delta, const std::vector<node>::size_type index);
};

#endif