/**
 * @file module_08_core_guidelines/04_concurrency_and_performance.cpp
 * @brief C++ Core Guidelines: Concurrency & Performance Rules / Eşzamanlılık ve Performans
 * Kuralları
 *
 * @details
 * =============================================================================
 * [CP.20: Use RAII Locks, Never touch raw mutexes / Çıplak Mutex yasaktır, RAII kullan!]
 * =============================================================================
 * EN: "Never use plain lock() and unlock(). Use RAII." If you manually call `mtx.lock()` and
 * subsequently throw an exception before reaching `mtx.unlock()`, your underlying mutex NEVER
 * unlocks. Your server will FREEZE FOREVER (Deadlock). Always use `std::lock_guard` or
 * `std::scoped_lock` (C++17).
 *
 * TR: "ASLA çıplak mtx.lock() ve mtx.unlock() KULLANMAYIN." Eğer manuel `lock()` yaparsanız ve
 * hemen alt satırda bir Exception (Hata) fırlatılırsa kod `unlock()` kısmına asırlar geçse
 * varamaz. Tüm sunucunuz sonsuza dek kitlenir (Deadlock). Kilit mekanizmasını bir Scope (Kapsam)
 * içine kilitleyen `std::lock_guard` kullanın.
 *
 * =============================================================================
 * [Per.1 & Per.2: Premature Optimization / Erken Optimizasyon (Şeytanın İşi)]
 * =============================================================================
 * EN: "Don't optimize without reason." & "Don't optimize prematurely." C++ Programmers love
 * doing bitwise magic or raw pointer arithmetic to save 2 nanoseconds. 99% of the time, the
 * compiler's optimizer flag (`-O3`) does it much better and safer. First, write CLEAN,
 * INTENTIONAL, READABLE CODE. Second, use a PROFILER (like Valgrind/Callgrind). Third, ONLY
 * optimize the bottleneck!
 *
 * TR: "Sebepsiz yere optimizasyon yapma!" ve "Erken optimizasyon bütün kötülüklerin anasıdır."
 * C++ Mimarisi: 3 nanosaniye kazanmak için okunması imkansız Bitwise kodlar YAZMAYIN! * Halbuki
 * `-O3` (Optimizasyon Bayrağı) ile çalışan C++ Derleyicisi sizden 100 kat akıllıdır. Önce TEMİZ,
 * NİYETİ BELLİ OLAN KOD yaz (STL kullan). Sonra Valgrind Profiler ile ölç. Ve SADECE Oyunun
 * Cidden Kasma Yaptığı yerleri (Bottleneck) optimize et!
 *
 * =============================================================================
 * [CPPREF DEPTH: Hardware Concurrency limit & Cache Coherence! / CPPREF DERİNLİK: Donanım
 * Eşzamanlılık Sınırı ve Önbellek Tutarlılığı!]
 * =============================================================================
 * EN: CppReference warns against creating 1000 threads for performance. Use
 * `std::thread::hardware_concurrency()` to find the exact number of physical/ logical cores you
 * have. Spawning more threads than CPU cores causes extreme lag. Also, multiple threads
 * modifying variables physically close together in RAM causes "False Sharing" (L1/L2 Cache
 * Coherence ping-pong), drastically hurting performance!
 *
 * TR: CppReference'in dev uyarısı: Performans için "1000 tane Thread açayım" derseniz sistem
 * çöker. Gerçek fiziksel ve mantıksal çekirdek sayısını öğrenmek için
 * `std::thread::hardware_concurrency()` kullanılmalıdır (Maksimum thread limiti). İkinci
 * Felaket: Birden çok Thread, RAM'de birbirine çok yakın duran iki farklı değişkene sürekli
 * yazıp bozarsa, İşlemci Ön Bellekleri (L1/L2 Cache) çıldırır. Sürekli birbirlerinin verisini
 * geçersiz kılarlar ("False Sharing" / Cache Coherence). Performans 10 kat düşer!
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex databaseLock;
int globalOnlineECUs = 0;

void registerECU_Bad() {
  // EN: CP.20 VIOLATION: Manual Lock/Unlock. VERY DANGEROUS!
  // TR: CORE GUIDELINE İHLALİ: Manuel Kilit. EĞER ARADA HATA ÇIKARSA SİSTEM ÇÖKER!
  databaseLock.lock();

  globalOnlineECUs++;
  // If a 'throw' happens exactly here, unlock() is silently skipped -> FATAL DEADLOCK!

  databaseLock.unlock();
}

void registerECU_Good() {
  // EN: C++ CORE GUIDELINES (CP.20): Use RAII (std::lock_guard explicitly)
  // TR: MÜKEMMEL KOD (RAII): Kapsama girince Mutex Kilitlenir, süslü parantez kapanınca Otomatik
  // Açılır!
  std::lock_guard<std::mutex> lock(databaseLock); // Automatic Safe Locker!

  globalOnlineECUs++;
  // EN: Even if an Exception throws here, RAII strictly un-winds the lock back to OS. No
  // deadlocks. TR: Burada Hata/Exception atılsa dahi C++ sistemi kilidi garantili olarak İşletim
  // sistemine iade eder.
}

int main() {
  std::cout << "=== MODULE 8: C++ CORE GUIDELINES - CONCURRENCY & PERFORMANCE ===\n" << std::endl;

  std::vector<std::thread> workers;

  // EN: Launch 10 Threads simultaneously to register ECU modules.
  // TR: 10 Thread Aynı Anda ECU Kayıt (Simülasyon). Race Condition'ı RAII Lock Guard ile
  // engelliyoruz.
  for (int i = 0; i < 10; ++i) {
    workers.push_back(std::thread(registerECU_Good));
  }

  // EN: Join all threads (Wait for them to finish properly).
  // TR: Tüm çalışma grupları birleşene (Bitiş) kadar ana sunucuyu bekle.
  for (auto &w : workers) {
    w.join();
  }

  std::cout << "All ECUs Registered Safely! Total Online: " << globalOnlineECUs << std::endl;

  std::cout << "\n[ARCHITECTURAL CONCLUSION / MİMARİ SONUÇ]:" << std::endl;
  std::cout << "1. Threads + Shared Global Data = CATASTROPHE. Always use "
               "std::lock_guard / std::scoped_lock."
            << std::endl;
  std::cout << "2. PERFORMANCE MYTH: Let the '-O3' generic compiler flags do "
               "the 'clever' micro-optimizations!"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Per.7 — Design to enable optimization (data layout matters)
  // TR: Per.7 — Optimizasyonu mümkün kılacak şekilde tasarla (veri düzeni önemlidir)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // EN: C++ has no standard `restrict` keyword (C99 feature). Some compilers offer
  // `__restrict__` as an extension, but it's non-portable.
  // TR: C++'da standart bir `restrict` anahtar kelimesi yoktur (C99 özelliği). Bazı derleyiciler
  // `__restrict__` uzantısını sunar ama taşınabilir değildir.

  // ─── 1. Avoid unnecessary copies in range-for loops ────────────────────────────────────────────
  // EN: Using `auto` (value) copies every element. Use `const auto&` instead.
  // TR: `auto` (değer) her elemanı kopyalar. Bunun yerine `const auto&` kullanın.

  std::vector<std::string> sensorNames = {
      "Temperature", "Pressure", "Humidity", "Altitude", "Speed"};

  // EN: BAD — copies every string (expensive for large objects).
  // TR: KÖTÜ — her string'i kopyalar (büyük nesneler için pahalıdır). for (auto name :
  // sensorNames) { /* copy per iteration */ }

  // EN: GOOD — zero-copy read, compiler can auto-vectorize.
  // TR: İYİ — sıfır kopyalı okuma, derleyici otomatik vektörleştirebilir.
  std::cout << "\n[Per.7] Sensor names (const auto& — zero copy):" << std::endl;
  for (const auto& name : sensorNames) {
    std::cout << "  -> " << name << std::endl;
  }

  std::cout << "[Per.7] Always use 'const auto&' in range-for when you only read!"
            << std::endl;

  return 0;
}
