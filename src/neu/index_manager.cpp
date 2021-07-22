#include "neu/index_manager.h"

#include "neu/core.h"

#include <sstream>
#include <regex>
#include <string_view>
#include <algorithm>

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
        auto merged_str{merge_str(basic_str_, delta)};

        offset_type begin, end;
        bool is_find_begin{false};
        for (size_t i{0}; i < merged_str.size(); ++i)
        {
            const auto &ch{merged_str[i]};
            if (ch != ' ' && ch != '\"' && ch != '.' && ch != '\'' && ch != '(' && ch != ')' )
            {
                if (!is_find_begin)
                {
                    is_find_begin = true;
                    begin = i;
                }
                end = i;
            }
            if (ch == ' ' || ch == '\"' || ch == '.' || ch == '\'' || ch == '(' || ch == ')' || i == merged_str.size() - 1 && is_find_begin)
            {
                is_find_begin = false;
                const auto &token = merged_str.substr(begin, end - begin + 1);

                auto &doc_id_umap{inverted_index_[token]};
                auto &offset_set{doc_id_umap[doc_id]};
                offset_set.emplace(begin);
            }
        }
    }

    std::vector<std::unordered_set<index_manager::offset_type>>
    index_manager::regex_query(std::string regex_str)
    {
        std::vector<std::unordered_set<index_manager::offset_type>> result(delta_vec_.size());
        std::transform(std::begin(regex_str), std::end(regex_str), std::begin(regex_str), tolower);
        std::regex pattern{regex_str};
        std::smatch match{};
        for (const auto &[token, doc_id_umap] : inverted_index_)
        {
            if (regex_match(token, match, pattern))
            {
                for (const auto &[doc_id, offset_set] : doc_id_umap)
                {
                    result[doc_id] = offset_set;
                }
            }
        }
        return result;
    }
};