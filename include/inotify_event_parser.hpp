#ifndef INOTIFY_EVENT_PARSER_HPP
#define INOTIFY_EVENT_PARSER_HPP

extern "C" {
#include <sys/inotify.h>
} // extern "C"

#include <asio/buffers_iterator.hpp>

template<typename BufferSequence, typename ByteType = char>
struct inotify_event_parser {
    using state_type = asio::buffers_iterator<BufferSequence, ByteType>;

    using difference_type = std::ptrdiff_t;
    using value_type = struct inotify_event;
    using pointer = const value_type *;
    using reference = const value_type &;
    using iterator_category = std::forward_iterator_tag;

    inotify_event_parser(state_type state) : m_state(std::move(state)) {}

    reference dereference() const {
        return *reinterpret_cast<pointer>(m_state.operator->());
    }
    void increment() {
        m_state += (sizeof(struct inotify_event) + dereference().len);
    }
    friend bool operator==(const inotify_event_parser & lhs, const inotify_event_parser & rhs) {
        return lhs.m_state == rhs.m_state;
    }
    friend bool operator!=(const inotify_event_parser & lhs, const inotify_event_parser & rhs) {
        return lhs.m_state != rhs.m_state;
    }
private:
    state_type m_state;
};

#endif // INOTIFY_EVENT_PARSER_HPP
