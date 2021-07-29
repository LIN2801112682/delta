#ifndef _NEU_CORE_H_
#define _NEU_CORE_H_

#include "neu/types.h"

namespace neu
{
    delta_t extract_delta(str_v_t basic_str_v, str_v_t native_str_v);
    str_t merge_str(str_v_t basic_str_v, const delta_t &delta);
    bool is_es_dlm(const ch_t &ch);
};

#endif