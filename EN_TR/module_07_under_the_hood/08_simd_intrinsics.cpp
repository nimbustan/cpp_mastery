/**
 * @file module_07_under_the_hood/08_simd_intrinsics.cpp
 * @brief SIMD: Single Instruction, Multiple Data — Vektörel İşleme
 *
 * @details
 * =============================================================================
 * [THEORY: What Is SIMD? / TEORİ: SIMD Nedir?]
 * =============================================================================
 *
 * EN: SIMD lets a single CPU instruction operate on MULTIPLE data elements
 *     simultaneously. Instead of adding two numbers, you add 4, 8, or 16 at once.
 *
 *     ┌───────────────────────────────────────────────────────────────────┐
 *     │ Scalar (without SIMD):           SIMD (SSE: 128-bit):             │
 *     │   a[0] + b[0] = c[0]              a[0..3] + b[0..3] = c[0..3]     │
 *     │   a[1] + b[1] = c[1]              (single instruction!)           │
 *     │   a[2] + b[2] = c[2]                                              │
 *     │   a[3] + b[3] = c[3]            AVX (256-bit): 8 floats at once   │
 *     │   (4 instructions)              AVX-512: 16 floats at once        │
 *     └───────────────────────────────────────────────────────────────────┘
 *
 *     x86 SIMD evolution:
 *     - SSE  (1999): 128-bit registers (XMM0-XMM15), 4 floats
 *     - SSE2 (2001): + 64-bit doubles, integer SIMD
 *     - SSE4 (2006): + blend, round, string processing
 *     - AVX  (2011): 256-bit registers (YMM0-YMM15), 8 floats
 *     - AVX2 (2013): + 256-bit integers
 *     - AVX-512 (2016): 512-bit registers (ZMM0-ZMM31), 16 floats
 *
 * TR: SIMD, tek bir CPU talimatının aynı anda BİRDEN FAZLA veri elemanı üzerinde
 *     çalışmasını sağlar. İki sayı toplamak yerine 4, 8 veya 16 sayı birden toplanır.
 *
 *     ┌───────────────────────────────────────────────────────────────────┐
 *     │ Skaler (SIMD'siz):              SIMD (SSE: 128-bit):              │
 *     │   a[0] + b[0] = c[0]              a[0..3] + b[0..3] = c[0..3]     │
 *     │   a[1] + b[1] = c[1]              (tek talimat!)                  │
 *     │   a[2] + b[2] = c[2]                                              │
 *     │   a[3] + b[3] = c[3]            AVX (256-bit): 8 float birden     │
 *     │   (4 talimat)                   AVX-512: 16 float birden          │
 *     └───────────────────────────────────────────────────────────────────┘
 *
 *     x86 SIMD evrimi:
 *     - SSE  (1999): 128-bit yazmaçlar (XMM0-XMM15), 4 float
 *     - SSE2 (2001): + 64-bit double, tamsayı SIMD
 *     - SSE4 (2006): + blend, round, string işleme
 *     - AVX  (2011): 256-bit yazmaçlar (YMM0-YMM15), 8 float
 *     - AVX2 (2013): + 256-bit tamsayılar
 *     - AVX-512 (2016): 512-bit yazmaçlar (ZMM0-ZMM31), 16 float
 *
 * =============================================================================
 * [THEORY: Intrinsics vs Auto-Vectorization / TEORİ: İntrinsikler vs Oto-Vektörleştirme]
 * =============================================================================
 *
 * EN: Two ways to use SIMD:
 *
 *     1. COMPILER AUTO-VECTORIZATION (preferred):
 *        Write normal C++ loops → compiler generates SIMD automatically
 *        Enabled by: -O2/-O3, -march=native, -ftree-vectorize
 *        Works BEST with: simple loops, contiguous memory, no dependencies
 *
 *     2. EXPLICIT INTRINSICS (when compiler can't auto-vectorize):
 *        Use Intel intrinsics functions (_mm_*, _mm256_*, _mm512_*)
 *        Full control, but: non-portable, hard to read, hard to maintain
 *
 *     Rule of thumb: ALWAYS try auto-vectorization first.
 *     Use intrinsics only when benchmarks prove the compiler isn't vectorizing.
 *
 * TR: SIMD kullanmanın iki yolu:
 *
 *     1. DERLEYİCİ OTO-VEKTÖRLEŞTİRME (tercih edilen):
 *        Normal C++ döngüleri yaz → derleyici SIMD'i otomatik üretir
 *        Etkinleştirme: -O2/-O3, -march=native, -ftree-vectorize
 *        EN İYİ çalışır: basit döngüler, bitisik bellek, bağımlılık yok
 *
 *     2. AÇIK İNTRİNSİKLER (derleyici vektörleştiremediğinde):
 *        Intel intrinsics fonksiyonlarını kullan (_mm_*, _mm256_*, _mm512_*)
 *        Tam kontrol, ama: taşınamaz, okunması zor, bakımı zor
 *
 *     Altın kural: HER ZAMAN önce oto-vektörleştirmeyi dene.
 *     İntrinsikleri sadece benchmark'lar derleyicinin vektörleştirmediğini kanıtlarsa kullan.
 *
 * =============================================================================
 * [THEORY: Memory Alignment / TEORİ: Bellek Hizalama]
 * =============================================================================
 *
 * EN: SIMD instructions require (or strongly prefer) aligned memory:
 *     - SSE: 16-byte aligned (_mm_load_ps requires 16-byte alignment)
 *     - AVX: 32-byte aligned (_mm256_load_ps requires 32-byte alignment)
 *     - Use `alignas(N)` to align stack/static arrays
 *     - Use aligned allocators for heap memory
 *     - Unaligned loads (_mm_loadu_ps) work but may be slower on older CPUs
 *       (On modern CPUs, aligned/unaligned performance is nearly identical)
 *
 * TR: SIMD talimatları hizalanmış bellek gerektirir (veya şiddetle tercih eder):
 *     - SSE: 16-bayt hizalı (_mm_load_ps 16-bayt hizalama gerektirir)
 *     - AVX: 32-bayt hizalı (_mm256_load_ps 32-bayt hizalama gerektirir)
 *     - Yığın/statik dizileri hizalamak için `alignas(N)` kullanın
 *     - Heap belleği için hizalı ayırıcılar kullanın
 *     - Hizasız yüklemeler (_mm_loadu_ps) çalışır ama eski CPU'larda daha yavaş
 *       (Modern CPU'larda hizalı/hizasız performans neredeyse aynıdır)
 *
 * @note Requires x86 CPU with SSE2+AVX support. Compile with: -mavx2 or -march=native
 *       x86 CPU gerektirir. Derleme: -mavx2 veya -march=native
 *
 * @see https://www.intel.com/content/www/us/en/docs/intrinsics-guide/
 * @see https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 08_simd_intrinsics.cpp -o 08_simd_intrinsics
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <chrono>
#include <array>
#include <numeric>
#include <iomanip>
#include <cmath>

// EN: SIMD intrinsic headers — each includes the previous generation
//     <immintrin.h> includes everything: SSE, SSE2, ..., AVX, AVX2, AVX-512
// TR: SIMD intrinsic başlık dosyaları — her biri önceki nesli içerir
#include <immintrin.h>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: SSE — 128-bit SIMD (4 floats / 2 doubles at once)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: SSE intrinsic naming convention:
 *     _mm_<operation>_<type>
 *
 *     Types: ps = packed single (4×float)
 *            pd = packed double (2×double)
 *            epi32 = packed 32-bit integer
 *            si128 = 128-bit integer
 *
 *     Operations: add, sub, mul, div, load, store, set, setzero, sqrt, min, max
 *
 *     Example: _mm_add_ps = Packed Single-precision Add → adds 4 floats at once
 *
 * TR: SSE intrinsic adlandırma kuralı:
 *     _mm_<işlem>_<tür>
 *     Türler: ps = paketli tek hassasiyet (4×float), pd = paketli çift (2×double)
 */

/**
 * EN: Add two arrays of 4 floats using SSE intrinsics.
 *     This demonstrates the basic load → operate → store pattern:
 *     1. _mm_load_ps: Load 4 floats from memory into __m128 register
 *     2. _mm_add_ps:  Add two __m128 registers (4 floats simultaneously)
 *     3. _mm_store_ps: Store __m128 register back to memory
 *
 * TR: SSE intrinsikleri ile 4 float'lık iki diziyi topla.
 *     Temel yükle → işle → sakla kalıbı gösterilir.
 */
void sse_add_example() {
    // EN: alignas(16) ensures 16-byte alignment for SSE loads
    //     Without this, _mm_load_ps would crash (SIGSEGV) on some CPUs!
    // TR: alignas(16), SSE yükleme için 16-bayt hizalamayı garanti eder
    alignas(16) float a[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    alignas(16) float b[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    alignas(16) float c[4];

    // EN: Load 4 floats from a[] and b[] into 128-bit SSE registers
    // TR: a[] ve b[]'den 4 float'ı 128-bit SSE yazmaçlarına yükle
    __m128 va = _mm_load_ps(a);   // va = [1.0, 2.0, 3.0, 4.0]
    __m128 vb = _mm_load_ps(b);   // vb = [5.0, 6.0, 7.0, 8.0]

    // EN: Add 4 pairs simultaneously — ONE instruction for 4 additions!
    // TR: 4 çifti aynı anda topla — 4 toplama için TEK talimat!
    __m128 vc = _mm_add_ps(va, vb);  // vc = [6.0, 8.0, 10.0, 12.0]

    // EN: Store result back to memory
    // TR: Sonucu belleğe geri sakla
    _mm_store_ps(c, vc);

    std::cout << "  SSE _mm_add_ps: [";
    for (int i = 0; i < 4; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << c[i];
    }
    std::cout << "]\n";
}

/**
 * EN: SSE provides rich math operations beyond basic arithmetic.
 *     This function demonstrates several in one place.
 *
 * TR: SSE temel aritmetiğin ötesinde zengin matematik işlemleri sağlar.
 */
void sse_operations_showcase() {
    alignas(16) float a[4] = {16.0f, 9.0f, 25.0f, 4.0f};
    alignas(16) float b[4] = {3.0f, 7.0f, 2.0f, 8.0f};
    alignas(16) float result[4];

    __m128 va = _mm_load_ps(a);
    __m128 vb = _mm_load_ps(b);

    // EN: Square root of each element / TR: Her elemanın karekökü
    __m128 vsqrt = _mm_sqrt_ps(va);
    _mm_store_ps(result, vsqrt);
    std::cout << "  sqrt([16,9,25,4])    = [" << result[0] << ", " << result[1]
              << ", " << result[2] << ", " << result[3] << "]\n";

    // EN: Element-wise minimum / TR: Eleman bazlı minimum
    __m128 vmin = _mm_min_ps(va, vb);
    _mm_store_ps(result, vmin);
    std::cout << "  min(a,b)             = [" << result[0] << ", " << result[1]
              << ", " << result[2] << ", " << result[3] << "]\n";

    // EN: Element-wise maximum / TR: Eleman bazlı maksimum
    __m128 vmax = _mm_max_ps(va, vb);
    _mm_store_ps(result, vmax);
    std::cout << "  max(a,b)             = [" << result[0] << ", " << result[1]
              << ", " << result[2] << ", " << result[3] << "]\n";

    // EN: Multiply then add: (a * b) + c — one operation, better precision
    //     FMA (Fused Multiply-Add) avoids one rounding step
    // TR: Çarp sonra topla: (a * b) + c — tek işlem, daha iyi hassasiyet
    alignas(16) float addend[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    __m128 vadd = _mm_load_ps(addend);
    __m128 vfma = _mm_fmadd_ps(va, vb, vadd);  // (a*b) + 1
    _mm_store_ps(result, vfma);
    std::cout << "  fma(a*b+1)           = [" << result[0] << ", " << result[1]
              << ", " << result[2] << ", " << result[3] << "]\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: AVX — 256-bit SIMD (8 floats / 4 doubles at once)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: AVX doubles SSE's width: 256-bit registers process 8 floats at once.
 *     Naming: _mm256_<operation>_<type>
 *
 *     Important difference from SSE:
 *     - AVX registers (YMM) are split into two 128-bit "lanes"
 *     - Some operations don't cross lane boundaries (shuffle, permute)
 *     - This is a common source of bugs!
 *
 * TR: AVX, SSE'nin genişliğini ikiye katlar: 256-bit yazmaçlar aynı anda 8 float işler.
 *     Önemli fark: AVX yazmaçları iki 128-bit "şerit"e bölünmüştür.
 */
void avx_add_example() {
    // EN: alignas(32) for AVX's 256-bit = 32-byte alignment requirement
    // TR: AVX'in 256-bit = 32-bayt hizalama gereksinimi için alignas(32)
    alignas(32) float a[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    alignas(32) float b[8] = {10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f};
    alignas(32) float c[8];

    __m256 va = _mm256_load_ps(a);   // EN: Load 8 floats into 256-bit YMM register
    __m256 vb = _mm256_load_ps(b);
    __m256 vc = _mm256_add_ps(va, vb);  // EN: 8 additions in one instruction!
    _mm256_store_ps(c, vc);

    std::cout << "  AVX _mm256_add_ps: [";
    for (int i = 0; i < 8; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << c[i];
    }
    std::cout << "]\n";
}

/**
 * EN: AVX integer operations (AVX2 required).
 *     Processes 8 × 32-bit integers simultaneously.
 *
 * TR: AVX tam sayı işlemleri (AVX2 gerektirir).
 *     Aynı anda 8 × 32-bit tam sayı işler.
 */
void avx2_integer_example() {
    alignas(32) int32_t a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    alignas(32) int32_t b[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    alignas(32) int32_t c[8];

    __m256i va = _mm256_load_si256(reinterpret_cast<const __m256i*>(a));
    __m256i vb = _mm256_load_si256(reinterpret_cast<const __m256i*>(b));
    __m256i vc = _mm256_add_epi32(va, vb);  // EN: 8 integer adds at once
    _mm256_store_si256(reinterpret_cast<__m256i*>(c), vc);

    std::cout << "  AVX2 _mm256_add_epi32: [";
    for (int i = 0; i < 8; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << c[i];
    }
    std::cout << "]\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Benchmark — Scalar vs SIMD
// ═════════════════════════════════════════════════════════════════════════════

static constexpr size_t BENCH_SIZE = 1024 * 1024;  // EN: 1M elements / TR: 1M eleman

/**
 * EN: Scalar (non-SIMD) vector addition — baseline for comparison.
 *     The compiler MAY auto-vectorize this with -O2, so we use
 *     -fno-tree-vectorize in the benchmark to see the true scalar speed.
 *     (Or compare with -O2 to see how well the compiler auto-vectorizes.)
 *
 * TR: Skaler (SIMD olmayan) vektör toplama — karşılaştırma temeli.
 */
void __attribute__((noinline)) scalar_add(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

/**
 * EN: AVX-vectorized addition — 8 floats per iteration.
 *     Processes 8 elements per loop iteration (256 bits / 32 bits per float).
 *     Handles tail elements (remainder after 8-element chunks) with scalar code.
 *
 * TR: AVX ile vektörleştirilmiş toplama — yineleme başına 8 float.
 *     Kalan elemanlar (8'lik gruplardan arta kalanlar) skaler kodla işlenir.
 */
void __attribute__((noinline)) avx_add(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    // EN: Main loop: process 8 floats per iteration
    // TR: Ana döngü: her yinelemede 8 float işle
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);  // EN: loadu = unaligned load (safe for any address)
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);
    }
    // EN: Handle remaining elements that don't fill a full 8-element chunk
    // TR: Tam 8 elemanlık grubu doldurmayan kalan elemanları işle
    for (; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

struct BenchResult {
    double scalar_ms;
    double simd_ms;
    double speedup;
};

/**
 * EN: Benchmark function that runs both versions multiple times and reports average.
 * TR: Her iki sürümü birden çok kez çalıştırıp ortalamayı raporlayan kıyaslama fonksiyonu.
 */
BenchResult run_benchmark() {
    // EN: Allocate aligned arrays on heap using aligned_alloc
    //     aligned_alloc(alignment, size) — size must be multiple of alignment
    // TR: aligned_alloc ile yığında hizalanmış diziler ayır
    constexpr size_t align = 32;
    constexpr size_t bytes = BENCH_SIZE * sizeof(float);

    float* a = static_cast<float*>(std::aligned_alloc(align, bytes));
    float* b = static_cast<float*>(std::aligned_alloc(align, bytes));
    float* c = static_cast<float*>(std::aligned_alloc(align, bytes));

    // EN: Initialize with some data / TR: Verilerle başlat
    for (size_t i = 0; i < BENCH_SIZE; ++i) {
        a[i] = static_cast<float>(i) * 0.5f;
        b[i] = static_cast<float>(i) * 0.25f;
    }

    constexpr int ITERATIONS = 100;

    // EN: Benchmark scalar version
    // TR: Skaler sürümü benchmark'la
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        scalar_add(a, b, c, BENCH_SIZE);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    double scalar_ms = std::chrono::duration<double, std::milli>(t2 - t1).count() / ITERATIONS;

    // EN: Quick correctness check / TR: Hızlı doğruluk kontrolü
    float expected_last = static_cast<float>(BENCH_SIZE - 1) * 0.75f;
    float got_scalar = c[BENCH_SIZE - 1];

    // EN: Benchmark SIMD version
    // TR: SIMD sürümü benchmark'la
    t1 = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        avx_add(a, b, c, BENCH_SIZE);
    }
    t2 = std::chrono::high_resolution_clock::now();
    double simd_ms = std::chrono::duration<double, std::milli>(t2 - t1).count() / ITERATIONS;

    float got_simd = c[BENCH_SIZE - 1];

    std::free(a);
    std::free(b);
    std::free(c);

    // EN: Verify both produce same result / TR: İkisinin aynı sonucu ürettiğini doğrula
    if (std::abs(got_scalar - expected_last) > 0.01f || std::abs(got_simd - expected_last) > 0.01f) {
        std::cout << "  WARNING: Correctness check failed!\n";
    }

    return {scalar_ms, simd_ms, scalar_ms / simd_ms};
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Auto-Vectorization Tips
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Patterns that help auto-vectorization:
 *
 *     ✅ GOOD (vectorizer-friendly):
 *     - Simple counted loops: for (int i=0; i<n; ++i)
 *     - Contiguous memory access: a[i], *(ptr + i)
 *     - No cross-iteration dependencies
 *     - Use __restrict__ to promise no aliasing
 *     - Use -O2 -march=native -ftree-vectorize
 *
 *     ❌ BAD (prevents auto-vectorization):
 *     - Pointer aliasing: c[i] = a[i] + b[i] where c could overlap a or b
 *     - Function calls inside loop (unless simple inline functions)
 *     - Data-dependent branches: if (a[i] > 0) c[i] = a[i];
 *     - Cross-iteration dependencies: a[i] = a[i-1] + 1
 *     - Complex control flow in loop body
 *
 * TR: Oto-vektörleştirmeye yardımcı kalıplar:
 *     ✅ İYİ: Basit sayılı döngüler, bitişik bellek erişimi, bağımlılık yok
 *     ❌ KÖTÜ: İşaretçi örtüşmesi, döngü içi fonksiyon çağrısı, veri bağımlı dallar
 */
void __attribute__((noinline)) autovec_friendly(
    float* __restrict__ c,
    const float* __restrict__ a,
    const float* __restrict__ b,
    size_t n)
{
    // EN: `__restrict__` tells the compiler: a, b, c point to non-overlapping memory
    //     This is the C++ equivalent of C99's `restrict` keyword
    //     Without it, compiler must assume c[i] might alias a[j] or b[j]
    // TR: `__restrict__` derleyiciye söyler: a, b, c örtüşmeyen belleğe işaret eder
    for (size_t i = 0; i < n; ++i) {
        c[i] = a[i] * b[i] + a[i];  // EN: Should auto-vectorize with -O2
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 07 - SIMD: Vectorized Computing\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: SSE Basics ──────────────────────────────────────────────
    {
        std::cout << "--- Demo 1: SSE (128-bit) Operations ---\n";
        sse_add_example();
        sse_operations_showcase();
        std::cout << "\n";
    }

    // ─── Demo 2: AVX Basics ──────────────────────────────────────────────
    {
        std::cout << "--- Demo 2: AVX/AVX2 (256-bit) Operations ---\n";
        avx_add_example();
        avx2_integer_example();
        std::cout << "\n";
    }

    // ─── Demo 3: Benchmark ───────────────────────────────────────────────
    {
        std::cout << "--- Demo 3: Benchmark — Scalar vs AVX (1M floats) ---\n";
        auto result = run_benchmark();
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "  Scalar add: " << result.scalar_ms << " ms\n";
        std::cout << "  AVX add:    " << result.simd_ms << " ms\n";
        std::cout << "  Speedup:    " << result.speedup << "x\n";
        std::cout << "  (Theoretical max for AVX: 8x, real-world limited by memory bandwidth)\n\n";
    }

    // ─── Demo 4: Alignment Importance ────────────────────────────────────
    {
        std::cout << "--- Demo 4: Memory Alignment ---\n";

        alignas(32) float aligned_arr[8];
        float unaligned_arr[8];  // EN: May not be 32-byte aligned / TR: 32-bayt hizalı olmayabilir

        std::cout << "  alignas(32) array address: " << static_cast<void*>(aligned_arr)
                  << " (aligned: " << (reinterpret_cast<uintptr_t>(aligned_arr) % 32 == 0 ? "YES" : "NO") << ")\n";
        std::cout << "  regular array address:     " << static_cast<void*>(unaligned_arr)
                  << " (32-byte aligned: " << (reinterpret_cast<uintptr_t>(unaligned_arr) % 32 == 0 ? "YES" : "NO") << ")\n";

        std::cout << "\n  Register sizes:\n";
        std::cout << "    __m128 (SSE):  " << sizeof(__m128) << " bytes = " << sizeof(__m128) * 8 << " bits\n";
        std::cout << "    __m256 (AVX):  " << sizeof(__m256) << " bytes = " << sizeof(__m256) * 8 << " bits\n";
        std::cout << "    __m256i (AVX2): " << sizeof(__m256i) << " bytes = " << sizeof(__m256i) * 8 << " bits\n\n";
    }

    // ─── Demo 5: Auto-Vectorization Report ───────────────────────────────
    {
        std::cout << "--- Demo 5: Auto-Vectorization Tips ---\n";
        std::cout << "  Compile flags to enable auto-vectorization:\n";
        std::cout << "    g++ -O2 -march=native -ftree-vectorize\n\n";
        std::cout << "  Diagnostic flags to SEE what was vectorized:\n";
        std::cout << "    g++ -fopt-info-vec-optimized     (successful)\n";
        std::cout << "    g++ -fopt-info-vec-missed        (failures + reasons)\n";
        std::cout << "    g++ -fopt-info-vec-all           (everything)\n\n";
        std::cout << "  Key rules for vectorizable loops:\n";
        std::cout << "    1. Use __restrict__ to prevent aliasing assumptions\n";
        std::cout << "    2. Keep loop body simple (no function calls if possible)\n";
        std::cout << "    3. Access memory contiguously (a[i], not a[i*stride])\n";
        std::cout << "    4. Avoid cross-iteration dependencies\n";
        std::cout << "    5. Use -march=native to target YOUR CPU's SIMD level\n\n";

        // EN: Quick demo of auto-vectorized function
        // TR: Oto-vektörleştirilmiş fonksiyonun hızlı demosu
        alignas(32) float a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
        alignas(32) float b[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        alignas(32) float c[8];
        autovec_friendly(c, a, b, 8);
        std::cout << "  autovec_friendly (a*b+a): [";
        for (int i = 0; i < 8; ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << c[i];
        }
        std::cout << "]\n\n";
    }

    // ─── Demo 6: Common Intrinsics Reference ─────────────────────────────
    {
        std::cout << "--- Demo 6: Common SIMD Intrinsics Reference ---\n";
        std::cout << "  ┌───────────────────────────┬──────────────────────────────────────┐\n";
        std::cout << "  │ Intrinsic                 │ Description                          │\n";
        std::cout << "  ├───────────────────────────┼──────────────────────────────────────┤\n";
        std::cout << "  │ _mm_load_ps(p)            │ Load 4 aligned floats (SSE)          │\n";
        std::cout << "  │ _mm_loadu_ps(p)           │ Load 4 unaligned floats (SSE)        │\n";
        std::cout << "  │ _mm256_load_ps(p)         │ Load 8 aligned floats (AVX)          │\n";
        std::cout << "  │ _mm_add_ps(a,b)           │ Add 4 floats (SSE)                   │\n";
        std::cout << "  │ _mm256_add_ps(a,b)        │ Add 8 floats (AVX)                   │\n";
        std::cout << "  │ _mm_mul_ps(a,b)           │ Multiply 4 floats (SSE)              │\n";
        std::cout << "  │ _mm_fmadd_ps(a,b,c)       │ Fused multiply-add: a*b+c (FMA)      │\n";
        std::cout << "  │ _mm_sqrt_ps(a)            │ Square root 4 floats (SSE)           │\n";
        std::cout << "  │ _mm256_setzero_ps()       │ Set all 8 floats to 0 (AVX)          │\n";
        std::cout << "  │ _mm256_set1_ps(x)         │ Broadcast x to all 8 slots (AVX)     │\n";
        std::cout << "  │ _mm_cmpgt_ps(a,b)         │ Compare greater, returns mask (SSE)  │\n";
        std::cout << "  │ _mm256_blend_ps(a,b,mask) │ Select elements by mask (AVX)        │\n";
        std::cout << "  └───────────────────────────┴──────────────────────────────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of SIMD Intrinsics\n";
    std::cout << "============================================\n";

    return 0;
}
