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
        for (int mid{0}; mid < delta.size(); ++mid)
        {
            const auto &node{delta[mid]};
            str_t partial_merged_str{};
            offset_t native_left_right_offset{0}, basic_left_left_offset{0}, basic_left_right_offset{0}, basic_right_left_offset{0}, right_right_basic_offset{0}; 
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

            for (int left{mid - 1}; !has_left_split && 0 <= left; --left)
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

                for (int i{0}; basic_left_left_offset <= basic_left_right_offset - i; ++i)
                {
                    const ch_t &ch{basic_str_[basic_left_right_offset - i]};
                    if (is_separator(ch))
                    {
                        has_left_split = true;
                        partial_merged_str = basic_str_.substr(basic_left_right_offset - i + 1, i) + partial_merged_str;
                        native_left_right_offset -= (i - 1);
                        break;
                    }
                }
                if (has_left_split)
                {
                    break;
                }
                if (!has_left_split && basic_left_left_offset <= basic_left_right_offset)
                {
                    partial_merged_str = basic_str_.substr(basic_left_right_offset, basic_left_right_offset - basic_left_left_offset + 1) + partial_merged_str;
                }

                switch (left_node.type_)
                {
                case node_type_enum::insert:
                    for (int i{0}; 0 <= left_node.content_.size() - 1 - i; ++i)
                    {
                        const ch_t &ch{basic_str_[left_node.content_.size() - 1 - i]};
                        if (is_separator(ch))
                        {
                            has_left_split = true;
                            partial_merged_str = basic_str_.substr(basic_left_right_offset - i + 1, i) + partial_merged_str;
                            native_left_right_offset -= (i - 1);
                            break;
                        }
                    }
                    break;
                case node_type_enum::deletE:
                    break;
                case node_type_enum::replace:
                    break;
                default:
                    std::cerr << "error node type enum: " << static_cast<int>(left_node.type_) << '\n';
                    break;
                }

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