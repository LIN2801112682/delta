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
            basic_token_vec_.emplace_back(token);
            auto &doc_id_umap{basic_inverted_index_[token]};
            auto &offset_uset{doc_id_umap[k_basic_doc_id]};
            offset_uset.emplace(offset);
        }
    }

    void
    index_manager::add_delta_index(const doc_id_t doc_id, const delta_t &delta)
    {
        std::cout << "basic_str: " << basic_str_ << '\n';
        auto merged_str{neu::merge_str_by_delta(basic_str_, delta)};
        std::cout << "merged_str: " << merged_str << '\n';

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
                native_left_right_offset = node.native_left_right_offset_;
                basic_left_right_offset = node.low_;
                basic_right_left_offset = node.high_;
                has_left_split = is_separator(partial_merged_str[0]);
                has_right_split = is_separator(partial_merged_str[partial_merged_str.size() - 1]);
                break;
            case node_type_enum::deletE:
                native_left_right_offset = node.native_left_right_offset_;
                basic_left_right_offset = node.low_ - 1;
                basic_right_left_offset = node.high_ + 1;
                break;
            case node_type_enum::replace:
                partial_merged_str = node.content_;
                native_left_right_offset = node.native_left_right_offset_;
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
                            native_left_right_offset += (i - 1);
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
                        native_left_right_offset = left_node.native_left_right_offset_;
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
                        native_left_right_offset = left_node.native_left_right_offset_;
                        basic_left_right_offset = left_node.low_ - 1;
                        break;
                    case node_type_enum::replace:
                        native_left_right_offset = left_node.native_left_right_offset_;
                        for (offset_t i{0}; i <= left_node.content_.size() - 1; ++i)
                        {
                            if (i > 50)
                            {
                                std::cout << "          i: " << left_node.content_.size() - 1 - i << '\n';
                            }
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

            std::cout << "  mid: " << mid << " partial_merged_str: " << partial_merged_str << '\n';
        }
    }

    doc_id_umap_t
    index_manager::regex_query(const str_t &regex_str)
    {
        doc_id_umap_t result{};
        std::regex pattern{regex_str};
        std::smatch regex_result;
        for (const auto &[token, doc_id_umap] : basic_inverted_index_)
        {
            if (regex_match(token, regex_result, pattern))
            {
                for (const auto &[doc_id, offset_uset] : doc_id_umap)
                {
                    // todo
                    result[doc_id] = offset_uset;
                }
            }
        }
        for (const auto &[token, doc_id_umap] : delta_inverted_index_)
        {
            if (regex_match(token, regex_result, pattern))
            {
                for (const auto &[doc_id, offset_uset] : doc_id_umap)
                {
                    result[doc_id] = offset_uset;
                }
            }
        }
        return result;
    }
};