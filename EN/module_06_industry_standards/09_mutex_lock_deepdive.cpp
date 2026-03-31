/**
 * @file module_06_industry_standards/09_mutex_lock_deepdive.cpp
 * @brief Mutex & Lock Deep Dive
 *
 * @details
 * =============================================================================
 * [THEORY: C++ Lock Guard Hierarchy]
 * =============================================================================
 *
 * EN: C++ provides multiple lock wrappers with increasing flexibility:
 *
 *     ┌──────────────────────┬───────────────────────────────────────────────┐
 *     │ Type                 │ Use Case                                      │
 *     ├──────────────────────┼───────────────────────────────────────────────┤
 *     │ lock_guard           │ Simple RAII lock, no manual unlock/relock.    │
 *     │                      │ Locks in ctor, unlocks in dtor. Fastest.      │
 *     ├──────────────────────┼───────────────────────────────────────────────┤
 *     │ unique_lock          │ Deferred locking, try_lock, timed_lock,       │
 *     │                      │ manual unlock/relock. Required for cond_var.  │
 *     ├──────────────────────┼───────────────────────────────────────────────┤
 *     │ scoped_lock (C++17)  │ Locks MULTIPLE mutexes atomically without     │
 *     │                      │ deadlock (uses deadlock avoidance algorithm). │
 *     ├──────────────────────┼───────────────────────────────────────────────┤
 *     │ shared_lock (C++17)  │ Shared (reader) access with shared_mutex.     │
 *     │                      │ Multiple readers, exclusive writer.           │
 *     └──────────────────────┴───────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: std::unique_lock]
 * =============================================================================
 *
 * EN: unique_lock is the Swiss Army knife of locks:
 *
 *     unique_lock<mutex> lk(mtx);                  // Lock immediately
 *     unique_lock<mutex> lk(mtx, defer_lock);      // Don't lock yet
 *     unique_lock<mutex> lk(mtx, try_to_lock);     // Try, don't block
 *     unique_lock<mutex> lk(mtx, adopt_lock);      // Already locked
 *
 *     lk.lock();           // Lock manually (after defer_lock)
 *     lk.unlock();         // Unlock manually (e.g., for long non-critical section)
 *     lk.try_lock();       // Non-blocking attempt
 *     lk.owns_lock();      // Check if currently locked
 *
 *     WHY use unique_lock over lock_guard?
 *     1. condition_variable::wait() REQUIRES unique_lock
 *     2. You need to unlock early (minimize lock scope)
 *     3. You need try_lock or timed_lock
 *     4. You need to transfer lock ownership (move)
 *
 * =============================================================================
 * [THEORY: std::shared_mutex — Reader-Writer Lock]
 * =============================================================================
 *
 * EN: shared_mutex (C++17) allows:
 *     - Multiple simultaneous READERS (shared_lock)
 *     - Exclusive single WRITER (unique_lock or lock_guard)
 *
 *     Perfect for: caches, config stores, sensor data buffers
 *     where reads are much more frequent than writes.
 *
 *     Read path:  shared_lock<shared_mutex> lk(mtx);  // Multiple ok
 *     Write path: unique_lock<shared_mutex> lk(mtx);  // Exclusive
 *
 *     ┌──────────────┬──────────┬──────────────────────────────────────┐
 *     │ Scenario     │ Readers  │ Writer                               │
 *     ├──────────────┼──────────┼──────────────────────────────────────┤
 *     │ Read-only    │ N (∞)    │ 0 — all readers proceed concurrently │
 *     │ Write        │ 0        │ 1 — writer waits for readers to end  │
 *     │ Read + Write │ blocked  │ 1 — readers wait until writer done   │
 *     └──────────────┴──────────┴──────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: std::scoped_lock — Deadlock-Free Multi-Lock]
 * =============================================================================
 *
 * EN: scoped_lock (C++17) locks multiple mutexes atomically using a
 *     deadlock avoidance algorithm (std::lock under the hood).
 *
 *     DEADLOCK EXAMPLE (without scoped_lock):
 *       Thread 1: lock(A) → lock(B)  // A then B
 *       Thread 2: lock(B) → lock(A)  // B then A → DEADLOCK!
 *
 *     scoped_lock solution:
 *       Thread 1: scoped_lock lk(A, B);  // Always locks in safe order
 *       Thread 2: scoped_lock lk(A, B);  // Same — no deadlock possible
 *
 *     Rule: ALWAYS prefer scoped_lock when locking multiple mutexes.
 *
 * =============================================================================
 * [THEORY: std::recursive_mutex]
 * =============================================================================
 *
 * EN: recursive_mutex allows the same thread to lock multiple times.
 *     It maintains a lock count — unlock must be called the same number of times.
 *
 *     When to use: Functions that call each other, all needing the lock.
 *     WARNING: Recursive mutex is often a design smell! Consider refactoring:
 *       - Extract locked helper (private, no lock) + public locking wrapper
 *       - This avoids the overhead and complexity of recursive_mutex
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 09_mutex_lock_deepdive.cpp -o 09_mutex_lock_deepdive
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <atomic>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: std::unique_lock — Flexible Locking
// ═════════════════════════════════════════════════════════════════════════════

// EN: Sensor data buffer with deferred locking
class SensorBuffer {
public:
    void update(double value) {
        // EN: Deferred locking — lock only when needed
        std::unique_lock<std::mutex> lk(mtx_, std::defer_lock);

        // EN: Some non-critical preprocessing (no lock needed here)
        double processed = value * 1.8 + 32.0;  // Celsius → Fahrenheit

        // EN: Now lock for the critical section
        lk.lock();
        buffer_.push_back(processed);
        last_update_ = std::chrono::steady_clock::now();
        // EN: lk automatically unlocks when it goes out of scope
    }

    double average() const {
        std::lock_guard<std::mutex> lk(mtx_);
        if (buffer_.empty()) return 0.0;
        double sum = 0.0;
        for (auto v : buffer_) sum += v;
        return sum / static_cast<double>(buffer_.size());
    }

    size_t size() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return buffer_.size();
    }

    bool try_update(double value) {
        // EN: try_to_lock — don't block if mutex is busy
        std::unique_lock<std::mutex> lk(mtx_, std::try_to_lock);
        if (!lk.owns_lock()) {
            return false;  // EN: Mutex busy, caller can retry or skip
        }
        buffer_.push_back(value);
        return true;
    }

private:
    mutable std::mutex mtx_;
    std::vector<double> buffer_;
    std::chrono::steady_clock::time_point last_update_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: std::shared_mutex — Reader-Writer Lock
// ═════════════════════════════════════════════════════════════════════════════

// EN: Configuration store — reads are frequent, writes are rare
class ConfigStore {
public:
    void set(const std::string& key, const std::string& value) {
        // EN: Exclusive lock for writing — blocks ALL readers and writers
        std::unique_lock<std::shared_mutex> lk(mtx_);
        config_[key] = value;
        write_count_++;
    }

    std::string get(const std::string& key) const {
        // EN: Shared lock for reading — multiple readers can proceed
        std::shared_lock<std::shared_mutex> lk(mtx_);
        read_count_++;
        auto it = config_.find(key);
        return (it != config_.end()) ? it->second : "(not found)";
    }

    size_t read_ops() const { return read_count_.load(); }
    size_t write_ops() const { return write_count_.load(); }

private:
    mutable std::shared_mutex mtx_;
    std::map<std::string, std::string> config_;
    mutable std::atomic<size_t> read_count_{0};
    std::atomic<size_t> write_count_{0};
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: std::scoped_lock — Deadlock-Free Multi-Lock
// ═════════════════════════════════════════════════════════════════════════════

// EN: Bank accounts — transfer requires locking TWO mutexes
class BankAccount {
public:
    explicit BankAccount(std::string name, double balance)
        : name_(std::move(name)), balance_(balance) {}

    // EN: SAFE transfer using scoped_lock — no deadlock!
    static void transfer(BankAccount& from, BankAccount& to, double amount) {
        // EN: scoped_lock atomically locks BOTH mutexes in a safe order
        //     Even if another thread calls transfer(to, from, ...) simultaneously
        std::scoped_lock lock(from.mtx_, to.mtx_);

        if (from.balance_ >= amount) {
            from.balance_ -= amount;
            to.balance_ += amount;
        }
    }

    double balance() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return balance_;
    }
    const std::string& name() const { return name_; }

private:
    std::string name_;
    mutable std::mutex mtx_;
    double balance_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: std::recursive_mutex — And Why to Avoid It
// ═════════════════════════════════════════════════════════════════════════════

// EN: BAD DESIGN — needs recursive_mutex because public functions call each other
class SensorLoggerBad {
public:
    void log(const std::string& msg) {
        std::lock_guard<std::recursive_mutex> lk(mtx_);
        logs_.push_back(msg);
    }

    void log_with_count(const std::string& msg) {
        // EN: This calls log(), which also locks mtx_ → needs recursive_mutex!
        std::lock_guard<std::recursive_mutex> lk(mtx_);
        log(msg + " (count: " + std::to_string(logs_.size()) + ")");
    }

    size_t count() const {
        std::lock_guard<std::recursive_mutex> lk(mtx_);
        return logs_.size();
    }

private:
    mutable std::recursive_mutex mtx_;
    std::vector<std::string> logs_;
};

// EN: GOOD DESIGN — refactored to avoid recursive_mutex
class SensorLoggerGood {
public:
    void log(const std::string& msg) {
        std::lock_guard<std::mutex> lk(mtx_);
        log_impl(msg);  // EN: Delegate to unlocked helper
    }

    void log_with_count(const std::string& msg) {
        std::lock_guard<std::mutex> lk(mtx_);
        log_impl(msg + " (count: " + std::to_string(logs_.size()) + ")");
    }

    size_t count() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return logs_.size();
    }

private:
    // EN: Private helper — assumes lock is already held (no locking!)
    void log_impl(const std::string& msg) {
        logs_.push_back(msg);
    }

    mutable std::mutex mtx_;
    std::vector<std::string> logs_;
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 06 - Mutex & Lock Deep Dive\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: unique_lock Deferred & Try Locking ─────────────────────
    {
        std::cout << "--- Demo 1: std::unique_lock ---\n";
        SensorBuffer buffer;

        // EN: Multiple threads updating sensor buffer
        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i) {
            threads.emplace_back([&buffer, i]() {
                for (int j = 0; j < 100; ++j) {
                    buffer.update(20.0 + static_cast<double>(i) + static_cast<double>(j) * 0.1);
                }
            });
        }
        for (auto& t : threads) t.join();

        std::cout << "    Buffer size: " << buffer.size() << " readings\n";
        std::cout << "    Average (Fahrenheit): " << buffer.average() << "\n";

        // EN: Demonstrate try_lock
        int success = 0, fail = 0;
        std::vector<std::thread> try_threads;
        std::mutex count_mtx;
        for (int i = 0; i < 10; ++i) {
            try_threads.emplace_back([&]() {
                for (int j = 0; j < 50; ++j) {
                    bool ok = buffer.try_update(99.9);
                    std::lock_guard<std::mutex> lk(count_mtx);
                    if (ok) success++; else fail++;
                }
            });
        }
        for (auto& t : try_threads) t.join();
        std::cout << "    try_update: " << success << " success, "
                  << fail << " skipped (mutex busy)\n\n";
    }

    // ─── Demo 2: shared_mutex Reader-Writer ─────────────────────────────
    {
        std::cout << "--- Demo 2: std::shared_mutex (Reader-Writer) ---\n";
        ConfigStore config;

        // EN: Writer thread — updates config periodically
        std::thread writer([&config]() {
            for (int i = 0; i < 10; ++i) {
                config.set("sensor_rate", std::to_string(100 + i * 10));
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });

        // EN: Reader threads — read config frequently (much more than writes)
        std::vector<std::thread> readers;
        for (int i = 0; i < 5; ++i) {
            readers.emplace_back([&config]() {
                for (int j = 0; j < 100; ++j) {
                    [[maybe_unused]] auto val = config.get("sensor_rate");
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            });
        }

        writer.join();
        for (auto& t : readers) t.join();

        std::cout << "    Read ops:  " << config.read_ops() << "\n";
        std::cout << "    Write ops: " << config.write_ops() << "\n";
        std::cout << "    Final sensor_rate: " << config.get("sensor_rate") << "\n";
        std::cout << "    (shared_lock allows concurrent reads!)\n\n";
    }

    // ─── Demo 3: scoped_lock Deadlock-Free Transfer ─────────────────────
    {
        std::cout << "--- Demo 3: std::scoped_lock (Deadlock-Free) ---\n";
        BankAccount alice("Alice", 1000.0);
        BankAccount bob("Bob", 1000.0);

        std::cout << "    Before: Alice=" << alice.balance()
                  << ", Bob=" << bob.balance() << "\n";

        // EN: Two threads doing opposite transfers — scoped_lock prevents deadlock
        std::thread t1([&]() {
            for (int i = 0; i < 1000; ++i) {
                BankAccount::transfer(alice, bob, 1.0);
            }
        });
        std::thread t2([&]() {
            for (int i = 0; i < 1000; ++i) {
                BankAccount::transfer(bob, alice, 1.0);
            }
        });
        t1.join();
        t2.join();

        std::cout << "    After:  Alice=" << alice.balance()
                  << ", Bob=" << bob.balance() << "\n";
        std::cout << "    Total preserved: " << (alice.balance() + bob.balance())
                  << " (should be 2000)\n\n";
    }

    // ─── Demo 4: recursive_mutex vs Refactored Design ───────────────────
    {
        std::cout << "--- Demo 4: recursive_mutex vs Good Design ---\n";

        // EN: BAD: uses recursive_mutex
        SensorLoggerBad bad_logger;
        bad_logger.log_with_count("Sensor reading");
        bad_logger.log("Direct log");
        std::cout << "    Bad logger (recursive_mutex): "
                  << bad_logger.count() << " entries\n";

        // EN: GOOD: refactored, uses regular mutex
        SensorLoggerGood good_logger;
        good_logger.log_with_count("Sensor reading");
        good_logger.log("Direct log");
        std::cout << "    Good logger (regular mutex):  "
                  << good_logger.count() << " entries\n";

        std::cout << "\n    recursive_mutex: Same thread can lock multiple times\n";
        std::cout << "    BETTER: Extract unlocked helper + locking wrapper\n\n";
    }

    // ─── Demo 5: Lock Type Decision Table ────────────────────────────────
    {
        std::cout << "--- Demo 5: Lock Type Decision Table ---\n";
        std::cout << "  ┌───────────────────────┬────────────────────────────────────┐\n";
        std::cout << "  │ Scenario              │ Use                                │\n";
        std::cout << "  ├───────────────────────┼────────────────────────────────────┤\n";
        std::cout << "  │ Simple critical sect. │ lock_guard (fastest, simplest)     │\n";
        std::cout << "  │ condition_variable    │ unique_lock (REQUIRED)             │\n";
        std::cout << "  │ Try without blocking  │ unique_lock + try_to_lock          │\n";
        std::cout << "  │ Unlock early          │ unique_lock + lk.unlock()          │\n";
        std::cout << "  │ Lock 2+ mutexes       │ scoped_lock (deadlock-free)        │\n";
        std::cout << "  │ Many readers, 1 writer│ shared_mutex + shared_lock         │\n";
        std::cout << "  │ Recursive call chain  │ Refactor! (or recursive_mutex)     │\n";
        std::cout << "  └───────────────────────┴────────────────────────────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Mutex & Lock Deep Dive\n";
    std::cout << "============================================\n";

    return 0;
}
