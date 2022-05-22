#ifndef INOTIFY_DESCRIPTOR_HPP
#define INOTIFY_DESCRIPTOR_HPP

extern "C" {
#include <sys/inotify.h>
} // extern "C"

#include <asio/posix/stream_descriptor.hpp>

#include "inotify_event_results.hpp"

class inotify_descriptor {
public:
    using watch_descriptor = int;

    using bytes_readable = asio::posix::stream_descriptor::bytes_readable;
    using executor_type = asio::posix::stream_descriptor::executor_type;
    using lowest_layer_type = asio::posix::stream_descriptor::lowest_layer_type;
    using native_handle_type = asio::posix::stream_descriptor::native_handle_type;
    using wait_type = asio::posix::stream_descriptor::wait_type;

    inotify_descriptor() = delete;
    inotify_descriptor(asio::io_context & io_context) :
        m_stream(io_context)
    {}
    inotify_descriptor(asio::io_context & io_context, const native_handle_type & native_descriptor) :
        m_stream(io_context, native_descriptor)
    {}
    inotify_descriptor(const inotify_descriptor &) = delete;
    inotify_descriptor(inotify_descriptor &&) = default;
    inotify_descriptor & operator=(const inotify_descriptor &) = delete;
    inotify_descriptor & operator=(inotify_descriptor &&) = default;
    ~inotify_descriptor() = default;

    watch_descriptor add_watch(const char * path, uint32_t mask) {
        const int wd = ::inotify_add_watch(m_stream.native_handle(), path, mask);
        if (wd == -1) {
            throw asio::system_error(asio::error_code(errno, asio::system_category()), __FUNCTION__);
        } else {
            return wd;
        }
    }

    watch_descriptor add_watch(const char * path, uint32_t mask, asio::error_code & ec) {
        const int wd = ::inotify_add_watch(m_stream.native_handle(), path, mask);
        if (wd == -1) {
            ec = asio::error_code(errno, asio::system_category());
        } else {
            ec = asio::error_code();
        }
        return wd;
    }

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
                    inotify_event_results<MutableBufferSequence>(
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
    inotify_event_results<MutableBufferSequence> read_events(const MutableBufferSequence & buffers) {
        const std::size_t bytes_transferred = m_stream.read_some(buffers);
        return inotify_event_results<MutableBufferSequence>(
            asio::buffers_begin(buffers),
            asio::buffers_begin(buffers) + bytes_transferred
        );
    }

    template<typename MutableBufferSequence>
    inotify_event_results<MutableBufferSequence> read_events(const MutableBufferSequence & buffers, asio::error_code & ec) {
        const std::size_t bytes_transferred = m_stream.read_some(buffers, ec);
        return inotify_event_results<MutableBufferSequence>(
            asio::buffers_begin(buffers),
            ec ? asio::buffers_begin(buffers) : asio::buffers_begin(buffers) + bytes_transferred
        );
    }

    native_handle_type release() {
        return m_stream.release();
    }

    void remove_watch(watch_descriptor wd) {
        if (::inotify_rm_watch(m_stream.native_handle(), wd) == -1) {
            throw asio::system_error(asio::error_code(errno, asio::system_category()), __FUNCTION__);
        }
    }

    void remove_watch(watch_descriptor wd, asio::error_code & ec) {
        if (::inotify_rm_watch(m_stream.native_handle(), wd) == -1) {
            ec = asio::error_code(errno, asio::system_category());
        } else {
            ec = asio::error_code();
        }
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

#endif // INOTIFY_DESCRIPTOR_HPP
