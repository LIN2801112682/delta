#ifndef _NEU_INDEX_MANAGER_H_
#define _NEU_INDEX_MANAGER_H_

#include <string>

namespace neu
{
    template <typename inverted_index>
    class index_manager
    {
    public:
        index_manager();
        void push_doc_by_id_and_str(const int doc_id, const std::string &doc_str);
    private:
        inverted_index inverted_index_;
    };

    template <typename T>
    void index_manager<T>::push_doc_by_id_and_str(const int doc_id, const std::string &doc_str)
    {
        return;
    }
};

#endif