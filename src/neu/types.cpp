#include "neu/types.h"
#include <bitset>
#include <sstream>

namespace neu
{
    str_t serialize(const delta_t &delta)
    {
        char buff[4096]{};
        std::size_t i{0};
        std::size_t inc;

#define write_buff( src )\
    inc = sizeof(src);\
    memcpy(buff + i, &src, inc);\
    i += inc;\

        auto delte_size{delta.size()};
        write_buff(delte_size)

        for (const auto &node : delta)
        {
            auto node_type{static_cast<char>(node.type_)};
            write_buff(node_type)

            write_buff(node.low_)

            write_buff(node.high_)

            write_buff(node.native_right_left_offset_)

            auto node_content_length{node.content_.length()};
            write_buff(node_content_length)

            inc = node_content_length;
            memcpy(buff + i, node.content_.c_str(), inc);
            i += inc;
        }

#undef write_buff

        return str_t(buff, i);
    }

    delta_t deserialize(const str_t &str)
    {
        delta_t delta;

        const char *buff = str.c_str();
        std::size_t i{0};
        std::size_t inc;

#define read_buff( dst )\
    inc = sizeof(dst);\
    memcpy(&dst, buff + i, inc);\
    i += inc;\

        delta_t::size_type delta_size;
        read_buff(delta_size)

        for (decltype(delta_size) delta_idx = 0; delta_idx < delta_size; ++delta_idx)
        {
            node_t node;
            
            char node_type;
            read_buff(node_type)
            node.type_ = static_cast<node_type_enum>(node_type);

            read_buff(node.low_)

            read_buff(node.high_)

            read_buff(node.native_right_left_offset_)

            decltype(node_t::content_.length()) node_content_length;
            read_buff(node_content_length)

            char node_content[1024]{};
            inc = node_content_length;
            memcpy(node_content, buff + i, inc);
            i += inc;
            node.content_ = str_t(node_content, node_content_length);

            delta.emplace_back(std::move(node));
        }
        return delta;
    }
};