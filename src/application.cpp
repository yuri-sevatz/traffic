#include <dirent.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <regex>
#include <thread>

#include <asio/post.hpp>

#include "application.hpp"

constexpr const std::array<std::uint32_t, 4> Application::s_inputs({17, 22, 23, 27});
constexpr const std::array<std::uint32_t, 5> Application::s_outputs({13, 19, 26, 20, 21});
constexpr const std::array<std::uint32_t, 1> Application::s_brightness({18});

void Application::enable(charlie_type charlie) {
    struct gpio_v2_line_config output_line_config;
    std::memset(&output_line_config, 0, sizeof(output_line_config));
    output_line_config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    output_line_config.num_attrs = 1;
    output_line_config.attrs[0].mask = mask_type().set().set(charlie.first, false).set(charlie.second, false).to_ullong();
    output_line_config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_FLAGS;
    output_line_config.attrs[0].attr.flags = GPIO_V2_LINE_FLAG_INPUT;
    m_output.set_config(output_line_config);

    struct gpio_v2_line_values output_line_values;
    std::memset(&output_line_values, 0, sizeof(output_line_values));
    output_line_values.bits = mask_type().set(charlie.first, true).set(charlie.second, false).to_ullong();
    output_line_values.mask = mask_type().set(charlie.first).set(charlie.second).to_ullong();
    m_output.set_values(output_line_values);
}

void Application::disable() {
    struct gpio_v2_line_config output_line_config;
    std::memset(&output_line_config, 0, sizeof(output_line_config));
    output_line_config.flags = GPIO_V2_LINE_FLAG_INPUT;
    output_line_config.num_attrs = 0;
    m_output.set_config(output_line_config);
}

Application::Application(asio::io_context & context, const Arguments & arguments) :
    m_context(context),
    m_inotify(context),
    m_chip(context),
    m_input(context),
    m_output(context),
    //m_pwm(context),
    //m_mark({std::chrono::microseconds(1000), std::chrono::microseconds(0)}),
    m_leds({
        led_type({{0, 1}}),
        led_type({{0, 2}}),
        led_type({{0, 3}}),
        led_type({{0, 4}}),
        led_type({{1, 2}}),

        led_type({{1, 0}}),
        led_type({{2, 0}}),
        led_type({{3, 0}}),
        led_type({{4, 0}}),
        led_type({{2, 1}}),

        led_type({{1, 3}}),
        led_type({{1, 4}}),
        led_type({{2, 3}}),
        led_type({{2, 4}}),
        led_type({{3, 4}}),

        led_type({{3, 1}}),
        led_type({{4, 1}}),
        led_type({{3, 2}}),
        led_type({{4, 2}}),
        led_type({{4, 3}}),
    })
{
    m_inotify.assign(::inotify_init());
    m_inotify.add_watch("/dev/input", IN_CREATE | IN_ONLYDIR | IN_ATTRIB);
    resync();
    async_read_inotify_events(std::make_shared<asio::streambuf>());

    const int chip_fd = ::open("/dev/gpiochip0", O_RDONLY);
    if (chip_fd) {
        m_chip.assign(chip_fd);

        const std::string_view consumer(arguments.name.substr(0, GPIO_MAX_NAME_SIZE - 1));

        struct gpio_v2_line_request input_line_request;
        std::memset(&input_line_request, 0, sizeof(input_line_request));
        std::copy(s_inputs.begin(), s_inputs.end(), input_line_request.offsets);
        std::copy(consumer.begin(), consumer.end(), input_line_request.consumer);
        input_line_request.consumer[consumer.size()] = '\0';
        input_line_request.config.flags = (
            GPIO_V2_LINE_FLAG_INPUT |
            GPIO_V2_LINE_FLAG_EDGE_RISING |
            GPIO_V2_LINE_FLAG_EDGE_FALLING |
            GPIO_V2_LINE_FLAG_BIAS_PULL_UP
        );
        input_line_request.config.num_attrs = 0;
        input_line_request.num_lines = s_inputs.size();
        input_line_request.event_buffer_size = 0;
        m_chip.get_line(input_line_request);
        m_input.assign(input_line_request.fd);
        async_read_gpio_line_events(m_input, std::make_shared<asio::streambuf>());

        struct gpio_v2_line_request output_line_request;
        std::memset(&output_line_request, 0, sizeof(output_line_request));
        std::copy(s_outputs.begin(), s_outputs.end(), output_line_request.offsets);
        std::copy(consumer.begin(), consumer.end(), output_line_request.consumer);
        output_line_request.consumer[consumer.size()] = '\0';
        output_line_request.config.flags = (
            GPIO_V2_LINE_FLAG_INPUT
        );
        output_line_request.config.num_attrs = 0;
        output_line_request.num_lines = s_outputs.size();
        output_line_request.event_buffer_size = 0;
        m_chip.get_line(output_line_request);
        m_output.assign(output_line_request.fd);

        /*
        struct gpio_v2_line_request brightness_line_request;
        std::memset(&brightness_line_request, 0, sizeof(brightness_line_request));
        std::copy(s_brightness.begin(), s_brightness.end(), brightness_line_request.offsets);
        std::copy(consumer.begin(), consumer.end(), brightness_line_request.consumer);
        brightness_line_request.consumer[consumer.size()] = '\0';
        brightness_line_request.config.flags = (
            GPIO_V2_LINE_FLAG_OUTPUT
        );
        brightness_line_request.config.num_attrs = 0;
        brightness_line_request.num_lines = s_brightness.size();
        brightness_line_request.event_buffer_size = 0;
        m_chip.get_line(brightness_line_request);
        m_pwm.assign(brightness_line_request.fd);
        */

        asio::post(m_context, [this](){
            update();
        });

        /*
        asio::post(m_context, [this](){
            pwm();
        });
        */
    }
}

void Application::async_read_inotify_events(
    const std::shared_ptr<asio::streambuf> & buffer
) {
    m_inotify.async_read_events(
        asio::buffer(buffer->prepare(sizeof(struct inotify_event) + NAME_MAX + 1)),
        [this,buffer](const asio::error_code & error, const inotify_event_results<asio::mutable_buffers_1> & results){
            handle_inotify_events(buffer, error, results);
        }
    );
}

void Application::handle_inotify_events(
    const std::shared_ptr<asio::streambuf> & buffer,
    const asio::error_code & error,
    const inotify_event_results<asio::mutable_buffers_1> & results
) {
    if (!error) {
        for (auto event = results.begin(); event != results.end(); ++event) {
            if (event->mask & IN_Q_OVERFLOW) {
                resync();
            } else if (event->mask & IN_IGNORED) {
                m_context.stop();
            } else {
                insert(event->name);
            }
        }
        buffer->consume(sizeof(struct inotify_event) + NAME_MAX + 1);
        async_read_inotify_events(buffer);
    } else if (error != asio::error::operation_aborted) {
        m_context.stop();
    }
}

void Application::async_read_joystick_events(
    const std::shared_ptr<joystick_type> & joystick,
    const std::shared_ptr<asio::streambuf> & buffer
) {
    joystick->descriptor.async_read_events(
        asio::buffer(buffer->prepare(sizeof(struct js_event))),
        [this,joystick,buffer](const asio::error_code & error, const joystick_event_results<asio::mutable_buffers_1> & results){
            handle_joystick_events(joystick, buffer, error, results);
        }
    );
}

void Application::handle_joystick_events(
    const std::shared_ptr<joystick_type> & joystick,
    const std::shared_ptr<asio::streambuf> & buffer,
    const asio::error_code & error,
    const joystick_event_results<asio::mutable_buffers_1> & results
) {
    if (!error) {
        for(auto event = results.begin(); event != results.end(); ++event) {
            switch(event->type) {
            case JS_EVENT_INIT:
                std::cout << ' '
                    << "joystick: " << joystick.get() << ", "
                    << "init: " << static_cast<unsigned>(event->number) << ", "
                    << "value: " << static_cast<int>(event->value) << ", "
                    << "time: " << event->time << "ms" << std::endl;
                break;
            case JS_EVENT_BUTTON:
                std::cout << ' '
                    << "joystick: " << joystick.get() << ", "
                    << "button: " << static_cast<unsigned>(event->number) << ", "
                    << "value: " << static_cast<int>(event->value) << ", "
                    << "time: " << event->time << "ms" << std::endl;
                    if (event->number == 0) {
                        m_leds[5].state = event->value;
                    } else if (event->number == 1) {
                        m_leds[10].state = event->value;
                    } else if (event->number == 3) {
                        m_leds[0].state = event->value;
                    } else if (event->number == 4) {
                        m_leds[15].state = event->value;
                    }
                break;
            case JS_EVENT_AXIS:
                std::cout << ' '
                    << "joystick: " << joystick.get() << ", "
                    << "axis: " << static_cast<unsigned>(event->number) << ", "
                    << "value: " << static_cast<int>(event->value) << ", "
                    << "time: " << event->time << "ms" << std::endl;
                if (m_chip.is_open()) {
                    if (event->number & 1) {
                        for (decltype(m_leds)::size_type i = 0; i != 5; ++i) {
                            m_leds[i].state = event->value < 0;
                        }
                        for (decltype(m_leds)::size_type i = 10; i != 15; ++i) {
                            m_leds[i].state = event->value > 0;
                        }
                    } else {
                        for (decltype(m_leds)::size_type i = 5; i != 10; ++i) {
                            m_leds[i].state = event->value > 0;
                        }
                        for (decltype(m_leds)::size_type i = 15; i != 20; ++i) {
                            m_leds[i].state = event->value < 0;
                        }
                    }
                }
                break;
            }
        }
        buffer->consume(sizeof(struct js_event));
        async_read_joystick_events(joystick, buffer);
    } else {
        std::cout << '-'
            << "joystick: " << joystick.get() << std::endl;
        m_joysticks.erase(joystick->key);
    }
}

void Application::async_read_gpio_line_events(
    gpio_line_descriptor & line,
    const std::shared_ptr<asio::streambuf> & buffer
) {
    line.async_read_line_events(
        asio::buffer(buffer->prepare(sizeof(struct gpioevent_data) * 16)),
        [this,buffer,&line](const asio::error_code & error, const gpio_line_event_results<asio::mutable_buffers_1> & results){
            handle_read_gpio_line_events(line, buffer, error, results);
        }
    );
}

void Application::handle_read_gpio_line_events(
    gpio_line_descriptor & line,
    const std::shared_ptr<asio::streambuf> & buffer,
    const asio::error_code & error,
    const gpio_line_event_results<asio::mutable_buffers_1> & results
) {
    if (!error) {
        for (auto event = results.begin(); event != results.end(); ++event) {
            std::cout << ' '
                << "timestamp_ns: " << event->timestamp_ns << ", "
                << "id: " << event->id << ", "
                << "offset: " << event->offset << ", "
                << "seqno: " << event->seqno << ", "
                << "line_seqno: " << event->line_seqno << std::endl;
            if (event->id == GPIO_V2_LINE_EVENT_RISING_EDGE) {
                switch(event->offset) {
                case s_inputs[0]:
                    /*
                    static constexpr const std::chrono::microseconds s_inc(100);
                    static constexpr const std::chrono::microseconds s_min(s_inc);
                    static constexpr const std::chrono::microseconds s_max(1000 - s_min.count());
                    if (m_mark.first <= s_max) {
                        m_mark.first += s_inc;
                        m_mark.second -= s_inc;
                        std::cout << "Brightness: " << m_mark.first.count() << std::endl;
                    }
                    */
                    break;
                case s_inputs[1]:
                    /*
                    if (m_mark.first >= s_min) {
                        m_mark.first -= s_inc;
                        m_mark.second += s_inc;
                        std::cout << "Brightness: " << m_mark.first.count() << std::endl;
                    }
                    */
                    break;

                case s_inputs[2]:
                    break;
                case s_inputs[3]:
                    break;
                }
            }
        }
        buffer->consume(sizeof(struct gpioevent_data) * 16);
        async_read_gpio_line_events(line, buffer);
    } else if (error != asio::error::operation_aborted) {
        m_context.stop();
    }
}

void Application::resync() {
    if (DIR * const dir = ::opendir("/dev/input")) {
        struct dirent * entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_CHR) {
                insert(entry->d_name);
            }
        }
        closedir(dir);
    }
}

void Application::insert(std::string_view name) {
    using namespace std::literals;
    static constexpr const std::string_view input = "/dev/input/"sv;

    if (std::regex_match(name.begin(), name.end(), std::regex("js\\d+"))) {
        char path[input.size() + name.size() + 1];
        std::strncpy(path, input.data(), input.size());
        std::strncpy(path + input.size(), name.data(), name.size());
        path[input.size() + name.size()] = '\0';
        const int fd = ::open(path, O_RDONLY);
        if (fd != -1) {
            struct stat stat;
            if (::fstat(fd, &stat) != -1) {
                const decltype(joystick_type::key) joystick_key = std::make_pair(stat.st_dev, stat.st_ino);
                if (m_joysticks.find(joystick_key) == m_joysticks.end()) {
                    const std::shared_ptr joystick = std::make_shared<joystick_type>(
                        joystick_type{joystick_key, joystick_descriptor(m_context, fd)}
                    );

                    std::cout<< '+'
                        << "joystick: " << joystick.get() << ", "
                        << "name: \"" << joystick->descriptor.name().data() << "\", "
                        << "version: 0x" << std::hex << joystick->descriptor.version() << std::dec << ", "
                        << "axes: " << static_cast<unsigned>(joystick->descriptor.axes()) << ", "
                        << "buttons: " << static_cast<unsigned>(joystick->descriptor.buttons()) << std::endl;

                    m_joysticks.emplace(joystick_key, joystick);
                    async_read_joystick_events(joystick, std::make_shared<asio::streambuf>());
                    return;
                }
            }
            ::close(fd);
        }
    }
}

void Application::update() {
    for (const led_type & led : m_leds) {
        if (led.state) {
            enable(led.charlie);
            disable();
        }
    }
    asio::post(m_context, [this](){
        update();
    });
}

/*
void Application::pwm() {
    struct gpio_v2_line_values brightness_line_values;
    std::memset(&brightness_line_values, 0, sizeof(brightness_line_values));
    brightness_line_values.mask = mask_type().set(0).to_ullong();

    while (!m_context.stopped()) {
        if (m_mark.first.count()) {
            brightness_line_values.bits = 1;
            m_pwm.set_values(brightness_line_values);
            std::this_thread::sleep_for(m_mark.first);
        }
        if (m_mark.second.count()) {
            brightness_line_values.bits = 0;
            m_pwm.set_values(brightness_line_values);
            std::this_thread::sleep_for(m_mark.second);
        }
    }
}
*/
