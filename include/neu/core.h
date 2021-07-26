#ifndef _NEU_CORE_H_
#define _NEU_CORE_H_

#include "neu/types.h"

namespace neu
{
    node_stack_t extract_node_stack(str_v_t basic_str, str_v_t native_str);
    delta_t node_stack_to_delta(node_stack_t &&node_stack);
    str_t merge_str_by_node_stack(str_v_t basic_str, node_stack_t &&node_stack);
    str_t merge_str_by_delta(str_v_t basic_str, const delta_t &delta);
};

#endif