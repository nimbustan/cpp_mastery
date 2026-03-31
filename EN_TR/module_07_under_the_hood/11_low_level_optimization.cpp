/**
 * @file module_07_under_the_hood/11_low_level_optimization.cpp
 * @brief Low-Level Optimization — Düşük Seviye Optimizasyon
 *
 * @details
 * =============================================================================
 * [THEORY: Branch Prediction & Hints / TEORİ: Dal Tahmini ve İpuçları]
 * =============================================================================
 *
 * EN: Modern CPUs predict branch outcomes to keep the pipeline full.
 *     Wrong predictions cost ~15-20 cycles (pipeline flush).
 *
 *     C++20 attributes:
 *       [[likely]]   — This branch is probably taken (hot path)
 *       [[unlikely]] — This branch is probably NOT taken (error/cold path)
 *
 *     GCC/Clang builtin (pre-C++20):
 *       __builtin_expect(expr, expected_value)
 *       if (__builtin_expect(error_occurred, 0)) { ... }  // expect false
 *
 *     Real impact: 5-15% speedup on branch-heavy code (parsers, validators)
 *
 * TR: Modern CPU'lar pipeline'ı dolu tutmak için dal sonuçlarını tahmin eder.
 *     Yanlış tahmin ~15-20 döngüye mal olur (pipeline boşaltma).
 *
 *     C++20 öznitelikleri:
 *       [[likely]]   — Bu dal muhtemelen alınır (sıcak yol)
 *       [[unlikely]] — Bu dal muhtemelen ALINMAZ (hata/soğuk yol)
 *
 *     GCC/Clang yerleşiği (C++20 öncesi):
 *       __builtin_expect(expr, beklenen_deger)
 *       if (__builtin_expect(hata_olustu, 0)) { ... }  // false bekle
 *
 *     Gerçek etki: Dal yoğun kodda %5-15 hızlanma (ayrıştırıcı, doğrulayıcı)
 *
 * =============================================================================
 * [THEORY: Profile-Guided Optimization (PGO) / TEORİ: Profil Güdümlü Optimizasyon (PGO)]
 * =============================================================================
 *
 * EN: PGO uses REAL runtime data to optimize:
 *
 *     Step 1: Build instrumented binary
 *       g++ -fprofile-generate -O2 main.cpp -o main_instr
 *
 *     Step 2: Run with representative workload
 *       ./main_instr < typical_input.txt    # Generates .gcda profiles
 *
 *     Step 3: Rebuild with profile data
 *       g++ -fprofile-use -O2 main.cpp -o main_optimized
 *
 *     What PGO optimizes:
 *     - Branch prediction hints (based on actual branch frequencies)
 *     - Function inlining (inline frequently-called functions)
 *     - Code layout (hot code close together for cache)
 *     - Loop unrolling (based on actual iteration counts)
 *     - Register allocation (based on actual variable usage)
 *
 *     Typical improvement: 10-30% for branch-heavy code
 *
 * TR: PGO GERÇEK çalışma zamanı verileri kullanarak optimize eder:
 *
 *     Adım 1: Enstrümante ikili oluştur
 *       g++ -fprofile-generate -O2 main.cpp -o main_instr
 *
 *     Adım 2: Temsili iş yükü ile çalıştır
 *       ./main_instr < typical_input.txt    # .gcda profilleri üretir
 *
 *     Adım 3: Profil verileriyle yeniden derle
 *       g++ -fprofile-use -O2 main.cpp -o main_optimized
 *
 *     PGO'nun optimize ettiği şeyler:
 *     - Dal tahmini ipuçları (gerçek dal frekanslarına göre)
 *     - Fonksiyon satır içi açma (sık çağrılan fonksiyonlar)
 *     - Kod yerleşimi (sıcak kod önbellek için yan yana)
 *     - Döngü açma (gerçek iterasyon sayılarına göre)
 *     - Yazmaç tahsisi (gerçek değişken kullanımına göre)
 *
 *     Tipik iyileşme: Dal yoğun kodda %10-30
 *
 * =============================================================================
 * [THEORY: Hot/Cold Path Separation / TEORİ: Sıcak/Soğuk Yol Ayrımı]
 * =============================================================================
 *
 * EN: Keep hot (frequently executed) code compact and together:
 *
 *     HOT PATH:                           COLD PATH:
 *     - Normal execution flow             - Error handling
 *     - Inner loops                       - Logging/debug output
 *     - Frequently accessed data          - Initialization (one-time)
 *     - Short functions (inlined)         - Exception handling
 *
 *     Techniques:
 *     1. __attribute__((hot)) / __attribute__((cold))  — GCC/Clang
 *     2. [[likely]] / [[unlikely]] — C++20
 *     3. Move error strings to separate function (keeps hot path small)
 *     4. Avoid large inline functions on error paths
 *     5. Use noinline on cold functions to keep icache pressure low
 *
 * TR: Sıcak (sık çalıştırılan) kodu kompakt ve bir arada tut:
 *
 *     SICAK YOL:                          SOĞUK YOL:
 *     - Normal yürütme akışı             - Hata yönetimi
 *     - İç döngüler                       - Loglama/debug çıktısı
 *     - Sık erişilen veri               - Başlatma (tek seferlik)
 *     - Kısa fonksiyonlar (satır içi)    - İstisna yönetimi
 *
 *     Teknikler:
 *     1. __attribute__((hot)) / __attribute__((cold))  — GCC/Clang
 *     2. [[likely]] / [[unlikely]] — C++20
 *     3. Hata dizelerini ayrı fonksiyona taşı (sıcak yol küçük kalır)
 *     4. Hata yollarında büyük inline fonksiyonlardan kaçın
 *     5. Soğuk fonksiyonlarda noinline kullan (icache basıncını düşük tut)
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 11_low_level_optimization.cpp -o 11_low_level_optimization
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <numeric>
#include <cstdint>
#include <algorithm>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: [[likely]] / [[unlikely]] Attributes (C++20)
// ═════════════════════════════════════════════════════════════════════════════

// EN: Sensor validation with branch hints
// TR: Dal ipuçları ile sensör doğrulama
int validate_sensor_likely(int value) {
    if (value >= 0 && value <= 1023) [[likely]] {
        // EN: Normal case — sensor value in valid range (99% of the time)
        // TR: Normal durum — sensör değeri geçerli aralıkta (zamanın %99'u)
        return value;
    } else [[unlikely]] {
        // EN: Error case — out of range (rarely happens)
        // TR: Hata durumu — aralık dışı (nadiren olur)
        return -1;
    }
}

// EN: Switch with likely hint
// TR: likely ipucu ile switch
enum class PacketType : uint8_t { Data = 0, Heartbeat = 1, Error = 2, Config = 3 };

void process_packet(PacketType type) {
    switch (type) {
        case PacketType::Data: [[likely]]
            // EN: 90% of packets are data
            // TR: Paketlerin %90'ı veridir
            break;
        case PacketType::Heartbeat:
            break;
        case PacketType::Error: [[unlikely]]
            // EN: Errors are rare
            // TR: Hatalar nadirdir
            break;
        case PacketType::Config: [[unlikely]]
            break;
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: __builtin_expect (Pre-C++20)
// ═════════════════════════════════════════════════════════════════════════════

// EN: GCC/Clang builtin for branch prediction hints
// TR: Dal tahmini ipuçları için GCC/Clang yerleşik fonksiyonu
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

int validate_sensor_builtin(int value) {
    if (LIKELY(value >= 0 && value <= 1023)) {
        return value;
    } else {
        return -1;
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Hot/Cold Path Separation
// ═════════════════════════════════════════════════════════════════════════════

// EN: __attribute__((cold)) — tells compiler this is rarely called
// TR: __attribute__((cold)) — derleyiciye bunun nadiren çağrıldığını söyler
__attribute__((cold, noinline))
void handle_error(int sensor_id, int value) {
    // EN: Error handling code — large, but rarely executed
    //     Using noinline keeps it out of the hot path's icache footprint
    // TR: Hata yönetimi kodu — büyük ama nadiren çalıştırılır
    std::cerr << "ERROR: Sensor " << sensor_id << " value " << value
              << " out of range!\n";
}

// EN: __attribute__((hot)) — tells compiler to optimize aggressively
// TR: __attribute__((hot)) — derleyiciye agresif optimize etmesini söyler
__attribute__((hot))
int process_sensor_reading(int sensor_id, int raw_value) {
    // EN: HOT PATH — validate and convert (runs millions of times)
    // TR: SICAK YOL — doğrula ve dönüştür (milyonlarca kez çalışır)
    if (raw_value >= 0 && raw_value <= 1023) [[likely]] {
        return raw_value * 100 / 1024;  // Scale to 0-100
    }
    // EN: COLD PATH — error handling (separated to keep hot path small)
    // TR: SOĞUK YOL — hata yönetimi (sıcak yolu küçük tutmak için ayrılmış)
    handle_error(sensor_id, raw_value);
    return -1;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Compiler Intrinsics for Bit Operations
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Compiler intrinsics compile to single CPU instructions.
 *     Much faster than library implementations.
 *
 *     ┌─────────────────────────┬─────────────────────────────────────┐
 *     │ Intrinsic               │ Purpose                             │
 *     ├─────────────────────────┼─────────────────────────────────────┤
 *     │ __builtin_popcount(x)   │ Count set bits (population count)   │
 *     │ __builtin_clz(x)        │ Count leading zeros                 │
 *     │ __builtin_ctz(x)        │ Count trailing zeros                │
 *     │ __builtin_ffs(x)        │ Find first set bit (1-indexed)      │
 *     │ __builtin_parity(x)     │ Parity of set bits (odd=1, even=0)  │
 *     │ __builtin_bswap32(x)    │ Byte swap (endian conversion)       │
 *     │ __builtin_prefetch(p)   │ Prefetch memory into cache          │
 *     └─────────────────────────┴─────────────────────────────────────┘
 *
 * TR: Derleyici intrinsic'leri tek CPU talimatına derlenir.
 *     Kütüphane uygulamalarından çok daha hızlıdır.
 */

void demo_intrinsics() {
    uint32_t val = 0b1010'1100'0011'0000;  // 0xAC30

    std::cout << "    Value: 0x" << std::hex << val << std::dec << "\n";
    std::cout << "    popcount:      " << __builtin_popcount(val) << " bits set\n";
    std::cout << "    clz:           " << __builtin_clz(val) << " leading zeros\n";
    std::cout << "    ctz:           " << __builtin_ctz(val) << " trailing zeros\n";
    std::cout << "    ffs:           " << __builtin_ffs(static_cast<int>(val))
              << " (first set bit, 1-indexed)\n";
    std::cout << "    parity:        " << __builtin_parity(val)
              << " (" << (__builtin_parity(val) ? "odd" : "even") << " bits)\n";
    std::cout << "    bswap32:       0x" << std::hex << __builtin_bswap32(val)
              << std::dec << " (endian swap)\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Prefetch for Sequential Access
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: __builtin_prefetch(addr, rw, locality)
 *     - addr: pointer to memory to prefetch
 *     - rw: 0 = read, 1 = write
 *     - locality: 0 = non-temporal (use once), 3 = keep in all caches
 *
 *     Useful for: sequential array processing where you know you'll need
 *     future elements before you get to them.
 *
 * TR: __builtin_prefetch(adr, oku_yaz, yerellik)
 *     Ardışık dizi işlemlerinde, gelecek elementlere ihtiyaç duyacağınızı
 *     bildiğiniz durumlarda faydalıdır.
 */

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 07 - Low-Level Optimization\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: [[likely]] / [[unlikely]] ──────────────────────────────
    {
        std::cout << "--- Demo 1: [[likely]] / [[unlikely]] ---\n";
        std::cout << "    validate(512)  = " << validate_sensor_likely(512) << " (valid)\n";
        std::cout << "    validate(2000) = " << validate_sensor_likely(2000) << " (invalid)\n";
        std::cout << "    validate(-5)   = " << validate_sensor_likely(-5) << " (invalid)\n\n";
    }

    // ─── Demo 2: Branch Prediction Benchmark ────────────────────────────
    {
        std::cout << "--- Demo 2: Branch Prediction Impact ---\n";

        constexpr size_t N = 10'000'000;
        std::vector<int> data(N);

        // EN: Fill with values 0-1023 (valid) + occasional -1 (invalid)
        // TR: 0-1023 (geçerli) değerlerle doldur + ara sıra -1 (geçersiz)
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, 1023);
        for (auto& v : data) v = dist(rng);
        // EN: Sprinkle 0.1% invalid values
        // TR: %0.1 geçersiz değer serpiştir
        for (size_t i = 0; i < N / 1000; ++i) {
            data[rng() % N] = -1;
        }

        // EN: Sorted data (predictable branches)
        // TR: Sıralı veri (öngörülebilir dallanma)
        auto sorted = data;
        std::sort(sorted.begin(), sorted.end());

        // EN: Benchmark unsorted (unpredictable)
        // TR: Sırasız veriyi benchmark'la (öngörülemez)
        auto t1 = std::chrono::high_resolution_clock::now();
        volatile int sum1 = 0;
        for (auto v : data) {
            if (v >= 0) sum1 += v;
        }
        auto t2 = std::chrono::high_resolution_clock::now();

        // EN: Benchmark sorted (predictable)
        // TR: Sıralı veriyi benchmark'la (öngörülebilir)
        auto t3 = std::chrono::high_resolution_clock::now();
        volatile int sum2 = 0;
        for (auto v : sorted) {
            if (v >= 0) sum2 += v;
        }
        auto t4 = std::chrono::high_resolution_clock::now();

        auto us1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        auto us2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

        std::cout << "    Unsorted (random branches): " << us1 << " us\n";
        std::cout << "    Sorted (predictable):       " << us2 << " us\n";
        std::cout << "    (Sorted may be faster due to branch prediction)\n\n";
    }

    // ─── Demo 3: Hot/Cold Path Separation ───────────────────────────────
    {
        std::cout << "--- Demo 3: Hot/Cold Path Separation ---\n";
        std::cout << "    process_sensor_reading(1, 512) = "
                  << process_sensor_reading(1, 512) << "%\n";
        std::cout << "    process_sensor_reading(2, 1023) = "
                  << process_sensor_reading(2, 1023) << "%\n";
        std::cout << "    process_sensor_reading(3, -5)  = ";
        int err_result = process_sensor_reading(3, -5);
        std::cout << err_result << " (error)\n\n";

        std::cout << "    Hot path: validate + scale (inline, optimized)\n";
        std::cout << "    Cold path: handle_error (noinline, out of icache)\n\n";
    }

    // ─── Demo 4: Compiler Intrinsics ────────────────────────────────────
    {
        std::cout << "--- Demo 4: Compiler Intrinsics ---\n";
        demo_intrinsics();
        std::cout << "\n";
    }

    // ─── Demo 5: Prefetch Demo ──────────────────────────────────────────
    {
        std::cout << "--- Demo 5: Prefetch Demo ---\n";

        constexpr size_t N = 10'000'000;
        std::vector<int> data(N);
        std::iota(data.begin(), data.end(), 0);

        // EN: Without prefetch
        // TR: Prefetch olmadan
        auto t1 = std::chrono::high_resolution_clock::now();
        long long sum1 = 0;
        for (size_t i = 0; i < N; ++i) {
            sum1 += data[i];
        }
        auto t2 = std::chrono::high_resolution_clock::now();

        // EN: With prefetch (prefetch 16 elements ahead)
        // TR: Prefetch ile (16 eleman ilerisini önbellek'le)
        auto t3 = std::chrono::high_resolution_clock::now();
        long long sum2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (i + 16 < N) {
                __builtin_prefetch(&data[i + 16], 0, 0);
            }
            sum2 += data[i];
        }
        auto t4 = std::chrono::high_resolution_clock::now();

        auto us1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        auto us2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

        std::cout << "    Without prefetch: " << us1 << " us (sum=" << sum1 << ")\n";
        std::cout << "    With prefetch:    " << us2 << " us (sum=" << sum2 << ")\n";
        std::cout << "    (Sequential access already prefetched by HW;\n";
        std::cout << "     prefetch helps more with random access patterns)\n\n";
    }

    // ─── Demo 6: PGO & Optimization Reference ──────────────────────────
    {
        std::cout << "--- Demo 6: PGO & Optimization Reference ---\n";
        std::cout << "  Profile-Guided Optimization:\n";
        std::cout << "    g++ -fprofile-generate -O2 app.cpp -o app_instr\n";
        std::cout << "    ./app_instr < typical_input.txt\n";
        std::cout << "    g++ -fprofile-use -O2 app.cpp -o app_optimized\n\n";
        std::cout << "  ┌────────────────────────────┬───────────────────────────────┐\n";
        std::cout << "  │ Technique                  │ When to Use                   │\n";
        std::cout << "  ├────────────────────────────┼───────────────────────────────┤\n";
        std::cout << "  │ [[likely]]/[[unlikely]]    │ Known hot/cold branches       │\n";
        std::cout << "  │ __builtin_expect           │ Pre-C++20 branch hints        │\n";
        std::cout << "  │ __attribute__((hot/cold))  │ Function-level hint           │\n";
        std::cout << "  │ __builtin_prefetch         │ Random access patterns        │\n";
        std::cout << "  │ __builtin_popcount,clz,ctz │ Bit operations (1 instruction)│\n";
        std::cout << "  │ PGO (-fprofile-use)        │ After profiling real workload │\n";
        std::cout << "  │ LTO (-flto)                │ Cross-TU optimization         │\n";
        std::cout << "  │ -O3 -march=native          │ Maximum local optimization    │\n";
        std::cout << "  └────────────────────────────┴───────────────────────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Low-Level Optimization\n";
    std::cout << "============================================\n";

    return 0;
}
