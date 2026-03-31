/**
 * @file module_10_design_patterns/07_embedded_automotive_patterns.cpp
 * @brief Embedded & Automotive Design Patterns
 *
 * @details
 * =============================================================================
 * [THEORY: GoF vs Embedded Patterns]
 * =============================================================================
 *
 * EN: Classic GoF design patterns assume a desktop/server environment: heap allocation is
 *     cheap, virtual dispatch overhead is negligible, and memory is abundant. In embedded
 *     and automotive systems, these assumptions BREAK:
 *
 *     ┌────────────────────────┬────────────────────────┬────────────────────────┐
 *     │ Assumption             │ Desktop/Server         │ Embedded/Automotive    │
 *     ├────────────────────────┼────────────────────────┼────────────────────────┤
 *     │ Heap allocation        │ Fast, abundant         │ Forbidden or costly    │
 *     │ Virtual dispatch       │ ~ns overhead, fine     │ Predictability matters │
 *     │ Memory                 │ GB available           │ KB-MB (MCU: 32-512KB)  │
 *     │ Exceptions             │ Common error handling  │ Often disabled (-fno-) │
 *     │ RTTI (typeid/dyn_cast) │ Available              │ Usually disabled       │
 *     │ Dynamic containers     │ vector, map freely     │ Static buffers only    │
 *     │ Thread model           │ std::thread, pthreads  │ RTOS tasks, ISRs       │
 *     └────────────────────────┴────────────────────────┴────────────────────────┘
 *
 *     This file covers patterns specifically designed for resource-constrained,
 *     real-time, and safety-critical environments (AUTOSAR, RTOS, bare-metal).
 *
 *
 *     ┌────────────────────────┬────────────────────────┬────────────────────────┐
 *     ├────────────────────────┼────────────────────────┼────────────────────────┤
 *     │ Bellek                 │ GB mevcut              │ KB-MB (MCU: 32-512KB)  │
 *     └────────────────────────┴────────────────────────┴────────────────────────┘
 *
 *
 * =============================================================================
 * [THEORY: Memory Pool Pattern]
 * =============================================================================
 *
 * EN: In safety-critical embedded systems (ASIL-B and above), dynamic memory allocation
 *     (malloc/new) is either FORBIDDEN or severely restricted after initialization.
 *     Reasons:
 *     1. Fragmentation: repeated alloc/free fragments memory → eventual OOM
 *     2. Non-deterministic timing: malloc's worst-case time is unbounded
 *     3. No virtual memory: MCUs have no MMU, OOM = hard fault
 *     4. MISRA C++ Rule 21.3: "shall not use dynamic memory after init phase"
 *
 *     Memory Pool allocates a fixed block at startup and serves objects from it:
 *     ┌─────────────────────────────────────────────────────────┐
 *     │ Pool<Sensor, 16>                                        │
 *     │ ┌────┬────┬────┬────┬────┬────┬────┬────┬─── ···        │
 *     │ │ S0 │ S1 │FREE│ S3 │FREE│FREE│ S6 │FREE│               │
 *     │ └────┴────┴────┴────┴────┴────┴────┴────┴─── ···        │
 *     │ Free list: [2] → [4] → [5] → [7] → ···                  │
 *     └─────────────────────────────────────────────────────────┘
 *     Uses placement new to construct objects in pre-allocated storage.
 *
 *
 *     ┌─────────────────────────────────────────────────────────┐
 *     │ ┌────┬────┬────┬────┬────┬────┬────┬────┬─── ···        │
 *     │ └────┴────┴────┴────┴────┴────┴────┴────┴─── ···        │
 *     │ Serbest liste: [2] → [4] → [5] → [7] → ···              │
 *     └─────────────────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: Active Object Pattern]
 * =============================================================================
 *
 * EN: The Active Object pattern decouples METHOD INVOCATION from METHOD EXECUTION.
 *     Instead of calling a method directly (which blocks the caller), the request is
 *     placed in a message queue. A dedicated thread processes requests sequentially.
 *
 *     ┌──────────┐    enqueue()    ┌────────────────┐   dequeue()   ┌──────────┐
 *     │  Caller  │ ──────────────→ │  Message Queue │ ────────────→ │ Worker   │
 *     │ (Client) │                 │  (thread-safe) │               │ Thread   │
 *     └──────────┘                 └────────────────┘               └──────────┘
 *
 *     Why use it in embedded/automotive?
 *     • AUTOSAR Runnables are essentially Active Objects (triggered by OS schedule)
 *     • ISR → Task communication: ISR enqueues, task dequeues safely
 *     • Deterministic ordering: messages processed FIFO
 *     • Thread safety: only the worker thread accesses shared state
 *
 *
 *     ┌──────────┐    enqueue()    ┌─────────────────┐   dequeue()   ┌──────────┐
 *     └──────────┘                 └─────────────────┘               └──────────┘
 *
 *
 * =============================================================================
 * [THEORY: Embedded State Machine]
 * =============================================================================
 *
 * EN: State machines are ubiquitous in embedded systems. Every ECU mode controller,
 *     communication protocol stack, and peripheral driver is fundamentally an FSM.
 *
 *     Two main implementation approaches:
 *     ┌──────────────────────┬────────────────────────────────────────────────┐
 *     │ Approach             │ Characteristics                                │
 *     ├──────────────────────┼────────────────────────────────────────────────┤
 *     │ Switch-Case FSM      │ Simple, inline, fast. Hard to scale.           │
 *     │ Table-Driven FSM     │ Data-driven, extensible, MISRA-friendly.       │
 *     │                      │ State × Event → {Action, NextState}            │
 *     └──────────────────────┴────────────────────────────────────────────────┘
 *
 *     Table-driven FSMs are preferred in automotive (AUTOSAR BSW uses them internally):
 *     - Transition table is const data → stored in FLASH, not RAM
 *     - Adding states/events doesn't change the dispatch logic
 *     - Easy to auto-generate from UML state diagrams
 *     - MISRA C++ 2023 compliant (no complex control flow)
 *
 *
 *     ┌──────────────────────┬────────────────────────────────────────────────┐
 *     ├──────────────────────┼────────────────────────────────────────────────┤
 *     │                      │ Durum × Olay → {Aksiyon, SonrakiDurum}         │
 *     └──────────────────────┴────────────────────────────────────────────────┘
 *
 *
 * =============================================================================
 * [THEORY: Double Buffer & Hardware Proxy]
 * =============================================================================
 *
 * EN: Double Buffer: Used when one entity writes data while another reads it concurrently.
 *     Two buffers alternate roles — while the reader uses buffer A, the writer fills buffer B.
 *     On swap, roles reverse. ISR writes to back buffer; main loop reads front buffer.
 *     No locks needed if swap is atomic (single pointer/index swap).
 *
 *     Hardware Proxy: Wraps raw register access behind an OOP interface.
 *     Instead of: *(volatile uint32_t*)0x40020000 |= (1 << 5);
 *     You write:  gpio.set_pin(5);
 *     Uses CRTP + volatile to achieve zero-cost abstraction over MMIO registers.
 *
 * @see https://en.cppreference.com/w/cpp/language/new — placement new
 * @see https://en.cppreference.com/w/cpp/atomic/atomic — std::atomic for lock-free patterns
 * @see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rr-newdelete — R.10/R.11
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wnull-dereference -Wformat=2 -pthread 07_embedded_automotive_patterns.cpp -o 07_embedded_automotive_patterns
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <array>
#include <new>             // EN: placement new / TR: placement new
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include <optional>
#include <cassert>
#include <iomanip>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Static Memory Pool<T, N> — Heap-Free Object Management
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A fixed-size object pool that allocates from a static array. No heap, no fragmentation.
 *     Uses a free-list (singly linked via indices) for O(1) allocate/deallocate.
 *     placement new constructs objects in-place; explicit destructor call destroys them.
 *
 *     MISRA-friendly: no dynamic allocation after init, deterministic timing.
 *     Suitable for RTOS tasks, ISR-safe if wrapped with a spinlock.
 */
template<typename T, std::size_t N>
class MemoryPool {
    static_assert(N > 0, "Pool size must be > 0");

    // EN: Each slot is a union: either holds a T object or a free-list index.
    //     When free, 'next_free' points to the next available slot.
    //     When occupied, 'storage' holds the constructed T.
    struct Slot {
        alignas(T) std::byte storage[sizeof(T)];
        std::size_t next_free;
        bool occupied = false;
    };

    std::array<Slot, N> slots_;
    std::size_t free_head_;     // EN: Head of the free list
    std::size_t active_count_;  // EN: Number of allocated objects

public:
    MemoryPool() : free_head_(0), active_count_(0) {
        // EN: Initialize free list: slot[0] → slot[1] → ··· → slot[N-1] → SENTINEL
        for (std::size_t i = 0; i < N; ++i) {
            slots_[i].next_free = i + 1;  // EN: Last element points to N (sentinel)
            slots_[i].occupied = false;
        }
    }

    ~MemoryPool() {
        // EN: Destroy all still-alive objects — prevent resource leaks
        for (std::size_t i = 0; i < N; ++i) {
            if (slots_[i].occupied) {
                object_at(i)->~T();
            }
        }
    }

    // EN: Non-copyable, non-movable (owns raw storage)
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    /// EN: Construct a T in the pool. Returns pointer or nullptr if full.
    template<typename... Args>
    T* allocate(Args&&... args) {
        if (free_head_ >= N) return nullptr;  // EN: Pool exhausted / TR: Havuz dolu

        std::size_t idx = free_head_;
        free_head_ = slots_[idx].next_free;   // EN: Advance free list head / TR: Serbest listeyi ilerlet
        slots_[idx].occupied = true;
        ++active_count_;

        // EN: placement new — construct T in pre-allocated storage
        return new (slots_[idx].storage) T(std::forward<Args>(args)...);
    }

    /// EN: Destroy object and return its slot to the free list.
    void deallocate(T* ptr) {
        if (!ptr) return;

        // EN: Find slot index from pointer arithmetic
        auto* byte_ptr = reinterpret_cast<std::byte*>(ptr);
        std::size_t idx = 0;
        bool found = false;
        for (std::size_t i = 0; i < N; ++i) {
            if (slots_[i].storage == byte_ptr) {
                idx = i;
                found = true;
                break;
            }
        }

        if (!found || !slots_[idx].occupied) return;  // EN: Not from this pool

        ptr->~T();                            // EN: Explicit destructor call
        slots_[idx].occupied = false;
        slots_[idx].next_free = free_head_;   // EN: Push to front of free list
        free_head_ = idx;
        --active_count_;
    }

    [[nodiscard]] std::size_t active() const { return active_count_; }
    [[nodiscard]] std::size_t capacity() const { return N; }
    [[nodiscard]] std::size_t available() const { return N - active_count_; }

private:
    T* object_at(std::size_t idx) {
        return reinterpret_cast<T*>(slots_[idx].storage);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Double Buffer for Sensor Data — ISR/DMA Safe
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Double buffer for concurrent producer/consumer without locks.
 *     The producer writes to the "back" buffer; the consumer reads the "front" buffer.
 *     swap() atomically exchanges the two buffer indices.
 *
 *     Real-world use: DMA fills back buffer with ADC samples while the main loop
 *     processes the front buffer. After DMA complete interrupt → swap().
 *
 *     Template parameters:
 *       T = data type (e.g., SensorFrame)
 *       N = number of elements per buffer (e.g., 8 ADC channels)
 *
 *
 */
template<typename T, std::size_t N>
class DoubleBuffer {
    std::array<T, N> buffer_a_{};
    std::array<T, N> buffer_b_{};
    std::atomic<int> active_{0};  // EN: 0=A is front, 1=B is front

public:
    /// EN: Get read-only reference to the front (consumer) buffer.
    [[nodiscard]] const std::array<T, N>& front() const {
        return (active_.load(std::memory_order_acquire) == 0) ? buffer_a_ : buffer_b_;
    }

    /// EN: Get writable reference to the back (producer) buffer.
    [[nodiscard]] std::array<T, N>& back() {
        return (active_.load(std::memory_order_acquire) == 0) ? buffer_b_ : buffer_a_;
    }

    /// EN: Atomically swap front and back buffers. Call this after producer finishes writing.
    void swap() {
        int expected = active_.load(std::memory_order_relaxed);
        int desired = 1 - expected;
        active_.compare_exchange_strong(expected, desired, std::memory_order_release);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Active Object with Message Queue
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Active Object runs tasks asynchronously in a dedicated thread.
 *     Callers enqueue std::function<void()> messages; the worker thread processes them FIFO.
 *     Thread-safe: the queue is protected by a mutex + condition variable.
 *
 *     This is the foundation for:
 *     - AUTOSAR Runnables (OS-scheduled functions without shared state)
 *     - CAN message dispatchers (ISR enqueues, task processes)
 *     - Command pattern with deferred execution
 *
 *
 *     Bunun temeli:
 */
class ActiveObject {
    std::queue<std::function<void()>> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread worker_;
    std::atomic<bool> running_{true};

public:
    ActiveObject() : worker_([this] { run(); }) {}

    ~ActiveObject() {
        // EN: Signal the worker to stop and wait for it to finish
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_one();
        if (worker_.joinable()) worker_.join();
    }

    // EN: Non-copyable (owns a thread)
    ActiveObject(const ActiveObject&) = delete;
    ActiveObject& operator=(const ActiveObject&) = delete;

    /// EN: Enqueue a task for asynchronous execution.
    void enqueue(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(task));
        }
        cv_.notify_one();
    }

    /// EN: Wait until all pending tasks are processed.
    void flush() {
        // EN: Enqueue a synchronization task and wait on its completion
        std::mutex m;
        std::condition_variable cv;
        bool done = false;

        enqueue([&] {
            std::lock_guard<std::mutex> lock(m);
            done = true;
            cv.notify_one();
        });

        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&] { return done; });
    }

private:
    void run() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return !queue_.empty() || !running_; });

                if (!running_ && queue_.empty()) return;

                task = std::move(queue_.front());
                queue_.pop();
            }
            task();  // EN: Execute outside the lock
        }
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Table-Driven State Machine — MISRA Compliant
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A generic table-driven FSM where the transition table is const data.
 *     State × Event → { guard(), action(), next_state }
 *     The dispatch loop is fixed; only the table changes per application.
 *
 *     This is the pattern used in AUTOSAR Basic Software (BSW) modules:
 *     - BswM (BSW Mode Manager)
 *     - ComM (Communication Manager)
 *     - EcuM (ECU State Manager)
 *
 *
 *     - BswM
 *     - EcuM
 */

// EN: ECU operating states
enum class EcuState : uint8_t {
    STARTUP,      // EN: Power-on initialization
    NORMAL,       // EN: Normal operation mode
    DIAGNOSTIC,   // EN: Diagnostic session active
    SLEEP,        // EN: Low-power sleep mode
    COUNT         // EN: Number of states (sentinel)
};

// EN: Events that trigger state transitions
enum class EcuEvent : uint8_t {
    INIT_COMPLETE,     // EN: Initialization finished
    DIAG_REQUEST,      // EN: Diagnostic request received
    DIAG_TIMEOUT,      // EN: Diagnostic session timed out
    SLEEP_REQUEST,     // EN: Sleep mode requested (ignition off) / TR: Uyku modu istendi
    WAKEUP,            // EN: Wake-up signal (CAN, LIN, timer) / TR: Uyanma sinyali
    COUNT              // EN: Number of events (sentinel)
};

// EN: Convert enum to string for logging
const char* to_string(EcuState s) {
    constexpr const char* names[] = {"STARTUP", "NORMAL", "DIAGNOSTIC", "SLEEP"};
    return names[static_cast<uint8_t>(s)];
}

const char* to_string(EcuEvent e) {
    constexpr const char* names[] = {
        "INIT_COMPLETE", "DIAG_REQUEST", "DIAG_TIMEOUT", "SLEEP_REQUEST", "WAKEUP"
    };
    return names[static_cast<uint8_t>(e)];
}

// EN: Transition entry: guard + action + next state
struct Transition {
    using Guard  = bool(*)();    // EN: Pre-condition check
    using Action = void(*)();    // EN: Side effect / TR: Yan etki

    Guard  guard;                // EN: nullptr = always true
    Action action;               // EN: nullptr = no action / TR: nullptr = aksiyon yok
    EcuState next_state;
};

// EN: No-op transition (stay in current state)
constexpr Transition NO_TRANSITION{nullptr, nullptr, EcuState::COUNT};

// EN: Action functions — side effects of state transitions
void action_start_normal()   { std::cout << "    [ACTION] Starting normal operation — peripherals enabled\n"; }
void action_enter_diag()     { std::cout << "    [ACTION] Entering diagnostic session — UDS active\n"; }
void action_exit_diag()      { std::cout << "    [ACTION] Exiting diagnostic — returning to normal\n"; }
void action_enter_sleep()    { std::cout << "    [ACTION] Entering sleep — CAN transceiver to standby\n"; }
void action_wakeup()         { std::cout << "    [ACTION] Wake-up detected — re-initializing\n"; }

// EN: Guard functions — conditions that must be true for transition
bool guard_always_true() { return true; }

// EN: Transition table — the CORE of the table-driven FSM
//     Indexed: transition_table[state][event]
constexpr std::size_t STATE_COUNT = static_cast<std::size_t>(EcuState::COUNT);
constexpr std::size_t EVENT_COUNT = static_cast<std::size_t>(EcuEvent::COUNT);

// EN: We initialize transition table at runtime because function pointers aren't constexpr
Transition transition_table[STATE_COUNT][EVENT_COUNT];

void init_transition_table() {
    // EN: Fill with NO_TRANSITION (stay in current state for unhandled events)
    for (std::size_t s = 0; s < STATE_COUNT; ++s)
        for (std::size_t e = 0; e < EVENT_COUNT; ++e)
            transition_table[s][e] = NO_TRANSITION;

    // EN: STARTUP state transitions
    transition_table[0][0] = {guard_always_true, action_start_normal, EcuState::NORMAL};

    // EN: NORMAL state transitions
    transition_table[1][1] = {guard_always_true, action_enter_diag,   EcuState::DIAGNOSTIC};
    transition_table[1][3] = {guard_always_true, action_enter_sleep,  EcuState::SLEEP};

    // EN: DIAGNOSTIC state transitions
    transition_table[2][2] = {guard_always_true, action_exit_diag,    EcuState::NORMAL};
    transition_table[2][3] = {guard_always_true, action_enter_sleep,  EcuState::SLEEP};

    // EN: SLEEP state transitions
    transition_table[3][4] = {guard_always_true, action_wakeup,       EcuState::STARTUP};
}

/// EN: The FSM dispatch function — processes one event.
EcuState fsm_dispatch(EcuState current, EcuEvent event) {
    auto s = static_cast<std::size_t>(current);
    auto e = static_cast<std::size_t>(event);

    const auto& t = transition_table[s][e];

    // EN: If no transition defined, stay in current state
    if (t.next_state == EcuState::COUNT) return current;

    // EN: Check guard condition
    if (t.guard && !t.guard()) return current;

    // EN: Execute action
    if (t.action) t.action();

    std::cout << "    [FSM] " << to_string(current) << " --(" << to_string(event)
              << ")--> " << to_string(t.next_state) << "\n";

    return t.next_state;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Hardware Proxy Pattern — CRTP + volatile
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Wraps raw memory-mapped register access behind a type-safe OOP interface.
 *     Uses CRTP so derived classes specify the register base address and layout.
 *     volatile ensures the compiler doesn't optimize away register reads/writes.
 *
 *     On a real MCU, base_address would be e.g., 0x40020000 (GPIO port A on STM32).
 *     Here we simulate with a static array, but the interface is identical.
 *
 *
 */
template<typename Derived>
class RegisterBase {
protected:
    // EN: Read a 32-bit register at given offset from base
    [[nodiscard]] uint32_t read_reg(std::size_t offset) const {
        const volatile uint32_t* reg = static_cast<const Derived*>(this)->reg_ptr(offset);
        return *reg;
    }

    // EN: Write a 32-bit value to register at given offset
    void write_reg(std::size_t offset, uint32_t value) {
        volatile uint32_t* reg = static_cast<Derived*>(this)->reg_ptr(offset);
        *reg = value;
    }

    // EN: Set specific bits (read-modify-write)
    void set_bits(std::size_t offset, uint32_t mask) {
        write_reg(offset, read_reg(offset) | mask);
    }

    // EN: Clear specific bits
    void clear_bits(std::size_t offset, uint32_t mask) {
        write_reg(offset, read_reg(offset) & ~mask);
    }
};

/**
 * EN: Simulated GPIO port — demonstrates the Hardware Proxy pattern.
 *     In production code, regs_ would be removed and reg_ptr() would return
 *     reinterpret_cast<volatile uint32_t*>(BASE_ADDRESS + offset).
 *
 */
class GpioPort : public RegisterBase<GpioPort> {
    // EN: Register offsets (matching STM32 GPIO layout)
    static constexpr std::size_t MODER  = 0;  // EN: Mode register
    static constexpr std::size_t ODR    = 1;  // EN: Output data register
    static constexpr std::size_t IDR    = 2;  // EN: Input data register
    static constexpr std::size_t BSRR   = 3;  // EN: Bit set

    // EN: Simulated register file (in production: MMIO at fixed address)
    uint32_t regs_[4] = {0, 0, 0, 0};

    friend class RegisterBase<GpioPort>;
    volatile uint32_t* reg_ptr(std::size_t offset) {
        return reinterpret_cast<volatile uint32_t*>(&regs_[offset]);
    }
    const volatile uint32_t* reg_ptr(std::size_t offset) const {
        return reinterpret_cast<const volatile uint32_t*>(&regs_[offset]);
    }

public:
    /// EN: Configure pin as output (set 2 mode bits to 01)
    void configure_output(uint8_t pin) {
        assert(pin < 16);
        uint32_t shift = static_cast<uint32_t>(pin) * 2;
        uint32_t mode = read_reg(MODER);
        mode &= ~(0x3u << shift);    // EN: Clear 2 bits / TR: 2 biti temizle
        mode |=  (0x1u << shift);    // EN: Set to output (01)
        write_reg(MODER, mode);
    }

    /// EN: Set pin HIGH using the bit set/reset register (atomic on real HW)
    void set_pin(uint8_t pin) {
        assert(pin < 16);
        write_reg(BSRR, 1u << pin);    // EN: Write to lower 16 bits = SET / TR: Alt 16 bite yaz = AYARLA
        set_bits(ODR, 1u << pin);       // EN: Also update ODR for simulation
    }

    /// EN: Set pin LOW
    void clear_pin(uint8_t pin) {
        assert(pin < 16);
        write_reg(BSRR, 1u << (pin + 16));  // EN: Write to upper 16 bits = RESET
        clear_bits(ODR, 1u << pin);
    }

    /// EN: Read pin state from output data register
    [[nodiscard]] bool read_pin(uint8_t pin) const {
        assert(pin < 16);
        return (read_reg(ODR) & (1u << pin)) != 0;
    }

    /// EN: Read all output pins as bitmask
    [[nodiscard]] uint32_t read_all() const {
        return read_reg(ODR);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: Real-World — ECU Mode Manager (Integrating All Patterns)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Combines the patterns above into a realistic ECU scenario:
 *     - MemoryPool for CAN message objects
 *     - DoubleBuffer for sensor readings
 *     - ActiveObject for async task processing
 *     - Table-Driven FSM for ECU state management
 *     - GpioPort for hardware indicator (LED)
 *
 */
struct CanMessage {
    uint32_t id;
    uint8_t dlc;
    std::array<uint8_t, 8> data;

    friend std::ostream& operator<<(std::ostream& os, const CanMessage& msg) {
        os << "CAN[0x" << std::hex << std::setfill('0') << std::setw(3) << msg.id
           << std::dec << " DLC=" << static_cast<int>(msg.dlc) << " Data=";
        for (int i = 0; i < msg.dlc && i < 8; ++i) {
            os << std::hex << std::setw(2) << static_cast<int>(msg.data[static_cast<std::size_t>(i)]);
            if (i < msg.dlc - 1) os << ":";
        }
        os << std::dec << "]";
        return os;
    }
};

struct SensorReading {
    float temperature;
    float voltage;
    uint32_t timestamp_ms;
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 10 — Embedded & Automotive Patterns\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Static Memory Pool ──────────────────────────────────────
    {
        std::cout << "--- Demo 1: Static Memory Pool<CanMessage, 8> ---\n";
        // EN: Create a pool of 8 CAN messages — no heap allocation after this point
        MemoryPool<CanMessage, 8> pool;
        std::cout << "  Pool capacity: " << pool.capacity()
                  << ", available: " << pool.available() << "\n";

        // EN: Allocate some CAN messages from the pool
        auto* msg1 = pool.allocate(CanMessage{0x1A3, 8, {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0xFF, 0x12, 0x34}});
        auto* msg2 = pool.allocate(CanMessage{0x7DF, 3, {0x02, 0x01, 0x0D, 0, 0, 0, 0, 0}});
        auto* msg3 = pool.allocate(CanMessage{0x100, 2, {0x55, 0xAA, 0, 0, 0, 0, 0, 0}});

        std::cout << "  Allocated 3 messages. Active: " << pool.active()
                  << ", Available: " << pool.available() << "\n";

        if (msg1) std::cout << "  msg1: " << *msg1 << "\n";
        if (msg2) std::cout << "  msg2: " << *msg2 << "\n";
        if (msg3) std::cout << "  msg3: " << *msg3 << "\n";

        // EN: Return msg2 to the pool — slot is recycled
        pool.deallocate(msg2);
        std::cout << "  After deallocating msg2: Active=" << pool.active()
                  << ", Available=" << pool.available() << "\n";

        // EN: Allocate a new message — reuses the freed slot
        auto* msg4 = pool.allocate(CanMessage{0x200, 4, {0x01, 0x02, 0x03, 0x04, 0, 0, 0, 0}});
        if (msg4) std::cout << "  msg4 (reused slot): " << *msg4 << "\n";
        std::cout << "\n";
    }

    // ─── Demo 2: Double Buffer ───────────────────────────────────────────
    {
        std::cout << "--- Demo 2: Double Buffer for Sensor Data ---\n";
        DoubleBuffer<SensorReading, 4> sensor_buf;

        // EN: Simulate ISR/DMA: write to back buffer
        auto& back = sensor_buf.back();
        back[0] = {85.3f, 12.1f, 1000};
        back[1] = {90.1f, 11.9f, 1001};
        back[2] = {76.5f, 12.3f, 1002};
        back[3] = {88.8f, 12.0f, 1003};
        std::cout << "  [Producer] Wrote 4 sensor readings to back buffer\n";

        // EN: Swap buffers — now the producer's data becomes readable
        sensor_buf.swap();
        std::cout << "  [Swap] Buffers exchanged\n";

        // EN: Consumer reads from front buffer
        const auto& front = sensor_buf.front();
        for (std::size_t i = 0; i < 4; ++i) {
            std::cout << "  [Consumer] Sensor[" << i << "]: temp="
                      << front[i].temperature << "°C, voltage="
                      << front[i].voltage << "V, ts=" << front[i].timestamp_ms << "ms\n";
        }
        std::cout << "\n";
    }

    // ─── Demo 3: Active Object ───────────────────────────────────────────
    {
        std::cout << "--- Demo 3: Active Object (Async Task Queue) ---\n";
        ActiveObject worker;

        // EN: Enqueue 5 tasks — they execute asynchronously in the worker thread
        for (int i = 1; i <= 5; ++i) {
            worker.enqueue([i] {
                std::cout << "    [Worker] Processing task #" << i
                          << " (thread: " << std::this_thread::get_id() << ")\n";
            });
        }

        // EN: Flush ensures all enqueued tasks complete before we proceed
        worker.flush();

        std::cout << "  [Main] All tasks completed. Main thread: "
                  << std::this_thread::get_id() << "\n\n";
    }

    // ─── Demo 4: Table-Driven State Machine ──────────────────────────────
    {
        std::cout << "--- Demo 4: ECU Table-Driven State Machine ---\n";
        // EN: Initialize the transition table once
        init_transition_table();

        // EN: Simulate ECU lifecycle: STARTUP → NORMAL → DIAGNOSTIC → NORMAL → SLEEP → STARTUP
        EcuState state = EcuState::STARTUP;
        std::cout << "  Initial state: " << to_string(state) << "\n\n";

        // EN: Event sequence simulating a typical ECU session
        EcuEvent events[] = {
            EcuEvent::INIT_COMPLETE,   // EN: STARTUP → NORMAL / TR: STARTUP → NORMAL
            EcuEvent::DIAG_REQUEST,    // EN: NORMAL → DIAGNOSTIC / TR: NORMAL → DIAGNOSTIC
            EcuEvent::DIAG_TIMEOUT,    // EN: DIAGNOSTIC → NORMAL / TR: DIAGNOSTIC → NORMAL
            EcuEvent::SLEEP_REQUEST,   // EN: NORMAL → SLEEP / TR: NORMAL → SLEEP
            EcuEvent::WAKEUP,          // EN: SLEEP → STARTUP / TR: SLEEP → STARTUP
            EcuEvent::INIT_COMPLETE,   // EN: STARTUP → NORMAL / TR: STARTUP → NORMAL
        };

        for (auto event : events) {
            std::cout << "  Event: " << to_string(event) << "\n";
            state = fsm_dispatch(state, event);
            std::cout << "  Current state: " << to_string(state) << "\n\n";
        }
    }

    // ─── Demo 5: Hardware Proxy ──────────────────────────────────────────
    {
        std::cout << "--- Demo 5: Hardware Proxy (GPIO Port) ---\n";
        GpioPort gpio;

        // EN: Configure pins 0, 5, 13 as outputs (like on STM32 Nucleo: LED on pin 5)
        gpio.configure_output(0);
        gpio.configure_output(5);
        gpio.configure_output(13);

        // EN: Set/clear pins like controlling real hardware LEDs
        gpio.set_pin(0);
        gpio.set_pin(5);
        gpio.set_pin(13);
        std::cout << "  After set_pin(0,5,13): ODR = 0x"
                  << std::hex << gpio.read_all() << std::dec << "\n";
        std::cout << "  Pin 5 state: " << (gpio.read_pin(5) ? "HIGH" : "LOW") << "\n";

        gpio.clear_pin(5);
        std::cout << "  After clear_pin(5): ODR = 0x"
                  << std::hex << gpio.read_all() << std::dec << "\n";
        std::cout << "  Pin 5 state: " << (gpio.read_pin(5) ? "HIGH" : "LOW") << "\n";
        std::cout << "\n";
    }

    // ─── Demo 6: Integrated ECU Scenario ─────────────────────────────────
    {
        std::cout << "--- Demo 6: Integrated ECU Scenario ---\n";
        std::cout << "  Combining: MemoryPool + DoubleBuffer + ActiveObject + FSM + GPIO\n\n";

        // EN: Setup components
        MemoryPool<CanMessage, 16> can_pool;
        DoubleBuffer<SensorReading, 2> sensor_buf;
        ActiveObject ecu_task;
        GpioPort status_led;
        status_led.configure_output(5);  // EN: Status LED on pin 5 / TR: Durum LED'i pin 5'te

        init_transition_table();
        EcuState ecu_state = EcuState::STARTUP;

        // EN: Phase 1: Init complete → NORMAL, LED ON
        ecu_task.enqueue([&] {
            ecu_state = fsm_dispatch(ecu_state, EcuEvent::INIT_COMPLETE);
            status_led.set_pin(5);
            std::cout << "    [ECU] LED ON — system running\n";
        });

        // EN: Phase 2: Receive CAN message, allocate from pool
        ecu_task.enqueue([&] {
            auto* msg = can_pool.allocate(CanMessage{0x7DF, 3, {0x02, 0x01, 0x0D, 0, 0, 0, 0, 0}});
            if (msg) std::cout << "    [ECU] Received: " << *msg << "\n";
        });

        // EN: Phase 3: Update sensor data via double buffer
        ecu_task.enqueue([&] {
            auto& back = sensor_buf.back();
            back[0] = {95.2f, 13.8f, 2000};
            back[1] = {42.1f, 5.0f,  2001};
            sensor_buf.swap();
            const auto& front = sensor_buf.front();
            std::cout << "    [ECU] Engine temp: " << front[0].temperature
                      << "°C, Battery: " << front[1].voltage << "V\n";
        });

        // EN: Phase 4: Sleep request → LED OFF
        ecu_task.enqueue([&] {
            ecu_state = fsm_dispatch(ecu_state, EcuEvent::SLEEP_REQUEST);
            status_led.clear_pin(5);
            std::cout << "    [ECU] LED OFF — entering sleep\n";
        });

        ecu_task.flush();
        std::cout << "  Final ECU state: " << to_string(ecu_state) << "\n";
        std::cout << "  CAN pool active: " << can_pool.active() << "/" << can_pool.capacity() << "\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of Module 10 — Embedded Patterns\n";
    std::cout << "============================================\n";

    return 0;
}
