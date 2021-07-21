#include "neu/index_manager.h"

#include <sstream>

namespace neu
{
    index_manager::index_manager(const std::string &basic_str)
    {
        offset_type begin{0}, end{0};
        bool is_find_begin{false};
        for (offset_type i{0}; i < basic_str.size(); ++i)
        {
            const auto &ch = basic_str[i];
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
                auto &offset_set{basic_inverted_index_[token]};
                offset_set.emplace(begin);
            }
        }
    }
    
    void
    index_manager::push_doc_by_id_and_delta(const doc_id_type doc_id, const std::vector<node> &delta)
    {

    }
};