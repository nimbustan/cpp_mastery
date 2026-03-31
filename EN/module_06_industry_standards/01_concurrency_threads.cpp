/**
 * @file module_06_industry_standards/01_concurrency_threads.cpp
 * @brief Industry Standards: Threading and Concurrency
 *
 * @details
 * =============================================================================
 * [THEORY: Why Multithreading?]
 * =============================================================================
 * EN: Single-threaded programs execute one line at a time. If a video is rendering and taking 5
 * minutes, the entire UI freezes. Modern CPUs have 8, 16, or 32 cores. `std::thread` allows C++
 * to split work across these cores simultaneously.
 *
 *
 * =============================================================================
 * [THEORY: Race Condition and Mutex]
 * =============================================================================
 * EN: Two threads trying to write to the SAME variable at the EXACT same nanosecond causes a
 * "Race Condition" (corruption). We must protect shared data. `std::mutex` acts like a bathroom
 * key. Only one thread can hold the key, modify data, and return the key. Others wait at the
 * door.
 *
 *
 * =============================================================================
 * [THEORY: std::atomic — Lock-Free Thread Safety]
 * =============================================================================
 * EN: For simple types (int, bool, pointers), `std::atomic<T>` provides thread-safe reads and
 * writes WITHOUT needing a mutex. The CPU guarantees the operation completes in a single
 * indivisible step. This is dramatically faster than mutex for counters, flags, and simple
 * shared state. Operations like `fetch_add()`, `compare_exchange_strong()`, and `store()/load()`
 * are all atomic.
 *
 *
 * =============================================================================
 * [THEORY: std::condition_variable — Thread Signaling]
 * =============================================================================
 * EN: Sometimes a thread needs to WAIT until a certain condition is met (e.g., "wait until the
 * queue has data"). Busy-waiting (spin loop) wastes CPU. `std::condition_variable` lets a thread
 * sleep (zero CPU) until another thread calls `notify_one()` or `notify_all()`. This is the
 * foundation of Producer-Consumer patterns.
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: Data Race is PURE Undefined Behavior!]
 * =============================================================================
 * EN: CppReference explicitly states: A Data Race isn't just "getting the wrong math result". It
 * is strictly Undefined Behavior (UB)! The compiler is allowed to completely optimize away your
 * loop, crash the program, or print garbage. Even READING a variable while another thread is
 * WRITING to it is a Data Race!
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: std::atomic Memory Ordering — The Hidden Complexity]
 * =============================================================================
 * EN: By default, `std::atomic` uses `memory_order_seq_cst` (sequentially consistent), the
 * STRONGEST and slowest ordering. For performance-critical code, weaker orderings like
 * `memory_order_relaxed` (no ordering guarantees), `memory_order_acquire/release`
 * (one-directional fence) exist. Using the wrong ordering leads to subtle, platform-specific
 * bugs that only appear on ARM/weak-memory architectures.
 * cppreference.com/w/cpp/atomic/memory_order
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: Spurious Wakeups in condition_variable]
 * =============================================================================
 * EN: `condition_variable::wait()` can wake up WITHOUT anyone calling `notify_*()`. This is
 * called a "Spurious Wakeup" and is allowed by the POSIX and C++ standards. Always use the
 * predicate overload: `cv.wait(lock, [&]{ return !queue.empty(); })` to handle this. The lambda
 * is re-checked every time the thread wakes, spurious or not.
 * cppreference.com/w/cpp/thread/condition_variable/wait
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_concurrency_threads.cpp -o 01_concurrency_threads
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// EN: Shared Key
std::mutex mtx;
int sharedBankAccount = 0;

/**
 * @brief EN: A task to be run by multiple threads at the same time.
 */
void depositMoney(int id, int amount) {
  // EN: Thread ID for identification
  (void)id;
  for (int i = 0; i < 10000; i++) {
    // [THEORY: std::lock_guard & RAII]
    // EN: lock_guard works like unique_ptr (RAII). It locks the mutex when entering the scope
    // {...} and AUTOMATICALLY unlocks it when the scope ends. No risk of Deadlocks even if an
    // exception is thrown!
    //

    std::lock_guard<std::mutex> lock(mtx);

    // EN: CRITICAL SECTION. Only ONE thread is allowed here at any given time.
    sharedBankAccount += amount;

  // EN: lock_guard dies here and returns the key.
  }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEMO 2: std::atomic - Lock-Free Counter]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
std::atomic<int> atomicCounter{0};

void atomicIncrement(int iterations) {
  for (int i = 0; i < iterations; i++) {
    // EN: fetch_add is ONE atomic CPU instruction — no mutex needed!
    atomicCounter.fetch_add(1, std::memory_order_relaxed);
  }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEMO 3: Producer-Consumer with condition_variable]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
std::queue<int> taskQueue;
std::mutex queueMtx;
std::condition_variable cv;
bool producerDone = false;

/**
 * @brief EN: Producer pushes tasks into the queue and notifies consumers.
 */
void producer(int taskCount) {
  for (int i = 1; i <= taskCount; i++) {
    {
      std::lock_guard<std::mutex> lock(queueMtx);
      taskQueue.push(i);
      std::cout << "[PRODUCER] Pushed task #" << i << std::endl;
    }
    // EN: Wake ONE waiting consumer thread
    cv.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  {
    std::lock_guard<std::mutex> lock(queueMtx);
    producerDone = true;
  }
  // EN: Wake ALL remaining consumers to let them exit.
  cv.notify_all();
}

/**
 * @brief EN: Consumer waits for tasks and processes them.
 */
void consumer(int id) {
  while (true) {
    std::unique_lock<std::mutex> lock(queueMtx);
    // EN: Wait with predicate to handle SPURIOUS WAKEUPS safely!
    cv.wait(lock, [] { return !taskQueue.empty() || producerDone; });

    if (taskQueue.empty() && producerDone) {
      // EN: No more work
      break;
    }

    int task = taskQueue.front();
    taskQueue.pop();
    // EN: Release lock before processing
    lock.unlock();

    std::cout << "[CONSUMER " << id << "] Processing task #" << task << std::endl;
  }
}

int main() {
  std::cout << "=== MODULE 6: INDUSTRY STANDARDS - CONCURRENCY ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 1: Mutex-Protected Bank Account (Original Demo)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- PART 1: std::mutex + std::lock_guard ---\n" << std::endl;
  std::cout << "Main Thread started.\n" << std::endl;

  std::vector<std::thread> workers;
  for (int i = 0; i < 10; i++) {
    workers.push_back(std::thread(depositMoney, i, 1));
  }

  std::cout << "10 Threads LAUNCHED! Running massively in the background..."
            << std::endl;

  for (auto &w : workers) {
    w.join();
  }

  std::cout << "Expected Bank Account: 100000" << std::endl;
  std::cout << "Actual Bank Account:   " << sharedBankAccount << std::endl;

  if (sharedBankAccount == 100000) {
    std::cout << "=> [SUCCESS] std::mutex prevented the Race Condition!\n" << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 2: std::atomic — Lock-Free Counter
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: 10 threads each increment an atomic counter 10000 times. No mutex needed! The CPU
  // handles atomicity at the hardware level.
  std::cout << "--- PART 2: std::atomic (Lock-Free Counter) ---\n" << std::endl;

  std::vector<std::thread> atomicWorkers;
  for (int i = 0; i < 10; i++) {
    atomicWorkers.emplace_back(atomicIncrement, 10000);
  }
  for (auto &w : atomicWorkers) {
    w.join();
  }

  std::cout << "Expected atomic counter: 100000" << std::endl;
  std::cout << "Actual atomic counter:   " << atomicCounter.load() << std::endl;

  if (atomicCounter.load() == 100000) {
    std::cout << "=> [SUCCESS] std::atomic is lock-free and correct!\n" << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 3: Producer-Consumer with condition_variable
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Classic multi-threaded pattern: 1 producer pushes tasks, 2 consumers process them.
  // condition_variable coordinates without busy-waiting.
  std::cout << "--- PART 3: Producer-Consumer (condition_variable) ---\n"
            << std::endl;

  std::thread prod(producer, 6);
  std::thread cons1(consumer, 1);
  std::thread cons2(consumer, 2);

  prod.join();
  cons1.join();
  cons2.join();

  std::cout << "\n=> Producer-Consumer pattern completed with zero busy-waiting!"
            << std::endl;

  return 0;
}
