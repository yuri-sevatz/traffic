#ifndef GPIO_CHIP_DESCRIPTOR_HPP
#define GPIO_CHIP_DESCRIPTOR_HPP

extern "C" {
#include <linux/gpio.h>
#include <sys/inotify.h>
} // extern "C"

#include <asio/posix/stream_descriptor.hpp>

#include "gpio_line_info_changed_results.hpp"

class gpio_chip_descriptor {
public:
    using bytes_readable = asio::posix::stream_descriptor::bytes_readable;
    using executor_type = asio::posix::stream_descriptor::executor_type;
    using lowest_layer_type = asio::posix::stream_descriptor::lowest_layer_type;
    using native_handle_type = asio::posix::stream_descriptor::native_handle_type;
    using wait_type = asio::posix::stream_descriptor::wait_type;

    gpio_chip_descriptor() = delete;
    gpio_chip_descriptor(asio::io_context & io_context) :
        m_stream(io_context)
    {}
    gpio_chip_descriptor(asio::io_context & io_context, const native_handle_type & native_descriptor) :
        m_stream(io_context, native_descriptor)
    {}
    gpio_chip_descriptor(const gpio_chip_descriptor &) = delete;
    gpio_chip_descriptor(gpio_chip_descriptor &&) = default;
    gpio_chip_descriptor & operator=(const gpio_chip_descriptor &) = delete;
    gpio_chip_descriptor & operator=(gpio_chip_descriptor &&) = default;
    ~gpio_chip_descriptor() = default;

    void assign(const native_handle_type & native_descriptor) {
        m_stream.assign(native_descriptor);
    }

    void assign(const native_handle_type & native_descriptor, asio::error_code & ec) {
        m_stream.assign(native_descriptor, ec);
    }

    template<typename MutableBufferSequence, typename LineInfoChangedHandler>
    void async_read_line_info_changes(const MutableBufferSequence & buffers, LineInfoChangedHandler && handler) {
        m_stream.async_read_some(buffers,
            [buffers,handler=std::forward<LineInfoChangedHandler>(handler)](const asio::error_code & error, std::size_t bytes_transferred){
                handler(
                    error,
                    gpio_line_info_changed_results<MutableBufferSequence>(
                        asio::buffers_begin(buffers),
                        error ? asio::buffers_begin(buffers) : asio::buffers_begin(buffers) + bytes_transferred
                    )
                );
            }
        );
    }

    template<typename WaitHandler>
    auto async_wait(wait_type w, WaitHandler && handler) {
        return m_stream.async_wait(w, std::forward<WaitHandler>(handler));
    }

    void cancel() {
        return m_stream.cancel();
    }

    void cancel(asio::error_code & ec) {
        m_stream.cancel(ec);
    }

    void close() {
        return m_stream.close();
    }

    void close(asio::error_code & ec) {
        m_stream.close(ec);
    }

    void get_chip_info(struct gpiochip_info & chip_info) {
        asio::error_code ec;
        get_chip_info(chip_info, ec);
        if (ec) {
            throw asio::system_error(ec);
        }
    }

    void get_chip_info(struct gpiochip_info & chip_info, asio::error_code & ec) {
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), GPIO_GET_CHIPINFO_IOCTL, &chip_info) < 0 ? errno : 0,
            asio::error::system_category
        );
    }

    executor_type get_executor() {
        return m_stream.get_executor();
    }

    void get_line(struct gpio_v2_line_request & line_request) {
        asio::error_code ec;
        get_line(line_request, ec);
        if (ec) {
            throw asio::system_error(ec);
        }
    }

    void get_line(struct gpio_v2_line_request & line_request, asio::error_code & ec) {
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), GPIO_V2_GET_LINE_IOCTL, &line_request) < 0 ? errno : 0,
            asio::error::system_category
        );
    }

    /**
     * @brief Request a struct gpio_v2_line_info from the chip
     */
    void get_line_info(struct gpio_v2_line_info & line_info) {
        asio::error_code ec;
        get_line_info(line_info, ec);
        if (ec) {
            throw asio::system_error(ec);
        }
    }

    /**
     * @brief Request a struct gpio_v2_line_info from the chip
     */
    void get_line_info(struct gpio_v2_line_info & line_info, asio::error_code & ec) {
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), GPIO_V2_GET_LINEINFO_IOCTL, &line_info) < 0 ? errno : 0,
            asio::error::system_category
        );
    }

    /**
     * @brief Request a stream of struct gpio_v2_line_info_changed from the chip
     */
    void get_line_info_watch(struct gpio_v2_line_info & line_info) {
        asio::error_code ec;
        get_line_info_watch(line_info, ec);
        if (ec) {
            throw asio::system_error(ec);
        }
    }

    /**
     * @brief Request a stream of struct gpio_v2_line_info_changed from the chip
     */
    void get_line_info_watch(struct gpio_v2_line_info & line_info, asio::error_code & ec) {
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), GPIO_V2_GET_LINEINFO_WATCH_IOCTL, &line_info) < 0 ? errno : 0,
            asio::error::system_category
        );
    }

    /**
     * @brief Request the end of a stream of struct gpio_v2_line_info_changed from the chip
     */
    void get_line_info_unwatch(std::uint32_t offset) {
        asio::error_code ec;
        get_line_info_unwatch(offset, ec);
        if (ec) {
            throw asio::system_error(ec);
        }
    }

    /**
     * @brief Request the end of a stream of struct gpio_v2_line_info_changed from the chip
     */
    void get_line_info_unwatch(std::uint32_t offset, asio::error_code & ec) {
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), GPIO_GET_LINEINFO_UNWATCH_IOCTL, &offset) < 0 ? errno : 0,
            asio::error::system_category
        );
    }

    template<typename IoControlCommand>
    void io_control(IoControlCommand & command) {
        m_stream.io_control(command);
    }

    template<typename IoControlCommand>
    void io_control(IoControlCommand & command, asio::error_code & ec) {
        m_stream.io_control(command, ec);
    }

    bool is_open() const {
        return m_stream.is_open();
    }

    lowest_layer_type & lowest_layer() {
        return m_stream.lowest_layer();
    }

    const lowest_layer_type & lowest_layer() const {
        return m_stream.lowest_layer();
    }

    native_handle_type native_handle() {
        return m_stream.native_handle();
    }

    bool native_non_blocking() const {
        return m_stream.native_non_blocking();
    }

    void native_non_blocking(bool mode) {
        m_stream.native_non_blocking(mode);
    }

    void native_non_blocking(bool mode, asio::error_code & ec) {
        m_stream.native_non_blocking(mode, ec);
    }

    bool non_blocking() const {
        return m_stream.non_blocking();
    }

    void non_blocking(bool mode) {
        m_stream.non_blocking(mode);
    }

    void non_blocking(bool mode, asio::error_code & ec) {
        m_stream.non_blocking(mode, ec);
    }

    template<typename MutableBufferSequence>
    gpio_line_info_changed_results<MutableBufferSequence> read_line_info_changes(const MutableBufferSequence & buffers) {
        const std::size_t bytes_transferred = m_stream.read_some(buffers);
        return gpio_line_info_changed_results<MutableBufferSequence>(
            asio::buffers_begin(buffers),
            asio::buffers_begin(buffers) + bytes_transferred
        );
    }

    template<typename MutableBufferSequence>
    gpio_line_info_changed_results<MutableBufferSequence> read_line_info_changes(const MutableBufferSequence & buffers, asio::error_code & ec) {
        const std::size_t bytes_transferred = m_stream.read_some(buffers, ec);
        return gpio_line_info_changed_results<MutableBufferSequence>(
            asio::buffers_begin(buffers),
            ec ? asio::buffers_begin(buffers) : asio::buffers_begin(buffers) + bytes_transferred
        );
    }

    native_handle_type release() {
        return m_stream.release();
    }

    void wait(wait_type w) {
        m_stream.wait(w);
    }

    void wait(wait_type w, asio::error_code & ec) {
        m_stream.wait(w, ec);
    }

private:
    asio::posix::stream_descriptor m_stream;
};

#endif // GPIO_CHIP_DESCRIPTOR_HPP
