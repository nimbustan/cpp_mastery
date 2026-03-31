/**
 * @file module_01_basics/05_control_flow_deepdive.cpp
 * @brief Basics: Control Flow Deep Dive — Temeller: Akış Kontrolü Derinlemesine
 *
 * @details
 * =============================================================================
 * [THEORY: Decision Structures / TEORİ: Karar Yapıları]
 * =============================================================================
 * EN: Every program needs decision-making. C++ provides:
 *     1. if / else if / else: General-purpose branching.
 *     2. switch / case: Optimized jump-table for integral types (int, char, enum).
 *     3. Ternary (?:): Inline conditional (covered in 04_operators).
 *
 *     In automotive ECU firmware, decision structures control everything:
 *     engine management, climate, ABS activation, lighting logic.
 *
 * TR: Her program karar vermeye ihtiyaç duyar. C++ şunları sunar:
 *     1. if / else if / else: Genel amaçlı dallanma.
 *     2. switch / case: Tam sayı tipleri için optimize edilmiş atlama tablosu.
 *     3. Ternary (?:): Satır içi koşul (04_operators dosyasında anlatıldı).
 *
 *     Otomotiv ECU firmware'inde karar yapıları her şeyi kontrol eder:
 *     motor yönetimi, klima, ABS aktivasyonu, aydınlatma mantığı.
 *
 * =============================================================================
 * [THEORY: Loop Structures / TEORİ: Döngü Yapıları]
 * =============================================================================
 * EN: Loops repeat a block of code:
 *     1. for: When the iteration count is KNOWN.
 *     2. while: When it depends on a CONDITION (unknown iterations).
 *     3. do-while: Same as while, but executes AT LEAST ONCE.
 *     4. Range-based for (C++11): Safe iteration over containers.
 *     5. break: Exit the loop immediately.
 *     6. continue: Skip this iteration, go to next.
 *
 * TR: Döngüler bir kod bloğunu tekrarlar:
 *     1. for: İterasyon sayısı BİLİNDİĞİNDE.
 *     2. while: Bir KOŞULA bağlı olduğunda (belirsiz iterasyon sayısı).
 *     3. do-while: while ile aynı ama EN AZ BİR KEZ çalışır.
 *     4. Range-based for (C++11): Konteynerler üzerinde güvenli iterasyon.
 *     5. break: Döngüden anında çık.
 *     6. continue: Bu iterasyonu atla, sonrakine geç.
 *
 * =============================================================================
 * [CPPREF DEPTH: Structured Bindings & Init-Statements (C++17) / CPPREF DERİNLİK: Yapısal
 * Bağlamalar ve Başlatma İfadeleri (C++17)]
 * =============================================================================
 * EN: C++17 allows `if (auto x = compute(); x > threshold)` — the variable is scoped ONLY inside
 * the if-block. This reduces variable leakage. Also, `switch` supports init-statements: `switch
 * (auto code = getDTC(); code)`.
 *
 * TR: C++17 ile `if (auto x = hesapla(); x > eşik)` yazabilirsiniz. Değişken SADECE if bloğunun
 * içinde geçerlidir. Bu değişken sızıntısını azaltır. `switch` de başlatma ifadesini destekler.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/if
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/switch
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_control_flow_deepdive.cpp -o 05_control_flow_deepdive
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

int main() {
  std::cout << "=== MODULE 1: CONTROL FLOW DEEP DIVE ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. IF / ELSE IF / ELSE
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. if / else if / else ---" << std::endl;

  int coolantTempC = 105;

  // EN: Multi-branch decision for coolant temperature monitoring.
  // TR: Soğutma suyu sıcaklığı izleme için çok dallı karar yapısı.
  if (coolantTempC > 120) {
    std::cout << "[ALARM] CRITICAL OVERHEAT: " << coolantTempC << " °C!"
              << std::endl;
  } else if (coolantTempC > 100) {
    std::cout << "[WARNING] Coolant high: " << coolantTempC << " °C"
              << std::endl;
  } else if (coolantTempC > 80) {
    std::cout << "[NORMAL] Coolant normal: " << coolantTempC << " °C"
              << std::endl;
  } else {
    std::cout << "[INFO] Engine cold: " << coolantTempC << " °C" << std::endl;
  }

  // C++17 if with init-statement / C++17 başlatma ifadeli if
  // EN: C++17 allows declaring a variable in the if-statement itself.
  //     The variable lives in if + all else if + else blocks, but NOT outside.
  //     You can also reassign it inside any branch (unless declared const).
  //     Equivalent to: { double v = 13.8; if (v > 14.0) {...} else if (...) {...} else {...} }
  // TR: C++17 ile değişken doğrudan if ifadesinde tanımlanabilir.
  //     Değişken if + tüm else if + else bloklarında yaşar, dışarıda ÖLÜR.
  //     const olmadığı sürece herhangi bir dalda yeni değer atanabilir.
  //     Eşdeğeri: { double v = 13.8; if (v > 14.0) {...} else if (...) {...} else {...} }
  if (double voltage = 13.8; voltage > 14.0) {
    std::cout << "[ECU] Overcharge detected: " << voltage << " V" << std::endl;
  } else if (voltage > 12.0) {
    // EN: 'voltage' is still accessible here — same scope as if-block.
    // TR: 'voltage' burada da erişilebilir — if bloğuyla aynı scope.
    std::cout << "[ECU] Battery voltage OK: " << voltage << " V" << std::endl;
  } else {
    // EN: Even in else, the variable is alive. We can reassign it.
    // TR: else dalında da değişken yaşıyor. Yeni değer atayabiliriz.
    voltage = 0.0;
    std::cout << "[ECU] LOW voltage! Reset to: " << voltage << " V"
              << std::endl;
  }
  // EN: 'voltage' is now OUT OF SCOPE — cannot be used here.
  // TR: 'voltage' artık SCOPE DIŞI — burada kullanılamaz.
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. SWITCH / CASE
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. switch / case ---" << std::endl;

  // EN: DTC (Diagnostic Trouble Code) severity levels: 1=Info, 2=Warning, 3=Critical. switch
  // creates an O(1) jump table.
  // TR: DTC (Teşhis Arıza Kodu) önem seviyesi. switch O(1) atlama tablosu oluşturur.
  int dtcSeverity = 2;

  switch (dtcSeverity) {
  case 1:
    std::cout << "[DTC] Severity 1: Informational" << std::endl;
    break; // EN: Without break, execution "falls through" to next case!
           // TR: break olmadan bir sonraki case'e düşer!
  case 2:
    std::cout << "[DTC] Severity 2: Warning — schedule maintenance"
              << std::endl;
    break;
  case 3:
    std::cout << "[DTC] Severity 3: CRITICAL — stop vehicle!" << std::endl;
    break;
  default:
    std::cout << "[DTC] Unknown severity level" << std::endl;
    break;
  }

  // EN: Intentional fall-through demo (grouping cases).
  // TR: Kasıtlı fall-through gösterimi (case'leri gruplama).
  char gearPosition = 'D';
  std::cout << "Gear: " << gearPosition << " -> ";
  switch (gearPosition) {
  case 'D':
  case 'd':
    std::cout << "Drive mode" << std::endl;
    break;
  case 'R':
  case 'r':
    std::cout << "Reverse mode" << std::endl;
    break;
  case 'N':
  case 'n':
    std::cout << "Neutral" << std::endl;
    break;
  case 'P':
  case 'p':
    std::cout << "Park" << std::endl;
    break;
  default:
    std::cout << "Invalid gear!" << std::endl;
    break;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. CLASSIC FOR LOOP / KLASİK FOR DÖNGÜSÜ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Classic for loop ---" << std::endl;

  // EN: Simulate scanning 6 CAN bus message slots.
  // TR: 6 CAN bus mesaj slotunu taramak.
  for (int slot = 0; slot < 6; ++slot) {
    std::cout << "Scanning CAN slot #" << slot << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. WHILE LOOP / WHILE DÖNGÜSÜ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. while loop ---" << std::endl;

  // EN: Simulate coolant warm-up until threshold.
  // TR: Soğutma suyu ısınmasını eşiğe kadar simüle et.
  int warmUpTemp = 20;
  while (warmUpTemp < 80) {
    warmUpTemp += 15; // Simulating heating
    std::cout << "Warming up... " << warmUpTemp << " °C" << std::endl;
  }
  std::cout << "[ECU] Operating temperature reached!\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. DO-WHILE LOOP / DO-WHILE DÖNGÜSÜ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 5. do-while loop ---" << std::endl;

  // EN: Executes AT LEAST ONCE, even if condition is already false. Use case: Sensor self-test
  // that must run at least once on boot.
  // TR: Koşul baştan yanlış olsa bile EN AZ BİR KEZ çalışır. Kullanım: Açılışta en az bir kez
  // çalışması gereken sensör öz testi.
  int sensorRetry = 0;
  do {
    std::cout << "Sensor self-test attempt #" << (sensorRetry + 1)
              << std::endl;
    sensorRetry++;
  } while (sensorRetry < 3);
  std::cout << "[ECU] Self-test complete.\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. BREAK & CONTINUE
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 6. break & continue ---" << std::endl;

  // EN: Scan 10 ECU addresses. Skip address 5 (reserved), stop at 8 (limit).
  // TR: 10 ECU adresini tara. 5'i atla (ayrılmış), 8'de dur (sınır).
  for (int addr = 1; addr <= 10; ++addr) {
    if (addr == 5) {
      std::cout << "  Address " << addr << " -> RESERVED (skip)" << std::endl;
      continue; // Skip this iteration / Bu iterasyonu atla
    }
    if (addr == 8) {
      std::cout << "  Address " << addr << " -> LIMIT reached (break)"
                << std::endl;
      break; // Exit the for loop entirely / Döngüden tamamen çık
    }
    std::cout << "  Address " << addr << " -> OK" << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 7. NESTED LOOPS / İÇ İÇE DÖNGÜLER
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 7. Nested Loops ---" << std::endl;

  // EN: 4 cylinders × 3 sensors each = 12 readings.
  // TR: 4 silindir × 3 sensör = 12 okuma.
  for (int cyl = 1; cyl <= 4; ++cyl) {
    for (int sensor = 1; sensor <= 3; ++sensor) {
      std::cout << "  Cyl" << cyl << "-Sensor" << sensor << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 8. RANGE-BASED FOR (C++11) — revisited
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 8. Range-based for (C++11) ---" << std::endl;

  // EN: Iterating over DTC codes stored in an array.
  // TR: Dizide saklanan DTC kodlarını dolaşma.
  std::string dtcCodes[] = {"P0300", "P0171", "P0420", "P0442"};

  for (const auto &dtc : dtcCodes) {
    std::cout << "  Active DTC: " << dtc << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 9. INFINITE LOOP PATTERN (common in embedded) / SONSUZ DÖNGÜ KALIP
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 9. Infinite Loop Pattern ---" << std::endl;
  // EN: In real embedded firmware, `while(true)` or `for(;;)` is the main super-loop. Here we
  // simulate with a counter.
  // TR: Gerçek gömülü firmware'de `while(true)` veya `for(;;)` ana süper-döngüdür. Burada bir
  // sayaçla simüle ediyoruz.
  int tick = 0;
  while (true) {
    std::cout << "  ECU tick #" << tick << std::endl;
    ++tick;
    if (tick >= 5) {
      std::cout << "  [DEMO] Exiting super-loop after 5 ticks.\n" << std::endl;
      break;
    }
  }

  return 0;
}
