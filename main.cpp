#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <chrono>
#include <memory>
#include <thread>

static void run_example(
        std::shared_ptr<spdlog::logger> left,
        std::shared_ptr<spdlog::logger> right)
{
    static constexpr auto sleep_count = std::chrono::milliseconds(1);

    std::thread left_thread([left]()
    {
        for (int i = 1; i < 6; ++i) {
            left->info("left msg {}", i);

            std::this_thread::sleep_for(sleep_count);
        }
    });
    std::thread right_thread([right]() {
        for (int i = 1; i < 6; ++i) {
            right->info("right msg {}", i);

            std::this_thread::sleep_for(sleep_count);
        }
    });

    left_thread.join();
    right_thread.join();

    left->flush();
    right->flush();
}

int main(int argc, char** argv)
{
    auto file_sink =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true);
    auto left_logger = std::make_shared<spdlog::logger>("L", file_sink);
    auto right_logger = std::make_shared<spdlog::logger>("R", file_sink);
    spdlog::register_logger(left_logger);
    spdlog::register_logger(right_logger);

    spdlog::set_level(spdlog::level::trace);

    left_logger->debug("start run_example() with spdlog::level::off");
    left_logger->flush();

    spdlog::flush_on(spdlog::level::off); // spdlog::level::off by default in spdlog 1.6.0.
    run_example(left_logger, right_logger);

    right_logger->debug("start run_example() with spdlog::level::trace");
    right_logger->flush();

    spdlog::flush_on(spdlog::level::trace); // flush every.
    run_example(left_logger, right_logger);

    return 0;
}
