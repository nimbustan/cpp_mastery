/**
 * @file module_06_industry_standards/10_advanced_logging.cpp
 * @brief Advanced Logging
 *
 * @details
 * =============================================================================
 * [THEORY: Production Logging Requirements]
 * =============================================================================
 *
 * EN: Production logging goes far beyond std::cout. Requirements:
 *
 *     1. LOG LEVELS: Debug < Info < Warning < Error < Critical
 *     2. THREAD SAFETY: Multiple threads logging simultaneously
 *     3. LOG ROTATION: Don't fill the disk — rotate by size or time
 *     4. STRUCTURED LOGGING: Machine-parseable (JSON) + human-readable
 *     5. ASYNC LOGGING: Don't block hot paths for I/O
 *     6. SOURCE LOCATION: File, line, function — automatically
 *     7. SINKS: Console, file, syslog, network — configurable
 *
 *     ┌─────────────────────┬──────────────────────────────────────────────┐
 *     │ Library             │ Description                                  │
 *     ├─────────────────────┼──────────────────────────────────────────────┤
 *     │ spdlog              │ Fast, header-only, async. THE standard.      │
 *     │ glog (Google)       │ LOG(INFO), CHECK(), VLOG(). Thread-safe.     │
 *     │ Boost.Log           │ Full-featured, configurable. Heavy.          │
 *     │ plog                │ Header-only, portable, tiny.                 │
 *     │ Custom (this file)  │ Zero-dependency learning implementation.     │
 *     └─────────────────────┴──────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: spdlog — The Industry Standard]
 * =============================================================================
 *
 * EN: spdlog quick reference (CMake: FetchContent or find_package):
 *
 *     // Create logger with rotating file sink
 *     auto logger = spdlog::rotating_logger_mt(
 *         "ecu_log",                      // Logger name
 *         "logs/ecu.log",                 // File path
 *         1024 * 1024 * 5,                // 5 MB max size
 *         3                               // Keep 3 rotated files
 *     );
 *
 *     // Log with format args (fmtlib-style)
 *     logger->info("Sensor {} reading: {:.2f}C", sensor_id, temp);
 *     logger->warn("Temperature above threshold: {}C", temp);
 *     logger->error("Sensor {} communication failed", sensor_id);
 *
 *     // Set pattern
 *     spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread %t] %v");
 *
 *     // Async logging (background thread for I/O)
 *     spdlog::init_thread_pool(8192, 1);  // Queue size, 1 worker thread
 *     auto async_logger = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(
 *         "async_ecu", "logs/ecu.log", 5*1024*1024, 3);
 *
 *     auto logger = spdlog::rotating_logger_mt(
 *         "logs/ecu.log",                 // Dosya yolu
 *         1024 * 1024 * 5,                // 5 MB maks boyut
 *     );
 *
 *     logger->info("Sensor {} reading: {:.2f}C", sensor_id, temp);
 *     logger->warn("Temperature above threshold: {}C", temp);
 *     logger->error("Sensor {} communication failed", sensor_id);
 *
 *     // Desen ayarla
 *     spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread %t] %v");
 *
 *     auto async_logger = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(
 *         "async_ecu", "logs/ecu.log", 5*1024*1024, 3);
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 10_advanced_logging.cpp -o 10_advanced_logging
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <iomanip>
#include <vector>
#include <functional>
#include <filesystem>
#include <atomic>
#include <cstdint>

namespace fs = std::filesystem;

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Log Level System
// ═════════════════════════════════════════════════════════════════════════════

enum class LogLevel : uint8_t {
    Debug    = 0,
    Info     = 1,
    Warning  = 2,
    Error    = 3,
    Critical = 4
};

const char* to_string(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO ";
        case LogLevel::Warning:  return "WARN ";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRIT ";
        default:                 return "?????";
    }
}

// EN: ANSI color codes for console output
const char* level_color(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:    return "\033[36m";   // Cyan
        case LogLevel::Info:     return "\033[32m";   // Green
        case LogLevel::Warning:  return "\033[33m";   // Yellow
        case LogLevel::Error:    return "\033[31m";   // Red
        case LogLevel::Critical: return "\033[1;31m"; // Bold Red
        default:                 return "\033[0m";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Structured Log Entry
// ═════════════════════════════════════════════════════════════════════════════

struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::thread::id thread_id;
    std::chrono::system_clock::time_point timestamp;

    // EN: Format as human-readable text
    std::string format_text() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count()
            << " [" << to_string(level) << "]"
            << " [" << thread_id << "]"
            << " " << message;
        return oss.str();
    }

    // EN: Format as JSON (structured logging)
    std::string format_json() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << "{\"timestamp\":\"";
        oss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S");
        oss << "." << std::setfill('0') << std::setw(3) << ms.count() << "\"";
        oss << ",\"level\":\"" << to_string(level) << "\"";
        oss << ",\"thread\":\"" << thread_id << "\"";
        oss << ",\"file\":\"" << file << ":" << line << "\"";
        oss << ",\"message\":\"" << message << "\"}";
        return oss.str();
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Async Logger with Queue
// ═════════════════════════════════════════════════════════════════════════════

// EN: Async logger — logs are buffered in a queue, written by background thread
class AsyncLogger {
public:
    explicit AsyncLogger(const std::string& name,
                         LogLevel min_level = LogLevel::Debug,
                         size_t max_queue = 4096)
        : name_(name), min_level_(min_level), max_queue_(max_queue),
          running_(true) {
        worker_ = std::thread(&AsyncLogger::worker_loop, this);
    }

    ~AsyncLogger() {
        {
            std::lock_guard<std::mutex> lk(mtx_);
            running_ = false;
        }
        cv_.notify_one();
        if (worker_.joinable()) worker_.join();

        // EN: Flush remaining entries
        while (!queue_.empty()) {
            process_entry(queue_.front());
            queue_.pop();
        }
    }

    void log(LogLevel level, const std::string& msg,
             const char* file = "", int line = 0) {
        if (level < min_level_) return;

        LogEntry entry{level, msg, file, line,
                       std::this_thread::get_id(),
                       std::chrono::system_clock::now()};

        std::lock_guard<std::mutex> lk(mtx_);
        if (queue_.size() < max_queue_) {
            queue_.push(std::move(entry));
            cv_.notify_one();
        } else {
            dropped_++;
        }
    }

    void set_console_output(bool enabled) { console_enabled_ = enabled; }
    void set_json_format(bool enabled) { json_format_ = enabled; }
    size_t dropped_count() const { return dropped_.load(); }
    size_t processed_count() const { return processed_.load(); }

private:
    void worker_loop() {
        while (true) {
            LogEntry entry;
            {
                std::unique_lock<std::mutex> lk(mtx_);
                cv_.wait(lk, [this] { return !queue_.empty() || !running_; });
                if (!running_ && queue_.empty()) break;
                entry = std::move(queue_.front());
                queue_.pop();
            }
            process_entry(entry);
        }
    }

    void process_entry(const LogEntry& entry) {
        if (console_enabled_) {
            std::cout << level_color(entry.level)
                      << entry.format_text()
                      << "\033[0m\n";
        }
        processed_++;
    }

    std::string name_;
    LogLevel min_level_;
    size_t max_queue_;
    bool console_enabled_ = true;
    bool json_format_ = false;
    std::atomic<bool> running_;
    std::atomic<size_t> dropped_{0};
    std::atomic<size_t> processed_{0};

    std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<LogEntry> queue_;
    std::thread worker_;
};

// EN: Convenience macro with source location
#define LOG(logger, level, msg) \
    (logger).log(level, msg, __FILE__, __LINE__)

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Log Rotation
// ═════════════════════════════════════════════════════════════════════════════

// EN: Rotating file logger — rotates when file exceeds max size
class RotatingFileLogger {
public:
    RotatingFileLogger(const std::string& base_path,
                       size_t max_size_bytes,
                       size_t max_files)
        : base_path_(base_path)
        , max_size_(max_size_bytes)
        , max_files_(max_files)
        , current_size_(0) {
        open_file();
    }

    ~RotatingFileLogger() {
        if (file_.is_open()) file_.close();
    }

    void write(const LogEntry& entry) {
        std::lock_guard<std::mutex> lk(mtx_);
        auto text = entry.format_text() + "\n";
        auto len = text.size();

        if (current_size_ + len > max_size_) {
            rotate();
        }

        file_ << text;
        file_.flush();
        current_size_ += len;
    }

    size_t rotation_count() const { return rotation_count_; }

private:
    void open_file() {
        if (file_.is_open()) file_.close();
        file_.open(base_path_, std::ios::app);
        if (fs::exists(base_path_)) {
            current_size_ = fs::file_size(base_path_);
        }
    }

    void rotate() {
        file_.close();
        // EN: Shift existing rotated files: .2 → .3, .1 → .2
        for (size_t i = max_files_ - 1; i > 0; --i) {
            auto src = base_path_ + "." + std::to_string(i);
            auto dst = base_path_ + "." + std::to_string(i + 1);
            if (fs::exists(src)) {
                std::error_code ec;
                fs::rename(src, dst, ec);
            }
        }
        // EN: Current → .1
        if (fs::exists(base_path_)) {
            std::error_code ec;
            fs::rename(base_path_, base_path_ + ".1", ec);
        }
        // EN: Remove excess files
        auto excess = base_path_ + "." + std::to_string(max_files_);
        if (fs::exists(excess)) {
            std::error_code ec;
            fs::remove(excess, ec);
        }
        current_size_ = 0;
        file_.open(base_path_, std::ios::out);
        rotation_count_++;
    }

    std::string base_path_;
    size_t max_size_;
    size_t max_files_;
    size_t current_size_;
    size_t rotation_count_ = 0;
    std::ofstream file_;
    std::mutex mtx_;
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 06 - Advanced Logging\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Log Levels & Formatting ────────────────────────────────
    {
        std::cout << "--- Demo 1: Log Levels & Formatting ---\n";

        LogEntry entry{LogLevel::Info, "Engine started successfully",
                       "ecu_main.cpp", 42,
                       std::this_thread::get_id(),
                       std::chrono::system_clock::now()};

        std::cout << "  Text: " << entry.format_text() << "\n";
        std::cout << "  JSON: " << entry.format_json() << "\n\n";

        // EN: Show all log levels with colors
        for (auto lvl : {LogLevel::Debug, LogLevel::Info, LogLevel::Warning,
                         LogLevel::Error, LogLevel::Critical}) {
            std::cout << "  " << level_color(lvl) << "[" << to_string(lvl) << "]"
                      << " Sample message at this level\033[0m\n";
        }
        std::cout << "\n";
    }

    // ─── Demo 2: Async Logger ───────────────────────────────────────────
    {
        std::cout << "--- Demo 2: Async Logger ---\n";

        AsyncLogger logger("ecu_async", LogLevel::Info);

        // EN: Multiple threads logging simultaneously
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&logger, i]() {
                for (int j = 0; j < 5; ++j) {
                    LOG(logger, LogLevel::Info,
                        "Thread " + std::to_string(i) + " msg " + std::to_string(j));
                }
            });
        }
        for (auto& t : threads) t.join();

        // EN: Give worker thread time to process
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::cout << "  Processed: " << logger.processed_count()
                  << ", Dropped: " << logger.dropped_count() << "\n\n";
    }

    // ─── Demo 3: Log Rotation ───────────────────────────────────────────
    {
        std::cout << "--- Demo 3: Log Rotation ---\n";

        auto log_path = fs::temp_directory_path() / "ecu_rotating.log";
        {
            // EN: Create rotating logger: 200 bytes per file, keep 3 rotated files
            RotatingFileLogger rot_logger(log_path.string(), 200, 3);

            for (int i = 0; i < 20; ++i) {
                LogEntry entry{LogLevel::Info,
                               "Sensor reading #" + std::to_string(i) + ": 25.3C",
                               "sensors.cpp", 100 + i,
                               std::this_thread::get_id(),
                               std::chrono::system_clock::now()};
                rot_logger.write(entry);
            }
            std::cout << "  Rotations: " << rot_logger.rotation_count() << "\n";
        }

        // EN: Show rotated files
        for (int i = 0; i <= 3; ++i) {
            auto p = (i == 0) ? log_path : fs::path(log_path.string() + "." + std::to_string(i));
            if (fs::exists(p)) {
                std::cout << "  " << p.filename().string()
                          << ": " << fs::file_size(p) << " bytes\n";
            }
        }

        // EN: Cleanup
        for (int i = 0; i <= 3; ++i) {
            auto p = (i == 0) ? log_path : fs::path(log_path.string() + "." + std::to_string(i));
            std::error_code ec;
            fs::remove(p, ec);
        }
        std::cout << "\n";
    }

    // ─── Demo 4: spdlog Reference ──────────────────────────────────────
    {
        std::cout << "--- Demo 4: spdlog Quick Reference ---\n";
        std::cout << "  CMakeLists.txt:\n";
        std::cout << "    FetchContent_Declare(spdlog\n";
        std::cout << "      GIT_REPOSITORY https://github.com/gabime/spdlog\n";
        std::cout << "      GIT_TAG v1.13.0)\n";
        std::cout << "    FetchContent_MakeAvailable(spdlog)\n";
        std::cout << "    target_link_libraries(app PRIVATE spdlog::spdlog)\n\n";
        std::cout << "  Usage:\n";
        std::cout << "    #include <spdlog/spdlog.h>\n";
        std::cout << "    #include <spdlog/sinks/rotating_file_sink.h>\n\n";
        std::cout << "    auto logger = spdlog::rotating_logger_mt(\n";
        std::cout << "        \"ecu\", \"logs/ecu.log\", 5*1024*1024, 3);\n";
        std::cout << "    logger->info(\"Sensor {}: {:.2f}C\", id, temp);\n";
        std::cout << "    logger->set_level(spdlog::level::debug);\n\n";
        std::cout << "  Async:\n";
        std::cout << "    spdlog::init_thread_pool(8192, 1);\n";
        std::cout << "    auto async = spdlog::create_async<rotating_file_sink_mt>(\n";
        std::cout << "        \"async_ecu\", \"logs/ecu.log\", 5*1024*1024, 3);\n\n";
    }

    // ─── Demo 5: Logging Best Practices ─────────────────────────────────
    {
        std::cout << "--- Demo 5: Logging Best Practices ---\n";
        std::cout << "  ┌───────────────────────────────┬──────────────────────────────────┐\n";
        std::cout << "  │ Practice                      │ Why                              │\n";
        std::cout << "  ├───────────────────────────────┼──────────────────────────────────┤\n";
        std::cout << "  │ Use log levels properly       │ Filter noise, find issues fast   │\n";
        std::cout << "  │ Include context (IDs, values) │ Debug without reproducing        │\n";
        std::cout << "  │ Structured format (JSON)      │ Machine-parseable, searchable    │\n";
        std::cout << "  │ Async logging in hot paths    │ Don't block business logic       │\n";
        std::cout << "  │ Rotate by size + count        │ Don't fill disk                  │\n";
        std::cout << "  │ Thread ID in log entries      │ Trace concurrent execution       │\n";
        std::cout << "  │ Never log secrets/passwords   │ Security compliance              │\n";
        std::cout << "  │ Rate-limit repeated errors    │ Don't flood on known issue       │\n";
        std::cout << "  └───────────────────────────────┴──────────────────────────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Advanced Logging\n";
    std::cout << "============================================\n";

    return 0;
}
