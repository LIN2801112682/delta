#include "neu/core.h"
#include <iostream>
#include <stack>

namespace neu
{
    delta_t
    extract_delta(str_v_t basic_str, str_v_t native_str)
    {
        delta_t delta{};
        std::stack<node_t> node_stack{};
        offset_t basic_str_size{static_cast<offset_t>(basic_str.size())};
        offset_t native_str_size{static_cast<offset_t>(native_str.size())};
        std::vector<std::vector<offset_t>> dp(basic_str_size + 1, std::vector<offset_t>(native_str_size + 1, 0));
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
                            dp[basic_str_offset - 1][native_str_offset - 1] + 1,
                            std::min(
                                dp[basic_str_offset - 1][native_str_offset] + 1,
                                dp[basic_str_offset][native_str_offset - 1] + 1));
                }
            }
        }
#if 0
        std::cout << "distance: " << dp[basic_str_size][native_str_size] << '\n';
#endif

        auto basic_str_offset{basic_str_size};
        auto native_str_offset{native_str_size};
        while (basic_str_offset >= 0 && native_str_offset >= 0)
        {
            if (native_str_offset > 0 && dp[basic_str_offset][native_str_offset - 1] + 1 == dp[basic_str_offset][native_str_offset])
            {
#if 0
                std::cout << "insert: " << native_str[native_str_offset - 1] << " at: " << basic_str_offset - 1 << '\n';
#endif
                node_t node{
                    .content_ = str_t{native_str[native_str_offset - 1]},
                    .low_ = basic_str_offset - 1,
                    .high_ = basic_str_offset,
                    .native_right_left_offset_ = native_str_offset - 2,
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
                --native_str_offset;
                continue;
            }
            else if (basic_str_offset > 0 && dp[basic_str_offset - 1][native_str_offset] + 1 == dp[basic_str_offset][native_str_offset])
            {
#if 0
                std::cout << "delete: " << basic_str[basic_str_offset - 1] << " at: " << basic_str_offset - 1 << '\n';
#endif
                node_t node{
                    .content_ = {},
                    .low_ = basic_str_offset - 1,
                    .high_ = basic_str_offset - 1,
                    .native_right_left_offset_ = native_str_offset - 1,
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
                --basic_str_offset;
                continue;
            }
            else if (basic_str_offset > 0 && native_str_offset > 0 && dp[basic_str_offset - 1][native_str_offset - 1] + 1 == dp[basic_str_offset][native_str_offset])
            {
#if 0
                std::cout << "replace: " << basic_str[basic_str_offset - 1] << " to: " << native_str[native_str_offset - 1] << " at: " << basic_str_offset - 1 << '\n';
#endif
                node_t node{
                    .content_ = str_t{native_str[native_str_offset - 1]},
                    .low_ = basic_str_offset - 1,
                    .high_ = basic_str_offset - 1,
                    .native_right_left_offset_ = native_str_offset - 2,
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
                --basic_str_offset;
                --native_str_offset;
                continue;
            }
            --basic_str_offset;
            --native_str_offset;
        }

        while (!node_stack.empty())
        {
            auto &node{node_stack.top()};
            node_stack.pop();
            delta.emplace_back(node);
        }

        return delta;
    }

    str_t
    merge_str(str_v_t basic_str, const delta_t &delta)
    {
        str_t merged_str{};
        offset_t offset{0};
        delta_t::size_type delta_idx{0};
        while (offset < basic_str.size() || delta_idx < delta.size())
        {
            if (delta_idx == delta.size())
            {
                merged_str += basic_str.substr(offset);
                break;
            }
            auto node{delta[delta_idx]};
            ++delta_idx;
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

    bool is_es_dlm(const ch_t &ch)
    {
        switch (ch)
        {
        case ' ':
        case '.':
        case '\'':
        case '\"':
        case '(':
        case ')':
            return true;
        default:
            return false;
        }
    }
};