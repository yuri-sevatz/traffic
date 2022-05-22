#ifndef GPIO_LINE_INFO_CHANGED_PARSER_HPP
#define GPIO_LINE_INFO_CHANGED_PARSER_HPP

extern "C" {
#include <linux/gpio.h>
} // extern "C"

#include <asio/buffers_iterator.hpp>

template<typename BufferSequence, typename ByteType = char>
struct gpio_line_info_changed_parser {
    using state_type = asio::buffers_iterator<BufferSequence, ByteType>;

    using difference_type = std::ptrdiff_t;
    using value_type = struct gpio_v2_line_info_changed;
    using pointer = const value_type *;
    using reference = const value_type &;
    using iterator_category = std::random_access_iterator_tag;

    gpio_line_info_changed_parser(state_type state) : m_state(std::move(state)) {}

    reference dereference() const {
        return *reinterpret_cast<pointer>(m_state.operator->());
    }
    void increment() {
        m_state += sizeof(value_type);
    }
    void decrement() {
        m_state -= sizeof(value_type);
    }
    void advance(difference_type difference) {
        m_state += sizeof(value_type) * difference;
    }
    difference_type difference(const state_type & rhs) {
        return (m_state - rhs) / sizeof(value_type);
    }
    friend bool operator==(const gpio_line_info_changed_parser & lhs, const gpio_line_info_changed_parser & rhs) {
        return lhs.m_state == rhs.m_state;
    }
    friend bool operator!=(const gpio_line_info_changed_parser & lhs, const gpio_line_info_changed_parser & rhs) {
        return lhs.m_state != rhs.m_state;
    }
    friend bool operator<(const gpio_line_info_changed_parser & lhs, const gpio_line_info_changed_parser & rhs) {
        return lhs.m_state < rhs.m_state;
    }
    friend bool operator>(const gpio_line_info_changed_parser & lhs, const gpio_line_info_changed_parser & rhs) {
        return lhs.m_state > rhs.m_state;
    }
    friend bool operator<=(const gpio_line_info_changed_parser & lhs, const gpio_line_info_changed_parser & rhs) {
        return lhs.m_state <= rhs.m_state;
    }
    friend bool operator>=(const gpio_line_info_changed_parser & lhs, const gpio_line_info_changed_parser & rhs) {
        return lhs.m_state >= rhs.m_state;
    }
private:
    state_type m_state;
};

#endif // GPIO_LINE_INFO_CHANGED_PARSER_HPP
