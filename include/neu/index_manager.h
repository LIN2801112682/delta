#ifndef _NEU_INDEX_MANAGER_H_
#define _NEU_INDEX_MANAGER_H_

#include <string>

namespace neu
{
    class index_manager
    {
    public:
        index_manager();
        void push_doc_line(const int doc_id, const std::string &doc_str);
    };
}