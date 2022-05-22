#include <thread>
#include <vector>

#include <asio/signal_set.hpp>

#include "application.hpp"

int main(int argc, char * argv[]) {
    asio::io_context context;
    asio::signal_set signal(context, SIGTERM);
    signal.async_wait([&context](const asio::error_code & error, int){
        if (!error) {
            context.stop();
        }
    });
    Application application(
        context,
        Arguments(argv[0], std::vector<std::string_view>(argv + 1, argv + argc))
    );

    std::vector<std::thread> threads;
    const auto cores = std::thread::hardware_concurrency();
    for(std::remove_const_t<decltype(cores)> i = 0; i != cores; ++i) {
        threads.emplace_back(
            std::thread([&context](){
                context.run();
            })
        );
    }
    for(std::thread & thread : threads) {
        thread.join();
    }
    return EXIT_SUCCESS;
}
