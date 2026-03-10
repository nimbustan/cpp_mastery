/**
 * @file module_01_basics/05_control_flow_deepdive.cpp
 * @brief Basics: Control Flow Deep Dive / Temeller: Akış Kontrolü Derinlemesine
 *
 * @details
 * =============================================================================
 * [THEORY: Decision Structures / TEORİ: Karar Yapıları]
 * =============================================================================
 * EN: Every program needs decision-making. C++ provides: - if / else if / else: General-purpose
 * branching. - switch / case: Optimized jump-table for integral types (int, char, enum). -
 * Ternary (?:): Inline conditional (covered in 04_operators).
 *
 * In automotive ECU firmware, decision structures control everything: engine management,
 * climate, ABS activation, lighting logic.
 *
 * TR: Her program karar vermeye ihtiyaç duyar. C++ şunları sunar: - if / else if / else: Genel
 * amaçlı dallanma. - switch / case: Tam sayı tipleri için optimize edilmiş atlama tablosu. -
 * Ternary (?:): Satır içi koşul (04_operators dosyasında anlatıldı).
 *
 * =============================================================================
 * [THEORY: Loop Structures / TEORİ: Döngü Yapıları]
 * =============================================================================
 * EN: Loops repeat a block of code: - for: When the iteration count is KNOWN. - while: When it
 * depends on a CONDITION (unknown iterations). - do-while: Same as while, but executes AT LEAST
 * ONCE. - Range-based for (C++11): Safe iteration over containers. - break: Exit the loop
 * immediately. - continue: Skip this iteration, go to next.
 *
 * TR: Döngüler bir kod bloğunu tekrarlar: - for: İterasyon sayısı BİLİNDİĞİNDE. - while: Bir
 * KOŞULA bağlı olduğunda (belirsiz iterasyon sayısı). - do-while: while ile aynı ama EN AZ BİR
 * KEZ çalışır. - Range-based for (C++11): Konteynerler üzerinde güvenli iterasyon. - break:
 * Döngüden anında çık. - continue: Bu iterasyonu atla, sonrakine geç.
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
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

int main() {
  std::cout << "=== MODULE 1: CONTROL FLOW DEEP DIVE ===\n" << std::endl;

  // ===============================================================================================
  // 1. IF / ELSE IF / ELSE
  // ===============================================================================================
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
  // EN: Variable 'voltage' is scoped ONLY inside this if-block.
  // TR: 'voltage' değişkeni SADECE bu if bloğunun içinde geçerlidir.
  if (double voltage = 13.8; voltage > 12.0) {
    std::cout << "[ECU] Battery voltage OK: " << voltage << " V\n"
              << std::endl;
  }

  // ===============================================================================================
  // 2. SWITCH / CASE
  // ===============================================================================================
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

  // ===============================================================================================
  // 3. CLASSIC FOR LOOP / KLASİK FOR DÖNGÜSÜ
  // ===============================================================================================
  std::cout << "--- 3. Classic for loop ---" << std::endl;

  // EN: Simulate scanning 6 CAN bus message slots.
  // TR: 6 CAN bus mesaj slotunu taramak.
  for (int slot = 0; slot < 6; ++slot) {
    std::cout << "Scanning CAN slot #" << slot << std::endl;
  }
  std::cout << std::endl;

  // ===============================================================================================
  // 4. WHILE LOOP / WHILE DÖNGÜSÜ
  // ===============================================================================================
  std::cout << "--- 4. while loop ---" << std::endl;

  // EN: Simulate coolant warm-up until threshold.
  // TR: Soğutma suyu ısınmasını eşiğe kadar simüle et.
  int warmUpTemp = 20;
  while (warmUpTemp < 80) {
    warmUpTemp += 15; // Simulating heating
    std::cout << "Warming up... " << warmUpTemp << " °C" << std::endl;
  }
  std::cout << "[ECU] Operating temperature reached!\n" << std::endl;

  // ===============================================================================================
  // 5. DO-WHILE LOOP / DO-WHILE DÖNGÜSÜ
  // ===============================================================================================
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

  // ===============================================================================================
  // 6. BREAK & CONTINUE
  // ===============================================================================================
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

  // ===============================================================================================
  // 7. NESTED LOOPS / İÇ İÇE DÖNGÜLER
  // ===============================================================================================
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

  // ===============================================================================================
  // 8. RANGE-BASED FOR (C++11) — revisited
  // ===============================================================================================
  std::cout << "--- 8. Range-based for (C++11) ---" << std::endl;

  // EN: Iterating over DTC codes stored in an array.
  // TR: Dizide saklanan DTC kodlarını dolaşma.
  std::string dtcCodes[] = {"P0300", "P0171", "P0420", "P0442"};

  for (const auto &dtc : dtcCodes) {
    std::cout << "  Active DTC: " << dtc << std::endl;
  }
  std::cout << std::endl;

  // ===============================================================================================
  // 9. INFINITE LOOP PATTERN (common in embedded) / SONSUZ DÖNGÜ KALIP
  // ===============================================================================================
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
