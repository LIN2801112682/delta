#include "neu/core.h"
#include <iostream>

namespace neu
{
    node_stack_t
    extract_node_stack(str_v_t basic_str, str_v_t native_str)
    {
        node_stack_t node_stack{};
        int m{static_cast<int>(basic_str.size())};
        int n{static_cast<int>(native_str.size())};
        std::vector<std::vector<offset_t>> dp(m + 1, std::vector<offset_t>(n + 1, 0));
        for (offset_t offset{0}; offset < dp.size(); ++offset)
        {
            dp[offset][0] = offset;
        }
        for (offset_t offset{0}; offset < dp[0].size(); ++offset)
        {
            dp[0][offset] = offset;
        }
        for (offset_t basic_str_offset{1}; basic_str_offset < dp.size(); ++basic_str_offset)
        {
            for (offset_t native_str_offset{1}; native_str_offset < dp[0].size(); ++native_str_offset)
            {
                if (basic_str[basic_str_offset - 1] == native_str[native_str_offset - 1])
                {
                    dp[basic_str_offset][native_str_offset] = dp[basic_str_offset - 1][native_str_offset - 1];
                }
                else
                {
                    dp[basic_str_offset][native_str_offset] =
                        std::min(
                            std::min(
                                dp[basic_str_offset - 1][native_str_offset] + 1,
                                dp[basic_str_offset][native_str_offset - 1] + 1),
                            dp[basic_str_offset - 1][native_str_offset - 1] + 1);
                }
            }
        }

#if 0
        std::cout << "distance: " << dp[m][n] << '\n';
#endif

        while (m >= 0 && n >= 0)
        {
            if (n > 0 && dp[m][n - 1] + 1 == dp[m][n])
            {
#if 0
                std::cout << "insert: " << native_str[n - 1] << " at: " << m - 1 << '\n';
#endif
                node_t node{
                    .content_ = str_t{native_str[n - 1]},
                    .low_ = static_cast<offset_t>(m - 1),
                    .high_ = static_cast<offset_t>(m),
                    .type_ = node_type_enum::insert,
                };
                if (node_stack.empty())
                {
                    node_stack.push(node);
                }
                else
                {
                    auto next_node{node_stack.top()};
                    if (node.type_ == next_node.type_ && node.low_ == next_node.low_)
                    {
                        node.content_ += next_node.content_;
                        node_stack.pop();
                    }
                    node_stack.push(node);
                }
                --n;
                continue;
            }
            else if (m > 0 && dp[m - 1][n] + 1 == dp[m][n])
            {
#if 0
                std::cout << "delete: " << basic_str[m - 1] << " at: " << m - 1 << '\n';
#endif
                node_t node{
                    .content_ = {},
                    .low_ = static_cast<offset_t>(m - 1),
                    .high_ = static_cast<offset_t>(m - 1),
                    .type_ = node_type_enum::deletE,
                };
                if (node_stack.empty())
                {
                    node_stack.push(node);
                }
                else
                {
                    auto next_node{node_stack.top()};
                    if (node.type_ == next_node.type_ && node.low_ + 1 == next_node.low_)
                    {
                        node.high_ = next_node.high_;
                        node_stack.pop();
                    }
                    node_stack.push(node);
                }
                --m;
                continue;
            }
            else if (m > 0 && n > 0 && dp[m - 1][n - 1] + 1 == dp[m][n])
            {
#if 0
                std::cout << "replace: " << basic_str[m - 1] << " to: " << native_str[n - 1] << " at: " << m - 1 << '\n';
#endif
                node_t node{
                    .content_ = str_t{native_str[n - 1]},
                    .low_ = static_cast<offset_t>(m - 1),
                    .high_ = static_cast<offset_t>(m - 1),
                    .type_ = node_type_enum::replace,
                };
                if (node_stack.empty())
                {
                    node_stack.push(node);
                }
                else
                {
                    auto next_node{node_stack.top()};
                    if (node.type_ == next_node.type_ && node.low_ + 1 == next_node.low_)
                    {
                        node.high_ = next_node.high_;
                        node.content_ += next_node.content_;
                        node_stack.pop();
                    }
                    node_stack.push(node);
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
        offset_t offset{0};
        while (offset < basic_str.size() || !node_stack.empty())
        {
            if (node_stack.empty())
            {
                merged_str += basic_str.substr(offset);
                break;
            }
            auto node{node_stack.top()};
            node_stack.pop();
            switch (node.type_)
            {
            case node_type_enum::insert:
                merged_str += basic_str.substr(offset, node.low_ - offset + 1);
                merged_str += node.content_;
                offset = node.high_;
                break;
            case node_type_enum::deletE:
                merged_str += basic_str.substr(offset, node.low_ - offset);
                offset = node.high_ + 1;
                break;
            case node_type_enum::replace:
                merged_str += basic_str.substr(offset, node.low_ - offset);
                merged_str += node.content_;
                offset = node.high_ + 1;
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
        offset_t offset{0};
        delta_t::size_type i{0};
        while (offset < basic_str.size() || i < delta.size())
        {
            if (i == delta.size())
            {
                merged_str += basic_str.substr(offset);
                break;
            }
            auto node{delta[i]};
            ++i;
            switch (node.type_)
            {
            case node_type_enum::insert:
                merged_str += basic_str.substr(offset, node.low_ - offset + 1);
                merged_str += node.content_;
                offset = node.high_;
                break;
            case node_type_enum::deletE:
                merged_str += basic_str.substr(offset, node.low_ - offset);
                offset = node.high_ + 1;
                break;
            case node_type_enum::replace:
                merged_str += basic_str.substr(offset, node.low_ - offset);
                merged_str += node.content_;
                offset = node.high_ + 1;
                break;
            default:
                std::cerr << "error node type enum: " << static_cast<int>(node.type_) << '\n'; 
                break;
            }
        }
        return merged_str;
    }
};