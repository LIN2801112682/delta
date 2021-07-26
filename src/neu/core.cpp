#include "neu/core.h"
#include <iostream>

namespace neu
{
    node_stack_t
    extract_node_stack(str_v_t basic_str, str_v_t native_str)
    {
        node_stack_t node_stack{};
        auto m{basic_str.size()};
        auto n{native_str.size()};
        std::vector<std::vector<offset_t>> dp(m + 1, std::vector<offset_t>(n + 1, 0));

        for (offset_t i{0}; i < m + 1; ++i)
        {
            dp[i][0] = i;
        }
        for (offset_t i{0}; i < n + 1; ++i)
        {
            dp[0][i] = i;
        }
        for (offset_t i{1}; i < m + 1; ++i)
        {
            for (offset_t j{1}; j < n + 1; ++j)
            {
                if (basic_str[i - 1] == native_str[j - 1])
                {
                    dp[i][j] = dp[i - 1][j - 1];
                }
                else
                {
                    dp[i][j] = std::min(std::min(dp[i - 1][j] + 1, dp[i][j - 1] + 1), dp[i - 1][j - 1] + 1);
                }
            }
        }

#if 0
        std::cout << "distance: " << dp[m][n] << '\n';
#endif

        while (m >= 0 || n >= 0)
        {

            if (n != 0 && dp[m][n - 1] + 1 == dp[m][n])
            {
#if 0
                std::cout << "insert: " << native_str[n - 1] << " at: " << m - 1 << '\n';
#endif
                node_t a{
                    .content_ = {native_str[n - 1]},
                    .low_ = m - 1,
                    .high_ = m,
                    .type_ = node_type_enum::insert};
                if (node_stack.empty())
                {
                    node_stack.push(a);
                }
                else
                {
                    node_t b{node_stack.top()};
                    if (a.type_ == b.type_ && a.low_ == b.low_)
                    {
                        a.content_ += b.content_;
                        node_stack.pop();
                    }
                    node_stack.push(a);
                }
                --n;
                continue;
            }
            else if (m != 0 && dp[m - 1][n] + 1 == dp[m][n])
            {
#if 0
                std::cout << "delete: " << native_str[m - 1] << " at: " << m - 1 << '\n';
#endif
                node_t a{
                    .content_ = {},
                    .low_ = m - 1,
                    .high_ = m - 1,
                    .type_ = node_type_enum::deletE};
                if (node_stack.empty())
                {
                    node_stack.push(a);
                }
                else
                {
                    node_t b{node_stack.top()};
                    if (a.type_ == b.type_ && a.low_ + 1 == b.low_)
                    {
                        a.high_ = b.high_;
                        node_stack.pop();
                    }
                    node_stack.push(a);
                }
                --m;
                continue;
            }
            else if (dp[m - 1][n - 1] + 1 == dp[m][n])
            {
#if 0
                std::cout << "replace: " << basic_str[m - 1] << " to: " << native_str[n - 1] << " at: " << m - 1 << '\n';
#endif
                node_t a{
                    .content_ = {native_str[n - 1]},
                    .low_ = m - 1,
                    .high_ = m - 1,
                    .type_ = node_type_enum::replace};
                if (node_stack.empty())
                {
                    node_stack.push(a);
                }
                else
                {
                    node_t b{node_stack.top()};
                    if (a.type_ == b.type_ && a.low_ + 1 == b.low_)
                    {
                        a.high_ = b.high_;
                        a.content_ += b.content_;
                        node_stack.pop();
                    }
                    node_stack.push(a);
                }
                --m;
                --n;
                continue;
            }
            --m;
            --n;
        }
        return node_stack;
    }

    str_t
    merge_str_by_node_stack(str_v_t basic_str, node_stack_t &&node_stack)
    {
        str_t merged_str{};
        offset_t i{0};
        while (i < basic_str.size() || !node_stack.empty())
        {
            if (node_stack.empty())
            {
                merged_str += basic_str.substr(i);
                break;
            }
            auto node{node_stack.top()};
            node_stack.pop();
            switch (node.type_)
            {
            case node_type_enum::insert:
                merged_str += basic_str.substr(i, node.low_ - i + 1);
                merged_str += node.content_;
                i = node.high_;
                break;
            case node_type_enum::deletE:
                merged_str += basic_str.substr(i, node.low_ - i);
                i = node.high_ + 1;
                break;
            case node_type_enum::replace:
                merged_str += basic_str.substr(i, node.low_ - i);
                merged_str += node.content_;
                i = node.high_ + 1;
                break;
            default:
                std::cerr << "error node type enum: " << static_cast<int>(node.type_) << '\n'; 
                break;
            }
        }
        return merged_str;
    }

    str_t
    merge_str_by_delta(str_v_t basic_str, const delta_t &delta)
    {
        str_t merged_str{};
        offset_t i{0};
        delta_t::size_type j{0};
        while (i < basic_str.size() || j < delta.size())
        {
            if (j == delta.size())
            {
                merged_str += basic_str.substr(i);
                break;
            }
            auto node{delta[j]};
            ++j;
            switch (node.type_)
            {
            case node_type_enum::insert:
                merged_str += basic_str.substr(i, node.low_ - i + 1);
                merged_str += node.content_;
                i = node.high_;
                break;
            case node_type_enum::deletE:
                merged_str += basic_str.substr(i, node.low_ - i);
                i = node.high_ + 1;
                break;
            case node_type_enum::replace:
                merged_str += basic_str.substr(i, node.low_ - i);
                merged_str += node.content_;
                i = node.high_ + 1;
                break;
            default:
                std::cerr << "error node type enum: " << static_cast<int>(node.type_) << '\n';
                break;
            }
        }
        return merged_str;
    }
};