#include "neu/types.h"

std::size_t calc_serialized_size(const neu::delta_t &delta)
{
    std::size_t size{0};
    size += sizeof(decltype(delta.size()));
    for (const auto &node : delta)
    {
        size += sizeof(node.type_);
        size += sizeof(node.low_);
        size += sizeof(node.high_);
        size += sizeof(node.native_right_left_offset_);
        size += sizeof(decltype(node.content_.length()));
        size += node.content_.length();
    }
    return size;
}

std::ostream &operator<<(std::ostream &os, const neu::delta_t &delta)
{
    auto delta_size{delta.size()};
    os.write((char *)&delta_size, sizeof(delta_size));
    // std::cout << "<< delta_size: " << delta_size << '\n';
    for (const auto &node : delta)
    {
        os.write((char *)&node.type_, sizeof(node.type_));
        os.write((char *)&node.low_, sizeof(node.low_));
        os.write((char *)&node.high_, sizeof(node.high_));
        os.write((char *)&node.native_right_left_offset_, sizeof(node.native_right_left_offset_));
        auto node_content_length{node.content_.length()};
        os.write((char *)&node_content_length, sizeof(node_content_length));
        os.write(node.content_.c_str(), node_content_length);
        /*
        std::cout << "      type: " << static_cast<int>(node.type_)
                  << " low: " << node.low_
                  << " high: " << node.high_
                  << " native: " << node.native_right_left_offset_
                  << " contentLength: " << node_content_length
                  << " content: " << node.content_
                  << '\n';
        */
    }
    return os;
}

std::istream &operator>>(std::istream &is, neu::delta_t &delta)
{
    decltype(delta.size()) delta_size{};
    is.read((char *)&delta_size, sizeof(delta_size));
    // std::cout << ">> delta_size: " << delta_size << '\n';
    delta.resize(delta_size);
    for (auto &node : delta)
    {
        is.read((char *)&node.type_, sizeof(node.type_));
        is.read((char *)&node.low_, sizeof(node.low_));
        is.read((char *)&node.high_, sizeof(node.high_));
        is.read((char *)&node.native_right_left_offset_, sizeof(node.native_right_left_offset_));
        decltype(node.content_.length()) node_content_length{};
        is.read((char *)&node_content_length, sizeof(node_content_length));
        char *node_content_buff = (char *)malloc(node_content_length * sizeof(char));
        is.read(node_content_buff, node_content_length);
        node.content_ = std::move(neu::str_t(node_content_buff, node_content_length));
        free(node_content_buff);
        /*
        std::cout << "      type: " << static_cast<int>(node.type_)
                  << " low: " << node.low_
                  << " high: " << node.high_
                  << " native: " << node.native_right_left_offset_
                  << " contentLength: " << node_content_length
                  << " content: " << node.content_
                  << '\n';
        */
    }
    return is;
}