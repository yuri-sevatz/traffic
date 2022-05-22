#ifndef GPIO_LINE_EVENT_ITERATOR_HPP
#define GPIO_LINE_EVENT_ITERATOR_HPP

#include "detail/buffer_iterator.hpp"

#include "gpio_line_event_parser.hpp"

template<typename BufferSequence, typename ByteType = char>
using gpio_line_event_iterator = detail::buffer_iterator<
    gpio_line_event_parser<BufferSequence, ByteType>
>;

#endif // GPIO_LINE_EVENT_ITERATOR_HPP
