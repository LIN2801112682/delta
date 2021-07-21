#include "neu/index_manager.h"

#include "neu/core.h"

#include <sstream>
#include <regex>

namespace neu
{
    auto
    init_basic_inverted_index(const std::string &basic_str)
    {
        index_manager::basic_inverted_index_type basic_inverted_index{};
        index_manager::offset_type begin{0}, end{0};
        bool is_find_begin{false};
        for (index_manager::offset_type i{0}; i < basic_str.size(); ++i)
        {
            const auto &ch{basic_str[i]};
            if (ch != ' ')
            {
                if (!is_find_begin)
                {
                    is_find_begin = true;
                    begin = i;
                }
                end = i;
            }
            if (ch == ' ' || i == basic_str.size() - 1 && is_find_begin)
            {
                is_find_begin = false;
                auto token{basic_str.substr(begin, end - begin + 1)};
                auto &offset_set{basic_inverted_index[token]};
                offset_set.emplace(begin);
            }
        }
        return basic_inverted_index;
    }

    index_manager::index_manager(const std::string &basic_str)
        : basic_str_{basic_str},
        basic_inverted_index_{init_basic_inverted_index(basic_str)}
    {
    }

    void
    index_manager::push_delta(const std::vector<node> &delta)
    {
        delta_vec_.emplace_back(delta);
        auto doc_id{delta_vec_.size() - 1};
        for (std::vector<node>::size_type index{0}; index < delta.size(); ++index)
        {
            auto &cur_node{delta[index]};
            /*
            switch (cur_node.type_)
            {
            case node_type::insert:
                merged_str += basic_str.substr(i, d.low_ - i + 1);
                merged_str += d.content_;
                i = d.high_;
                break;
            case node_type::del:
                merged_str += basic_str.substr(i, d.low_ - i);
                i = d.high_ + 1;
                break;
            case node_type::replace:
                break; 
            default:
                assert(false);
                break;
            }
            */
            auto [merged_str, left_index, _] = partial_merge_str(basic_str_, delta, index);
            std::string token{};
            offset_type offset{};
            auto &doc_id_umap{inverted_index_[token]};
            auto &offset_set{doc_id_umap[doc_id]};
            offset_set.emplace(offset);
        }
    }

    std::vector<std::unordered_set<index_manager::offset_type>>
    index_manager::regex_query(const std::string &regex_str)
    {
        std::regex pattern{regex_str};
        std::smatch result{};
        for (const auto &[basic_token, offset_set] : basic_inverted_index_)
        {
            if (std::regex_match(basic_token, result, pattern))
            {

            }
        }

        for (const auto &delta : delta_vec_)
        {
            for (std::vector<node>::size_type index{0}; index < delta.size(); ++index)
            {

            }
        }
        return {};
    }
};