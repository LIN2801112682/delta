#include "neu/index_manager.h"

#include "neu/core.h"

#include <sstream>
#include <regex>
#include <string_view>
#include <algorithm>

namespace neu
{
    auto
    isSeparator(const char ch)
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

    auto
    init_basic_inverted_index(const std::string &basic_str)
    {
        basic_inverted_index_type basic_inverted_index{};
        offset_type begin{0}, end{0};
        bool is_find_begin{false};
        for (offset_type i{0}; i < basic_str.size(); ++i)
        {
            const auto &ch{basic_str[i]};
            if (!isSeparator(ch))
            {
                if (!is_find_begin)
                {
                    is_find_begin = true;
                    begin = i;
                }
                end = i;
            }
            if (isSeparator(ch) || i == basic_str.size() - 1 && is_find_begin)
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
    index_manager::add_delta(const doc_id_type doc_id, const delta_type &delta)
    {
        delta_umap_.emplace(doc_id, delta);
    }

    std::unordered_map<doc_id_type, offset_uset_type>
    index_manager::regex_query(const std::string &regex_str)
    {
        return {};
    }
};