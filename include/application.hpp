#ifndef APPLICATION_HPP
#define APPLICATION_HPP

extern "C" {
#include <sys/stat.h>
} // extern "C"

#include <array>
#include <bitset>
#include <unordered_map>
#include <utility>

#include <asio/streambuf.hpp>

#include "arguments.hpp"
#include "gpio_chip_descriptor.hpp"
#include "gpio_line_descriptor.hpp"
#include "inotify_descriptor.hpp"
#include "joystick_descriptor.hpp"
#include "utility.hpp"

class Application {
public:
    Application() = delete;
    Application(asio::io_context & context, const Arguments & arguments);
    Application(const Application &) = delete;
    Application(Application &&) = delete;
    Application & operator=(const Application &) = delete;
    Application & operator=(Application &&) = delete;
    ~Application() = default;

private:
    typedef std::bitset<64> mask_type;
    typedef std::pair<std::size_t, std::size_t> charlie_type;

    static const std::array<std::uint32_t, 4> s_inputs;
    static const std::array<std::uint32_t, 5> s_outputs;
    static const std::array<std::uint32_t, 1> s_brightness;

    struct joystick_type{
        std::pair<dev_t,ino_t> key;
        joystick_descriptor descriptor;
    };

    struct led_type{
        const charlie_type charlie;
        bool state = false;
    };

    void enable(charlie_type charlie);
    void disable();

    void async_read_inotify_events(
        const std::shared_ptr<asio::streambuf> & buffer
    );

    void handle_inotify_events(
        const std::shared_ptr<asio::streambuf> & buffer,
        const asio::error_code & error,
        const inotify_event_results<asio::mutable_buffers_1> & results
    );

    void async_read_joystick_events(
        const std::shared_ptr<joystick_type> & joystick,
        const std::shared_ptr<asio::streambuf> & buffer
    );

    void handle_joystick_events(
        const std::shared_ptr<joystick_type> & joystick,
        const std::shared_ptr<asio::streambuf> & buffer,
        const asio::error_code & error,
        const joystick_event_results<asio::mutable_buffers_1> & results
    );

    void async_read_gpio_line_events(
        gpio_line_descriptor & line,
        const std::shared_ptr<asio::streambuf> & buffer
    );

    void handle_read_gpio_line_events(
        gpio_line_descriptor & line,
        const std::shared_ptr<asio::streambuf> & buffer,
        const asio::error_code & error,
        const gpio_line_event_results<asio::mutable_buffers_1> & results
    );

    void resync();
    void insert(std::string_view name);
    void remove(std::string_view name);

    void update();
    void pwm();

    void setBrightness();
    void resetBrightness();

    asio::io_context & m_context;
    inotify_descriptor m_inotify;

    std::unordered_map<decltype(joystick_type::key), std::shared_ptr<joystick_type>> m_joysticks;

    gpio_chip_descriptor m_chip;
    gpio_line_descriptor m_input;
    gpio_line_descriptor m_output;
    //gpio_line_descriptor m_pwm;

    //std::pair<std::chrono::microseconds, std::chrono::microseconds> m_mark;

    std::array<led_type, 20> m_leds;
};

#endif // APPLICATION_HPP
