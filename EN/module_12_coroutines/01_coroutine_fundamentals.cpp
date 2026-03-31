/**
 * @file module_12_coroutines/01_coroutine_fundamentals.cpp
 * @brief C++20 Coroutines: Fundamentals and Mechanics
 *
 * @details
 * =============================================================================
 * =============================================================================
 *
 * EN: A coroutine is a function that can SUSPEND its execution and RESUME later.
 *     Unlike a regular function that runs from start to finish, a coroutine can:
 *     - Pause midway (yield a value or wait for something)
 *     - Resume from exactly where it left off (local variables preserved!)
 *     - Pause and resume multiple times before final completion
 *
 *     Think of it like reading a book: you can put a bookmark, do something else,
 *     and come back to exactly where you stopped. Regular functions are like
 *     eating a meal — you start and finish in one sitting.
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: A function becomes a coroutine if its body contains ANY of these keywords:
 *
 *     ┌──────────────┬─────────────────────────────────────────────────────────┐
 *     │ Keyword      │ What It Does                                            │
 *     ├──────────────┼─────────────────────────────────────────────────────────┤
 *     │ co_await     │ Suspends execution, waits for an async operation        │
 *     │              │ (like "await" in Python/JS/C#)                          │
 *     ├──────────────┼─────────────────────────────────────────────────────────┤
 *     │ co_yield     │ Suspends and produces a value (lazy generator)          │
 *     │              │ Equivalent to co_await promise.yield_value(expr)        │
 *     ├──────────────┼─────────────────────────────────────────────────────────┤
 *     │ co_return    │ Completes the coroutine and optionally returns a value  │
 *     │              │ Unlike regular return: also destroys the coroutine      │
 *     │              │ frame's promise and does final cleanup                  │
 *     └──────────────┴─────────────────────────────────────────────────────────┘
 *
 *     IMPORTANT: You CANNOT mix `return` and `co_return` in the same function!
 *     If the function uses any co_* keyword, it's a coroutine — use co_return.
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: When compiler sees a co_* keyword, it transforms the function:
 *
 *     1. COROUTINE FRAME (heap-allocated*):
 *        - Holds local variables, parameters, promise object
 *        - Persists between suspensions (that's why locals survive!)
 *        - *Can be heap-elided (HALO optimization) if lifetime is bounded
 *
 *     2. PROMISE TYPE (promise_type):
 *        - Controls coroutine behavior at key points
 *        - Like a "policy class" the compiler consults
 *        - You define: initial_suspend, final_suspend, yield_value, etc.
 *
 *     3. COROUTINE HANDLE (std::coroutine_handle<Promise>):
 *        - A non-owning pointer to the coroutine frame
 *        - Used to RESUME or DESTROY the coroutine from outside
 *        - handle.resume() → continues execution
 *        - handle.destroy() → deallocates the frame
 *
 *     Compiler transformation (simplified):
 *     ┌─────────────────────────────────────────────────────────┐
 *     │  Your Code          →  Compiler Generates               │
 *     ├─────────────────────────────────────────────────────────┤
 *     │  Generator count()  →  1. Allocate coroutine frame      │
 *     │  {                  →  2. Construct promise object      │
 *     │    co_yield 1;      →  3. Call initial_suspend()        │
 *     │    co_yield 2;      →  4. Execute body with suspensions │
 *     │  }                  →  5. Call final_suspend()          │
 *     │                     →  6. Destroy frame (if needed)     │
 *     └─────────────────────────────────────────────────────────┘
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: co_await operates on "awaitable" objects. An awaitable must have 3 methods:
 *
 *     bool await_ready()        → Should we suspend at all? (false = yes, suspend)
 *     void await_suspend(h)     → What to do when suspending (schedule, store handle)
 *     T    await_resume()       → What value to produce when resumed
 *
 *     Built-in awaitables:
 *     - std::suspend_always  → await_ready() returns false (always suspends)
 *     - std::suspend_never   → await_ready() returns true  (never suspends)
 *
 */

#include <iostream>
#include <coroutine>   // EN: C++20 coroutine support
#include <string>
#include <vector>
#include <cstdint>
#include <optional>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Generator<T> — A Complete Coroutine Type From Scratch
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Generator<T> is the simplest useful coroutine type: a lazy sequence producer.
 *     It produces values one at a time via co_yield, pausing between each one.
 *     The caller pulls values with next() — no value is computed until requested.
 *
 *     This is the "Hello World" of coroutines. We build it from scratch to show
 *     exactly what the compiler expects.
 *
 */
template<typename T>
class Generator {
public:
    // ── promise_type: The "policy" that tells the compiler how this coroutine behaves ──
    // EN: The compiler looks for a nested type called `promise_type` inside the return type.
    //     This is NOT optional — without it, compilation fails.
    //     Each method answers a specific question the compiler asks:
    //
    struct promise_type {
        T current_value;             // EN: Stores the last co_yield'd value
        std::exception_ptr exception; // EN: Captures exceptions from coroutine

        // EN: Q: "What object should be returned to the caller?"
        //     A: A Generator wrapping a handle to this coroutine.
        //     C: Bu coroutine'e bir handle saran bir Generator.
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // EN: Q: "Should the coroutine suspend BEFORE running any user code?"
        //     A: YES (suspend_always) — lazy start, nothing runs until first next() call.
        //     If we said suspend_never, the coroutine would run eagerly to first co_yield.
        std::suspend_always initial_suspend() noexcept { return {}; }

        // EN: Q: "Should the coroutine suspend AFTER it finishes (at final_suspend)?"
        //     A: YES — we suspend so the caller can detect completion and clean up the handle.
        //     If we said suspend_never, the frame auto-destroys and our handle dangles!
        //     WARNING: final_suspend MUST be noexcept (standard requirement).
        //     UYARI: final_suspend noexcept OLMALIDIR (standart gereksiniml).
        std::suspend_always final_suspend() noexcept { return {}; }

        // EN: Q: "A co_yield expression was hit — what should happen?"
        //     A: Store the value and suspend (suspend_always).
        //     The caller will read current_value after resume returns to them.
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }

        // EN: Q: "co_return was hit (or end of function reached) — any return value?"
        //     A: No value for generators — void return.
        void return_void() {}

        // EN: Q: "An unhandled exception escaped the coroutine body — what to do?"
        //     A: Capture it. The caller can rethrow it later if needed.
        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    // ── Generator class body ──

    // EN: Type alias for the coroutine handle specific to our promise
    using handle_type = std::coroutine_handle<promise_type>;

    // EN: Constructor — takes ownership of the coroutine handle
    explicit Generator(handle_type h) : handle_(h) {}

    // EN: Destructor — MUST destroy the handle to free the coroutine frame!
    //     If you forget this, the frame LEAKS (heap memory not freed).
    ~Generator() {
        if (handle_) handle_.destroy();
    }

    // EN: Move-only type (coroutine ownership cannot be shared)
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    // EN: Advance the coroutine to the next co_yield point and return the value.
    //     Returns std::optional<T> — empty if coroutine is finished.
    //
    //     Flow: next() → handle_.resume() → coroutine runs until co_yield → suspends
    //           → control returns here → read promise().current_value
    //
    std::optional<T> next() {
        if (!handle_ || handle_.done()) return std::nullopt;

        handle_.resume();  // EN: Continue coroutine until next co_yield or end

        if (handle_.done()) return std::nullopt;

        // EN: Check for exceptions that escaped the coroutine
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }

        return handle_.promise().current_value;
    }

private:
    handle_type handle_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Simple Task — co_await + co_return
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Task<T> represents an asynchronous computation that produces a single value.
 *     Unlike Generator (which yields many values), Task produces ONE result via co_return.
 *     This is the coroutine equivalent of std::future<T>.
 *
 */
template<typename T>
class Task {
public:
    struct promise_type {
        T result;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // EN: Lazy start — suspend initially, don't run until explicitly resumed
        std::suspend_always initial_suspend() noexcept { return {}; }

        // EN: Suspend at end so caller can read the result before frame is destroyed
        std::suspend_always final_suspend() noexcept { return {}; }

        // EN: co_return value; → store the result
        void return_value(T value) {
            result = std::move(value);
        }

        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit Task(handle_type h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    // EN: Run the coroutine to completion and return the result.
    //     Loop because the coroutine may have MULTIPLE co_await suspension points.
    //     Each resume() advances to the next co_await or co_return.
    //     Her resume() sonraki co_await veya co_return'e ilerler.
    T get() {
        while (!handle_.done()) {
            handle_.resume();
        }
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
        return std::move(handle_.promise().result);
    }

private:
    handle_type handle_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Coroutine Functions (the actual coroutines!)
// ═════════════════════════════════════════════════════════════════════════════

// ── 3a: Simple counter generator ──

/**
 * EN: The simplest possible generator: counts from 'start' to 'end'.
 *     Each co_yield suspends the coroutine and makes the value available to the caller.
 *     When the function body ends, the coroutine finishes (return_void is called).
 *
 *     Execution flow step-by-step for count_up(1, 3):
 *       1. Caller calls count_up(1, 3) → coroutine frame allocated on heap
 *       2. initial_suspend() returns suspend_always → coroutine suspends immediately
 *       3. Generator object returned to caller
 *       4. Caller calls next() → handle_.resume()
 *       5. Coroutine runs: i=1, hits co_yield 1 → yield_value(1) stores 1, suspends
 *       6. Control returns to caller, next() returns 1
 *       7. Caller calls next() → handle_.resume()
 *       8. Coroutine continues: i=2, hits co_yield 2 → stores 2, suspends
 *       9. ... and so on until i > end, function body ends, return_void + final_suspend
 *
 */
Generator<int> count_up(int start, int end) {
    // EN: This looks like a normal loop, but co_yield makes it a coroutine!
    //     The loop "pauses" at each co_yield and the caller gets the value.
    for (int i = start; i <= end; ++i) {
        co_yield i;
        // EN: ↑ Execution suspends here. Local variable 'i' is preserved in the frame.
        //     When next() is called again, execution continues from the line AFTER co_yield.
    }
    // EN: Function body ends → return_void() is called → final_suspend() suspends
}

// ── 3b: Fibonacci generator (infinite!) ──

/**
 * EN: An INFINITE generator — it never returns, just keeps yielding Fibonacci numbers.
 *     This is perfectly fine! The caller decides when to stop pulling values.
 *     This is the power of lazy evaluation: compute only what you need.
 *     Memory usage: O(1) — only 3 variables in the coroutine frame (a, b, temp).
 *     Compare to pre-computing: storing N Fibonacci numbers needs O(N) memory.
 *
 */
Generator<uint64_t> fibonacci() {
    uint64_t a = 0, b = 1;
    while (true) {                // EN: Infinite loop — but suspends at each yield!
        co_yield a;
        uint64_t temp = a + b;
        a = b;
        b = temp;
    }
    // EN: This line is never reached — the coroutine runs forever (or until destroyed)
}

// ── 3c: Filtered generator — composing coroutines ──

/**
 * EN: Generator that filters another generator's output - only even numbers.
 *     This shows coroutine COMPOSITION: one coroutine wrapping another.
 *     The inner generator produces values, our coroutine filters and re-yields.
 *
 */
Generator<uint64_t> even_fibonacci(int count) {
    auto fib = fibonacci();
    int found = 0;
    while (found < count) {
        auto val = fib.next();
        if (!val.has_value()) break;  // EN: Inner generator finished
        if (*val % 2 == 0) {
            co_yield *val;
            ++found;
        }
    }
}

// ── 3d: String tokenizer generator ──

/**
 * EN: Real-world use case: tokenize a string lazily.
 *     Instead of splitting the entire string upfront (allocating a vector of strings),
 *     we yield one token at a time. For huge strings, this saves memory.
 *
 */
Generator<std::string> tokenize(const std::string& input, char delimiter) {
    std::string token;
    for (char ch : input) {
        if (ch == delimiter) {
            if (!token.empty()) {
                co_yield token;
                token.clear();
            }
        } else {
            token += ch;
        }
    }
    if (!token.empty()) {
        co_yield token;   // EN: Don't forget the last token!
    }
}

// ── 3e: Task-based coroutine — co_return demo ──

/**
 * EN: A Task coroutine that does a "computation" and returns a single result.
 *     Uses co_return instead of co_yield.
 *     In real code, before co_return you'd co_await some async I/O.
 *
 */
// EN: IMPORTANT: Coroutine parameters MUST be taken by VALUE, not by reference!
//     Why? Because initial_suspend() suspends BEFORE the body runs.
//     If a param is a reference to a temporary, that temporary dies while
//     the coroutine is suspended → dangling reference → undefined behavior!
//     Rule: Always pass by value to coroutines (the compiler copies into the frame).
//
Task<std::string> compute_sensor_report(std::string sensorId, double temp) {
    // EN: In a real system, this could co_await a database query or network call
    std::string report = "Sensor " + sensorId + ": " + std::to_string(temp) + "C";

    if (temp > 100.0) {
        report += " [CRITICAL]";
    } else if (temp > 75.0) {
        report += " [WARNING]";
    } else {
        report += " [NORMAL]";
    }

    co_return report;  // EN: Complete the Task with this value
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Custom Awaitable — Understanding co_await Mechanics
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A custom awaitable that simulates a "delay" or async operation.
 *     When co_await is used on this, the three methods are called:
 *       1. await_ready() → "Is the result available immediately?"
 *       2. await_suspend(handle) → "Suspend the coroutine, here's the handle"
 *       3. await_resume() → "Coroutine was resumed, what's the result?"
 *
 */
struct SimulatedDelay {
    std::string operation_name;
    int result_value;

    // EN: "Is the result immediately available?" → No, we always need to "wait"
    bool await_ready() const noexcept {
        std::cout << "    [await_ready] '" << operation_name << "' — not ready, will suspend\n";
        return false;  // EN: false = suspend
    }

    // EN: "The coroutine is being suspended. What should we do?"
    //     We could schedule work on a thread pool, register with an event loop, etc.
    //     For this demo, we just print a message.
    //     IMPORTANT: We can choose to:
    //       - void return → always suspend
    //       - bool return → false = don't actually suspend (optimization)
    //       - coroutine_handle return → symmetric transfer to another coroutine
    void await_suspend(std::coroutine_handle<> /*handle*/) const noexcept {
        std::cout << "    [await_suspend] '" << operation_name << "' — simulating async work...\n";
        // EN: In real code: store the handle, schedule resume on completion
    }

    // EN: "The coroutine is being resumed. What value should co_await produce?"
    //     This return value becomes the result of the `co_await expr` expression.
    int await_resume() const noexcept {
        std::cout << "    [await_resume] '" << operation_name << "' — result = " << result_value << "\n";
        return result_value;
    }
};

/**
 * EN: A coroutine that uses our custom awaitable to demonstrate co_await mechanics.
 *     Each co_await suspends the coroutine and resumes it (in our simple case, immediately).
 *
 */
Task<int> sensor_pipeline() {
    std::cout << "  [coroutine] Starting sensor pipeline...\n";

    // EN: co_await SimulatedDelay{...} calls:
    //     1. SimulatedDelay::await_ready() → false → suspend
    //     2. SimulatedDelay::await_suspend(handle) → prints message
    //     3. (Later, when resumed) SimulatedDelay::await_resume() → returns value
    int raw_value = co_await SimulatedDelay{"ADC Read", 2048};
    std::cout << "  [coroutine] Got raw ADC value: " << raw_value << "\n";

    int calibrated = co_await SimulatedDelay{"Calibration", raw_value / 4};
    std::cout << "  [coroutine] Calibrated value: " << calibrated << "\n";

    co_return raw_value + calibrated;
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 12 - Coroutine Fundamentals\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Simple Counter Generator ────────────────────────────────
    {
        std::cout << "--- Demo 1: Simple Counter Generator (co_yield) ---\n";
        // EN: count_up(1, 5) creates a coroutine that yields 1, 2, 3, 4, 5
        //     Each next() call resumes the coroutine, which runs until the next co_yield
        auto counter = count_up(1, 5);
        while (auto val = counter.next()) {
            std::cout << "  Got: " << *val << "\n";
        }
        std::cout << "  Generator exhausted.\n\n";
    }

    // ─── Demo 2: Infinite Fibonacci Generator ────────────────────────────
    {
        std::cout << "--- Demo 2: Infinite Fibonacci Generator ---\n";
        // EN: fibonacci() is an INFINITE generator — it never stops on its own.
        //     We decide to pull only 15 values. Lazy evaluation at its finest!
        //     No array is pre-computed. Each value calculated on-demand.
        auto fib = fibonacci();
        std::cout << "  First 15 Fibonacci numbers:\n  ";
        for (int i = 0; i < 15; ++i) {
            auto val = fib.next();
            if (val) std::cout << *val << " ";
        }
        std::cout << "\n\n";
        // EN: fib destructor destroys the coroutine frame — no leak!
    }

    // ─── Demo 3: Composed Generator — Even Fibonacci ─────────────────────
    {
        std::cout << "--- Demo 3: Coroutine Composition — Even Fibonacci ---\n";
        // EN: even_fibonacci wraps fibonacci and filters: only yields even numbers
        //     This demonstrates coroutine composition — generators calling generators
        auto even_fib = even_fibonacci(5);
        std::cout << "  First 5 even Fibonacci numbers:\n  ";
        while (auto val = even_fib.next()) {
            std::cout << *val << " ";
        }
        std::cout << "\n\n";
    }

    // ─── Demo 4: String Tokenizer Generator ──────────────────────────────
    {
        std::cout << "--- Demo 4: Lazy String Tokenizer ---\n";
        // EN: Real-world pattern: tokenize a CAN bus log line lazily
        //     Each token is produced only when requested — no vector<string> allocation
        std::string can_log = "1A3:8:DE:AD:BE:EF:00:FF:12:34";
        auto tokens = tokenize(can_log, ':');
        int token_num = 1;
        while (auto tok = tokens.next()) {
            std::cout << "  Token " << token_num++ << ": " << *tok << "\n";
        }
        std::cout << "\n";
    }

    // ─── Demo 5: Task with co_return ─────────────────────────────────────
    {
        std::cout << "--- Demo 5: Task with co_return ---\n";
        // EN: Task produces a SINGLE value via co_return (not co_yield)
        //     .get() resumes the coroutine and retrieves the result
        auto report1 = compute_sensor_report("EXHAUST_01", 110.5);
        auto report2 = compute_sensor_report("COOLANT_02", 65.0);
        auto report3 = compute_sensor_report("OIL_TEMP", 88.3);

        std::cout << "  " << report1.get() << "\n";
        std::cout << "  " << report2.get() << "\n";
        std::cout << "  " << report3.get() << "\n";
        std::cout << "\n";
    }

    // ─── Demo 6: Custom Awaitable — co_await Mechanics ───────────────────
    {
        std::cout << "--- Demo 6: Custom Awaitable — co_await Mechanics ---\n";
        // EN: This demonstrates the 3-step co_await process:
        //     await_ready → await_suspend → (resume) → await_resume
        //     Our SimulatedDelay always suspends, then returns a value on resume.
        //
        //     In real-world code, await_suspend would register the coroutine handle
        //     with an I/O event loop or thread pool, and resume() would be called
        //     later when the async operation completes.
        //
        auto pipeline = sensor_pipeline();
        // EN: pipeline is a Task. get() calls resume() which drives the coroutine.
        //     Inside, each co_await creates a suspension point.
        //     After await_suspend, we call resume again, which triggers await_resume.

        // EN: We need to manually resume through all suspension points in our simple case
        int result = pipeline.get();
        std::cout << "  [main] Pipeline final result: " << result << "\n";
        std::cout << "\n";
    }

    // ─── Demo 7: Generator as Range-like Iteration ───────────────────────
    {
        std::cout << "--- Demo 7: Collecting Generator to Vector ---\n";
        // EN: You can collect all generator values into a container if needed
        //     This shows the bridge between lazy and eager evaluation
        auto counter = count_up(10, 20);
        std::vector<int> collected;
        while (auto val = counter.next()) {
            collected.push_back(*val);
        }
        std::cout << "  Collected " << collected.size() << " values: [";
        for (size_t i = 0; i < collected.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << collected[i];
        }
        std::cout << "]\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Module 12 - Coroutine Fundamentals\n";
    std::cout << "============================================\n";

    return 0;
}
