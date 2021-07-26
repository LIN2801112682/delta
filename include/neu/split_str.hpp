#ifndef _SPLIT_STR_HPP_
#define _SPLIT_STR_HPP_

#include "neu/types.h"
#include <functional>
#include <tuple>
#include <iostream>

namespace neu
{
    class split_str_t
    {
    public:
        split_str_t(str_v_t str, std::function<bool(const ch_t ch)> check_separator_func)
            : str_{str},
              check_separator_func_{check_separator_func} {}

        bool has_next()
        {
            if (next_offset_ != k_offset_minus_num)
            {
                return true;
            }
            do_get_next();
            return next_offset_ != k_offset_minus_num;
        }

        std::tuple<str_t, offset_t> get_next()
        {
            if (has_next())
            {
                std::tuple<str_t, offset_t> result{next_, next_offset_};
                next_offset_ = k_offset_minus_num;
                return result;
            }
            else
            {
                std::cerr << "please get_next() after has_next().\n";
                return {};
            }
        }

    private:
        void do_get_next()
        {
            offset_t begin{0}, end{0};
            bool is_find_begin{false};
            for (; offset_ < str_.size(); ++offset_)
            {
                const ch_t &ch{str_[offset_]};
                if (!check_separator_func_(ch))
                {
                    if (!is_find_begin)
                    {
                        is_find_begin = true;
                        begin = offset_;
                    }
                    end = offset_;
                }
                if ((check_separator_func_(ch) || offset_ == str_.size() - 1) && is_find_begin)
                {
                    is_find_begin = false;
                    next_ = str_.substr(begin, end - begin + 1);
                    next_offset_ = begin;
                    ++offset_;
                    return;
                }
            }
            return;
        }

    private:
        const str_t str_;
        const std::function<bool(const ch_t ch)> check_separator_func_;
        offset_t offset_{0};
        str_t next_{};
        offset_t next_offset_{k_offset_minus_num};
    };
};

#endif