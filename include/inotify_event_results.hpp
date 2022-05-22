#ifndef INOTIFY_EVENT_RESULTS_HPP
#define INOTIFY_EVENT_RESULTS_HPP

#include "detail/buffer_results.hpp"

#include "inotify_event_iterator.hpp"

template<typename BufferSequence, typename ByteType = char>
using inotify_event_results = detail::buffer_results<
    inotify_event_iterator<BufferSequence, ByteType>
>;

#endif // INOTIFY_EVENT_RESULTS_HPP
