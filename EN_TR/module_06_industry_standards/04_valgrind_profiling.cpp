/**
 * @file module_06_industry_standards/04_valgrind_profiling.cpp
 * @brief Industry Standards: Memory Profiling and Valgrind — Profiling ve Valgrind ile Bellek
 * Denetimi
 *
 * @details
 * =============================================================================
 * [THEORY: Why Use Valgrind and Profiling? / TEORİ: Neden Valgrind Kullanılır?]
 * =============================================================================
 * EN: Writing code that compiles without errors is NOT enough. If your program consumes 1
 * Megabyte of RAM extra every hour, it will crash your server after a week (OOM - Out of
 * Memory). 'Valgrind' is a magical Linux tool. You run your program "inside" Valgrind, and it
 * tracks EVERY SINGLE BYTE of memory you allocated and checks if you freed it.
 *
 * TR: Hata vermeden (0 error) derlenen bir kod yazmak YETERLİ DEĞİLDİR! C++ programınız her saat
 * fazladan 1 MB RAM tüketirse (Memory Leak - Bellek Sızıntısı), o program 1 hafta sonra sunucuyu
 * patlatır (Out of Memory - OOM).
 *
 * Linux tarafında (Kurumsal firmalarda %99 Unix tabanlı iş yapılır) "Valgrind" isimli inanilmaz
 * bir dedektif programı vardır. C++ executable (çalıştırılabilir) dosyanızı normal `./main` diye
 * çalıştırmak yerine `valgrind ./main` diye çalıştırırsınız. Valgrind, programınızın etrafını
 * sarı şeritlerle çevirir ve istediğiniz HER BİR BAYT belleği not alır. Program kapanırken "Sen
 * işletim sisteminden şu kadar bellek aldın ama şu kadarını geri iade etmemişsin, RAM'in kanayan
 * yeri şurasıdır!" der.
 *
 * =============================================================================
 * [HOW TO APPLY? / NASIL UYGULANIR?]
 * =============================================================================
 * EN: Add the "-g" flag while compiling to inject line numbers into the binary.
 * TR: Derlerken (Compile) kod satır numaralarını içine gömmek için "-g" bayrağı eklenir:
 *
 * 1. Compile: `g++ -g -o leak_test 04_valgrind_profiling.cpp`
 * 2. Execute: `valgrind --leak-check=full ./leak_test`
 *
 * [CPPREF DEPTH: Sanitizer Internals — How ASan, TSan, UBSan Actually Work / CPPREF DERİNLİK:
 * Sanitizer İç Yapısı — ASan, TSan, UBSan Nasıl Çalışır?]
 * =============================================================================
 * EN: ASan (AddressSanitizer) uses shadow memory with a 1:8 mapping — every 8 bytes of
 * application memory have 1 byte of shadow tracking whether that region is fully accessible,
 * partially accessible, or poisoned. Typical slowdown is ~2x with ~3x memory overhead. TSan
 * (ThreadSanitizer) uses happens-before analysis backed by vector clocks to detect data races;
 * it incurs ~5-15x slowdown and 5-10x memory overhead. UBSan (UndefinedBehaviorSanitizer)
 * instruments arithmetic and type operations at compile time (signed overflow, null dereference,
 * misaligned access) with minimal runtime overhead. ASan and TSan CANNOT be combined in the same
 * binary — they use incompatible shadow memory layouts. MSan (MemorySanitizer) detects reads of
 * uninitialized memory; also incompatible with ASan. In CI pipelines, run separate builds for
 * each sanitizer: one ASan+UBSan build and one TSan build.
 *
 * TR: ASan (AddressSanitizer) 1:8 eşlemeli gölge bellek kullanır — uygulama belleğinin her 8
 * baytı için 1 bayt gölge, o bölgenin tamamen erişilebilir, kısmen erişilebilir veya zehirlenmiş
 * olduğunu izler. Tipik yavaşlama ~2x, bellek ek yükü ~3x'tir. TSan (ThreadSanitizer) veri
 * yarışlarını tespit etmek için vektör saatleriyle desteklenen happens-before analizi kullanır;
 * ~5-15x yavaşlama ve 5-10x bellek ek yükü getirir. UBSan (UndefinedBehavior Sanitizer)
 * aritmetik ve tür işlemlerini derleme zamanında enstrümante eder (işaretli taşma, null
 * referans, hizasız erişim) — minimal çalışma zamanı ek yükü vardır. ASan ve TSan aynı binary'de
 * BİRLEŞTİRİLEMEZ — uyumsuz gölge bellek düzenleri kullanırlar. MSan (MemorySanitizer)
 * başlatılmamış bellek okumalarını tespit eder; o da ASan ile uyumsuzdur. CI pipeline'larında
 * her sanitizer için ayrı derleme çalıştırın: bir ASan+UBSan, bir TSan.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_valgrind_profiling.cpp -o 04_valgrind_profiling
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: AddressSanitizer (ASan) — Demonstrates what ASan targets.
// TR: AddressSanitizer (ASan) — ASan'ın tespit ettiği hataları gösterir.
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: ASan is a compile-time instrumentation flag: -fsanitize=address. It detects
// heap-buffer-overflow, stack-buffer-overflow, use-after-free, and memory leaks at ~2x runtime
// cost (vs Valgrind's ~20x slowdown). Compile: g++ -g -fsanitize=address -o asan_test file.cpp
// TR: ASan, derleme zamanı enstrümantasyon bayrağıdır: -fsanitize=address. Heap tampon taşması,
// stack tampon taşması, serbest bırakılmış bellek erişimi ve bellek sızıntılarını ~2x çalışma
// maliyetiyle tespit eder (Valgrind'in ~20x yavaşlamasına karşılık). Derleme: g++ -g
// -fsanitize=address -o asan_test dosya.cpp
void demonstrateASanTargets() {
  std::cout << "\n--- ASan Target Patterns (Commented — triggering would crash!) ---\n";

  // ─── Heap-Buffer-Overflow ──────────────────────────────────────────────────────────────────────
  // EN: Writing past allocated heap memory. ASan catches this immediately.
  // TR: Tahsis edilen heap belleğinin ötesine yazma. ASan bunu anında yakalar. DANGEROUS — DO
  // NOT UNCOMMENT: int *buf = new int[10]; buf[10] = 0xDEAD;  // heap-buffer-overflow! delete[]
  // buf;
  std::cout << "[ASan] Heap-buffer-overflow: writing buf[10] on a 10-element array\n";

  // ─── Stack-Buffer-Overflow ─────────────────────────────────────────────────────────────────────
  // EN: Overrunning a local stack array. ASan instruments stack frames too.
  // TR: Yerel stack dizisini taşırma. ASan stack çerçevelerini de izler. DANGEROUS — DO NOT
  // UNCOMMENT: int stack_arr[5]; stack_arr[5] = 42;  // stack-buffer-overflow!
  std::cout << "[ASan] Stack-buffer-overflow: stack_arr[5] on a 5-element array\n";

  // ─── Use-After-Free ────────────────────────────────────────────────────────────────────────────
  // EN: Accessing memory after delete. Classic dangling pointer.
  // TR: Delete sonrası belleğe erişim. Klasik sarkan işaretçi. DANGEROUS — DO NOT UNCOMMENT: int
  // *p = new int(7); delete p; *p = 99;  // use-after-free!
  std::cout << "[ASan] Use-after-free: dereferencing deleted pointer\n";
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: ThreadSanitizer (TSan) — Data Race Detection
// TR: ThreadSanitizer (TSan) — Veri Yarışı Tespiti
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: TSan is enabled with -fsanitize=thread. It detects data races where two threads access the
// same memory without synchronization and at least one is a write. CANNOT be used simultaneously
// with ASan. Compile: g++ -g -fsanitize=thread -o tsan_test file.cpp
// TR: TSan, -fsanitize=thread ile etkinleştirilir. İki iş parçacığının senkronizasyon olmadan
// aynı belleğe eriştiği ve en az birinin yazma yaptığı veri yarışlarını tespit eder. ASan ile
// aynı anda KULLANILAMAZ. Derleme: g++ -g -fsanitize=thread -o tsan_test dosya.cpp
//
// EN: Example data race pattern (DO NOT run without TSan):
// TR: Örnek veri yarışı deseni (TSan olmadan ÇALIŞTIRMAYIN): int sharedCounter = 0; // Thread 1:
// sharedCounter++;  // WRITE — no lock! // Thread 2:  sharedCounter++;  // WRITE — no lock! =>
// DATA RACE
//
// EN: Fix: protect with std::mutex:
// TR: Çözüm: std::mutex ile koruma: std::mutex mtx; // Thread 1:  { std::lock_guard<std::mutex>
// lk(mtx); sharedCounter++; } // Thread 2:  { std::lock_guard<std::mutex> lk(mtx);
// sharedCounter++; }

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: UndefinedBehaviorSanitizer (UBSan) — Catches Silent Killers
// TR: UndefinedBehaviorSanitizer (UBSan) — Sessiz Katilleri Yakalar
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: UBSan is enabled with -fsanitize=undefined. It catches signed integer overflow, null
// pointer dereference, misaligned access, and more. These are "undefined behavior" — compiler
// may do ANYTHING. Compile: g++ -g -fsanitize=undefined -o ubsan_test file.cpp
// TR: UBSan, -fsanitize=undefined ile etkinleştirilir. İşaretli tamsayı taşması, null işaretçi
// referansı, hizalama hataları vb. yakalar. Bunlar "tanımsız davranış"tır — derleyici HERHANGİ
// bir şey yapabilir. Derleme: g++ -g -fsanitize=undefined -o ubsan_test dosya.cpp
int potentialSignedOverflow(int a, int b) {
  // EN: If a + b exceeds INT_MAX, this is undefined behavior. UBSan will report: "signed integer
  // overflow: X + Y".
  // TR: Eğer a + b, INT_MAX'ı aşarsa bu tanımsız davranıştır. UBSan şunu raporlar: "signed
  // integer overflow: X + Y".
  return a + b;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: RAII Approach — Valgrind Reports 0 Leaks
// TR: RAII Yaklaşımı — Valgrind 0 Sızıntı Raporlar
// ═════════════════════════════════════════════════════════════════════════════════════════════════
void properRAII_VectorApproach() {
  // EN: std::vector manages heap memory automatically. When the function returns, the vector
  // destructor frees all memory — zero leaks.
  // TR: std::vector heap belleği otomatik yönetir. Fonksiyon döndüğünde vector yıkıcısı tüm
  // belleği serbest bırakır — sıfır sızıntı.
  std::vector<int> sensorBuffer(1000000);
  sensorBuffer[0] = 42;
  std::cout << "[RAII Vector] Data loaded: " << sensorBuffer[0]
            << " — memory auto-freed on scope exit" << std::endl;
}

void properRAII_UniquePtrApproach() {
  // EN: std::unique_ptr owns the resource exclusively. Automatically calls delete[] when it goes
  // out of scope — impossible to leak.
  // TR: std::unique_ptr kaynağa münhasır sahiptir. Kapsam dışına çıkınca otomatik delete[]
  // çağırır — sızıntı imkansız.
  auto canBuffer = std::make_unique<int[]>(1000000);
  canBuffer[0] = 0xCAFE;
  std::cout << "[RAII unique_ptr] CAN frame loaded: 0x" << std::hex
            << canBuffer[0] << std::dec
            << " — unique_ptr frees on scope exit" << std::endl;
}

void theLeakyFunction() {
  // EN: Allocating 1 million integers (approx 4 MB of RAM).
  // TR: 1 Milyon tam sayı (yaklasık 4 MB RAM tüketimi) istiyoruz.
  int *databaseSimulation = new int[1000000];

  databaseSimulation[0] = 42;
  std::cout << "[Valgrind Test] Data loaded (Veri Yüklendi): " << databaseSimulation[0] <<
      std::endl;

  // EN: OH NO! We exit the function WITHOUT calling 'delete[]'! This is a disaster. When the
  // function ends, the pointer 'databaseSimulation' vanishes, but the 4MB memory it grabbed
  // remains occupied forever as a ZOMBIE.
  //
  // TR: AMAN TANRIM! DELETE YAPMADAN FONKSİYONDAN CIKIYORUZ! İşte burası bir felakettir. Zira
  // fonksiyon kapandığında 'databaseSimulation' isimli işaretçi YOK OLUR ama RAM'de ayırdığı o
  // devasa 4MB alan sahipsiz (Zombi) şekilde sistem yeniden başlayana kadar havada kalır!
  // (MEMORY LEAK).
}

int main() {
  std::cout << "=== MODULE 6: VALGRIND & MEMORY PROFILING ===\n" << std::endl;

  std::cout << "[SYSTEM] WARNING: Triggering 4MB Memory Leak intentionally...\n";
  std::cout << "[SYSTEM] DİKKAT: Bilerek 4MB Bellek sızıntısı tetikleniyor...\n";

  theLeakyFunction();

  std::cout << "\n[SYSTEM] Function ended. But 4 Megabytes of garbage remains in RAM." << std::endl;
  std::cout << "[SYSTEM] Fonksiyon bitti ama 4MB çöp kaldı. Bunu bulan "
               "dedektif Valgrind aracıdır!"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: How to fix this in Modern C++? NEVER use RAW POINTERS inside 'theLeakyFunction'. Use
  // `std::vector<int>` or `std::unique_ptr` instead! When they go out of scope, they instantly
  // wipe the 4MB data (RAII rules supreme).
  //
  // TR: Modern C++ ile Nasıl Düzeltilir? 'theLeakyFunction' içerisinde ASLA ÇIPLAK İŞARETÇİ
  // KULLANMA. `std::vector<int>` veya akıllı işaretçi kullan. Cünkü std::vector kapsam dışına
  // cıktığında içindeki o 4MB datayı saniyesinde sisteme iade eder! (RAII)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 2 — ASan Target Patterns
  // TR: Demo 2 — ASan Hedef Desenleri
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 2: AddressSanitizer (ASan) Targets ---" << std::endl;
  demonstrateASanTargets();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 3 — Proper RAII: Vector vs Raw Pointer
  // TR: Demo 3 — Doğru RAII: Vector vs Ham İşaretçi
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 3: RAII Vector Approach (0 Leaks Under Valgrind) ---" << std::endl;
  properRAII_VectorApproach();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 4 — unique_ptr Automatic Cleanup
  // TR: Demo 4 — unique_ptr Otomatik Temizlik
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 4: unique_ptr Automatic Cleanup ---" << std::endl;
  properRAII_UniquePtrApproach();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 5 — UBSan: Signed Overflow Example
  // TR: Demo 5 — UBSan: İşaretli Taşma Örneği
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 5: UBSan Signed Overflow ---" << std::endl;
  // EN: Safe call — no overflow here. Compile with -fsanitize=undefined to instrument and catch
  // overflows automatically.
  // TR: Güvenli çağrı — burada taşma yok. Taşmaları otomatik yakalamak için -fsanitize=undefined
  // ile derleyin.
  int safeResult = potentialSignedOverflow(100, 200);
  std::cout << "100 + 200 = " << safeResult << " (safe, no overflow)" << std::endl;
  std::cout << "Compile with -fsanitize=undefined to catch cases like INT_MAX + 1\n";

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 6 — Sanitizer & Profiling Compilation Flags Reference
  // TR: Demo 6 — Sanitizer & Profiling Derleme Bayrakları Referansı
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 6: Sanitizer Compilation Flags Reference ---" << std::endl;
  std::cout << R"(
  +----------+----------------------------------+--------------------------------------+
  | Tool     | Compile Flag                     | Detects                              |
  +----------+----------------------------------+--------------------------------------+
  | ASan     | -fsanitize=address               | Buffer overflow, use-after-free, leak|
  | TSan     | -fsanitize=thread                | Data races between threads           |
  | UBSan    | -fsanitize=undefined             | Signed overflow, null deref, align   |
  | MSan     | -fsanitize=memory (clang)        | Uninitialized memory reads           |
  +----------+----------------------------------+--------------------------------------+
  | Valgrind | valgrind --leak-check=full ./bin | All leaks, ~20x slower               |
  | Perf     | perf record ./bin; perf report   | CPU hotspots, cache misses           |
  | Callgrind| valgrind --tool=callgrind ./bin  | Function-level profiling             |
  +----------+----------------------------------+--------------------------------------+
)" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Practical Profiling Workflow (Automotive ECU Software)
  // TR: Pratik Profiling İş Akışı (Otomotiv ECU Yazılımı)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Step 1 — Compile with debug symbols: g++ -g -O0 -o ecu_test main.cpp
  // TR: Adım 1 — Debug sembolleriyle derle: g++ -g -O0 -o ecu_test main.cpp
  //
  // EN: Step 2 — Run under Valgrind Memcheck for full leak report:
  // TR: Adım 2 — Tam sızıntı raporu için Valgrind Memcheck altında çalıştır: valgrind
  // --leak-check=full --show-leak-kinds=all ./ecu_test
  //
  // EN: Step 3 — Compile with ASan for faster turnaround (~2x vs 20x):
  // TR: Adım 3 — Daha hızlı geri dönüş için ASan ile derle (~2x vs 20x): g++ -g
  // -fsanitize=address -o ecu_test main.cpp && ./ecu_test
  //
  // EN: Step 4 — Profile CPU hotspots with perf:
  // TR: Adım 4 — CPU darboğazlarını perf ile profilleyin: perf record -g ./ecu_test && perf
  // report
  //
  // EN: Step 5 — Function-level profiling with Callgrind:
  // TR: Adım 5 — Fonksiyon düzeyinde profil için Callgrind: valgrind --tool=callgrind ./ecu_test
  // kcachegrind callgrind.out.*   (GUI visualization)

  return 0;
}
