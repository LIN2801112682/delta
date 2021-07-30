#ifndef _NEU_CORE_H_
#define _NEU_CORE_H_

#include "neu/types.h"

namespace neu
{
    delta_t extract_delta(str_v_t basic_str_v, str_v_t native_str_v);
    str_t merge_str(str_v_t basic_str_v, const delta_t &delta);
    std::tuple<str_t, offset_t> partial_merge_str(str_v_t basic_str_v, const delta_t &delta, const size_t delta_idx, const check_dlm_func_t &check_dlm_func);
    void batch_cal_native_offset(str_v_t basic_str_v, const delta_t &delta, std::vector<token_offset_t> &token_offset_vec, const check_dlm_func_t &check_dlm_func);
};

#endif