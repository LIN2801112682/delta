#ifndef _SPLIT_STR_HPP_
#define _SPLIT_STR_HPP_

#include "neu/types.h"
#include <iostream>
#include <tuple>

namespace neu
{
    class split_str_t
    {
    public:
        split_str_t(str_v_t str_v, check_dlm_func_t check_dlm_func)
            : str_{str_v},
              check_dlm_func_{check_dlm_func} {}

        bool has_next()
        {
            if (0 <= next_offset_)
            {
                return true;
            }
            do_get_next();
            return (0 <= next_offset_);
        }

        std::tuple<str_t, offset_t> get_next()
        {
            if (has_next())
            {
                std::tuple<str_t, offset_t> result{next_token_, next_offset_};
                next_offset_ = -1;
                return result;
            }
            else
            {
                std::cerr << "please get_next() after has_next().\n";
                return {{}, -1};
            }
        }

    private:
        void do_get_next()
        {
            offset_t begin{};
            offset_t end{};
            auto is_find_begin{false};
            for (; offset_ < str_.size(); ++offset_)
            {
                const ch_t &ch{str_[offset_]};
                if (!check_dlm_func_(ch))
                {
                    if (!is_find_begin)
                    {
                        is_find_begin = true;
                        begin = offset_;
                    }
                    end = offset_;
                }
                if ((check_dlm_func_(ch) || offset_ == str_.size() - 1) && is_find_begin)
                {
                    is_find_begin = false;
                    next_token_ = str_.substr(begin, end - begin + 1);
                    next_offset_ = begin;
                    ++offset_;
                    return;
                }
            }
            return;
        }

        const str_t str_;
        const check_dlm_func_t check_dlm_func_;
        offset_t offset_{0};
        str_t next_token_{};
        offset_t next_offset_{-1};
    };
};

#endif