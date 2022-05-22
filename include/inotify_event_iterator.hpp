#ifndef INOTIFY_EVENT_ITERATOR_HPP
#define INOTIFY_EVENT_ITERATOR_HPP

#include "detail/buffer_iterator.hpp"

#include "inotify_event_parser.hpp"

template<typename BufferSequence, typename ByteType = char>
using inotify_event_iterator = detail::buffer_iterator<
    inotify_event_parser<BufferSequence, ByteType>
>;

#endif // INOTIFY_EVENT_ITERATOR_HPP
