#ifndef JOYSTICK_DESCRIPTOR_HPP
#define JOYSTICK_DESCRIPTOR_HPP

extern "C" {
#include <linux/joystick.h>
#include <sys/ioctl.h>
} // extern "C"

#include <array>

#include <asio/posix/stream_descriptor.hpp>

#include "joystick_event_results.hpp"

class joystick_descriptor {
public:
    typedef std::array<char, 255> name_type;
    typedef std::uint32_t version_type;
    typedef std::uint8_t axes_type;
    typedef std::uint8_t buttons_type;

    using bytes_readable = asio::posix::stream_descriptor::bytes_readable;
    using executor_type = asio::posix::stream_descriptor::executor_type;
    using lowest_layer_type = asio::posix::stream_descriptor::lowest_layer_type;
    using native_handle_type = asio::posix::stream_descriptor::native_handle_type;
    using wait_type = asio::posix::stream_descriptor::wait_type;

    joystick_descriptor() = delete;
    joystick_descriptor(asio::io_context & io_context) :
        m_stream(io_context)
    {}
    joystick_descriptor(asio::io_context & io_context, const native_handle_type & native_descriptor) :
        m_stream(io_context, native_descriptor)
    {}
    joystick_descriptor(const joystick_descriptor &) = delete;
    joystick_descriptor(joystick_descriptor &&) = default;
    joystick_descriptor & operator=(const joystick_descriptor &) = delete;
    joystick_descriptor & operator=(joystick_descriptor &&) = default;
    ~joystick_descriptor() = default;

    void assign(const native_handle_type & native_descriptor) {
        m_stream.assign(native_descriptor);
    }

    void assign(const native_handle_type & native_descriptor, asio::error_code & ec) {
        m_stream.assign(native_descriptor, ec);
    }

    template<typename MutableBufferSequence, typename EventHandler>
    void async_read_events(const MutableBufferSequence & buffers, EventHandler && handler) {
        m_stream.async_read_some(buffers,
            [buffers,handler=std::forward<EventHandler>(handler)](const asio::error_code & error, std::size_t bytes_transferred){
                handler(
                    error,
                    joystick_event_results<MutableBufferSequence>(
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


    axes_type axes() {
        asio::error_code ec;
        const axes_type axes = this->axes(ec);
        if (ec) {
            throw asio::system_error(ec);
        }
        return axes;
    }

    axes_type axes(asio::error_code & ec) {
        axes_type axes;
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), JSIOCGAXES, &axes) < 0 ? errno : 0,
            asio::error::system_category
        );
        return axes;
    }

    buttons_type buttons() {
        asio::error_code ec;
        const buttons_type buttons = this->buttons(ec);
        if (ec) {
            throw asio::system_error(ec);
        }
        return buttons;
    }

    buttons_type buttons(asio::error_code & ec) {
        buttons_type buttons;
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), JSIOCGBUTTONS, &buttons) < 0 ? errno : 0,
            asio::error::system_category
        );
        return buttons;
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

    executor_type get_executor() {
        return m_stream.get_executor();
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

    name_type name() {
        asio::error_code ec;
        const name_type name = this->name(ec);
        if (ec) {
            throw asio::system_error(ec);
        }
        return name;
    }

    name_type name(asio::error_code & ec) {
        name_type name;
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), JSIOCGNAME(255), name.data()) < 0 ? errno : 0,
            asio::error::system_category
        );
        return name;
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
    joystick_event_results<MutableBufferSequence> read_events(const MutableBufferSequence & buffers) {
        const std::size_t bytes_transferred = m_stream.read_some(buffers);
        return joystick_event_results<MutableBufferSequence>(
            asio::buffers_begin(buffers),
            asio::buffers_begin(buffers) + bytes_transferred
        );
    }

    template<typename MutableBufferSequence>
    joystick_event_results<MutableBufferSequence> read_events(const MutableBufferSequence & buffers, asio::error_code & ec) {
        const std::size_t bytes_transferred = m_stream.read_some(buffers, ec);
        return joystick_event_results<MutableBufferSequence>(
            asio::buffers_begin(buffers),
            ec ? asio::buffers_begin(buffers) : asio::buffers_begin(buffers) + bytes_transferred
        );
    }

    native_handle_type release() {
        return m_stream.release();
    }

    version_type version() {
        asio::error_code ec;
        const version_type version = this->version(ec);
        if (ec) {
            throw asio::system_error(ec);
        }
        return version;
    }

    version_type version(asio::error_code & ec) {
        version_type version;
        ec = asio::error_code(
            ::ioctl(m_stream.native_handle(), JSIOCGVERSION, &version) < 0 ? errno : 0,
            asio::error::system_category
        );
        return version;
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

#endif // JOYSTICK_DESCRIPTOR_HPP
