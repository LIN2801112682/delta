#ifndef _DELTA_CORE_H_
#define _DELTA_CORE_H_

#include "neu/node.h"
#include <stack>

namespace neu
{
    std::stack<node> backtracking_path(const std::string &basic_str, const std::string &log_str);
};

#endif