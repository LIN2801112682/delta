#include "neu/core.h"
#include <iostream>
#include <stack>

namespace neu
{
    delta_t
    extract_delta(str_v_t basic_str_v, str_v_t native_str_v)
    {
        auto basic_str_size{static_cast<offset_t>(basic_str_v.size())};
        auto native_str_size{static_cast<offset_t>(native_str_v.size())};
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
                if (basic_str_v[basic_str_offset - 1] == native_str_v[native_str_offset - 1])
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
        // std::cout << "distance: " << dp[basic_str_size][native_str_size] << '\n';

        std::stack<node_t> node_stack{};
        auto basic_str_offset{basic_str_size};
        auto native_str_offset{native_str_size};
        while (basic_str_offset >= 0 && native_str_offset >= 0)
        {
            if (native_str_offset > 0 && dp[basic_str_offset][native_str_offset - 1] + 1 == dp[basic_str_offset][native_str_offset])
            {
                //std::cout << "insert: " << native_str_v[native_str_offset - 1] << " at: " << basic_str_offset - 1 << '\n';

                node_t node{
                    .content_ = str_t{native_str_v[native_str_offset - 1]},
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
                        node.content_.append(next_node.content_);
                        node_stack.pop();
                    }
                    node_stack.push(node);
                }
                --native_str_offset;
                continue;
            }
            else if (basic_str_offset > 0 && dp[basic_str_offset - 1][native_str_offset] + 1 == dp[basic_str_offset][native_str_offset])
            {
                // std::cout << "delete: " << basic_str_v[basic_str_offset - 1] << " at: " << basic_str_offset - 1 << '\n';

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
                // std::cout << "replace: " << basic_str_v[basic_str_offset - 1] << " to: " << native_str_v[native_str_offset - 1] << " at: " << basic_str_offset - 1 << '\n';

                node_t node{
                    .content_ = str_t{native_str_v[native_str_offset - 1]},
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
                        node.content_.append(next_node.content_);
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

        delta_t delta{};
        while (!node_stack.empty())
        {
            auto node{node_stack.top()};
            node_stack.pop();
            delta.emplace_back(std::move(node));
        }
        return delta;
    }

    str_t
    merge_str(str_v_t basic_str_v, const delta_t &delta)
    {
        str_t merged_str{};
        offset_t offset{0};
        size_t delta_idx{0};
        while (offset < basic_str_v.size() || delta_idx < delta.size())
        {
            if (delta_idx == delta.size())
            {
                merged_str.append(basic_str_v.substr(offset));
                break;
            }
            auto node{delta[delta_idx]};
            ++delta_idx;
            switch (node.type_)
            {
            case node_type_enum::insert:
                merged_str.append(basic_str_v.substr(offset, node.low_ - offset + 1));
                merged_str.append(node.content_);
                offset = node.high_;
                break;
            case node_type_enum::deletE:
                merged_str.append(basic_str_v.substr(offset, node.low_ - offset));
                offset = node.high_ + 1;
                break;
            case node_type_enum::replace:
                merged_str.append(basic_str_v.substr(offset, node.low_ - offset));
                merged_str.append(node.content_);
                offset = node.high_ + 1;
                break;
            default:
                std::cerr << "error node type enum: " << static_cast<int>(node.type_) << '\n';
                break;
            }
        }
        return merged_str;
    }

    std::tuple<str_t, offset_t>
    partial_merge_str(str_v_t basic_str_v, const delta_t &delta, const size_t delta_idx, const check_dlm_func_t &check_dlm_func)
    {
        str_t partial_merged_str{};

        auto mid{static_cast<std::make_signed_t<size_t>>(delta_idx)};
        auto left{mid - 1};
        auto right{mid + 1};
        offset_t native_left_right_offset{};
        offset_t basic_left_left_offset{};
        offset_t basic_left_right_offset{};
        offset_t basic_right_left_offset{};
        offset_t basic_right_right_offset{};
        auto has_left_split{false};
        auto has_right_split{false};

        const auto &node{delta[mid]};
        switch (node.type_)
        {
        case node_type_enum::insert:
            partial_merged_str = node.content_;
            native_left_right_offset = node.native_right_left_offset_;
            basic_left_right_offset = node.low_;
            basic_right_left_offset = node.high_;
            /*
            can't judge in here. eg:
            [_abc_]def[_ghi_], [_]abc[_def], [abc_]def[_ghi], |abc[_def], [abc_]def|...
            */
            // has_left_split = check_dlm_func(partial_merged_str[0]); 
            // has_right_split = check_dlm_func(partial_merged_str[partial_merged_str.size() - 1]);
            break;
        case node_type_enum::deletE:
            native_left_right_offset = node.native_right_left_offset_;
            basic_left_right_offset = node.low_ - 1;
            basic_right_left_offset = node.high_ + 1;
            break;
        case node_type_enum::replace:
            partial_merged_str = node.content_;
            native_left_right_offset = node.native_right_left_offset_;
            basic_left_right_offset = node.low_ - 1;
            basic_right_left_offset = node.high_ + 1;
            // has_left_split = check_dlm_func(partial_merged_str[0]);
            // has_right_split = check_dlm_func(partial_merged_str[partial_merged_str.size() - 1]);
            break;
        default:
            std::cerr << "error node type enum: " << static_cast<int>(node.type_) << '\n';
            break;
        }

        while (!has_left_split)
        {
            if (0 <= left)
            {
                const auto &left_node{delta[left]};
                switch (left_node.type_)
                {
                case node_type_enum::insert:
                    basic_left_left_offset = left_node.high_;
                    break;
                case node_type_enum::deletE:
                    basic_left_left_offset = left_node.high_ + 1;
                    break;
                case node_type_enum::replace:
                    basic_left_left_offset = left_node.high_ + 1;
                    break;
                default:
                    std::cerr << "error node type enum: " << static_cast<int>(left_node.type_) << '\n';
                    break;
                }
            }
            else
            {
                basic_left_left_offset = 0;
            }

            if (basic_left_left_offset <= basic_left_right_offset)
            {
                for (offset_t i{0}; basic_left_left_offset + i <= basic_left_right_offset; ++i)
                {
                    const ch_t &ch{basic_str_v[basic_left_right_offset - i]};
                    if (check_dlm_func(ch))
                    {
                        has_left_split = true;
                        partial_merged_str.insert(0, basic_str_v.substr(basic_left_right_offset - i + 1, i));
                        native_left_right_offset -= i;
                        break;
                    }
                }
                if (has_left_split)
                {
                    break;
                }
                partial_merged_str.insert(0, basic_str_v.substr(basic_left_left_offset, basic_left_right_offset - basic_left_left_offset + 1));
            }

            if (0 <= left)
            {
                const auto &left_node{delta[left]};
                switch (left_node.type_)
                {
                case node_type_enum::insert:
                    native_left_right_offset = left_node.native_right_left_offset_;
                    for (offset_t i{0}; i <= left_node.content_.size() - 1; ++i)
                    {
                        const ch_t &ch{left_node.content_[left_node.content_.size() - 1 - i]};
                        if (check_dlm_func(ch))
                        {
                            has_left_split = true;
                            partial_merged_str.insert(0, left_node.content_.substr(left_node.content_.size() - i));
                            native_left_right_offset += (left_node.content_.size() - i);
                            break;
                        }
                    }
                    if (has_left_split)
                    {
                        break;
                    }
                    partial_merged_str.insert(0, left_node.content_);
                    basic_left_right_offset = left_node.low_;
                    break;
                case node_type_enum::deletE:
                    native_left_right_offset = left_node.native_right_left_offset_;
                    basic_left_right_offset = left_node.low_ - 1;
                    break;
                case node_type_enum::replace:
                    native_left_right_offset = left_node.native_right_left_offset_;
                    for (offset_t i{0}; i <= left_node.content_.size() - 1; ++i)
                    {
                        const ch_t &ch{left_node.content_[left_node.content_.size() - 1 - i]};
                        if (check_dlm_func(ch))
                        {
                            has_left_split = true;
                            partial_merged_str.insert(0, left_node.content_.substr(left_node.content_.size() - i));
                            native_left_right_offset += (left_node.content_.size() - i);
                            break;
                        }
                    }
                    if (has_left_split)
                    {
                        break;
                    }
                    partial_merged_str.insert(0, left_node.content_);
                    basic_left_right_offset = left_node.low_ - 1;
                    break;
                default:
                    std::cerr << "error node type enum: " << static_cast<int>(left_node.type_) << '\n';
                    break;
                }
                --left;
            }
            else
            {
                has_left_split = true;
                native_left_right_offset = -1;
                break;
            }
        }

        while (!has_right_split)
        {
            if (right <= delta.size() - 1)
            {
                const auto &right_node{delta[right]};
                switch (right_node.type_)
                {
                case node_type_enum::insert:
                    basic_right_right_offset = right_node.low_;
                    break;
                case node_type_enum::deletE:
                    basic_right_right_offset = right_node.low_ - 1;
                    break;
                case node_type_enum::replace:
                    basic_right_right_offset = right_node.low_ - 1;
                    break;
                default:
                    std::cerr << "error node type enum: " << static_cast<int>(right_node.type_) << '\n';
                    break;
                }
            }
            else
            {
                basic_right_right_offset = basic_str_v.size() - 1;
            }

            if (basic_right_left_offset <= basic_right_right_offset)
            {
                for (offset_t i{0}; basic_right_left_offset + i <= basic_right_right_offset; ++i)
                {
                    const ch_t &ch{basic_str_v[basic_right_left_offset + i]};
                    if (check_dlm_func(ch))
                    {
                        has_right_split = true;
                        partial_merged_str.append(basic_str_v.substr(basic_right_left_offset, i));
                        break;
                    }
                }
                if (has_right_split)
                {
                    break;
                }
                partial_merged_str.append(basic_str_v.substr(basic_right_left_offset, basic_right_right_offset - basic_right_left_offset + 1));
            }

            if (right <= delta.size() - 1)
            {
                const auto &right_node{delta[right]};
                switch (right_node.type_)
                {
                case node_type_enum::insert:
                    for (offset_t i{0}; i <= right_node.content_.size() - 1; ++i)
                    {
                        const ch_t &ch{right_node.content_[i]};
                        if (check_dlm_func(ch))
                        {
                            has_right_split = true;
                            partial_merged_str.append(right_node.content_.substr(0, i));
                            break;
                        }
                    }
                    if (has_right_split)
                    {
                        break;
                    }
                    partial_merged_str.append(right_node.content_);
                    basic_right_left_offset = right_node.high_;
                    break;
                case node_type_enum::deletE:
                    basic_right_left_offset = right_node.high_ + 1;
                    break;
                case node_type_enum::replace:
                    for (offset_t i{0}; i <= right_node.content_.size() - 1; ++i)
                    {
                        const ch_t &ch{right_node.content_[i]};
                        if (check_dlm_func(ch))
                        {
                            has_right_split = true;
                            partial_merged_str.append(right_node.content_.substr(0, i));
                            break;
                        }
                    }
                    if (has_right_split)
                    {
                        break;
                    }
                    partial_merged_str.append(right_node.content_);
                    basic_right_left_offset = right_node.high_ + 1;
                    break;
                default:
                    std::cerr << "error node type enum: " << static_cast<int>(right_node.type_) << '\n';
                    break;
                }
                ++right;
            }
            else
            {
                has_right_split = true;
                break;
            }
        }

        return {partial_merged_str, native_left_right_offset + 1};
    }

    void batch_cal_native_offset(str_v_t basic_str_v, const delta_t &delta, std::vector<token_offset_t> &token_offset_vec, const check_dlm_func_t &check_dlm_func)
    {
        offset_t native_left_offset{0};
        offset_t basic_left_offset{0};
        offset_t basic_right_offset{static_cast<offset_t>(basic_str_v.size()) - 1};
        int token_offset_vec_idx{0};
        int mid{0};
        while (mid < delta.size() && token_offset_vec_idx < token_offset_vec.size())
        {
            const auto &node{delta[mid]};
            switch (node.type_)
            {
            case node_type_enum::insert:
                basic_right_offset = node.low_;
                break;
            case node_type_enum::deletE:
                basic_right_offset = node.low_ - 1;
                break;
            case node_type_enum::replace:
                basic_right_offset = node.low_ - 1;
                break;
            default:
                std::cerr << "error node type enum: " << static_cast<int>(node.type_) << '\n';
                break;
            }

            if (basic_left_offset <= basic_right_offset)
            {
                for (; token_offset_vec_idx < token_offset_vec.size(); ++token_offset_vec_idx)
                {
                    auto &token_offset{token_offset_vec[token_offset_vec_idx]};
                    if (basic_right_offset < token_offset.offset)
                    {
                        break;
                    }

                    bool has_left_split{false};
                    if (basic_left_offset < token_offset.offset)
                    {
                        has_left_split = true;
                    }
                    else if (basic_left_offset == token_offset.offset)
                    {
                        auto left{mid - 1};
                        if (left < 0)
                        {
                            has_left_split = true;
                        }
                        else
                        {
                            const auto &left_node{delta[left]};
                            offset_t pre_offset{-1}; // case init
                            switch (left_node.type_)
                            {
                            case node_type_enum::insert:
                                if (check_dlm_func(left_node.content_[left_node.content_.size() - 1]))
                                {
                                    has_left_split = true;
                                }
                                break;
                            case node_type_enum::deletE:
                                // ignore continuial detla situation, todo
                                pre_offset = left_node.low_ - 1;
                                if (pre_offset < 0)
                                {
                                    has_left_split = true;
                                }
                                if (0 <= pre_offset && check_dlm_func(basic_str_v[pre_offset]))
                                {
                                    has_left_split = true;
                                }
                                break;
                            case node_type_enum::replace:
                                if (check_dlm_func(left_node.content_[left_node.content_.size() - 1]))
                                {
                                    has_left_split = true;
                                }
                                break;
                            default:
                                std::cerr << "error node type enum: " << static_cast<int>(left_node.type_) << '\n';
                                break;
                            }
                        }
                    }

                    if (has_left_split)
                    {
                        bool has_right_split{false};
                        if (token_offset.offset + token_offset.token.size() - 1 < basic_right_offset)
                        {
                            has_right_split = true;
                        }
                        else if (token_offset.offset + token_offset.token.size() - 1 == basic_right_offset)
                        {
                            auto right{mid + 1};
                            if (delta.size() - 1 < right)
                            {
                                has_right_split = true;
                            }
                            else
                            {
                                const auto &right_node{delta[right]};
                                offset_t post_offset{static_cast<offset_t>(basic_str_v.size())}; // case init
                                switch (right_node.type_)
                                {
                                case node_type_enum::insert:
                                    if (check_dlm_func(right_node.content_[0]))
                                    {
                                        has_right_split = true;
                                    }
                                    break;
                                case node_type_enum::deletE:
                                    // ignore continuial detla situation, todo
                                    post_offset = right_node.high_ + 1;
                                    if (basic_str_v.size() - 1 < post_offset)
                                    {
                                        has_right_split = true;
                                    }
                                    if (post_offset <= basic_str_v.size() - 1 && check_dlm_func(basic_str_v[post_offset]))
                                    {
                                        has_right_split = true;
                                    }
                                    break;
                                case node_type_enum::replace:
                                    if (check_dlm_func(right_node.content_[0]))
                                    {
                                        has_right_split = true;
                                    }
                                    break;
                                default:
                                    std::cerr << "error node type enum: " << static_cast<int>(right_node.type_) << '\n';
                                    break;
                                }
                            }
                        }
                        if (has_right_split)
                        {
                            token_offset.offset += (native_left_offset - basic_left_offset);
                        }
                        else
                        {
                            token_offset.offset = -1;
                        }
                    }
                    else
                    {
                        token_offset.offset = -1;
                    }
                }
            }

            switch (node.type_)
            {
            case node_type_enum::insert:
                native_left_offset = node.native_right_left_offset_ + node.content_.size() + 1;
                basic_left_offset = node.high_;
                break;
            case node_type_enum::deletE:
                native_left_offset = node.native_right_left_offset_ + 1;
                basic_left_offset = node.high_ + 1;
                break;
            case node_type_enum::replace:
                native_left_offset = node.native_right_left_offset_ + node.content_.size() + 1;
                basic_left_offset = node.high_ + 1;
                break;
            default:
                std::cerr << "error node type enum: " << static_cast<int>(node.type_) << '\n';
                break;
            }
            ++mid;
        }

        if (token_offset_vec_idx < token_offset_vec.size())
        {
            basic_right_offset = static_cast<offset_t>(basic_str_v.size()) - 1;
            if (basic_left_offset <= basic_right_offset)
            {
                for (; token_offset_vec_idx < token_offset_vec.size(); ++token_offset_vec_idx)
                {
                    auto &token_offset{token_offset_vec[token_offset_vec_idx]};
                    if (basic_right_offset < token_offset.offset)
                    {
                        break;
                    }

                    bool has_left_split{false};
                    if (basic_left_offset < token_offset.offset)
                    {
                        has_left_split = true;
                    }
                    else if (basic_left_offset == token_offset.offset)
                    {
                        auto left{mid - 1};
                        if (left < 0)
                        {
                            has_left_split = true;
                        }
                        else
                        {
                            const auto &left_node{delta[left]};
                            offset_t pre_offset{-1}; // case init
                            switch (left_node.type_)
                            {
                            case node_type_enum::insert:
                                if (check_dlm_func(left_node.content_[left_node.content_.size() - 1]))
                                {
                                    has_left_split = true;
                                }
                                break;
                            case node_type_enum::deletE:
                                // ignore continuial detla situation, todo
                                pre_offset = left_node.low_ - 1;
                                if (0 <= pre_offset && check_dlm_func(basic_str_v[pre_offset]))
                                {
                                    has_left_split = true;
                                }
                                break;
                            case node_type_enum::replace:
                                if (check_dlm_func(left_node.content_[left_node.content_.size() - 1]))
                                {
                                    has_left_split = true;
                                }
                                break;
                            default:
                                std::cerr << "error node type enum: " << static_cast<int>(left_node.type_) << '\n';
                                break;
                            }
                        }
                    }

                    if (has_left_split)
                    {
                        bool has_right_split{false};
                        if (token_offset.offset + token_offset.token.size() <= basic_right_offset)
                        {
                            has_right_split = true;
                        }

                        if (has_right_split)
                        {
                            token_offset.offset += (native_left_offset - basic_left_offset);
                        }
                        else
                        {
                            token_offset.offset = -1;
                        }
                    }
                    else
                    {
                        token_offset.offset = -1;
                    }
                }
            }
        }
    }
};