#include "neu/index_manager.h"
#include "neu/split_str.hpp"
#include <regex>
#include <iostream>

namespace neu
{
    auto
    is_separator(const ch_t ch)
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

    index_manager::index_manager(str_v_t basic_str)
        : basic_str_{basic_str}
    {
#if 0
        std::cout << "basic_str_: " << basic_str_ << '\n';
#endif
        split_str_t split_str{basic_str_, is_separator};
        while (split_str.has_next())
        {
            auto [token, offset]{split_str.get_next()};
#if 0
            std::cout << "  token: " << token << " offset: " << offset << '\n';
#endif
            auto &doc_id_umap{basic_inverted_index_[token]};
            auto &offset_uset{doc_id_umap[k_basic_doc_id]};
            offset_uset.emplace(offset);
        }
    }

    void
    index_manager::add_delta_index(const doc_id_t doc_id, delta_t &&delta)
    {
#if 0
        auto merged_str{merge_str_by_delta(basic_str_, delta)};
        std::cout << "basic_str: " << basic_str_ << '\n';
        std::cout << "merged_str: " << merged_str << '\n';
#endif
        delta_umap_.emplace(doc_id, delta);
        for (int mid{0}; mid < delta.size(); ++mid)
        {
            const auto &node{delta[mid]};
            str_t partial_merged_str{};
            int left{mid - 1}, right{mid + 1};
            offset_t native_left_right_offset{0};
            offset_t basic_left_left_offset{0};
            offset_t basic_left_right_offset{0};
            offset_t basic_right_left_offset{0};
            offset_t basic_right_right_offset{0}; 
            auto has_left_split{false}, has_right_split{false};

            switch (node.type_)
            {
            case node_type_enum::insert:
                partial_merged_str = node.content_;
                native_left_right_offset = node.native_right_left_offset_;
                basic_left_right_offset = node.low_;
                basic_right_left_offset = node.high_;
                has_left_split = is_separator(partial_merged_str[0]);
                has_right_split = is_separator(partial_merged_str[partial_merged_str.size() - 1]);
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
                has_left_split = is_separator(partial_merged_str[0]);
                has_right_split = is_separator(partial_merged_str[partial_merged_str.size() - 1]);
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
                        const ch_t &ch{basic_str_[basic_left_right_offset - i]};
                        if (is_separator(ch))
                        {
                            has_left_split = true;
                            partial_merged_str = basic_str_.substr(basic_left_right_offset - i + 1, i) + partial_merged_str;
                            native_left_right_offset -= i;
                            break;
                        }
                    }
                    if (has_left_split)
                    {
                        break;
                    }
                    partial_merged_str = basic_str_.substr(basic_left_left_offset, basic_left_right_offset - basic_left_left_offset + 1) + partial_merged_str;
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
                            if (is_separator(ch))
                            {
                                has_left_split = true;
                                partial_merged_str = left_node.content_.substr(left_node.content_.size() - i, i) + partial_merged_str;
                                native_left_right_offset += (left_node.content_.size() - i);
                                break;
                            }
                        }
                        if (has_left_split)
                        {
                            break;
                        }
                        partial_merged_str = left_node.content_ + partial_merged_str;
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
                            if (is_separator(ch))
                            {
                                has_left_split = true;
                                partial_merged_str = left_node.content_.substr(left_node.content_.size() - i, i) + partial_merged_str;
                                native_left_right_offset += (left_node.content_.size() - i);
                                break;
                            }
                        }
                        if (has_left_split)
                        {
                            break;
                        }
                        partial_merged_str = left_node.content_ + partial_merged_str;
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
                    basic_right_right_offset = basic_str_.size() - 1;
                }

                if (basic_right_left_offset <= basic_right_right_offset)
                {
                    for (offset_t i{0}; basic_right_left_offset + i <= basic_right_right_offset; ++i)
                    {
                        const ch_t &ch{basic_str_[basic_right_left_offset + i]};
                        if (is_separator(ch))
                        {
                            has_right_split = true;
                            partial_merged_str += basic_str_.substr(basic_right_left_offset + i - 1, i);
                            break;
                        }
                    }
                    if (has_right_split)
                    {
                        break;
                    }
                    partial_merged_str += basic_str_.substr(basic_right_left_offset, basic_right_right_offset - basic_right_left_offset + 1);
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
                            if (is_separator(ch))
                            {
                                has_right_split = true;
                                partial_merged_str += right_node.content_.substr(0, i);
                                break;
                            }
                        }
                        if (has_right_split)
                        {
                            break;
                        }
                        partial_merged_str += right_node.content_;
                        basic_right_left_offset = right_node.high_;
                        break;
                    case node_type_enum::deletE:
                        basic_right_left_offset = right_node.high_ + 1;
                        break;
                    case node_type_enum::replace:
                        for (offset_t i{0}; i <= right_node.content_.size() - 1; ++i)
                        {
                            const ch_t &ch{right_node.content_[i]};
                            if (is_separator(ch))
                            {
                                has_right_split = true;
                                partial_merged_str += right_node.content_.substr(0, i);
                                break;
                            }
                        }
                        if (has_right_split)
                        {
                            break;
                        }
                        partial_merged_str += right_node.content_;
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
#if 0
            std::cout << "  mid: " << mid << " native_left_right_offset: " << native_left_right_offset << " partial_merged_str: " << partial_merged_str << '\n';
#endif
            split_str_t split_str{partial_merged_str, is_separator};
            while (split_str.has_next())
            {
                auto [token, offset]{split_str.get_next()};
                offset += native_left_right_offset + 1;
#if 0
            std::cout << "  token: " << token << " offset: " << offset << '\n';
#endif
                auto &doc_id_umap{delta_inverted_index_[token]};
                auto &offset_uset{doc_id_umap[doc_id]};
                offset_uset.emplace(offset);
            }
        }
    }

    struct token_offset_t
    {
        str_t token;
        offset_t offset;
    };

    doc_id_umap_t
    index_manager::regex_query(const str_t &regex_str)
    {
        doc_id_umap_t result{};
        std::regex pattern{regex_str};
        std::smatch regex_result;

        std::vector<token_offset_t> token_offset_vec{};
        for (const auto &[token, doc_id_umap] : basic_inverted_index_)
        {
            if (regex_match(token, regex_result, pattern))
            {
                for (const auto &[_, offset_uset] : doc_id_umap)
                {
                    for (const auto &offset : offset_uset)
                    {
                        token_offset_vec.emplace_back(
                            token_offset_t{
                                .token = token,
                                .offset = offset,
                            });
                    }
                }
            }
        }
        std::sort(std::begin(token_offset_vec),
                  std::end(token_offset_vec),
                  [](const auto &lhs, const auto &rhs)
                  {
                      return lhs.offset < rhs.offset;
                  });
        for (const auto &[doc_id, delta] : delta_umap_)
        {
            offset_t native_left_offset{0};
            offset_t basic_left_offset{0};
            offset_t basic_right_offset{static_cast<offset_t>(basic_str_.size()) - 1};
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
                        const auto &token_offset{token_offset_vec[token_offset_vec_idx]};
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
                                    if (is_separator(left_node.content_[left_node.content_.size() - 1]))
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
                                    if (0 <= pre_offset && is_separator(basic_str_[pre_offset]))
                                    {
                                        has_left_split = true;
                                    }
                                    break;
                                case node_type_enum::replace:
                                    if (is_separator(left_node.content_[left_node.content_.size() - 1]))
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
                            if (token_offset.offset + token_offset.token.size() < basic_right_offset)
                            {
                                has_right_split = true;
                            }
                            else if (token_offset.offset + token_offset.token.size() == basic_right_offset)
                            {
                                auto right{mid - 1};
                                if (delta.size() - 1 < right)
                                {
                                    has_right_split = true;
                                }
                                else
                                {
                                    const auto &right_node{delta[right]};
                                    offset_t post_offset{static_cast<offset_t>(basic_str_.size())}; // case init
                                    switch (right_node.type_)
                                    {
                                    case node_type_enum::insert:
                                        if (is_separator(right_node.content_[0]))
                                        {
                                            has_right_split = true;
                                        }
                                        break;
                                    case node_type_enum::deletE:
                                        // ignore continuial detla situation, todo
                                        post_offset = right_node.high_ + 1;
                                        if (basic_str_.size() - 1 < post_offset)
                                        {
                                            has_right_split = true;
                                        }
                                        if (post_offset <= basic_str_.size() - 1 && is_separator(basic_str_[post_offset]))
                                        {
                                            has_right_split = true;
                                        }
                                        break;
                                    case node_type_enum::replace:
                                        if (is_separator(right_node.content_[0]))
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
#if 1
                                auto merged_str{merge_str_by_delta(basic_str_, delta)};
                                auto basic_token{token_offset.token};
                                auto delta_token{merged_str.substr(token_offset.offset + native_left_offset - basic_left_offset, token_offset.token.size())};
                                if (basic_token != delta_token)
                                {
                                    std::cout << "basic_token: " << basic_token << " delta_token: " << delta_token << '\n';
                                }
#endif
                                result[doc_id].emplace(token_offset.offset + native_left_offset - basic_left_offset);
                            }
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
                basic_right_offset = static_cast<offset_t>(basic_str_.size()) - 1;
                if (basic_left_offset <= basic_right_offset)
                {
                    for (; token_offset_vec_idx < token_offset_vec.size(); ++token_offset_vec_idx)
                    {
                        const auto &token_offset{token_offset_vec[token_offset_vec_idx]};
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
                                    if (is_separator(left_node.content_[left_node.content_.size() - 1]))
                                    {
                                        has_left_split = true;
                                    }
                                    break;
                                case node_type_enum::deletE:
                                    // ignore continuial detla situation, todo
                                    pre_offset = left_node.low_ - 1;
                                    if (0 <= pre_offset && is_separator(basic_str_[pre_offset]))
                                    {
                                        has_left_split = true;
                                    }
                                    break;
                                case node_type_enum::replace:
                                    if (is_separator(left_node.content_[left_node.content_.size() - 1]))
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
                            if (token_offset.offset + token_offset.token.size() < basic_right_offset)
                            {
                                has_right_split = true;
                            }
                            else if (token_offset.offset + token_offset.token.size() == basic_right_offset)
                            {
                                auto right{mid - 1};
                                if (delta.size() - 1 < right)
                                {
                                    has_right_split = true;
                                }
                                else
                                {
                                    const auto &right_node{delta[right]};
                                    offset_t post_offset{static_cast<offset_t>(basic_str_.size())}; // case init
                                    switch (right_node.type_)
                                    {
                                    case node_type_enum::insert:
                                        if (is_separator(right_node.content_[0]))
                                        {
                                            has_right_split = true;
                                        }
                                        break;
                                    case node_type_enum::deletE:
                                        // ignore continuial detla situation, todo
                                        post_offset = right_node.high_ + 1;
                                        if (post_offset <= basic_str_.size() - 1 && is_separator(basic_str_[post_offset]))
                                        {
                                            has_right_split = true;
                                        }
                                        break;
                                    case node_type_enum::replace:
                                        if (is_separator(right_node.content_[0]))
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
#if 1
                                auto merged_str{merge_str_by_delta(basic_str_, delta)};
                                auto basic_token{token_offset.token};
                                auto delta_token{merged_str.substr(token_offset.offset + native_left_offset - basic_left_offset, token_offset.token.size())};
                                if (basic_token != delta_token)
                                {
                                    std::cout << "basic_token: " << basic_token << " delta_token: " << delta_token << '\n';
                                }
#endif
                                result[doc_id].emplace(token_offset.offset + native_left_offset - basic_left_offset);
                            }
                        }
                    }
                }
            }
        }

        for (const auto &[token, doc_id_umap] : delta_inverted_index_)
        {
            if (regex_match(token, regex_result, pattern))
            {
                for (const auto &[doc_id, offset_uset] : doc_id_umap)
                {
                    for (const auto &offset : offset_uset)
                    {
                        result[doc_id].emplace(offset);
                    }
                }
            }
        }
        return result;
    }
};