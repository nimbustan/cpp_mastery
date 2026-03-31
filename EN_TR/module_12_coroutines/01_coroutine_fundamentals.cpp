/**
 * @file module_12_coroutines/01_coroutine_fundamentals.cpp
 * @brief C++20 Coroutines: Temeller ve Mekanizma — Fundamentals and Mechanics
 *
 * @details
 * =============================================================================
 * [THEORY: What Are Coroutines? / TEORİ: Coroutine Nedir?]
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
 * TR: Coroutine, çalışmasını ASKIya alıp daha sonra devam edebilen bir fonksiyondur.
 *     Baştan sona çalışan normal bir fonksiyondan farklı olarak:
 *     - Ortada duraklar (değer üretir veya bir şeyi bekler)
 *     - Kaldığı yerden tam olarak devam eder (yerel değişkenler korunur!)
 *     - Son tamamlanmadan önce birçok kez duraklar ve devam eder
 *
 *     Kitap okumak gibi düşün: ayraç koyup başka şey yapıp kaldığın yere dönebilirsin.
 *     Normal fonksiyonlar yemek yemek gibi — başlarsın ve bir oturumda bitirirsin.
 *
 * =============================================================================
 * [THEORY: The Three Magic Keywords / TEORİ: ÜÇ SİHİRLİ ANAHTAR KELİME]
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
 * TR: Bir fonksiyon, gövdesinde bu anahtar kelimelerden HERHANGİ BİRİ varsa coroutine olur:
 *
 *     ┌──────────────┬─────────────────────────────────────────────────────────┐
 *     │ Anahtar      │ Ne Yapar                                                │
 *     ├──────────────┼─────────────────────────────────────────────────────────┤
 *     │ co_await     │ Çalışmayı askıya alır, asenkron işlemi bekler           │
 *     ├──────────────┼─────────────────────────────────────────────────────────┤
 *     │ co_yield     │ Askıya alır ve değer üretir (tembel üretici)            │
 *     ├──────────────┼─────────────────────────────────────────────────────────┤
 *     │ co_return    │ Coroutine'i tamamlar, isteğe bağlı değer döndürür       │
 *     └──────────────┴─────────────────────────────────────────────────────────┘
 *
 *     ÖNEMLİ: Aynı fonksiyonda `return` ve `co_return` KARIŞTIRILAMAZ!
 *
 * =============================================================================
 * [THEORY: Coroutine Anatomy / TEORİ: Coroutine Anatomisi]
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
 * TR: Derleyici co_* anahtar kelimesini görünce fonksiyonu dönüştürür:
 *
 *     1. COROUTINE FRAME (heap'te ayrılır*):
 *        - Yerel değişkenleri, parametreleri, promise nesnesini tutar
 *        - Askıya almalar arasında kalıcıdır (yerel değişkenlerin yaşama sebebi!)
 *        - *HALO optimizasyonu ile heap kullanımı elenebilir
 *
 *     2. PROMISE TYPE (promise_type):
 *        - Kritik noktalarda coroutine davranışını kontrol eder
 *        - Derleyicinin danıştığı bir "politika sınıfı" gibi
 *        - Siz tanımlarsınız: initial_suspend, final_suspend, yield_value, vb.
 *
 *     3. COROUTINE HANDLE (std::coroutine_handle<Promise>):
 *        - Coroutine frame'e sahip olmayan (non-owning) işaretçi
 *        - Dışarıdan coroutine'i DEVAM ETTİRMEK veya YOK ETMEK için kullanılır
 *        - handle.resume() → çalışmaya devam eder
 *        - handle.destroy() → frame'i serbest bırakır
 *
 *     Derleyici dönüşümü (basitleştirilmiş):
 *     ┌────────────────────────────────────────────────────────────┐
 *     │  Sizin Kodunuz      →  Derleyicinin Ürettikleri            │
 *     ├────────────────────────────────────────────────────────────┤
 *     │  Generator count()  →  1. Coroutine frame tahsis et        │
 *     │  {                  →  2. Promise nesnesi oluştur          │
 *     │    co_yield 1;      →  3. initial_suspend() çağrısı        │
 *     │    co_yield 2;      →  4. Gövdeyi askıya almalarla çalıştır│
 *     │  }                  →  5. final_suspend() çağrısı          │
 *     │                     →  6. Frame'i yok et (gerekirse)       │
 *     └────────────────────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: Awaitable Concept / TEORİ: Awaitable Kavramı]
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
 * TR: co_await "awaitable" nesneler üzerinde çalışır. 3 metodu olmalı:
 *
 *     bool await_ready()        → Askıya almalı mıyız? (false = evet, askıya al)
 *     void await_suspend(h)     → Askıya alınırken ne yapılmalı
 *     T    await_resume()       → Devam edilince hangi değer üretilmeli
 *
 * @see https://en.cppreference.com/w/cpp/language/coroutines
 * @see https://lewissbaker.github.io/ — "Asymmetric Transfer" blog (excellent)
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_coroutine_fundamentals.cpp -o 01_coroutine_fundamentals
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <coroutine>   // EN: C++20 coroutine support / TR: C++20 coroutine desteği
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
 * TR: Generator<T> en basit kullanışlı coroutine tipidir: tembel dizi üretici.
 *     co_yield ile her seferinde bir değer üretir, aralarında duraklar.
 *     Çağıran next() ile değer çeker — istenene kadar hesaplama yapılmaz.
 *
 *     Bu coroutine'lerin "Merhaba Dünya"sıdır. Derleyicinin tam olarak ne
 *     beklediğini göstermek için sıfırdan inşa ediyoruz.
 */
template<typename T>
class Generator {
public:
    // ── promise_type: The "policy" that tells the compiler how this coroutine behaves ──
    // EN: The compiler looks for a nested type called `promise_type` inside the return type.
    //     This is NOT optional — without it, compilation fails.
    //     Each method answers a specific question the compiler asks:
    //
    // TR: Derleyici, dönüş tipinin içinde `promise_type` adlı iç içe bir tip arar.
    //     Bu opsiyonel DEĞİLDİR — olmadan derleme başarısız olur.
    //     Her metot, derleyicinin sorduğu belirli bir soruyu yanıtlar:
    struct promise_type {
        T current_value;             // EN: Stores the last co_yield'd value / TR: Son co_yield edilen değeri tutar
        std::exception_ptr exception; // EN: Captures exceptions from coroutine / TR: Coroutine'den gelen istisnaları yakalar

        // EN: Q: "What object should be returned to the caller?"
        //     A: A Generator wrapping a handle to this coroutine.
        // TR: S: "Çağırana hangi nesne döndürülmeli?"
        //     C: Bu coroutine'e bir handle saran bir Generator.
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // EN: Q: "Should the coroutine suspend BEFORE running any user code?"
        //     A: YES (suspend_always) — lazy start, nothing runs until first next() call.
        //     If we said suspend_never, the coroutine would run eagerly to first co_yield.
        // TR: S: "Coroutine, kullanıcı kodu çalıştırmadan ÖNCE askıya alınmalı mı?"
        //     C: EVET (suspend_always) — tembel başlangıç, ilk next() çağrısına kadar çalışmaz.
        std::suspend_always initial_suspend() noexcept { return {}; }

        // EN: Q: "Should the coroutine suspend AFTER it finishes (at final_suspend)?"
        //     A: YES — we suspend so the caller can detect completion and clean up the handle.
        //     If we said suspend_never, the frame auto-destroys and our handle dangles!
        //     WARNING: final_suspend MUST be noexcept (standard requirement).
        // TR: S: "Coroutine bittikten SONRA askıya alınmalı mı?"
        //     C: EVET — çağıran tamamlanmayı algılayıp handle'ı temizleyebilsin.
        //     suspend_never desek frame otomatik yıkılır ve handle sarkar!
        //     UYARI: final_suspend noexcept OLMALIDIR (standart gereksiniml).
        std::suspend_always final_suspend() noexcept { return {}; }

        // EN: Q: "A co_yield expression was hit — what should happen?"
        //     A: Store the value and suspend (suspend_always).
        //     The caller will read current_value after resume returns to them.
        // TR: S: "co_yield ifadesine ulaşıldı — ne olmalı?"
        //     C: Değeri sakla ve askıya al (suspend_always).
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }

        // EN: Q: "co_return was hit (or end of function reached) — any return value?"
        //     A: No value for generators — void return.
        // TR: S: "co_return'e ulaşıldı — dönüş değeri?"
        //     C: Generator'lar için değer yok — void dönüş.
        void return_void() {}

        // EN: Q: "An unhandled exception escaped the coroutine body — what to do?"
        //     A: Capture it. The caller can rethrow it later if needed.
        // TR: S: "Coroutine gövdesinden yakalanmamış istisna çıktı — ne yapılmalı?"
        //     C: Yakala. Çağıran gerekirse sonra yeniden fırlatabilir.
        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    // ── Generator class body ──

    // EN: Type alias for the coroutine handle specific to our promise
    // TR: Promise'imize özgü coroutine handle için tür takma adı
    using handle_type = std::coroutine_handle<promise_type>;

    // EN: Constructor — takes ownership of the coroutine handle
    // TR: Constructor — coroutine handle'ın sahipliğini alır
    explicit Generator(handle_type h) : handle_(h) {}

    // EN: Destructor — MUST destroy the handle to free the coroutine frame!
    //     If you forget this, the frame LEAKS (heap memory not freed).
    // TR: Destructor — Coroutine frame'i serbest bırakmak için handle'ı yıkmalı!
    //     Unutursan frame SIZDIRIR (heap belleği serbest bırakılmaz).
    ~Generator() {
        if (handle_) handle_.destroy();
    }

    // EN: Move-only type (coroutine ownership cannot be shared)
    // TR: Sadece taşınabilir tip (coroutine sahipliği paylaşılamaz)
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
    // TR: Coroutine'i sonraki co_yield noktasına ilerlet ve değeri döndür.
    //     std::optional<T> döndürür — coroutine bittiyse boş.
    std::optional<T> next() {
        if (!handle_ || handle_.done()) return std::nullopt;

        handle_.resume();  // EN: Continue coroutine until next co_yield or end
                           // TR: Sonraki co_yield veya sona kadar coroutine'i devam ettir

        if (handle_.done()) return std::nullopt;

        // EN: Check for exceptions that escaped the coroutine
        // TR: Coroutine'den kaçan istisnaları kontrol et
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
 * TR: Task<T> tek bir değer üreten asenkron hesaplamayı temsil eder.
 *     Generator'dan (birçok değer üreten) farklı olarak, Task co_return ile TEK sonuç üretir.
 *     std::future<T>'nin coroutine karşılığıdır.
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
        // TR: Tembel başlangıç — başlangıçta askıya al, açıkça devam ettirilene kadar çalıştırma
        std::suspend_always initial_suspend() noexcept { return {}; }

        // EN: Suspend at end so caller can read the result before frame is destroyed
        // TR: Çağıran, frame yıkılmadan sonucu okuyabilsin diye sonunda askıya al
        std::suspend_always final_suspend() noexcept { return {}; }

        // EN: co_return value; → store the result
        // TR: co_return value; → sonucu sakla
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
    // TR: Coroutine'i tamamlanana kadar çalıştır ve sonucu döndür.
    //     Coroutine birden fazla co_await askıya alma noktasına sahip olabilir.
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
 * TR: Mümkün olan en basit generator: 'start'tan 'end'e kadar sayar.
 *     Her co_yield coroutine'i askıya alır ve değeri çağırana sunar.
 */
Generator<int> count_up(int start, int end) {
    // EN: This looks like a normal loop, but co_yield makes it a coroutine!
    //     The loop "pauses" at each co_yield and the caller gets the value.
    // TR: Normal bir döngüye benziyor, ama co_yield onu coroutine yapıyor!
    //     Döngü her co_yield'da "duraklar" ve çağıran değeri alır.
    for (int i = start; i <= end; ++i) {
        co_yield i;
        // EN: ↑ Execution suspends here. Local variable 'i' is preserved in the frame.
        //     When next() is called again, execution continues from the line AFTER co_yield.
        // TR: ↑ Çalışma burada askıya alınır. Yerel değişken 'i' frame'de korunur.
        //     next() tekrar çağrılınca çalışma co_yield'DAN SONRAKİ satırdan devam eder.
    }
    // EN: Function body ends → return_void() is called → final_suspend() suspends
    // TR: Fonksiyon gövdesi biter → return_void() çağrılır → final_suspend() askıya alır
}

// ── 3b: Fibonacci generator (infinite!) ──

/**
 * EN: An INFINITE generator — it never returns, just keeps yielding Fibonacci numbers.
 *     This is perfectly fine! The caller decides when to stop pulling values.
 *     This is the power of lazy evaluation: compute only what you need.
 *     Memory usage: O(1) — only 3 variables in the coroutine frame (a, b, temp).
 *     Compare to pre-computing: storing N Fibonacci numbers needs O(N) memory.
 *
 * TR: SONSUZ bir generator — asla dönmez, Fibonacci sayıları üretmeye devam eder.
 *     Sorun değil! Çağıran ne zaman değer çekmeyi durduracağına karar verir.
 *     Tembel değerlendirmenin gücü: sadece ihtiyacın olanı hesapla.
 *     Bellek kullanımı: O(1) — frame'de sadece 3 değişken (a, b, temp).
 */
Generator<uint64_t> fibonacci() {
    uint64_t a = 0, b = 1;
    while (true) {                // EN: Infinite loop — but suspends at each yield!
        co_yield a;               // TR: Sonsuz döngü — ama her yield'da askıya alınır!
        uint64_t temp = a + b;
        a = b;
        b = temp;
    }
    // EN: This line is never reached — the coroutine runs forever (or until destroyed)
    // TR: Bu satıra asla ulaşılmaz — coroutine sonsuza kadar çalışır (veya yıkılana dek)
}

// ── 3c: Filtered generator — composing coroutines ──

/**
 * EN: Generator that filters another generator's output - only even numbers.
 *     This shows coroutine COMPOSITION: one coroutine wrapping another.
 *     The inner generator produces values, our coroutine filters and re-yields.
 *
 * TR: Başka bir generator'ın çıktısını filtreleyen generator — sadece çift sayılar.
 *     Coroutine BİLEŞİMİNİ gösterir: bir coroutine başkasını sarar.
 */
Generator<uint64_t> even_fibonacci(int count) {
    auto fib = fibonacci();
    int found = 0;
    while (found < count) {
        auto val = fib.next();
        if (!val.has_value()) break;  // EN: Inner generator finished / TR: İç generator bitti
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
 * TR: Gerçek dünya kullanımı: string'i tembel olarak tokenize et.
 *     Tüm string'i önceden bölmek yerine (string vektörü ayırmak),
 *     her seferinde bir token üretiriz. Büyük string'ler için bellek tasarrufu.
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
        co_yield token;   // EN: Don't forget the last token! / TR: Son token'ı unutma!
    }
}

// ── 3e: Task-based coroutine — co_return demo ──

/**
 * EN: A Task coroutine that does a "computation" and returns a single result.
 *     Uses co_return instead of co_yield.
 *     In real code, before co_return you'd co_await some async I/O.
 *
 * TR: "Hesaplama" yapan ve tek sonuç döndüren Task coroutine.
 *     co_yield yerine co_return kullanır.
 *     Gerçek kodda co_return öncesinde asenkron I/O co_await edersiniz.
 */
// EN: IMPORTANT: Coroutine parameters MUST be taken by VALUE, not by reference!
//     Why? Because initial_suspend() suspends BEFORE the body runs.
//     If a param is a reference to a temporary, that temporary dies while
//     the coroutine is suspended → dangling reference → undefined behavior!
//     Rule: Always pass by value to coroutines (the compiler copies into the frame).
//
// TR: ÖNEMLİ: Coroutine parametreleri referans DEĞİL, DEĞER ile alınmalı!
//     Neden? initial_suspend() gövde çalışmadan ÖNCE askıya alır.
//     Parametre geçici bir nesneye referans ise, o geçici coroutine askıya alınırken
//     ölür → sarkan referans → tanımsız davranış!
//     Kural: Coroutine'lere her zaman değer ile geç (derleyici frame'e kopyalar).
Task<std::string> compute_sensor_report(std::string sensorId, double temp) {
    // EN: In a real system, this could co_await a database query or network call
    // TR: Gerçek sistemde burası veritabanı sorgusu veya ağ çağrısı co_await edebilirdi
    std::string report = "Sensor " + sensorId + ": " + std::to_string(temp) + "C";

    if (temp > 100.0) {
        report += " [CRITICAL]";
    } else if (temp > 75.0) {
        report += " [WARNING]";
    } else {
        report += " [NORMAL]";
    }

    co_return report;  // EN: Complete the Task with this value
                       // TR: Task'ı bu değerle tamamla
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
 * TR: "Gecikme" veya asenkron işlemi simüle eden özel bir awaitable.
 *     co_await bu üzerinde kullanılınca üç metot çağrılır:
 *       1. await_ready() → "Sonuç hemen mevcut mu?"
 *       2. await_suspend(handle) → "Coroutine'i askıya al, handle burada"
 *       3. await_resume() → "Coroutine devam etti, sonuç ne?"
 */
struct SimulatedDelay {
    std::string operation_name;
    int result_value;

    // EN: "Is the result immediately available?" → No, we always need to "wait"
    // TR: "Sonuç hemen mevcut mu?" → Hayır, her zaman "beklememiz" gerekiyor
    bool await_ready() const noexcept {
        std::cout << "    [await_ready] '" << operation_name << "' — not ready, will suspend\n";
        return false;  // EN: false = suspend / TR: false = askıya al
    }

    // EN: "The coroutine is being suspended. What should we do?"
    //     We could schedule work on a thread pool, register with an event loop, etc.
    //     For this demo, we just print a message.
    //     IMPORTANT: We can choose to:
    //       - void return → always suspend
    //       - bool return → false = don't actually suspend (optimization)
    //       - coroutine_handle return → symmetric transfer to another coroutine
    // TR: "Coroutine askıya alınıyor. Ne yapmalıyız?"
    //     İş parçacığı havuzunda iş zamanlayabilir, olay döngüsüne kaydedebiliriz.
    void await_suspend(std::coroutine_handle<> /*handle*/) const noexcept {
        std::cout << "    [await_suspend] '" << operation_name << "' — simulating async work...\n";
        // EN: In real code: store the handle, schedule resume on completion
        // TR: Gerçek kodda: handle'ı sakla, tamamlanınca resume zamanla
    }

    // EN: "The coroutine is being resumed. What value should co_await produce?"
    //     This return value becomes the result of the `co_await expr` expression.
    // TR: "Coroutine devam ettiriliyor. co_await hangi değeri üretmeli?"
    int await_resume() const noexcept {
        std::cout << "    [await_resume] '" << operation_name << "' — result = " << result_value << "\n";
        return result_value;
    }
};

/**
 * EN: A coroutine that uses our custom awaitable to demonstrate co_await mechanics.
 *     Each co_await suspends the coroutine and resumes it (in our simple case, immediately).
 *
 * TR: co_await mekanizmasını göstermek için özel awaitable kullanan coroutine.
 */
Task<int> sensor_pipeline() {
    std::cout << "  [coroutine] Starting sensor pipeline...\n";

    // EN: co_await SimulatedDelay{...} calls:
    //     1. SimulatedDelay::await_ready() → false → suspend
    //     2. SimulatedDelay::await_suspend(handle) → prints message
    //     3. (Later, when resumed) SimulatedDelay::await_resume() → returns value
    // TR: co_await SimulatedDelay{...} şunları çağırır:
    //     1. await_ready() → false → askıya al
    //     2. await_suspend(handle) → mesaj yazdır
    //     3. (Devam ettirilince) await_resume() → değer döndür
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
        // TR: count_up(1, 5) 1, 2, 3, 4, 5 üreten bir coroutine oluşturur
        //     Her next() çağrısı coroutine'i devam ettirir, sonraki co_yield'a kadar çalışır
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
        // TR: fibonacci() SONSUZ bir generator — kendiliğinden asla durmaz.
        //     Sadece 15 değer çekmeye karar veriyoruz. Tembel değerlendirmenin en iyisi!
        auto fib = fibonacci();
        std::cout << "  First 15 Fibonacci numbers:\n  ";
        for (int i = 0; i < 15; ++i) {
            auto val = fib.next();
            if (val) std::cout << *val << " ";
        }
        std::cout << "\n\n";
        // EN: fib destructor destroys the coroutine frame — no leak!
        // TR: fib destructor'ı coroutine frame'i yıkar — sızıntı yok!
    }

    // ─── Demo 3: Composed Generator — Even Fibonacci ─────────────────────
    {
        std::cout << "--- Demo 3: Coroutine Composition — Even Fibonacci ---\n";
        // EN: even_fibonacci wraps fibonacci and filters: only yields even numbers
        //     This demonstrates coroutine composition — generators calling generators
        // TR: even_fibonacci, fibonacci'yi sarlayıp filtreler: sadece çift sayıları üretir
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
        // TR: Gerçek dünya kalıbı: CAN bus log satırını tembelce tokenize et
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
        // TR: Task co_return ile TEK değer üretir (co_yield değil)
        //     .get() coroutine'i devam ettirir ve sonucu alır
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
        // TR: co_await'in 3 adımlı sürecini gösterir:
        //     await_ready → await_suspend → (devam ettir) → await_resume
        //     Gerçek kodda await_suspend, handle'ı I/O olay döngüsüne kaydeder
        //     ve asenkron işlem tamamlanınca resume() çağrılır.
        auto pipeline = sensor_pipeline();
        // EN: pipeline is a Task. get() calls resume() which drives the coroutine.
        //     Inside, each co_await creates a suspension point.
        //     After await_suspend, we call resume again, which triggers await_resume.
        // TR: pipeline bir Task. get(), resume()'ı çağırarak coroutine'i yürütür.

        // EN: We need to manually resume through all suspension points in our simple case
        // TR: Basit durumumuzda tüm askıya alma noktalarından manuel geçmeliyiz
        int result = pipeline.get();
        std::cout << "  [main] Pipeline final result: " << result << "\n";
        std::cout << "\n";
    }

    // ─── Demo 7: Generator as Range-like Iteration ───────────────────────
    {
        std::cout << "--- Demo 7: Collecting Generator to Vector ---\n";
        // EN: You can collect all generator values into a container if needed
        //     This shows the bridge between lazy and eager evaluation
        // TR: Gerekirse tüm generator değerlerini bir konteyner'a toplayabilirsin
        //     Tembel ve hevesli değerlendirme arasındaki köprüyü gösterir
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
