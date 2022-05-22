#ifndef GPIO_LINE_INFO_CHANGED_ITERATOR_HPP
#define GPIO_LINE_INFO_CHANGED_ITERATOR_HPP

#include "detail/buffer_iterator.hpp"

#include "gpio_line_info_changed_parser.hpp"

template<typename BufferSequence, typename ByteType = char>
using gpio_line_info_changed_iterator = detail::buffer_iterator<
    gpio_line_info_changed_parser<BufferSequence, ByteType>
>;

#endif // GPIO_LINE_INFO_CHANGED_ITERATOR_HPP
