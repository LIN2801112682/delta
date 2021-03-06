#include "neu/delta_index_manager.h"
#include "neu/core.h"
#include "utils/split_str.hpp"
#include <regex>
#include <functional>
#include <iostream>

namespace neu
{
    delta_index_manager_t::delta_index_manager_t(str_v_t basic_str_v, const check_dlm_func_t &check_dlm_func)
        : basic_str_{basic_str_v},
        check_dlm_func_{check_dlm_func}
    {
        split_str_t split_str{basic_str_, check_dlm_func_};
        while (true)
        {
            auto [has_next, token, offset]{split_str.get_next()};
            if (!has_next)
            {
                break;
            }
            auto &doc_id_umap{basic_inverted_index_[token]};
            auto &offset_uset{doc_id_umap[k_basic_doc_id]};
            offset_uset.emplace(offset);
        }
    }

#define DEBUG_ADD_DELTA_INDEX 1

    void
    delta_index_manager_t::add_delta_index(const doc_id_t doc_id, delta_t &&delta)
    {
#if DEBUG_ADD_DELTA_INDEX
        auto merged_str{merge_str(basic_str_, delta)};
        std::unordered_map<offset_t, str_t> offset_umap{};
        split_str_t split_str_m{merged_str, check_dlm_func_};
        while (true)
        {
            auto [has_next, token, offset]{split_str_m.get_next()};
            if (!has_next)
            {
                break;
            }
            offset_umap[offset] = token;
        }
#endif
        for (size_t delta_idx{0}; delta_idx < delta.size(); ++delta_idx)
        {
            auto [partial_merged_str, offset]{partial_merge_str(basic_str_, delta, delta_idx, check_dlm_func_)};
            split_str_t split_str{partial_merged_str, check_dlm_func_};
            while (true)
            {
                auto [has_next, token, relative_offset]{split_str.get_next()};
                if (!has_next)
                {
                    break;
                }
                auto &doc_id_umap{delta_inverted_index_[token]};
                auto &offset_uset{doc_id_umap[doc_id]};
                offset_uset.emplace(offset + relative_offset);
#if DEBUG_ADD_DELTA_INDEX
                const auto &true_value{offset_umap[offset + relative_offset]};
                if (token != true_value)
                {
                    const auto &node{delta[delta_idx]};
                    std::cout << "doc_id: " << doc_id << '\n';
                    std::cout << "basic_str: " << basic_str_ << '\n';
                    std::cout << "merged_str: " << merged_str << '\n';
                    std::cout << "  delta_idx: " << delta_idx << '\n';
                    std::cout << "  delta:\n";
                    std::cout << "      node:\n";
                    std::cout << "          content: " << node.content_ << "|\n";
                    std::cout << "          low: " << node.low_ << '\n';
                    std::cout << "          high: " << node.low_ << '\n';
                    std::cout << "          native_left_right_offset: " << node.native_right_left_offset_ << '\n';
                    std::cout << "          type: " << static_cast<int>(node.type_) << '\n';
                    std::cout << "  partial_merged_str: " << partial_merged_str << '\n';
                    std::cout << "      true_value: " << true_value << '\n';
                    std::cout << "      token: " << token << '\n';
                }
#endif
            }
        }
#if DEBUG_ADD_DELTA_INDEX
        for (auto &[offset, token] : offset_umap)
        {
            if (basic_inverted_index_.count(token) != 1)
            {
                if (delta_inverted_index_.count(token) != 1)
                {
                    std::cout << "1\n";
                    std::cout << "basic_str: " << basic_str_ << '\n';
                    std::cout << "merged_str: " << merged_str << '\n';
                    std::cout << "miss token: " << token << '\n';
                    std::cout << "miss doc_id: " << doc_id << '\n';
                    std::cout << "miss offset: " << offset << '\n';
                    offset_t native_left_right_offset{-1};
                }
                else
                {
                    if (delta_inverted_index_[token].count(doc_id) != 1)
                    {
                        offset_t native_left_right_offset{-1};
                        for (auto i{0}; i < delta.size(); ++i)
                        {
                            if (native_left_right_offset + 1 <= offset && offset <= delta[i].native_right_left_offset_ + 1)
                            {
                                std::cout << "2\n";
                                std::cout << "doc_id: " << doc_id << '\n';
                                std::cout << "basic_str: " << basic_str_ << '\n';
                                std::cout << "merged_str: " << merged_str << '\n';

                                auto node{delta[i - 1]};
                                std::cout << "  delta_idx: " << i - 1 << '\n';
                                std::cout << "  delta:\n";
                                std::cout << "      node:\n";
                                std::cout << "          content: " << node.content_ << "|\n";
                                std::cout << "          low: " << basic_str_.substr(node.low_) << '\n';
                                std::cout << "          high: " << basic_str_.substr(node.high_) << '\n';
                                std::cout << "          native_left_right_offset: " << merged_str.substr(node.native_right_left_offset_) << '\n';
                                std::cout << "          type: " << static_cast<int>(node.type_) << '\n';
                                std::cout << "      token: " << token << '\n';

                                node = delta[i];
                                std::cout << "  delta_idx: " << i << '\n';
                                std::cout << "  delta:\n";
                                std::cout << "      node:\n";
                                std::cout << "          content: " << node.content_ << "|\n";
                                std::cout << "          low: " << basic_str_.substr(node.low_) << '\n';
                                std::cout << "          high: " << basic_str_.substr(node.high_) << '\n';
                                std::cout << "          native_left_right_offset: " << merged_str.substr(node.native_right_left_offset_) << '\n';
                                std::cout << "          type: " << static_cast<int>(node.type_) << '\n';
                                std::cout << "      token: " << token << '\n';
                                break;
                            }
                            native_left_right_offset = delta[i].native_right_left_offset_;
                        }
                    }
                    else
                    {
                        if (delta_inverted_index_[token][doc_id].count(offset) != 1)
                        {
                            std::cout << "3\n";
                            std::cout << "basic_str: " << basic_str_ << '\n';
                            std::cout << "merged_str: " << merged_str << '\n';
                            std::cout << "miss token: " << token << '\n';
                            std::cout << "miss doc_id: " << doc_id << '\n';
                            std::cout << "miss offset: " << offset << '\n';
                            offset_t native_left_right_offset{-1};
                        }
                    }
                }
            }
        }
#endif
        delta_umap_.emplace(doc_id, std::move(delta));
    }

    doc_id_umap_t
    delta_index_manager_t::regex_query(const str_t &regex_str)
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
        std::sort(std::begin(token_offset_vec), std::end(token_offset_vec),
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
                                    if (check_dlm_func_(left_node.content_[left_node.content_.size() - 1]))
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
                                    if (0 <= pre_offset && check_dlm_func_(basic_str_[pre_offset]))
                                    {
                                        has_left_split = true;
                                    }
                                    break;
                                case node_type_enum::replace:
                                    if (check_dlm_func_(left_node.content_[left_node.content_.size() - 1]))
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
                                    offset_t post_offset{static_cast<offset_t>(basic_str_.size())}; // case init
                                    switch (right_node.type_)
                                    {
                                    case node_type_enum::insert:
                                        if (check_dlm_func_(right_node.content_[0]))
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
                                        if (post_offset <= basic_str_.size() - 1 && check_dlm_func_(basic_str_[post_offset]))
                                        {
                                            has_right_split = true;
                                        }
                                        break;
                                    case node_type_enum::replace:
                                        if (check_dlm_func_(right_node.content_[0]))
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
                                    if (check_dlm_func_(left_node.content_[left_node.content_.size() - 1]))
                                    {
                                        has_left_split = true;
                                    }
                                    break;
                                case node_type_enum::deletE:
                                    // ignore continuial detla situation, todo
                                    pre_offset = left_node.low_ - 1;
                                    if (0 <= pre_offset && check_dlm_func_(basic_str_[pre_offset]))
                                    {
                                        has_left_split = true;
                                    }
                                    break;
                                case node_type_enum::replace:
                                    if (check_dlm_func_(left_node.content_[left_node.content_.size() - 1]))
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