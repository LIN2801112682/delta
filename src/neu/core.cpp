#include "neu/core.h"
#include <limits>
#include <iostream>

namespace neu
{
    node_stack_t
    extract_node_stack(str_v_t basic_str, str_v_t native_str)
    {
        node_stack_t node_stack{};
        auto basic_str_size{basic_str.size()};
        auto native_str_size{native_str.size()};
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
                            std::min(
                                dp[basic_str_offset - 1][native_str_offset] + 1,
                                dp[basic_str_offset][native_str_offset - 1] + 1),
                            dp[basic_str_offset - 1][native_str_offset - 1] + 1);
                }
            }
        }

#if 0
        std::cout << "distance: " << dp[basic_str_size][native_str_size] << '\n';
#endif
        auto basic_str_offset{basic_str_size};
        auto native_str_offset{native_str_size};
        /*
        if (offset_t == signed) offset >= 0
        if (offset_t == unsigned) offset != offset_t::max
        */
        while (basic_str_offset != (std::numeric_limits<offset_t>::max)() && native_str_offset != (std::numeric_limits<offset_t>::max)())
        {
            if (native_str_offset > 0 && dp[basic_str_offset][native_str_offset - 1] + 1 == dp[basic_str_offset][native_str_offset])
            {
#if 0
                std::cout << "insert: " << native_str[native_str_offset - 1]
                    << " at: " << basic_str_offset - 1 << '\n';
#endif
                node_t node{
                    .content_ = str_t{native_str[native_str_offset - 1]},
                    .low_ = basic_str_offset - 1,
                    .high_ = basic_str_offset,
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
                std::cout << "delete: " << basic_str[basic_str_offset - 1]
                    << " at: " << basic_str_offset - 1 << '\n';
#endif
                node_t node{
                    .content_ = {},
                    .low_ = basic_str_offset - 1,
                    .high_ = basic_str_offset - 1,
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
                std::cout << "replace: " << basic_str[basic_str_offset - 1]
                    << " to: " << native_str[native_str_offset - 1]
                    << " at: " << basic_str_offset - 1 << '\n';
#endif
                node_t node{
                    .content_ = str_t{native_str[native_str_offset - 1]},
                    .low_ = basic_str_offset - 1,
                    .high_ = basic_str_offset - 1,
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
        return node_stack;
    }

    delta_t
    node_stack_to_delta(node_stack_t &&node_stack)
    {
        delta_t delta{};
        while (!node_stack.empty())
        {
            auto &node{node_stack.top()};
            node_stack.pop();
            delta.emplace_back(node);
        }
        return delta;
    }

    void cal_native_str_offset(delta_t &delta)
    {
        for (delta_t::size_type i{0}; i < delta.size(); ++i)
        {
            offset_t offset{0};
            for (delta_t::size_type j{0}; j < i; ++j)
            {
                const node_t &pre_node{delta[j]};
                switch (pre_node.type_)
                {
                case node_type_enum::insert:
                    offset += pre_node.content_.size();
                    break;
                case node_type_enum::deletE:
                    offset -= (pre_node.high_ - pre_node.low_ + 1);
                    break;
                case node_type_enum::replace:
                    break;
                default:
                    std::cerr << "error node type enum: " << static_cast<int>(pre_node.type_) << '\n';
                    break;
                }
            }
            node_t &node{delta[i]};
            switch (node.type_)
            {
            case node_type_enum::insert:
                node.native_str_offset_ = offset + node.low_;
                break;
            case node_type_enum::deletE:
            case node_type_enum::replace:
                node.native_str_offset_ = offset + node.low_ - 1;
                break;
            default:
                std::cerr << "error node type enum: " << static_cast<int>(node.type_) << '\n';
                break;
            }
        }
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
};