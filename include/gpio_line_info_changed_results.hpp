#ifndef GPIO_LINE_INFO_CHANGED_RESULTS_HPP
#define GPIO_LINE_INFO_CHANGED_RESULTS_HPP

#include "detail/buffer_results.hpp"

#include "gpio_line_info_changed_iterator.hpp"

template<typename BufferSequence, typename ByteType = char>
using gpio_line_info_changed_results = detail::buffer_results<
    gpio_line_info_changed_iterator<BufferSequence, ByteType>
>;

#endif // GPIO_LINE_INFO_CHANGED_RESULTS_HPP
