#ifndef JOYSTICK_EVENT_PARSER_HPP
#define JOYSTICK_EVENT_PARSER_HPP

extern "C" {
#include <linux/joystick.h>
} // extern "C"

#include <asio/buffers_iterator.hpp>

template<typename BufferSequence, typename ByteType = char>
struct joystick_event_parser {
    using state_type = asio::buffers_iterator<BufferSequence, ByteType>;

    using difference_type = std::ptrdiff_t;
    using value_type = struct js_event;
    using pointer = const value_type *;
    using reference = const value_type &;
    using iterator_category = std::random_access_iterator_tag;

    joystick_event_parser(state_type state) : m_state(std::move(state)) {}

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
    friend bool operator==(const joystick_event_parser & lhs, const joystick_event_parser & rhs) {
        return lhs.m_state == rhs.m_state;
    }
    friend bool operator!=(const joystick_event_parser & lhs, const joystick_event_parser & rhs) {
        return lhs.m_state != rhs.m_state;
    }
    friend bool operator<(const joystick_event_parser & lhs, const joystick_event_parser & rhs) {
        return lhs.m_state < rhs.m_state;
    }
    friend bool operator>(const joystick_event_parser & lhs, const joystick_event_parser & rhs) {
        return lhs.m_state > rhs.m_state;
    }
    friend bool operator<=(const joystick_event_parser & lhs, const joystick_event_parser & rhs) {
        return lhs.m_state <= rhs.m_state;
    }
    friend bool operator>=(const joystick_event_parser & lhs, const joystick_event_parser & rhs) {
        return lhs.m_state >= rhs.m_state;
    }
private:
    state_type m_state;
};

#endif // JOYSTICK_EVENT_PARSER_HPP
