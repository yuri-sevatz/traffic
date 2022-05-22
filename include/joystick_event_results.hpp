#ifndef JOYSTICK_EVENT_RESULTS_HPP
#define JOYSTICK_EVENT_RESULTS_HPP

#include "detail/buffer_results.hpp"

#include "joystick_event_iterator.hpp"

template<typename BufferSequence, typename ByteType = char>
using joystick_event_results = detail::buffer_results<
    joystick_event_iterator<BufferSequence, ByteType>
>;

#endif // JOYSTICK_EVENT_RESULTS_HPP
