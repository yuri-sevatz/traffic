#ifndef JOYSTICK_EVENT_ITERATOR_HPP
#define JOYSTICK_EVENT_ITERATOR_HPP

#include "detail/buffer_iterator.hpp"

#include "joystick_event_parser.hpp"

template<typename BufferSequence, typename ByteType = char>
using joystick_event_iterator = detail::buffer_iterator<
    joystick_event_parser<BufferSequence, ByteType>
>;

#endif // JOYSTICK_EVENT_ITERATOR_HPP
