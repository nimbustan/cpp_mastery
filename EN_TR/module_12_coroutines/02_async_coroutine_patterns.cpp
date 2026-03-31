/**
 * @file module_12_coroutines/02_async_coroutine_patterns.cpp
 * @brief Advanced Async Coroutine Patterns — Gelişmiş Asenkron Coroutine Kalıpları
 *
 * @details
 * =============================================================================
 * [THEORY: Symmetric Transfer / TEORİ: Simetrik Transfer]
 * =============================================================================
 *
 * EN: When one coroutine awaits another, a naive implementation does:
 *       caller.suspend() → callee.resume() → ... → callee.suspend() → caller.resume()
 *     This nests coroutine frames on the system STACK. Chain enough coroutines and
 *     you get STACK OVERFLOW — one of the hardest coroutine bugs.
 *
 *     Symmetric Transfer solves this: instead of returning to the caller and
 *     then resuming the next coroutine, await_suspend returns a coroutine_handle
 *     that the COMPILER resumes directly — no stack growth!
 *
 *     ┌───────────────────────────┬─────────────────────────────────────────────┐
 *     │ await_suspend return type │ Behavior                                    │
 *     ├───────────────────────────┼─────────────────────────────────────────────┤
 *     │ void                      │ Suspend unconditionally, return to resumer  │
 *     │ bool                      │ true→suspend, false→don't suspend           │
 *     │ coroutine_handle<>        │ SYMMETRIC TRANSFER: suspend current,        │
 *     │                           │ resume returned handle (no stack growth)    │
 *     └───────────────────────────┴─────────────────────────────────────────────┘
 *
 *     std::noop_coroutine() returns a handle that does nothing when resumed.
 *     Used as a "return to caller" sentinel in the symmetric transfer chain.
 *
 * TR: Bir coroutine başka birini beklediğinde, naif uygulama şunu yapar:
 *       çağıran.askı() → çağrılan.devam() → ... → çağrılan.askı() → çağıran.devam()
 *     Bu, coroutine frame'lerini sistem YIĞINına (stack) yerleştirir. Yeterince
 *     coroutine zincirleyin → YIĞIN TAŞMASI — en zor coroutine hatalarından biri.
 *
 *     Simetrik Transfer bunu çözer: çağırana dönüp sonraki coroutine'i devam ettirmek
 *     yerine, await_suspend doğrudan DERLEYİCİNİN devam ettirdiği bir coroutine_handle
 *     döndürür — yığın büyümesi yok!
 *     ┌───────────────────────────┬─────────────────────────────────────────────┐
 *     │ await_suspend dönüş tipi  │ Davranış                                    │
 *     ├───────────────────────────┼─────────────────────────────────────────────┤
 *     │ void                      │ Koşulsuz askıya al, yeniden başlatıcıya dön │
 *     │ bool                      │ true→askıya al, false→askıya alma           │
 *     │ coroutine_handle<>        │ SİMETRİK TRANSFER: mevcut'u askıya al,      │
 *     │                           │ döndürülen handle'i devam ettir (yığın yok) │
 *     └───────────────────────────┴─────────────────────────────────────────────┘
 *     std::noop_coroutine() devam ettirildiğinde hiçbir şey yapmayan bir handle döndürür.
 *     Simetrik transfer zincirinde "çağırana dön" nöbetçisi olarak kullanılır.
 *
 * =============================================================================
 * [THEORY: Coroutine Exception Handling / TEORİ: Coroutine'de Hata Yönetimi]
 * =============================================================================
 *
 * EN: Exceptions in coroutines follow specific rules:
 *
 *     1. If an exception escapes the coroutine BODY:
 *        → promise.unhandled_exception() is called
 *        → Typically stores it: current_exception_ = std::current_exception()
 *        → Then final_suspend() runs (coroutine is at final suspend point)
 *
 *     2. The CALLER retrieves the exception later (when calling .get() or similar):
 *        → std::rethrow_exception(current_exception_) propagates it
 *
 *     3. If an exception happens DURING co_await (in await_suspend):
 *        → The coroutine is NOT suspended (it was in the process of suspending)
 *        → The exception propagates to the CALLER immediately
 *
 *     ┌──────────────────────────────┬──────────────────────────────────────┐
 *     │ Where exception occurs       │ What happens                         │
 *     ├──────────────────────────────┼──────────────────────────────────────┤
 *     │ Coroutine body               │ unhandled_exception() stores it      │
 *     │ co_await (await_suspend)     │ Propagates to caller immediately     │
 *     │ co_await (await_resume)      │ Propagates inside coroutine body     │
 *     │ promise constructor          │ Coroutine frame destroyed, throws    │
 *     └──────────────────────────────┴──────────────────────────────────────┘
 *
 * TR: Coroutine'lerdeki istisnalar belirli kuralları takip eder:
 *
 *     1. İstisna coroutine GÖVDESİNDEN kaçarsa:
 *        → promise.unhandled_exception() çağrılır
 *        → Genellikle saklar: current_exception_ = std::current_exception()
 *        → Sonra final_suspend() çalışır
 *
 *     2. ÇAĞIRAN istisnayı daha sonra alır (.get() veya benzeri çağrısında):
 *        → std::rethrow_exception(current_exception_) yayar
 *
 *     3. İstisna co_await SIRASINDA (await_suspend'de) olursa:
 *        → Coroutine askıya ALINMAZ
 *        → İstisna ÇAĞIRANA hemen yayılır *
 *     ┌──────────────────────────────┬──────────────────────────────────────┐
 *     │ İstisnanın oluştuğu yer      │ Ne olur                              │
 *     ├──────────────────────────────┼──────────────────────────────────────┤
 *     │ Coroutine gövdesi            │ unhandled_exception() saklar         │
 *     │ co_await (await_suspend)     │ Çağırana hemen yayılır               │
 *     │ co_await (await_resume)      │ Coroutine gövdesinde yayılır         │
 *     │ promise constructor          │ Coroutine frame yok edilir, fırlatır │
 *     └──────────────────────────────┴──────────────────────────────────────┘ 
 * =============================================================================
 * [THEORY: Coroutine Memory & Lifetime / TEORİ: Bellek ve Yaşam Süresi]
 * =============================================================================
 *
 * EN: Coroutine frames are typically HEAP allocated. The compiler MAY elide this
 *     allocation via HALO (Heap Allocation eLision Optimization) if:
 *     - The coroutine's lifetime is bounded by the caller
 *     - The frame size is known at compile time
 *     - The compiler can prove the coroutine doesn't outlive the frame holder
 *
 *     Critical lifetime pitfalls:
 *
 *     ┌──────────────────────────────────┬──────────────────────────────────┐
 *     │ Pitfall                          │ Consequence                      │
 *     ├──────────────────────────────────┼──────────────────────────────────┤
 *     │ Reference parameter to coroutine │ Dangling reference after caller  │
 *     │                                  │ returns (frame outlives caller)  │
 *     ├──────────────────────────────────┼──────────────────────────────────┤
 *     │ Lambda capturing local by ref    │ Dangling ref when lambda used as │
 *     │                                  │ coroutine body                   │
 *     ├──────────────────────────────────┼──────────────────────────────────┤
 *     │ Destroying handle before         │ Memory leak (frame not freed)    │
 *     │ final_suspend                    │ OR double free (if promise owns) │
 *     ├──────────────────────────────────┼──────────────────────────────────┤
 *     │ Forgetting to destroy handle     │ Memory leak of coroutine frame   │
 *     └──────────────────────────────────┴──────────────────────────────────┘
 *
 *     Best practice: Coroutine parameters should be taken BY VALUE.
 *     The coroutine frame copies them, so they survive suspension.
 *
 * TR: Coroutine frame'leri tipik olarak HEAP'te tahsis edilir. Derleyici bu tahsisi
 *     HALO (Heap Allocation eLision Optimization) ile eleyebilir:
 *     - Coroutine'in yaşam süresi çağıran tarafından sınırlıysa
 *     - Frame boyutu derleme zamanında biliniyorsa
 *     - Derleyici coroutine'in frame tutucudan uzun yaşamadığını kanıtlayabilirse
 *
 *     Kritik yaşam süresi tuzakları:
 *
 *     ┌──────────────────────────────────┬──────────────────────────────────┐
 *     │ Tuzak                            │ Sonuç                            │
 *     ├──────────────────────────────────┼──────────────────────────────────┤
 *     │ Coroutine'e referans parametre   │ Çağıran döndükten sonra sarkan   │
 *     │                                  │ referans (frame çağırandan uzun) │
 *     ├──────────────────────────────────┼──────────────────────────────────┤
 *     │ Referansla yakalayan lambda      │ Coroutine gövdesi olarak         │
 *     │                                  │ kullanılırsa sarkan ref          │
 *     ├──────────────────────────────────┼──────────────────────────────────┤
 *     │ Handle'ı final_suspend'den önce  │ Bellek sızıntısı (frame serbest  │
 *     │ yok etmek                        │ bırakılmaz) VEYA çift serbest    │
 *     ├──────────────────────────────────┼──────────────────────────────────┤
 *     │ Handle'ı yok etmeyi unutmak      │ Coroutine frame bellek sızıntısı │
 *     └──────────────────────────────────┴──────────────────────────────────┘
 *
 *     En iyi uygulama: Coroutine parametreleri DEĞER ile alınmalıdır.
 *     Coroutine frame onları kopyalar, böylece askıya almadan sonra yaşarlar.
 *
 * =============================================================================
 * [THEORY: Async Event Loop / TEORİ: Asenkron Olay Döngüsü]
 * =============================================================================
 *
 * EN: A coroutine scheduler is an event loop that manages multiple coroutines.
 *     When a coroutine suspends, the scheduler picks the next ready coroutine.
 *     When an async operation completes, the scheduler resumes the waiting coroutine.
 *
 *     This is the foundation of all async frameworks (Boost.Asio, libuv, tokio):
 *
 *     ┌────────────────────────────────────────────────────────────────────┐
 *     │ Scheduler (Event Loop)                                             │
 *     │ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐                    │
 *     │ │ Coroutine A │ │ Coroutine B │ │ Coroutine C │ ← Ready Queue      │
 *     │ └──────┬──────┘ └──────┬──────┘ └──────┬──────┘                    │
 *     │        │               │               │                           │
 *     │   co_await          co_await        co_yield                       │
 *     │   (I/O wait)        (timer)         (value)                        │
 *     │        │               │               │                           │
 *     │   ┌────▼───────────────▼───────────────▼─────┐                     │
 *     │   │          event_loop.run()                │                     │
 *     │   │   Pick next ready → resume → repeat      │                     │
 *     │   └──────────────────────────────────────────┘                     │
 *     └────────────────────────────────────────────────────────────────────┘
 *
 * TR: Bir coroutine zamanlayıcısı, birden fazla coroutine'i yöneten bir olay döngüsüdür.
 *     Bir coroutine askıya alındığında, zamanlayıcı bir sonraki hazır coroutine'i seçer.
 *     Asenkron bir işlem tamamlandığında, bekleyen coroutine'i devam ettirir.
 *
 *     Bu, tüm async çerçevelerinin (Boost.Asio, libuv, tokio) temelidir:
 *
 *     ┌────────────────────────────────────────────────────────────────────┐
 *     │ Zamanlayıcı (Olay Döngüsü)                                         │
 *     │ ┌─────────────┐ ┌─────────────┐ ┌─────────────┐                    │
 *     │ │ Coroutine A │ │ Coroutine B │ │ Coroutine C │ ← Hazır Kuyruğu    │
 *     │ └──────┬──────┘ └──────┬──────┘ └──────┬──────┘                    │
 *     │        │               │               │                           │
 *     │   co_await          co_await        co_yield                       │
 *     │   (I/O bekle)       (zamanlayıcı)    (değer)                       │
 *     │        │               │               │                           │
 *     │   ┌────▼───────────────▼───────────────▼─────────┐                 │
 *     │   │          olay_dongusu.run()                  │                 │
 *     │   │   Sonraki hazırı seç → devam ettir → tekrarla│                 │
 *     │   └──────────────────────────────────────────────┘                 │
 *     └────────────────────────────────────────────────────────────────────┘
 *
 * @see https://en.cppreference.com/w/cpp/coroutine/coroutine_handle
 * @see https://en.cppreference.com/w/cpp/coroutine/noop_coroutine
 * @see https://lewissbaker.github.io/2020/05/11/understanding_symmetric_transfer
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_async_coroutine_patterns.cpp -o 02_async_coroutine_patterns
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <coroutine>
#include <exception>
#include <stdexcept>
#include <queue>
#include <vector>
#include <functional>
#include <string>
#include <optional>
#include <variant>
#include <atomic>
#include <chrono>
#include <cassert>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Async Task with Exception Propagation
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Task<T> — An async coroutine type that supports:
 *     - co_return to set a result value
 *     - Exception propagation from coroutine body to caller
 *     - Symmetric transfer to avoid stack overflow in coroutine chains
 *     - RAII: the coroutine frame is destroyed when the Task goes out of scope
 *
 *     Unlike a Generator (which yields multiple values), a Task produces ONE result
 *     and may throw an exception. The caller retrieves the result via .result().
 *
 * TR: Task<T> — Şunları destekleyen asenkron coroutine türü:
 *     - co_return ile sonuç değeri ayarlama
 *     - Coroutine gövdesinden çağırana istisna yayma
 *     - Coroutine zincirlerinde yığın taşmasını önlemek için simetrik transfer
 *     - RAII: Task kapsam dışına çıktığında coroutine frame yok edilir
 *
 *     Generator'dan farklı olarak (birden fazla değer üretir), Task TEK sonuç üretir
 *     ve istisna fırlatabilir. Çağıran sonucu .result() ile alır.
 */
template<typename T>
class Task {
public:
    struct promise_type {
        /// EN: Stores the result: either a T value or an exception_ptr
        /// TR: Sonucu saklar: ya bir T değeri ya da exception_ptr
        std::variant<std::monostate, T, std::exception_ptr> result_;

        /// EN: Handle of the coroutine that is awaiting THIS task (the continuation)
        /// TR: BU görevi bekleyen coroutine'in handle'ı (devam)
        std::coroutine_handle<> continuation_ = std::noop_coroutine();

        /// EN: Create the Task object from the promise
        /// TR: Promise'ten Task nesnesini oluştur
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        /// EN: Don't start executing until awaited (lazy start)
        ///     This is critical for symmetric transfer to work correctly.
        /// TR: Beklenene kadar çalışmaya başlama (tembel başlatma)
        ///     Simetrik transferin doğru çalışması için kritik.
        std::suspend_always initial_suspend() noexcept { return {}; }

        /// EN: At final suspend, use symmetric transfer to resume the awaiting coroutine.
        ///     Returns a coroutine_handle (not void!) → compiler does symmetric transfer.
        /// TR: Son askıda, bekleyen coroutine'i devam ettirmek için simetrik transfer kullan.
        ///     void değil coroutine_handle döndürür → derleyici simetrik transfer yapar.
        struct FinalAwaiter {
            bool await_ready() noexcept { return false; }

            /// EN: This is where symmetric transfer happens — we don't return to
            ///     the resumer, we DIRECTLY resume the continuation coroutine.
            /// TR: Simetrik transfer burada gerçekleşir — yeniden başlatıcıya
            ///     dönmeyiz, doğrudan devam coroutine'ini devam ettiririz.
            std::coroutine_handle<> await_suspend(
                std::coroutine_handle<promise_type> h) noexcept {
                return h.promise().continuation_;
            }

            void await_resume() noexcept {}
        };

        FinalAwaiter final_suspend() noexcept { return {}; }

        /// EN: Store the return value / TR: Dönüş değerini sakla
        void return_value(T value) {
            result_.template emplace<1>(std::move(value));
        }

        /// EN: Capture the exception for later propagation to the caller.
        ///     std::current_exception() captures the active exception.
        /// TR: İstisnayı çağırana daha sonra yayma için yakala.
        ///     std::current_exception() aktif istisnayı yakalar.
        void unhandled_exception() {
            result_.template emplace<2>(std::current_exception());
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    // EN: RAII: destroy the coroutine frame when Task is destroyed
    // TR: RAII: Task yok edildiğinde coroutine frame'i yok et
    explicit Task(handle_type h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }

    // EN: Move-only (transferring frame ownership)
    // TR: Sadece taşınabilir (frame sahipliğini transfer eder)
    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    /// EN: Make Task co_awaitable — this lets you write: auto val = co_await someTask();
    /// TR: Task'ı co_await edilebilir yap — şöyle yazabilirsiniz: auto val = co_await someTask();
    struct Awaiter {
        handle_type handle;

        bool await_ready() noexcept {
            // EN: If the coroutine is already done, no need to suspend
            // TR: Coroutine zaten bittiyse askıya almaya gerek yok
            return handle.done();
        }

        /// EN: Symmetric transfer: store continuation handle, then resume the task.
        ///     Returning handle means: "resume THIS task's coroutine next"
        /// TR: Simetrik transfer: devam handle'ını sakla, sonra görevi devam ettir.
        ///     Handle döndürmek: "sonra BU görevin coroutine'ini devam ettir" demek
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept {
            handle.promise().continuation_ = awaiting;
            return handle;
        }

        /// EN: Retrieve the result (or rethrow the stored exception)
        /// TR: Sonucu al (veya saklanan istisnayı yeniden fırlat)
        T await_resume() {
            auto& result = handle.promise().result_;
            if (std::holds_alternative<std::exception_ptr>(result)) {
                std::rethrow_exception(std::get<std::exception_ptr>(result));
            }
            return std::move(std::get<T>(result));
        }
    };

    Awaiter operator co_await() noexcept { return Awaiter{handle_}; }

    /// EN: Synchronous retrieval — for calling from non-coroutine code (e.g., main)
    ///     Resumes the coroutine and returns the result or rethrows exception.
    /// TR: Senkron alma — coroutine olmayan koddan çağırmak için (ör. main)
    ///     Coroutine'i devam ettirir ve sonucu döndürür veya istisnayı yeniden fırlatır.
    T result() {
        if (!handle_.done()) {
            handle_.resume();
        }
        auto& r = handle_.promise().result_;
        if (std::holds_alternative<std::exception_ptr>(r)) {
            std::rethrow_exception(std::get<std::exception_ptr>(r));
        }
        return std::move(std::get<T>(r));
    }

private:
    handle_type handle_;
};

/// EN: Specialization for void return type / TR: void dönüş tipi için özelleştirme
template<>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr exception_;
        std::coroutine_handle<> continuation_ = std::noop_coroutine();

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }

        struct FinalAwaiter {
            bool await_ready() noexcept { return false; }
            std::coroutine_handle<> await_suspend(
                std::coroutine_handle<promise_type> h) noexcept {
                return h.promise().continuation_;
            }
            void await_resume() noexcept {}
        };
        FinalAwaiter final_suspend() noexcept { return {}; }

        void return_void() {}
        void unhandled_exception() { exception_ = std::current_exception(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit Task(handle_type h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }
    Task(Task&& o) noexcept : handle_(o.handle_) { o.handle_ = nullptr; }
    Task& operator=(Task&& o) noexcept {
        if (this != &o) { if (handle_) handle_.destroy(); handle_ = o.handle_; o.handle_ = nullptr; }
        return *this;
    }
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    struct Awaiter {
        handle_type handle;
        bool await_ready() noexcept { return handle.done(); }
        std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept {
            handle.promise().continuation_ = awaiting;
            return handle;
        }
        void await_resume() {
            if (handle.promise().exception_)
                std::rethrow_exception(handle.promise().exception_);
        }
    };
    Awaiter operator co_await() noexcept { return Awaiter{handle_}; }

    void result() {
        if (!handle_.done()) handle_.resume();
        if (handle_.promise().exception_)
            std::rethrow_exception(handle_.promise().exception_);
    }

private:
    handle_type handle_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Coroutine Scheduler (Round-Robin)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A simple round-robin coroutine scheduler. Coroutines yield control back
 *     to the scheduler which then picks the next one. This demonstrates how
 *     async frameworks (Boost.Asio, libuv) work under the hood.
 *
 *     How it works:
 *     1. Coroutines call co_await scheduler.suspend() to yield control
 *     2. The scheduler puts the yielding coroutine back in the ready queue
 *     3. The scheduler resumes the next coroutine in the queue
 *     4. When all coroutines are done, the loop exits
 *
 * TR: Basit bir round-robin coroutine zamanlayıcısı. Coroutine'ler kontrolü
 *     zamanlayıcıya geri verir, o da bir sonrakini seçer. Bu, async çerçevelerinin
 *     (Boost.Asio, libuv) perde arkasında nasıl çalıştığını gösterir.
 *
 *     Nasıl çalışır:
 *     1. Coroutine'ler kontrolü vermek için co_await scheduler.suspend() çağırır
 *     2. Zamanlayıcı, veren coroutine'i hazır kuyruğuna geri koyar
 *     3. Zamanlayıcı, kuyruktaki sonraki coroutine'i devam ettirir
 *     4. Tüm coroutine'ler bittiğinde döngü çıkar
 */
class Scheduler {
    std::queue<std::coroutine_handle<>> ready_queue_;

public:
    /// EN: A custom awaitable that puts the current coroutine back in the queue
    ///     and yields control to the scheduler.
    /// TR: Mevcut coroutine'i kuyruğa geri koyan ve kontrolü zamanlayıcıya
    ///     veren özel bir awaitable.
    struct SchedulerAwaiter {
        Scheduler& scheduler;

        bool await_ready() noexcept { return false; }  // EN: Always suspend / TR: Her zaman askıya al

        /// EN: Save the current coroutine handle and return to the scheduler.
        ///     The scheduler will pick the next coroutine to run.
        /// TR: Mevcut coroutine handle'ını kaydet ve zamanlayıcıya dön.
        ///     Zamanlayıcı çalıştırılacak sonraki coroutine'i seçecek.
        void await_suspend(std::coroutine_handle<> h) {
            scheduler.ready_queue_.push(h);
        }

        void await_resume() noexcept {}
    };

    /// EN: Returns an awaitable that yields control to the scheduler
    /// TR: Kontrolü zamanlayıcıya veren bir awaitable döndürür
    SchedulerAwaiter suspend() { return SchedulerAwaiter{*this}; }

    /// EN: Schedule a coroutine for execution (add to ready queue)
    /// TR: Bir coroutine'i çalıştırma için zamanla (hazır kuyruğuna ekle)
    void schedule(std::coroutine_handle<> h) {
        ready_queue_.push(h);
    }

    /// EN: Run until all coroutines complete (the event loop)
    /// TR: Tüm coroutine'ler tamamlanana kadar çalış (olay döngüsü)
    void run() {
        while (!ready_queue_.empty()) {
            auto h = ready_queue_.front();
            ready_queue_.pop();

            if (!h.done()) {
                h.resume();
            }
        }
    }

    [[nodiscard]] std::size_t pending() const { return ready_queue_.size(); }
};

/// EN: ScheduledTask — a coroutine type designed to work with the Scheduler.
///     Unlike Task<T>, this one auto-registers with the scheduler at initial_suspend.
/// TR: ScheduledTask — Zamanlayıcı ile çalışmak üzere tasarlanmış coroutine türü.
///     Task<T>'den farklı olarak, initial_suspend'de zamanlayıcıya otomatik kaydolur.
class ScheduledTask {
public:
    struct promise_type {
        ScheduledTask get_return_object() {
            return ScheduledTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        /// EN: Lazy start — scheduler will resume us when ready
        /// TR: Tembel başlatma — zamanlayıcı hazır olunca devam ettirecek
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit ScheduledTask(handle_type h) : handle_(h) {}
    ~ScheduledTask() { if (handle_) handle_.destroy(); }

    ScheduledTask(ScheduledTask&& o) noexcept : handle_(o.handle_) { o.handle_ = nullptr; }
    ScheduledTask& operator=(ScheduledTask&&) = delete;
    ScheduledTask(const ScheduledTask&) = delete;
    ScheduledTask& operator=(const ScheduledTask&) = delete;

    /// EN: Get the underlying coroutine handle for scheduling
    /// TR: Zamanlama için altta yatan coroutine handle'ını al
    [[nodiscard]] handle_type handle() const { return handle_; }

private:
    handle_type handle_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Async Pipeline (Producer → Transform → Consumer)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Demonstrates a coroutine pipeline where data flows through stages:
 *     Producer → Transform → Consumer
 *     Each stage is a coroutine. The pipeline is driven by the scheduler.
 *
 *     This models real-world data processing:
 *     - Sensor data → Filter → Display (automotive dashboard)
 *     - Network packet → Parse → Business logic (server)
 *     - Audio samples → DSP → Output (audio processing)
 *
 * TR: Verinin aşamalardan aktığı coroutine pipeline gösterir:
 *     Üretici → Dönüştür → Tüketici
 *     Her aşama bir coroutine'dir. Pipeline zamanlayıcı tarafından yürütülür.
 *
 *     Gerçek dünya veri işlemesini modeller:
 *     - Sensör verisi → Filtre → Gösterim (otomotiv gösterge paneli)
 *     - Ağ paketi → Ayrıştır → İş mantığı (sunucu)
 *     - Ses örnekleri → DSP → Çıkış (ses işleme)
 */

/// EN: A simple channel for passing data between coroutines (single-producer, single-consumer)
/// TR: Coroutine'ler arası veri geçişi için basit kanal (tek üretici, tek tüketici)
template<typename T>
class Channel {
    std::queue<T> buffer_;
    std::coroutine_handle<> waiting_consumer_ = nullptr;
    bool closed_ = false;

public:
    /// EN: Send a value into the channel / TR: Kanala bir değer gönder
    void send(T value) {
        buffer_.push(std::move(value));
        if (waiting_consumer_ && !waiting_consumer_.done()) {
            auto consumer = waiting_consumer_;
            waiting_consumer_ = nullptr;
            consumer.resume();
        }
    }

    /// EN: Close the channel — no more values will be sent
    /// TR: Kanalı kapat — daha fazla değer gönderilmeyecek
    void close() { closed_ = true; }

    /// EN: Check if there's a value to receive / TR: Alınacak değer var mı kontrol et
    [[nodiscard]] bool has_value() const { return !buffer_.empty(); }
    [[nodiscard]] bool is_closed() const { return closed_ && buffer_.empty(); }

    /// EN: Try to receive a value (non-blocking) / TR: Değer almayı dene (bloklamayan)
    std::optional<T> try_receive() {
        if (buffer_.empty()) return std::nullopt;
        T val = std::move(buffer_.front());
        buffer_.pop();
        return val;
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Cancellation Token Pattern
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A CancellationToken allows cooperative cancellation of coroutines.
 *     The caller sets the token; the coroutine checks it at each suspension point.
 *
 *     This is the same pattern used in:
 *     - .NET CancellationToken
 *     - Kotlin coroutine cancellation (isActive check)
 *     - Automotive ECU: diagnostic session timeout → cancel running diagnostics
 *
 *     Key principle: cancellation is COOPERATIVE, not preemptive.
 *     The coroutine must voluntarily check the token and exit gracefully.
 *
 * TR: CancellationToken, coroutine'lerin işbirlikçi iptali sağlar.
 *     Çağıran token'ı ayarlar; coroutine her askıya alma noktasında kontrol eder.
 *
 *     Aynı kalıp şuralarda kullanılır:
 *     - .NET CancellationToken
 *     - Kotlin coroutine iptali (isActive kontrolü)
 *     - Otomotiv ECU: teşhis oturumu zaman aşımı → çalışan teşhisleri iptal et
 *
 *     Temel ilke: iptal İŞBİRLİKÇİDİR, öncelikli değildir.
 *     Coroutine gönüllü olarak token'ı kontrol etmeli ve zarif bir şekilde çıkmalıdır.
 */
class CancellationToken {
    std::atomic<bool> cancelled_{false};

public:
    /// EN: Request cancellation / TR: İptal iste
    void cancel() { cancelled_.store(true, std::memory_order_release); }

    /// EN: Check if cancellation was requested / TR: İptal istenip istenmediğini kontrol et
    [[nodiscard]] bool is_cancelled() const {
        return cancelled_.load(std::memory_order_acquire);
    }

    /// EN: Reset for reuse / TR: Yeniden kullanım için sıfırla
    void reset() { cancelled_.store(false, std::memory_order_release); }
};

/// EN: Awaitable that throws if cancellation is requested — use in coroutine body:
///     co_await check_cancellation(token);
/// TR: İptal istenirse fırlatan awaitable — coroutine gövdesinde kullan:
///     co_await check_cancellation(token);
struct CancellationAwaitable {
    const CancellationToken& token;

    bool await_ready() { return false; }

    /// EN: Check cancellation at the suspension point. If cancelled, don't suspend
    /// TR: Askıya alma noktasında iptali kontrol et. İptal edildiyse askıya alma
    bool await_suspend(std::coroutine_handle<>) {
        // EN: Return false → don't actually suspend, continue to await_resume
        //     Return true → suspend (yield control point for scheduler)
        // TR: false döndür → askıya alma, await_resume'a devam et
        //     true döndür → askıya al (zamanlayıcı için kontrol verme noktası)
        return !token.is_cancelled();
    }

    void await_resume() {
        if (token.is_cancelled()) {
            throw std::runtime_error("Coroutine cancelled");
        }
    }
};

CancellationAwaitable check_cancellation(const CancellationToken& token) {
    return CancellationAwaitable{token};
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Real-World Example — Async Sensor Polling (Automotive)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Simulates an automotive scenario where multiple sensors are polled
 *     asynchronously using coroutines. Each sensor reading is a coroutine
 *     that "awaits" I/O completion. The scheduler runs them cooperatively.
 *
 *     Scenario: ECU reads 3 sensors (temperature, pressure, RPM) in parallel.
 *     Each sensor has different latency. The coroutine scheduler interleaves them.
 *
 * TR: Birden fazla sensörün coroutine'ler kullanılarak asenkron olarak sorgulandığı
 *     otomotiv senaryosunu simüle eder. Her sensör okuması, I/O tamamlanmasını
 *     "bekleyen" bir coroutine'dir. Zamanlayıcı bunları işbirlikçi olarak çalıştırır.
 *
 *     Senaryo: ECU, 3 sensörü (sıcaklık, basınç, RPM) paralel okur.
 *     Her sensörün farklı gecikmesi var. Coroutine zamanlayıcısı araya girer.
 */
struct SensorData {
    std::string name;
    double value;
    std::string unit;
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 12 — Async Coroutine Patterns\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Task<T> with Exception Propagation ──────────────────────
    {
        std::cout << "--- Demo 1: Task<T> with Exception Propagation ---\n";

        // EN: A coroutine that computes a value successfully
        // TR: Başarıyla değer hesaplayan bir coroutine
        auto compute_square = [](int x) -> Task<int> {
            std::cout << "  [coroutine] Computing " << x << " squared...\n";
            co_return x * x;
        };

        auto task1 = compute_square(7);
        std::cout << "  Result: " << task1.result() << "\n";

        // EN: A coroutine that throws an exception
        // TR: İstisna fırlatan bir coroutine
        auto failing_task = []() -> Task<int> {
            std::cout << "  [coroutine] About to throw...\n";
            throw std::runtime_error("Sensor read timeout!");
            co_return 0;  // EN: Never reached / TR: Asla ulaşılmaz
        };

        auto task2 = failing_task();
        try {
            auto val = task2.result();
            std::cout << "  Unexpected result: " << val << "\n";
        } catch (const std::runtime_error& e) {
            // EN: Exception was stored in the promise and re-thrown at result()
            // TR: İstisna promise'te saklandı ve result()'ta yeniden fırlatıldı
            std::cout << "  Caught exception: " << e.what() << "\n";
        }

        // EN: A Task<void> coroutine
        // TR: Task<void> coroutine
        auto void_task = []() -> Task<void> {
            std::cout << "  [coroutine] Void task executing...\n";
            co_return;
        };

        auto task3 = void_task();
        task3.result();
        std::cout << "  Void task completed successfully.\n\n";
    }

    // ─── Demo 2: Chained Tasks (Symmetric Transfer) ─────────────────────
    {
        std::cout << "--- Demo 2: Chained Tasks (Symmetric Transfer) ---\n";

        // EN: inner_task is co_awaited by outer_task — symmetric transfer prevents stack growth
        // TR: inner_task, outer_task tarafından co_await edilir — simetrik transfer yığın büyümesini önler
        auto inner_task = []() -> Task<int> {
            std::cout << "  [inner] Computing value...\n";
            co_return 42;
        };

        auto outer_task = [&inner_task]() -> Task<std::string> {
            std::cout << "  [outer] Awaiting inner task...\n";
            int val = co_await inner_task();
            std::cout << "  [outer] Got " << val << " from inner task\n";
            co_return "Answer is " + std::to_string(val);
        };

        auto result_task = outer_task();
        std::cout << "  Final result: " << result_task.result() << "\n";

        // EN: Exception propagation through chain: inner throws → outer catches
        // TR: Zincir boyunca istisna yayma: inner fırlatır → outer yakalar
        auto failing_inner = []() -> Task<int> {
            throw std::logic_error("Inner computation failed");
            co_return 0;
        };

        auto catching_outer = [&failing_inner]() -> Task<std::string> {
            try {
                int val = co_await failing_inner();
                co_return "Got: " + std::to_string(val);
            } catch (const std::logic_error& e) {
                std::cout << "  [outer] Caught from inner: " << e.what() << "\n";
                co_return "Fallback: error handled";
            }
        };

        auto chain_result = catching_outer();
        std::cout << "  Chain result: " << chain_result.result() << "\n\n";
    }

    // ─── Demo 3: Coroutine Scheduler (Round-Robin) ───────────────────────
    {
        std::cout << "--- Demo 3: Coroutine Scheduler (Round-Robin) ---\n";

        Scheduler scheduler;

        // EN: Create 3 coroutines that cooperatively yield to the scheduler
        // TR: Zamanlayıcıya işbirlikçi olarak veren 3 coroutine oluştur
        auto make_task = [&scheduler](std::string name, int steps) -> ScheduledTask {
            for (int i = 1; i <= steps; ++i) {
                std::cout << "  [" << name << "] Step " << i << "/" << steps << "\n";
                co_await scheduler.suspend();
            }
            std::cout << "  [" << name << "] DONE\n";
        };

        // EN: Create coroutines (they don't start until scheduled)
        // TR: Coroutine'leri oluştur (zamanlanana kadar başlamazlar)
        auto taskA = make_task("TaskA", 3);
        auto taskB = make_task("TaskB", 2);
        auto taskC = make_task("TaskC", 4);

        // EN: Register with the scheduler
        // TR: Zamanlayıcıya kaydet
        scheduler.schedule(taskA.handle());
        scheduler.schedule(taskB.handle());
        scheduler.schedule(taskC.handle());

        // EN: Run the event loop — observe round-robin interleaving
        // TR: Olay döngüsünü çalıştır — round-robin araya girmeyi gözlemle
        std::cout << "  [Scheduler] Starting event loop...\n";
        scheduler.run();
        std::cout << "  [Scheduler] All tasks completed.\n\n";
    }

    // ─── Demo 4: Async Pipeline ──────────────────────────────────────────
    {
        std::cout << "--- Demo 4: Async Pipeline (Producer → Transform → Consumer) ---\n";

        Scheduler scheduler;
        Channel<int> raw_channel;
        Channel<std::string> processed_channel;

        // EN: Producer: generates raw sensor values
        // TR: Üretici: ham sensör değerleri üretir
        auto producer = [&scheduler, &raw_channel]() -> ScheduledTask {
            int values[] = {25, 80, 3500, 42, 95};
            for (int v : values) {
                std::cout << "  [Producer] Sending raw value: " << v << "\n";
                raw_channel.send(v);
                co_await scheduler.suspend();
            }
            raw_channel.close();
            std::cout << "  [Producer] Channel closed.\n";
        };

        // EN: Transformer: reads raw values, processes them, sends to next stage
        // TR: Dönüştürücü: ham değerleri okur, işler, sonraki aşamaya gönderir
        auto transformer = [&scheduler, &raw_channel, &processed_channel]() -> ScheduledTask {
            while (!raw_channel.is_closed()) {
                auto val = raw_channel.try_receive();
                if (val) {
                    std::string result = "Processed(" + std::to_string(*val) + " → "
                                       + std::to_string(*val * 2) + ")";
                    std::cout << "  [Transform] " << result << "\n";
                    processed_channel.send(std::move(result));
                }
                co_await scheduler.suspend();
            }
            processed_channel.close();
            std::cout << "  [Transform] Done.\n";
        };

        // EN: Consumer: reads processed values and displays them
        // TR: Tüketici: işlenmiş değerleri okur ve gösterir
        auto consumer = [&scheduler, &processed_channel]() -> ScheduledTask {
            while (!processed_channel.is_closed()) {
                auto val = processed_channel.try_receive();
                if (val) {
                    std::cout << "  [Consumer] Received: " << *val << "\n";
                }
                co_await scheduler.suspend();
            }
            std::cout << "  [Consumer] Pipeline complete.\n";
        };

        auto prod = producer();
        auto trans = transformer();
        auto cons = consumer();

        scheduler.schedule(prod.handle());
        scheduler.schedule(trans.handle());
        scheduler.schedule(cons.handle());

        scheduler.run();
        std::cout << "\n";
    }

    // ─── Demo 5: Cancellation Token ──────────────────────────────────────
    {
        std::cout << "--- Demo 5: Cancellation Token ---\n";

        CancellationToken token;

        // EN: Coroutine that checks for cancellation at each step
        // TR: Her adımda iptali kontrol eden coroutine
        auto cancellable_task = [&token]() -> Task<std::string> {
            std::string progress;
            for (int i = 1; i <= 10; ++i) {
                // EN: Check cancellation — if cancelled, this throws
                // TR: İptali kontrol et — iptal edildiyse fırlatır
                co_await check_cancellation(token);
                progress += "[Step" + std::to_string(i) + "]";
                std::cout << "  [Task] Completed step " << i << "/10\n";

                // EN: Simulate: cancel after step 3
                // TR: Simülasyon: 3. adımdan sonra iptal et
                if (i == 3) {
                    std::cout << "  [External] Requesting cancellation!\n";
                    token.cancel();
                }
            }
            co_return progress;
        };

        auto task = cancellable_task();
        try {
            auto result = task.result();
            std::cout << "  Task completed: " << result << "\n";
        } catch (const std::runtime_error& e) {
            std::cout << "  Task cancelled: " << e.what() << "\n";
        }
        std::cout << "\n";
    }

    // ─── Demo 6: Sensor Polling Simulation ───────────────────────────────
    {
        std::cout << "--- Demo 6: Automotive Sensor Polling (Scheduler) ---\n";

        Scheduler scheduler;
        std::vector<SensorData> collected_data;

        // EN: Simulate reading multiple sensors — each takes different number of steps
        // TR: Birden fazla sensör okumayı simüle et — her biri farklı adım sayısı alır
        auto poll_sensor = [&scheduler, &collected_data](
            std::string name, double base_val, std::string unit, int latency_steps) -> ScheduledTask {

            for (int step = 0; step < latency_steps; ++step) {
                std::cout << "  [" << name << "] Polling step " << (step + 1)
                          << "/" << latency_steps << "...\n";
                co_await scheduler.suspend();
            }

            // EN: Simulate reading the sensor value
            // TR: Sensör değerini okumayı simüle et
            double value = base_val + static_cast<double>(latency_steps) * 0.5;
            collected_data.push_back(SensorData{name, value, unit});
            std::cout << "  [" << name << "] READ: " << value << " " << unit << "\n";
        };

        // EN: Create sensor polling tasks with different latencies
        // TR: Farklı gecikmelere sahip sensör sorgulama görevleri oluştur
        auto temp_task   = poll_sensor("Temperature", 85.0, "°C", 2);
        auto press_task  = poll_sensor("Pressure",    1013.0, "hPa", 3);
        auto rpm_task    = poll_sensor("RPM",         3500.0, "rpm", 1);

        scheduler.schedule(temp_task.handle());
        scheduler.schedule(press_task.handle());
        scheduler.schedule(rpm_task.handle());

        std::cout << "  [ECU] Starting cooperative sensor polling...\n";
        scheduler.run();

        std::cout << "\n  [ECU] All sensors polled. Results:\n";
        for (const auto& data : collected_data) {
            std::cout << "    " << data.name << " = " << data.value << " " << data.unit << "\n";
        }
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of Module 12 — Async Coroutine Patterns\n";
    std::cout << "============================================\n";

    return 0;
}
