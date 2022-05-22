#ifndef GPIO_LINE_EVENT_RESULTS_HPP
#define GPIO_LINE_EVENT_RESULTS_HPP

#include "detail/buffer_results.hpp"

#include "gpio_line_event_iterator.hpp"

template<typename BufferSequence, typename ByteType = char>
using gpio_line_event_results = detail::buffer_results<
    gpio_line_event_iterator<BufferSequence, ByteType>
>;

#endif // GPIO_LINE_EVENT_RESULTS_HPP
