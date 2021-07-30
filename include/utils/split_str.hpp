#ifndef _UTILS_SPLIT_STR_HPP_
#define _UTILS_SPLIT_STR_HPP_

#include <string>
#include <functional>

class split_str_t
{
public:
    split_str_t(std::string_view str_v, const std::function<bool(const char ch)> &check_dlm_func)
        : str_{str_v},
          check_dlm_func_{check_dlm_func} {}

    std::tuple<bool, std::string, std::string::size_type>
    get_next()
    {
        std::string::size_type begin{};
        std::string::size_type end{};
        auto is_find_begin{false};
        while (offset_ < str_.size())
        {
            const char &ch{str_[offset_]};
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
                ++offset_;
                return {true, str_.substr(begin, end - begin + 1), begin};
            }
            ++offset_;
        }
        return {false, {}, {}};
    }

private:
    const std::string str_;
    const std::function<bool(const char ch)> check_dlm_func_;
    std::string::size_type offset_{0};
};

#endif