/**
 * @file module_07_under_the_hood/07_inline_assembly.cpp
 * @brief Inline Assembly: Doğrudan CPU ile Konuşma — Talking Directly to the CPU
 *
 * @details
 * =============================================================================
 * [THEORY: What Is Inline Assembly? / TEORİ: Inline Assembly Nedir?]
 * =============================================================================
 *
 * EN: Inline assembly lets you embed raw CPU instructions inside C++ code.
 *     GCC/Clang use the extended asm syntax:
 *
 *       asm volatile("instruction"
 *                    : output operands     // =r(var) → result goes to var
 *                    : input operands      // "r"(var) → var loaded into register
 *                    : clobber list);      // registers modified by the asm
 *
 *     Keywords:
 *     - `asm` or `__asm__`   → introduces assembly block
 *     - `volatile`           → tells compiler: "don't optimize this away!"
 *                               Without volatile, compiler may DELETE the asm
 *                               if it thinks the output is unused.
 *     - Constraints: "r" = any register, "m" = memory, "i" = immediate
 *
 * TR: Inline assembly, C++ kodu içine doğrudan CPU talimatları gömmenizi sağlar.
 *     GCC/Clang genişletilmiş asm sözdizimini kullanır:
 *
 *       asm volatile("talimat"
 *                    : çıkış operandlar    // =r(var) → sonuç var'a gider
 *                    : giriş operandlar    // "r"(var) → var yazmaça yüklenir
 *                    : bozulan liste);     // asm tarafından değiştirilen yazmaçlar
 *
 *     Anahtar sözcükler:
 *     - `asm` veya `__asm__`  → assembly bloğunu başlatır
 *     - `volatile`            → derleyiciye "bunu optimize etme!" der.
 *                                volatile olmadan derleyici çıkış kullanılmıyorsa
 *                                asm'yi SİLEBİLİR.
 *     - Kısıtlamalar: "r" = herhangi yazmaç, "m" = bellek, "i" = anlık değer
 *
 * =============================================================================
 * [THEORY: Why Usually AVOID Inline Assembly / TEORİ: Neden Genellikle KAÇINILMALI]
 * =============================================================================
 *
 * EN: Inline assembly should be a LAST RESORT. Reasons to avoid:
 *
 *     1. NON-PORTABLE: x86 asm won't work on ARM (automotive ECUs often use ARM!)
 *     2. OPTIMIZER BLIND: Compiler can't optimize across asm boundaries
 *     3. ERROR-PRONE: Wrong constraints → silent data corruption
 *     4. MAINTENANCE NIGHTMARE: Few developers can read/write assembly
 *     5. COMPILER IS USUALLY BETTER: Modern compilers (GCC -O2/O3) generate
 *        better machine code than most humans can write
 *
 *     Alternatives (prefer these):
 *     - Compiler intrinsics: __builtin_popcount, __builtin_clz, __builtin_expect
 *     - SIMD intrinsics: _mm256_add_ps (portable across compilers)
 *     - Compiler hints: [[likely]], [[unlikely]], __attribute__((hot/cold))
 *
 *     When inline asm IS justified:
 *     - Accessing CPU features with no intrinsic (CPUID, RDTSC, MSR registers)
 *     - Implementing OS kernel primitives (context switch, interrupt handling)
 *     - Hardware-specific device drivers
 *     - Cryptographic constant-time operations (prevent compiler optimization)
 *
 * TR: Inline assembly SON ÇARE olmalıdır. Kaçınılmasının nedenleri:
 *
 *     1. TAŞINABİLİR DEĞİL: x86 asm ARM'de çalışmaz (otomotiv ECU'ları genelde ARM!)
 *     2. OPTİMİZATÖR KÖR: Derleyici asm sınırları arasında optimize edemez
 *     3. HATAYA AÇIK: Yanlış kısıtlamalar → sessiz veri bozulması
 *     4. BAKIM KABUSSU: Az geliştirici assembly okuyup yazabilir
 *     5. DERLEYİCİ GENELDE DAHA İYİ: Modern derleyiciler (GCC -O2/O3) çoğu
 *        insandan daha iyi makine kodu üretir
 *
 *     Alternatifler (bunları tercih et):
 *     - Derleyici intrinsikleri: __builtin_popcount, __builtin_clz, __builtin_expect
 *     - SIMD intrinsikleri: _mm256_add_ps (derleyiciler arası taşınabilir)
 *     - Derleyici ipuçları: [[likely]], [[unlikely]], __attribute__((hot/cold))
 *
 *     Inline asm'in HAKLI olduğu durumlar:
 *     - İntrinsik olmayan CPU özelliklerine erişim (CPUID, RDTSC, MSR yazmaçları)
 *     - OS kernel ilkellerini uygulama (bağlam değiştirme, kesme yönetimi)
 *     - Donanıma özel aygıt sürücüleri
 *     - Kriptografik sabit-zamanlı işlemler (derleyici optimizasyonunu önleme)
 *
 * =============================================================================
 * [THEORY: x86-64 Register Quick Reference / TEORİ: x86-64 Yazmaç Hızlı Başvuru]
 * =============================================================================
 *
 * EN: x86-64 has 16 general-purpose 64-bit registers:
 *
 *     ┌─────────┬───────────────────────────────────────────────────────────┐
 *     │ Register│ Convention                                                │
 *     ├─────────┼───────────────────────────────────────────────────────────┤
 *     │ RAX     │ Return value, accumulator                                 │
 *     │ RBX     │ Callee-saved (preserved across calls)                     │
 *     │ RCX     │ 4th argument (Windows) / counter                          │
 *     │ RDX     │ 3rd argument (Linux) / high part of multiply              │
 *     │ RSI     │ 2nd argument (Linux) / source index                       │
 *     │ RDI     │ 1st argument (Linux) / destination index                  │
 *     │ RSP     │ Stack pointer (DO NOT touch in inline asm!)               │
 *     │ RBP     │ Base/frame pointer                                        │
 *     │ R8-R15  │ Additional registers (R8-R11 caller-saved, R12-R15 callee)│
 *     └─────────┴───────────────────────────────────────────────────────────┘
 *
 *     Sub-registers: RAX(64) → EAX(32) → AX(16) → AL(8)/AH(8)
 *
 * TR: x86-64'te 16 genel amaçlı 64-bit yazmaç vardır:
 *
 *     ┌─────────┬────────────────────────────────────────────────────────────┐
 *     │ Yazmaç  │ Gelenek                                                    │
 *     ├─────────┼────────────────────────────────────────────────────────────┤
 *     │ RAX     │ Dönüş değeri, toplam yazmaç                                │
 *     │ RBX     │ Callee-saved (çağrılar boyunca korunur)                    │
 *     │ RCX     │ 4. argüman (Windows) / sayaç                               │
 *     │ RDX     │ 3. argüman (Linux) / çarpmanın üst kısmı                   │
 *     │ RSI     │ 2. argüman (Linux) / kaynak indeksi                        │
 *     │ RDI     │ 1. argüman (Linux) / hedef indeksi                         │
 *     │ RSP     │ Yığın işaretçisi (inline asm'de DOKUNMA!)                  │
 *     │ RBP     │ Taban/çerçeve işaretçisi                                   │
 *     │ R8-R15  │ Ek yazmaçlar (R8-R11 caller-saved, R12-R15 callee-saved)   │
 *     └─────────┴────────────────────────────────────────────────────────────┘
 *
 *     Alt-yazmaçlar: RAX(64) → EAX(32) → AX(16) → AL(8)/AH(8)
 *
 * @note This file uses x86-64 inline assembly. It will NOT compile on ARM/RISC-V.
 *       Bu dosya x86-64 inline assembly kullanır. ARM/RISC-V'de DERLENMEZ.
 *
 * @see https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
 * @see https://www.felixcloutier.com/x86/ — x86 instruction set reference
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 07_inline_assembly.cpp -o 07_inline_assembly
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <string>
#include <array>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Basic Inline Assembly — Arithmetic
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: The simplest possible inline asm: add two integers.
 *
 *     Breakdown of: asm volatile("addl %2, %0" : "=r"(result) : "0"(a), "r"(b));
 *
 *     "addl %2, %0"    → AT&T syntax: addl SOURCE, DEST  (l = 32-bit long)
 *     : "=r"(result)   → OUTPUT: result is written ("=") to any register ("r")
 *     : "0"(a)         → INPUT:  load 'a' into the SAME register as operand 0 (result)
 *       "r"(b)         → INPUT:  load 'b' into any register
 *     (no clobbers)    → We didn't modify any registers beyond the output
 *
 *     %0, %1, %2 refer to operands in order: result, a, b
 *
 * TR: Mümkün olan en basit inline asm: iki sayıyı topla.
 *     "addl %2, %0"    → AT&T sözdizimi: addl KAYNAK, HEDEF
 *     "=r"(result)     → ÇIKIŞ: result bir yazmaca yazılır
 *     "0"(a)           → GİRİŞ: 'a'yı operand 0 ile aynı yazmaca yükle
 *     "r"(b)           → GİRİŞ: 'b'yi herhangi yazmaca yükle
 */
int asm_add(int a, int b) {
    int result;
    asm volatile(
        "addl %2, %0"       // EN: addl = add 32-bit integers / TR: addl = 32-bit tam sayıları topla
        : "=r"(result)       // EN: Output constraint / TR: Çıkış kısıtlaması
        : "0"(a), "r"(b)    // EN: Input constraints / TR: Giriş kısıtlamaları
    );
    return result;
}

/**
 * EN: Multiply using x86 MUL instruction.
 *     MUL is special: it always uses RAX as one operand and puts the result in RDX:RAX.
 *     For 32-bit: EAX * operand → EDX:EAX (64-bit result)
 *
 * TR: x86 MUL talimatı ile çarpma.
 *     MUL özeldir: her zaman RAX'ı operand olarak kullanır, sonucu RDX:RAX'a koyar.
 */
uint64_t asm_multiply(uint32_t a, uint32_t b) {
    uint32_t low, high;
    asm volatile(
        "mull %2"                // EN: EAX * %2 → EDX:EAX / TR: EAX * %2 → EDX:EAX
        : "=a"(low), "=d"(high) // EN: "a" = EAX register, "d" = EDX register
        : "r"(b), "0"(a)        // EN: b in any reg, a loaded into EAX (operand 0)
    );
    return (static_cast<uint64_t>(high) << 32) | low;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: RDTSC — Reading the CPU Cycle Counter
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: RDTSC (Read Time-Stamp Counter) reads the CPU's cycle counter.
 *     This is a 64-bit counter that increments with every CPU clock cycle.
 *     It's useful for micro-benchmarking (measuring exact cycle counts).
 *
 *     The instruction returns: EDX:EAX (high 32 bits : low 32 bits)
 *
 *     WARNING: RDTSC can be reordered by the CPU! For precise measurement:
 *     - Use RDTSCP (serializing version) or
 *     - Use CPUID before RDTSC (acts as a barrier)
 *     - Or use std::chrono::high_resolution_clock (portable!)
 *
 * TR: RDTSC (Read Time-Stamp Counter) CPU'nun döngü sayacını okur.
 *     Her CPU saat döngüsünde artan 64-bit'lik bir sayaçtır.
 *     Mikro-kıyaslama (hassas döngü sayısı ölçme) için kullanışlıdır.
 *
 *     UYARI: RDTSC CPU tarafından yeniden sıralanabilir! Kesin ölçüm için:
 *     - RDTSCP (serileştiren sürüm) veya CPUID bariyeri kullanın
 *     - Veya std::chrono::high_resolution_clock kullanın (taşınabilir!)
 */
uint64_t rdtsc() {
    uint32_t lo, hi;
    asm volatile(
        "rdtsc"              // EN: Read Time-Stamp Counter → EDX:EAX
        : "=a"(lo), "=d"(hi) // EN: lo = EAX, hi = EDX
        :
        : "memory"           // EN: Clobber: tell compiler memory may have changed
                             // TR: Bozulan: derleyiciye bellek değişmiş olabilir de
    );
    return (static_cast<uint64_t>(hi) << 32) | lo;
}

/**
 * EN: RDTSCP is the serializing version of RDTSC.
 *     It guarantees all previous instructions have completed before reading.
 *     Also returns the processor ID in ECX (useful for multi-core profiling).
 *
 * TR: RDTSCP, RDTSC'nin serileştiren sürümüdür.
 *     Okumadan önce önceki tüm talimatların tamamlandığını garanti eder.
 */
uint64_t rdtscp() {
    uint32_t lo, hi, aux;
    asm volatile(
        "rdtscp"
        : "=a"(lo), "=d"(hi), "=c"(aux) // EN: EAX, EDX, ECX / TR: EAX, EDX, ECX
        :
        : "memory"
    );
    return (static_cast<uint64_t>(hi) << 32) | lo;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: CPUID — Querying CPU Features
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: CPUID is a special instruction that returns information about the CPU:
 *     - Brand string, vendor (Intel/AMD)
 *     - Feature flags (SSE, AVX, AES, etc.)
 *     - Cache sizes, core count
 *
 *     Input: EAX = leaf number (which info to query)
 *     Output: EAX, EBX, ECX, EDX = results
 *
 * TR: CPUID, CPU hakkında bilgi döndüren özel bir talimattır:
 *     - Marka, üretici (Intel/AMD)
 *     - Özellik bayrakları (SSE, AVX, AES vb.)
 *     - Önbellek boyutları, çekirdek sayısı
 */
struct CpuidResult {
    uint32_t eax, ebx, ecx, edx;
};

CpuidResult cpuid(uint32_t leaf, uint32_t subleaf = 0) {
    CpuidResult r;
    asm volatile(
        "cpuid"
        : "=a"(r.eax), "=b"(r.ebx), "=c"(r.ecx), "=d"(r.edx)
        : "a"(leaf), "c"(subleaf)
        // EN: CPUID clobbers many things, but we listed all outputs, so no extra clobbers needed
        // TR: CPUID birçok şeyi bozar, ama tüm çıkışları listeledik, ekstra bozulan gerekmez
    );
    return r;
}

/**
 * EN: Get CPU vendor string (12 characters: "GenuineIntel" or "AuthenticAMD")
 *     CPUID leaf 0 returns: EBX:EDX:ECX as a 12-byte string
 *     Note: The order is EBX, EDX, ECX (not EBX, ECX, EDX!)
 *
 * TR: CPU üretici string'ini al (12 karakter: "GenuineIntel" veya "AuthenticAMD")
 *     CPUID yaprak 0: EBX:EDX:ECX olarak 12 baytlık string döndürür
 */
std::string get_cpu_vendor() {
    auto r = cpuid(0);
    char vendor[13];
    std::memcpy(vendor + 0, &r.ebx, 4);
    std::memcpy(vendor + 4, &r.edx, 4);  // EN: Yes, EDX before ECX! (x86 quirk)
    std::memcpy(vendor + 8, &r.ecx, 4);  // TR: Evet, ECX'ten önce EDX! (x86 tuhaflığı)
    vendor[12] = '\0';
    return std::string(vendor);
}

/**
 * EN: Get CPU brand string (48 characters, e.g., "Intel(R) Core(TM) i7-...")
 *     Requires CPUID leaves 0x80000002, 0x80000003, 0x80000004
 *     Each leaf returns 16 bytes (EAX+EBX+ECX+EDX), total 48 bytes.
 *
 * TR: CPU marka string'ini al (48 karakter, ör: "Intel(R) Core(TM) i7-...")
 *     CPUID yaprakları 0x80000002, 0x80000003, 0x80000004 gerektirir
 */
std::string get_cpu_brand() {
    char brand[49];
    for (uint32_t i = 0; i < 3; ++i) {
        auto r = cpuid(0x80000002 + i);
        std::memcpy(brand + i * 16 + 0, &r.eax, 4);
        std::memcpy(brand + i * 16 + 4, &r.ebx, 4);
        std::memcpy(brand + i * 16 + 8, &r.ecx, 4);
        std::memcpy(brand + i * 16 + 12, &r.edx, 4);
    }
    brand[48] = '\0';
    // EN: Trim leading spaces (Intel pads with spaces)
    // TR: Baştaki boşlukları kırp (Intel boşluklarla doldurur)
    std::string result(brand);
    size_t start = result.find_first_not_of(' ');
    return (start != std::string::npos) ? result.substr(start) : result;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Compiler Intrinsics — The PREFERRED Alternative
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Compiler intrinsics are functions that map directly to CPU instructions
 *     but are understood by the compiler (so it can optimize around them).
 *     They are cross-platform within GCC/Clang family and MUCH safer than asm.
 *
 *     Common GCC/Clang intrinsics:
 *     ┌──────────────────────────┬────────────────────────────────────────────┐
 *     │ Intrinsic                │ What It Does                               │
 *     ├──────────────────────────┼────────────────────────────────────────────┤
 *     │ __builtin_popcount(x)    │ Count number of 1-bits in x                │
 *     │ __builtin_clz(x)         │ Count Leading Zeros (from MSB)             │
 *     │ __builtin_ctz(x)         │ Count Trailing Zeros (from LSB)            │
 *     │ __builtin_expect(x, v)   │ Branch prediction hint (x likely == v)     │
 *     │ __builtin_bswap32(x)     │ Byte-swap 32 bits (endian conversion)      │
 *     │ __builtin_prefetch(p)    │ Prefetch memory at address p into cache    │
 *     │ __builtin_unreachable()  │ Tell compiler this code path is impossible │
 *     └──────────────────────────┴────────────────────────────────────────────┘
 *
 * TR: Derleyici intrinsic'leri doğrudan CPU talimatlarına eşlenen ama derleyici
 *     tarafından anlaşılan fonksiyonlardır (böylece etraflarında optimize edebilir).
 *     asm'den ÇOK daha güvenli ve GCC/Clang ailesinde çapraz platformdur.
 */
void demo_intrinsics() {
    uint32_t x = 0b00101100;  // EN: Binary literal (C++14) / TR: İkili literali (C++14)

    std::cout << "  x = 0b00101100 (decimal " << x << ")\n";
    std::cout << "    __builtin_popcount(x)  = " << __builtin_popcount(x)
              << " (set bits: 3)\n";
    // EN: popcount counts the number of 1-bits → 00101100 has three 1s
    // TR: popcount, 1-bit sayısını sayar → 00101100'da üç tane 1 var

    std::cout << "    __builtin_clz(x)       = " << __builtin_clz(x)
              << " (leading zeros from bit 31)\n";
    // EN: CLZ = Count Leading Zeros from the most significant bit
    //     0b00000000_00000000_00000000_00101100 → 26 leading zeros
    // TR: CLZ = En anlamlı bitten itibaren öndeki sıfırları say

    std::cout << "    __builtin_ctz(x)       = " << __builtin_ctz(x)
              << " (trailing zeros from bit 0)\n";
    // EN: CTZ = Count Trailing Zeros from LSB → ...00101100 → 2 trailing zeros
    // TR: CTZ = LSB'den itibaren sondaki sıfırları say

    uint32_t y = 0xDEADBEEF;
    std::cout << "\n  y = 0xDEADBEEF\n";
    std::cout << "    __builtin_bswap32(y)   = 0x" << std::hex
              << __builtin_bswap32(y) << std::dec << "\n";
    // EN: bswap32 reverses byte order: DE-AD-BE-EF → EF-BE-AD-DE
    //     This is used for endian conversion (same as htonl/ntohl!)
    // TR: bswap32 bayt sırasını tersine çevirir: DE-AD-BE-EF → EF-BE-AD-DE

    // EN: Branch prediction hint
    // TR: Dal tahmini ipucu
    int value = 42;
    if (__builtin_expect(value == 42, 1)) {  // EN: We expect this is TRUE / TR: TRUE bekliyoruz
        std::cout << "\n  __builtin_expect: predicted correctly (value is 42)\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 07 - Inline Assembly & Intrinsics\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Basic Arithmetic via asm ────────────────────────────────
    {
        std::cout << "--- Demo 1: Inline Assembly Arithmetic ---\n";
        std::cout << "  asm_add(25, 17)      = " << asm_add(25, 17) << "\n";
        std::cout << "  asm_multiply(12, 34) = " << asm_multiply(12, 34) << "\n";
        // EN: For comparison — compiler generates the SAME instructions with plain C++:
        //     int result = a + b;  →  addl %esi, %eax (or similar)
        //     This shows asm isn't needed for basic math!
        // TR: Karşılaştırma için — derleyici düz C++ ile AYNI talimatları üretir:
        //     Bu, temel matematik için asm'ın gerekmediğini gösterir!
        std::cout << "  (Plain C++ would generate the same machine code)\n\n";
    }

    // ─── Demo 2: RDTSC Cycle Counter ────────────────────────────────────
    {
        std::cout << "--- Demo 2: RDTSC — CPU Cycle Counter ---\n";

        // EN: Measure how many CPU cycles a simple operation takes
        // TR: Basit bir işlemin kaç CPU döngüsü sürdüğünü ölç
        uint64_t start = rdtscp();
        volatile int sum = 0;  // EN: volatile prevents optimizer from removing the loop
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }
        uint64_t end = rdtscp();

        std::cout << "  Loop 0..999 sum:\n";
        std::cout << "    RDTSC start:  " << start << "\n";
        std::cout << "    RDTSC end:    " << end << "\n";
        std::cout << "    Cycles taken: " << (end - start) << "\n";
        std::cout << "    (~" << (end - start) / 1000 << " cycles per iteration)\n";

        // EN: Compare with std::chrono — MORE PORTABLE, uses the SAME counter
        // TR: std::chrono ile karşılaştır — DAHA TAŞINABİLİR, AYNI sayacı kullanır
        auto t1 = std::chrono::high_resolution_clock::now();
        volatile int sum2 = 0;
        for (int i = 0; i < 1000; ++i) {
            sum2 += i;
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        std::cout << "    std::chrono says: " << ns << " ns\n";
        std::cout << "    (Prefer std::chrono for portable benchmarks!)\n\n";
    }

    // ─── Demo 3: CPUID — CPU Information ─────────────────────────────────
    {
        std::cout << "--- Demo 3: CPUID — CPU Information ---\n";

        std::cout << "  CPU Vendor: " << get_cpu_vendor() << "\n";
        std::cout << "  CPU Brand:  " << get_cpu_brand() << "\n";

        // EN: Check feature flags (leaf 1, ECX and EDX)
        // TR: Özellik bayraklarını kontrol et (yaprak 1, ECX ve EDX)
        auto features = cpuid(1);

        // EN: Feature bit positions from Intel SDM Vol.2A, Table 3-10
        // TR: Intel SDM Vol.2A, Tablo 3-10'dan özellik bit pozisyonları
        bool has_sse2   = (features.edx >> 26) & 1;
        bool has_sse41  = (features.ecx >> 19) & 1;
        bool has_sse42  = (features.ecx >> 20) & 1;
        bool has_avx    = (features.ecx >> 28) & 1;
        bool has_aes    = (features.ecx >> 25) & 1;
        bool has_popcnt = (features.ecx >> 23) & 1;

        std::cout << "  Features:\n";
        std::cout << "    SSE2:   " << (has_sse2   ? "YES" : "NO") << "\n";
        std::cout << "    SSE4.1: " << (has_sse41  ? "YES" : "NO") << "\n";
        std::cout << "    SSE4.2: " << (has_sse42  ? "YES" : "NO") << "\n";
        std::cout << "    AVX:    " << (has_avx    ? "YES" : "NO") << "\n";
        std::cout << "    AES-NI: " << (has_aes    ? "YES" : "NO") << "\n";
        std::cout << "    POPCNT: " << (has_popcnt ? "YES" : "NO") << "\n";

        // EN: Check AVX2 (leaf 7, subleaf 0, EBX bit 5)
        // TR: AVX2 kontrol et (yaprak 7, alt yaprak 0, EBX bit 5)
        auto ext_features = cpuid(7, 0);
        bool has_avx2 = (ext_features.ebx >> 5) & 1;
        std::cout << "    AVX2:   " << (has_avx2 ? "YES" : "NO") << "\n\n";
    }

    // ─── Demo 4: Compiler Intrinsics ─────────────────────────────────────
    {
        std::cout << "--- Demo 4: Compiler Intrinsics (Preferred Alternative) ---\n";
        demo_intrinsics();
        std::cout << "\n";
    }

    // ─── Demo 5: Constraint Types Summary ────────────────────────────────
    {
        std::cout << "--- Demo 5: Assembly Constraint Reference ---\n";
        std::cout << "  ┌──────────────┬───────────────────────────────────────────┐\n";
        std::cout << "  │ Constraint   │ Meaning                                   │\n";
        std::cout << "  ├──────────────┼───────────────────────────────────────────┤\n";
        std::cout << "  │ \"r\"        │ Any general-purpose register              │\n";
        std::cout << "  │ \"a\"        │ EAX/RAX register specifically             │\n";
        std::cout << "  │ \"b\"        │ EBX/RBX register specifically             │\n";
        std::cout << "  │ \"c\"        │ ECX/RCX register specifically             │\n";
        std::cout << "  │ \"d\"        │ EDX/RDX register specifically             │\n";
        std::cout << "  │ \"m\"        │ Memory operand (address)                  │\n";
        std::cout << "  │ \"i\"        │ Immediate integer constant                │\n";
        std::cout << "  │ \"0\"..\"9\" │ Same register as operand N                │\n";
        std::cout << "  │ \"=\"        │ Output-only (write)                       │\n";
        std::cout << "  │ \"+\"        │ Read-write (both input and output)        │\n";
        std::cout << "  │ \"memory\"   │ Clobber: asm may read/write any memory    │\n";
        std::cout << "  │ \"cc\"       │ Clobber: asm modifies condition flags     │\n";
        std::cout << "  └──────────────┴───────────────────────────────────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Inline Assembly & Intrinsics\n";
    std::cout << "============================================\n";

    return 0;
}
