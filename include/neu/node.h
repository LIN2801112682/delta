#ifndef _DELTA_NODE_H_
#define _DELTA_NODE_H_

#include <string>
#include <iostream>
#include <unordered_map>

namespace neu
{
    enum class node_type : int
    {
        insert = 1,
        del,
        replace,
    };

    struct node
    {
        int low_;
        int high_;
        node_type type_;
        std::string content_;
    };
};

#endif