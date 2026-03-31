/**
 * @file module_03_modern_cpp/02_lambdas.cpp
 * @brief Modern C++: Lambda Expressions — From Zero to Mastery — Lambda İfadeleri — Sıfırdan
 * Ustalığa (C++11 / C++14 / C++17 / C++20)
 *
 * @details
 * =============================================================================
 * [THEORY: What is a Lambda? (Lambdas for Dummies) / TEORİ: Lambda Nedir? (Sıfırdan Anlatım)]
 * =============================================================================
 * EN: Imagine you need to tell the ECU diagnostic system: "Filter only the fault codes above
 * severity 3." Before C++11 you had to write a separate named function somewhere far away and
 * pass a pointer to it. With a Lambda you write the rule RIGHT WHERE you need it — inline,
 * compact, readable.
 *
 * A Lambda is an ANONYMOUS (unnamed) function object that the compiler generates for you behind
 * the scenes. Think of it as a disposable mini-function you define on the spot. Lambdas are the
 * cornerstone of modern STL algorithms (std::sort, std::for_each, std::find_if, std::transform,
 * ...).
 *
 * TR: ECU arıza teşhis sistemine: "Sadece önem derecesi 3 üzeri hata kodlarını filtrele" demek
 * istediğinizi düşünün. C++11 öncesi uzaklarda ayrı bir fonksiyon yazıp onun adresini (pointer)
 * göndermeniz gerekirdi. Lambda ile kuralı TAM İHTİYAÇ DUYDUĞUNUZ YERE, satır içi, kompakt ve
 * okunabilir şekilde yazarsınız.
 *
 * Lambda, derleyicinin arka planda sizin için ürettiği İSİMSİZ (anonim) bir fonksiyon
 * nesnesidir. Onu tek kullanımlık, yerinde tanımlanan mini-fonksiyon olarak düşünün. Lambdalar
 * modern STL algoritmalarının (std::sort, std::for_each, std::find_if, std::transform, ...)
 * temel taşıdır.
 *
 * =============================================================================
 * [THEORY: Lambda Anatomy — The Full Syntax / TEORİ: Lambda Anatomisi — Tam Sözdizimi]
 * =============================================================================
 * EN: The complete Lambda syntax is:
 *   [capture_list] (parameters) mutable -> return_type { body }
 *
 * - [capture_list] : HOW to grab outside variables (see next theory).
 * - (parameters)   : Input arguments, just like a normal function.
 * - mutable        : (Optional) Lets you modify captured-by-value copies.
 * - -> return_type : (Optional) Explicit return type. Usually auto-deduced.
 * - { body }       : The actual code.
 *
 * Minimal Lambda: [](){} — captures nothing, takes nothing, does nothing. When there are no
 * parameters you may even omit (): []{}.
 *
 * TR: Lambda'nın tam sözdizimi:
 *   [yakalama_listesi] (parametreler) mutable -> dönüş_tipi { gövde }
 *
 * - [yakalama_listesi] : Dış değişkenleri NASIL yakalayacağınız.
 * - (parametreler)     : Girdi argümanları, normal fonksiyon gibi.
 * - mutable            : (Opsiyonel) Değer ile yakalanan kopyaları değiştirmenize izin verir.
 * - -> dönüş_tipi      : (Opsiyonel) Açık dönüş tipi. Genelde otomatik çıkarılır.
 * - { gövde }          : Gerçek kod.
 *
 * En Basit Lambda: [](){} — hiçbir şey yakalamaz, almaz, yapmaz. Parametre yoksa () bile
 * atlanabilir: []{}.
 *
 * =============================================================================
 * [THEORY: Capture List Deep Dive / TEORİ: Yakalama Listesi Detaylı İnceleme]
 * =============================================================================
 * EN: The capture list is what makes Lambdas special. It controls how outside variables enter
 * the Lambda's body:
 *
 *   []          — Capture nothing. Pure function, no outside variables.
 *   [=]         — Capture ALL outside variables by VALUE (read-only copies).
 *   [&]         — Capture ALL outside variables by REFERENCE (modify originals).
 *   [x]         — Capture only 'x' by value.
 *   [&x]        — Capture only 'x' by reference.
 *   [=, &x]     — Capture everything by value EXCEPT 'x' by reference.
 *   [&, x]      — Capture everything by reference EXCEPT 'x' by value.
 *   [this]      — Capture the enclosing object's 'this' pointer (C++11).
 *   [*this]     — Capture a COPY of the enclosing object (C++17).
 *   [x = expr]  — Init-capture: create a new variable 'x' initialized to
 * 'expr' (C++14). Great for move-capturing!
 *
 * Golden Rule: Capture only what you need. Prefer explicit captures [x, &y] over blanket [=] or
 * [&] for clarity and safety.
 *
 * TR: Yakalama listesi Lambda'ları özel kılan şeydir. Dış değişkenlerin Lambda gövdesine nasıl
 * gireceğini kontrol eder:
 *
 *   []          — Hiçbir şey yakalama. Saf fonksiyon, dış değişken yok.
 *   [=]         — Tüm dış değişkenleri DEĞER ile yakala (salt okunur kopya).
 *   [&]         — Tüm dış değişkenleri REFERANS ile yakala (orijinali değiştir).
 *   [x]         — Sadece 'x' değişkenini değer ile yakala.
 *   [&x]        — Sadece 'x' değişkenini referans ile yakala.
 *   [=, &x]     — Her şeyi değer ile, SADECE 'x' referans ile yakala.
 *   [&, x]      — Her şeyi referans ile, SADECE 'x' değer ile yakala.
 *   [this]      — Kapsayan nesnenin 'this' işaretçisini yakala (C++11).
 *   [*this]     — Kapsayan nesnenin KOPYASInı yakala (C++17).
 *   [x = ifade] — Başlatıcılı yakalama: 'ifade' ile başlatılan yeni 'x'
 * değişkeni oluştur (C++14). Move-capture için mükemmel!
 *
 * Altın Kural: Sadece ihtiyacınız olanı yakalayın. Netlik ve güvenlik için toptan [=] veya [&]
 * yerine açık yakalama [x, &y] tercih edin.
 *
 * =============================================================================
 * [CPPREF DEPTH: Dangling Captures & Lifetime Traps / CPPREF DERİNLİK: Sarkan Yakalamalar ve
 * Ömür Tuzakları]
 * =============================================================================
 * EN: cppreference.com/w/cpp/language/lambda warns: If a Lambda captures local variables by
 * REFERENCE [&] and then OUTLIVES the scope where those variables were defined (e.g., returned
 * from a function, stored in a container, passed to std::thread), you get a DANGLING REFERENCE —
 * Undefined Behavior! The Lambda holds an address to memory that no longer exists.
 *
 * Safe rule: If the Lambda escapes the current scope, capture by VALUE [=] or use init-capture
 * [x = std::move(obj)] to own the data.
 *
 * TR: cppreference.com/w/cpp/language/lambda uyarır: Lambda yerel değişkenleri REFERANS [&] ile
 * yakalayıp, bu değişkenlerin tanımlandığı kapsamın dışına çıkarsa (fonksiyondan dönülürse,
 * container'a saklanırsa, std::thread'e verilirse), SARKAN REFERANS (Dangling Reference) oluşur
 * — Tanımsız Davranış! Lambda artık var olmayan belleğe işaret eder.
 *
 * Güvenli kural: Lambda mevcut kapsamdan dışarı çıkıyorsa DEĞER [=] ile yakalayın veya
 * init-capture [x = std::move(obj)] ile veriyi sahiplenin.
 *
 * =============================================================================
 * [CPPREF DEPTH: std::function — Type-Erased Lambda Storage / CPPREF DERİNLİK: std::function —
 * Tip Silmeli Lambda Saklama]
 * =============================================================================
 * EN: cppreference.com/w/cpp/utility/functional/function documents:
 * std::function<ReturnType(Args...)> is a general-purpose polymorphic function wrapper. It can
 * store ANY callable: a Lambda, a function pointer, a functor, or std::bind result. Use it when
 * you need to store a Lambda in a member variable, a container, or pass it across API
 * boundaries. Be aware: it has overhead (heap allocation, virtual dispatch). For
 * performance-critical automotive loops, prefer templates or auto.
 *
 * TR: cppreference.com/w/cpp/utility/functional/function belgeliyor:
 * std::function<DönüşTipi(Argümanlar...)> genel amaçlı polimorfik fonksiyon sarmalayıcıdır.
 * HERHANGİ bir çağrılabiliri saklayabilir: Lambda, fonksiyon işaretçisi, functor veya std::bind
 * sonucu. Lambda'yı üye değişkende, container'da saklamanız veya API sınırları arasında
 * geçirmeniz gerektiğinde kullanın. Dikkat: maliyeti vardır (heap tahsisi, sanal dağıtım).
 * Performans-kritik otomotiv döngülerinde template veya auto tercih edin.
 *
 * =============================================================================
 * [THEORY: Generic Lambdas (C++14) & Template Lambdas (C++20) / TEORİ: Jenerik Lambdalar (C++14)
 * ve Şablon Lambdalar (C++20)]
 * =============================================================================
 * EN: C++14 introduced generic Lambdas: use 'auto' in the parameter list and the compiler
 * generates a templated operator() internally. This is perfect for writing ECU-agnostic utility
 * functions.
 *
 * C++20 goes further: you can write explicit template parameter lists on Lambdas: []<typename
 * T>(T value){ ... }. This gives you SFINAE/concepts power directly inside a Lambda — useful for
 * compile-time filtering of sensor types in embedded systems.
 *
 * TR: C++14 jenerik Lambda'ları tanıttı: parametre listesinde 'auto' kullanın, derleyici içeride
 * şablonlu operator() üretir. Bu, ECU-bağımsız yardımcı fonksiyonlar yazmak için mükemmeldir.
 *
 * C++20 daha ileri gider: Lambda'larda açık şablon parametre listesi yazabilirsiniz: []<typename
 * T>(T value){ ... }. Bu size doğrudan Lambda içinde SFINAE/konsept gücü verir — gömülü
 * sistemlerde sensör tiplerini derleme zamanında filtrelemek için kullanışlıdır.
 *
 * =============================================================================
 * [CPPREF DEPTH: Lambda as Constexpr (C++17) & Consteval (C++20) / CPPREF DERİNLİK: constexpr
 * Lambda (C++17) ve consteval (C++20)]
 * =============================================================================
 * EN: cppreference.com/w/cpp/language/lambda documents: Since C++17, Lambdas are implicitly
 * constexpr if their body satisfies constexpr requirements. You can compute values at COMPILE
 * TIME using Lambdas — zero runtime cost. In automotive ECUs with limited flash, this means
 * look-up tables, CRC checksums, and protocol constants can be Lambda-computed at compile time.
 *
 * C++20 allows `consteval` Lambdas that MUST execute at compile time.
 *
 * TR: cppreference.com/w/cpp/language/lambda belgeliyor: C++17'den beri Lambda'lar gövdesi
 * constexpr gereksinimlerini karşılıyorsa otomatik olarak constexpr'dir. Lambda'lar ile DERLEME
 * ZAMANINDA değer hesaplayabilirsiniz — sıfır çalışma zamanı maliyeti. Sınırlı flash'e sahip
 * otomotiv ECU'larında bu, arama tabloları, CRC sağlama toplamları ve protokol sabitlerinin
 * derleme zamanında Lambda ile hesaplanabileceği anlamına gelir.
 *
 * C++20, derleme zamanında çalışMASI GEREKEN consteval Lambda'lara izin verir.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_lambdas.cpp -o 02_lambdas
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>  // std::sort, std::for_each, std::find_if, std::transform
#include <cstdint>    // uint8_t, uint16_t
#include <functional> // std::function
#include <iostream>
#include <numeric>    // std::accumulate
#include <string>
#include <vector>

// ─── Automotive Helper Structures ────────────────────────────────────────────────────────────────

// EN: Diagnostic Trouble Code (DTC) — core data in any automotive ECU.
// TR: Arıza Teşhis Kodu (DTC) — her otomotiv ECU'sunun temel verisi.
struct DTC {
  uint16_t code;        // e.g. 0x0171
  std::string name;     // e.g. "EGT Sensor Open Circuit"
  uint8_t severity;     // 1 = info, 2 = warning, 3 = critical, 4 = fatal
  bool active;          // currently present?
};

// EN: Sensor reading from a vehicle bus (CAN/LIN).
// TR: Araç veri yolundan (CAN/LIN) gelen sensör okuması.
struct SensorReading {
  std::string sensor;
  double value;
  std::string unit;
};

int main() {
  std::cout << "=== MODULE 3/02: LAMBDA EXPRESSIONS — FULL GUIDE ===\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. THE SIMPLEST LAMBDA / EN BASIT LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: A Lambda with no capture, no parameters. Just prints a message.
  // TR: Yakalama yok, parametre yok. Sadece bir mesaj yazdırır.

  auto greet = []() {
    std::cout << "1. ECU Boot: Lambda initialized successfully!" << std::endl;
  };
  // EN: Call the Lambda
  // TR: Lambda'yı çağır
  greet();

  // EN: Even shorter — omit () when there are no parameters:
  // TR: Daha kısa — parametre yoksa () atlanabilir:
  auto greetShort = [] {
    std::cout << "   (Same Lambda without parentheses)\n" << std::endl;
  };
  greetShort();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. LAMBDA WITH PARAMETERS / PARAMETRELI LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Lambdas accept parameters just like regular functions. Here we convert a raw ADC value
  // to a temperature (automotive formula).
  // TR: Lambda'lar normal fonksiyonlar gibi parametre alır. Ham ADC değerini sıcaklığa çeviren
  // otomotiv formülü.

  auto adcToTemperature = [](int rawAdc) -> double {
    // EN: Simplified EGT sensor formula: T = (raw * 0.1) - 40.0
    // TR: Basitleştirilmiş EGT sensör formülü: T = (raw * 0.1) - 40.0
    return (rawAdc * 0.1) - 40.0;
  };

  int rawValue = 850;
  std::cout << "2. ADC Raw=" << rawValue
            << " -> Temperature=" << adcToTemperature(rawValue)
            << " C\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. CAPTURE BY VALUE [=] / DEGER ILE YAKALAMA [=]
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: The Lambda copies the outside variable's VALUE at the moment of creation. Changes to the
  // original do NOT affect the Lambda's copy.
  // TR: Lambda oluşturulma anında dış değişkenin DEĞERİNİ kopyalar. Orijinaldeki değişiklikler
  // Lambda'nın kopyasını ETKİLEMEZ.

  // EN: Critical and above
  // TR: Kritik ve üzeri
  int severityThreshold = 3;
  auto isCritical = [=](const DTC& dtc) {
    return dtc.severity >= severityThreshold && dtc.active;
  };

  // EN: Even if we change threshold AFTER creating the Lambda, the Lambda still sees the OLD
  // value (3), because it captured a COPY.
  // TR: Lambda'yı oluşturduktan SONRA threshold'u değiştirsek bile, Lambda HÂL ESKİ değeri (3)
  // görür, çünkü KOPYAyı yakaladı.
  severityThreshold = 999;

  DTC testDtc{0x0171, "EGT Sensor Open", 4, true};
  std::cout << "3. Capture by value: DTC 0x0171 critical? "
            << (isCritical(testDtc) ? "YES" : "NO")
            << " (threshold was 3 at capture time)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. CAPTURE BY REFERENCE [&] / REFERANS ILE YAKALAMA [&]
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: The Lambda gets a REFERENCE to the outside variable. Any change the Lambda makes
  // modifies the ORIGINAL. Also, if the outside changes, the Lambda sees the new value.
  // TR: Lambda dış değişkene REFERANS alır. Lambda'nın yaptığı her değişiklik ORİJİNALİ
  // değiştirir. Dışarıda değişiklik olursa Lambda yeni değeri görür.

  int activeFaultCount = 0;
  std::vector<DTC> faultLog = {
      {0x0100, "Coolant Temp High",   3, true},
      {0x0200, "Oil Pressure Low",    4, true},
      {0x0300, "Battery Voltage",     2, false},
      {0x0400, "Throttle Pos Sensor", 3, true},
      {0x0500, "O2 Sensor Circuit",   2, true},
      {0x0600, "Knock Sensor Range",  4, false},
      {0x0700, "Misfire Cylinder 2",  3, true},
  };

  // EN: Count active faults by capturing counter BY REFERENCE.
  // TR: Sayacı REFERANS ile yakalayarak aktif arızaları say.
  std::for_each(faultLog.begin(), faultLog.end(),
                [&activeFaultCount](const DTC& dtc) {
    if (dtc.active) {
      activeFaultCount++;
    }
  });
  std::cout << "4. Capture by reference: Active faults = "
            << activeFaultCount << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. MIXED CAPTURE [=, &x] / KARISIK YAKALAMA [=, &x]
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Best practice — capture everything by value for safety, but explicitly capture specific
  // variables by reference when you need to modify them.
  // TR: En iyi pratik — güvenlik için her şeyi değer ile yakala, ama değiştirmen gereken belirli
  // değişkenleri açıkça referans ile yakala.

  int criticalCount = 0;
  int warningCount = 0;
  int critThreshold = 3;

  std::for_each(faultLog.begin(), faultLog.end(),
                [=, &criticalCount, &warningCount](const DTC& dtc) {
    if (!dtc.active) return;
    if (dtc.severity >= critThreshold) {
      // EN: Modified via reference
      // TR: Referansla değiştirilir
      criticalCount++;
    } else {
      warningCount++;
    }
  });
  std::cout << "5. Mixed capture: Critical=" << criticalCount
            << ", Warning=" << warningCount << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. MUTABLE LAMBDA / DEGISTIRILEBILIR LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: By default, a Lambda captured by VALUE cannot modify its copies (they are const). The
  // 'mutable' keyword lifts this restriction. Note: It modifies the COPY, not the original!
  // TR: Varsayılan olarak DEĞER ile yakalanan kopya const'tır, değiştirilemez. 'mutable' bu
  // kısıtlamayı kaldırır. Not: KOPYAyı değiştirir, orijinali DEĞİL!

  int sequenceId = 1000;
  auto generateId = [sequenceId]() mutable -> int {
    // EN: Each call increments the Lambda's internal copy.
    // TR: Her çağrıda Lambda'nın iç kopyası artar.
    return sequenceId++;
  };

  std::cout << "6. Mutable Lambda (sequence generator):" << std::endl;
  std::cout << "   ID: " << generateId() << std::endl; // 1000
  std::cout << "   ID: " << generateId() << std::endl; // 1001
  std::cout << "   ID: " << generateId() << std::endl; // 1002
  std::cout << "   Original sequenceId still = " << sequenceId
            << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 7. INIT CAPTURE / MOVE CAPTURE (C++14) / BASLATICI YAKALAMA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: C++14 allows creating new variables inside the capture list itself. This is essential
  // for MOVE SEMANTICS — you can move a unique_ptr or a large buffer into the Lambda without
  // copying.
  // TR: C++14 yakalama listesi içinde yeni değişken oluşturmaya izin verir. Bu, TAŞIMA SEMANTİĞİ
  // için kritiktir — unique_ptr veya büyük bir tampon belleği kopyalamadan Lambda'ya
  // taşıyabilirsiniz.

  std::string rawFrame = "CAN_ID:0x7E8 DATA:41 0C 1A F0";
  auto processFrame = [frame = std::move(rawFrame)]() {
    std::cout << "7. Init-capture (moved): Processing frame: "
              << frame << std::endl;
  };
  processFrame();
  std::cout << "   Original rawFrame is now empty: \""
            << rawFrame << "\"\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 8. LAMBDA WITH STL ALGORITHMS / STL ALGORITMALARI ILE LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: This is where Lambdas truly shine in automotive software.
  // TR: Lambda'ların otomotiv yazılımında gerçekten parladığı yer.

  // --- 8a. std::sort — Sort DTCs by severity (descending) ---
  // EN: Custom sorting: highest severity first for prioritized display.
  // TR: Özel sıralama: öncelikli görüntüleme için en yüksek önem derecesi önce.
  std::vector<DTC> sortedFaults = faultLog;
  std::sort(sortedFaults.begin(), sortedFaults.end(),
            [](const DTC& a, const DTC& b) {
    return a.severity > b.severity;
  });
  std::cout << "8a. std::sort by severity (descending):" << std::endl;
  for (const auto& dtc : sortedFaults) {
    std::cout << "    [" << static_cast<int>(dtc.severity) << "] 0x"
              << std::hex << dtc.code << std::dec
              << " " << dtc.name << std::endl;
  }

  // --- 8b. std::find_if — Find first fatal DTC ---
  // EN: Search for the first DTC with severity == 4 (fatal).
  // TR: Önem derecesi == 4 (ölümcül) olan ilk DTC'yi ara.
  auto it = std::find_if(faultLog.begin(), faultLog.end(),
                         [](const DTC& dtc) {
    return dtc.severity == 4;
  });
  if (it != faultLog.end()) {
    std::cout << "\n8b. std::find_if: First fatal DTC -> 0x"
              << std::hex << it->code << std::dec
              << " " << it->name << std::endl;
  }

  // --- 8c. std::transform — Extract fault names into a string vector ---
  // EN: Map DTCs to their names (functional-style projection).
  // TR: DTC'leri isimlerine dönüştür (fonksiyonel-stil projeksiyon).
  std::vector<std::string> faultNames(faultLog.size());
  std::transform(faultLog.begin(), faultLog.end(), faultNames.begin(),
                 [](const DTC& dtc) { return dtc.name; });
  std::cout << "\n8c. std::transform: DTC names extracted ("
            << faultNames.size() << " items)" << std::endl;

  // --- 8d. std::accumulate — Sum all severity scores ---
  // EN: Total severity score across all DTCs (risk assessment).
  // TR: Tüm DTC'lerin toplam önem puanı (risk değerlendirmesi).
  int totalSeverity = std::accumulate(
      faultLog.begin(), faultLog.end(), 0,
      [](int sum, const DTC& dtc) {
    return sum + static_cast<int>(dtc.severity);
  });
  std::cout << "8d. std::accumulate: Total severity score = "
            << totalSeverity << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 9. GENERIC LAMBDA (C++14 auto) / JENERIK LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Use 'auto' parameters to create Lambdas that work with ANY type. The compiler generates
  // a templated operator() behind the scenes.
  // TR: HERHANGİ bir tip ile çalışan Lambda'lar oluşturmak için 'auto' parametre kullanın.
  // Derleyici arka planda şablonlu operator() üretir.

  auto printReading = [](const auto& r) {
    std::cout << "   Sensor: " << r.sensor
              << " = " << r.value << " " << r.unit << std::endl;
  };

  std::vector<SensorReading> readings = {
      {"Coolant Temp",  92.5, "C"},
      {"Engine RPM",  3500.0, "rpm"},
      {"Battery",      13.8, "V"},
      {"Fuel Pressure", 3.2, "bar"},
  };

  std::cout << "9. Generic Lambda (auto) — Sensor Readings:" << std::endl;
  std::for_each(readings.begin(), readings.end(), printReading);
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 10. std::function — STORING LAMBDAS / LAMBDA SAKLAMA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: auto is great for local use, but if you need to store a Lambda in a class member,
  // container, or pass it through an API, use std::function. Trade-off: small overhead (type
  // erasure + possible heap allocation).
  // TR: auto yerel kullanım için harikadır, ama Lambda'yı sınıf üyesinde, container'da
  // saklamanız veya API'den geçirmeniz gerekirse std::function kullanın. Takas: küçük maliyet
  // (tip silme + olası heap tahsisi).

  // EN: ECU callback registry pattern — store different validation rules.
  // TR: ECU geri-çağırma kayıt deseni — farklı doğrulama kurallarını sakla.
  std::vector<std::function<bool(const DTC&)>> validators;

  validators.push_back([](const DTC& d) { return d.active; });
  validators.push_back([](const DTC& d) { return d.severity >= 3; });
  validators.push_back([](const DTC& d) { return d.code < 0x0500; });

  DTC sample{0x0400, "Throttle Pos Sensor", 3, true};
  std::cout << "10. std::function validators for DTC 0x0400:" << std::endl;
  int passCount = 0;
  for (size_t i = 0; i < validators.size(); ++i) {
    bool result = validators[i](sample);
    std::cout << "    Validator " << (i + 1) << ": "
              << (result ? "PASS" : "FAIL") << std::endl;
    if (result) passCount++;
  }
  std::cout << "    Passed " << passCount << "/"
            << validators.size() << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 11. LAMBDA AS FUNCTION PARAMETER (HIGHER-ORDER FUNCTIONS) LAMBDA'YI FONKSIYON PARAMETRESI
  // OLARAK GECIRME
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: You can write your own functions that ACCEPT Lambdas as parameters. This is the
  // "Higher-Order Function" pattern — a function that takes another function as an argument.
  // Using templates avoids std::function overhead.
  // TR: Lambda'ları parametre olarak KABUL EDEN kendi fonksiyonlarınızı yazabilirsiniz. Bu
  // "Yüksek Dereceli Fonksiyon (Higher-Order Function)" desenidir. Template kullanmak
  // std::function maliyetinden kaçınır.

  // EN: Using a Lambda with std::count_if for filtered counting.
  // TR: Filtrelenmiş sayma için std::count_if ile Lambda kullanımı.
  auto activeCount = std::count_if(faultLog.begin(), faultLog.end(),
                                   [](const DTC& d) { return d.active; });
  auto severeActive = std::count_if(faultLog.begin(), faultLog.end(),
                                    [](const DTC& d) {
    return d.active && d.severity >= 3;
  });
  std::cout << "11. Higher-order: Active DTCs = " << activeCount
            << ", Severe+Active = " << severeActive << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 12. IMMEDIATELY INVOKED LAMBDA EXPRESSION (IILE) ANINDA CAGIRILAN LAMBDA IFADESI
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Call a Lambda immediately at the point of definition. Useful for complex initialization
  // of const variables.
  // TR: Lambda'yı tanımlandığı noktada hemen çağırın. const değişkenlerin karmaşık başlatılması
  // için kullanışlıdır.

  const std::string ecuStatus = [&]() -> std::string {
    int fatal = 0;
    for (const auto& d : faultLog) {
      if (d.active && d.severity == 4) fatal++;
    }
    if (fatal > 0) return "LIMP MODE";
    if (activeFaultCount > 3) return "DEGRADED";
    return "NORMAL";
  }(); // EN: Note the () at the end — immediate invocation!
       // TR: Sondaki () — hemen çağırma!

  std::cout << "12. IILE: ECU Status = " << ecuStatus << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 13. CONSTEXPR LAMBDA (C++17) / DERLEME-ZAMANI LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Since C++17, Lambdas can be constexpr — computed at compile time. Perfect for automotive
  // look-up tables and protocol constants.
  // TR: C++17'den beri Lambda'lar constexpr olabilir — derleme zamanında hesaplanır. Otomotiv
  // arama tabloları ve protokol sabitleri için ideal.

  constexpr auto crcByte = [](uint8_t data) constexpr -> uint8_t {
    // EN: Simplified CRC-8 step for demonstration.
    // TR: Gösterim amaçlı basitleştirilmiş CRC-8 adımı.
    uint8_t crc = data;
    for (int i = 0; i < 8; ++i) {
      if (crc & 0x80U) {
        crc = static_cast<uint8_t>((crc << 1U) ^ 0x07U);
      } else {
        crc = static_cast<uint8_t>(crc << 1U);
      }
    }
    return crc;
  };

  constexpr uint8_t checksum = crcByte(0xA5);
  std::cout << "13. constexpr Lambda: CRC-8(0xA5) = 0x"
            << std::hex << static_cast<int>(checksum) << std::dec
            << " (computed at compile time!)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 14. RECURSIVE LAMBDA (via std::function) / OZYINELEMELI LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Lambdas cannot directly call themselves (they have no name). To make a recursive Lambda,
  // wrap it in std::function so it can reference itself through the captured variable.
  // TR: Lambda'lar kendilerini doğrudan çağıramaz (isimleri yoktur). Özyinelemeli Lambda yapmak
  // için std::function ile sarmalayın, böylece yakalanan değişken üzerinden kendine referans
  // verebilir.

  // EN: Compute checksum of a DTC code tree (recursive severity sum).
  // TR: DTC kodu ağacının sağlama toplamını hesapla (özyinelemeli önem toplamı).
  std::function<int(const std::vector<DTC>&, size_t)> recursiveSum =
      [&recursiveSum](const std::vector<DTC>& dtcs, size_t idx) -> int {
    if (idx >= dtcs.size()) return 0;
    return static_cast<int>(dtcs[idx].severity) +
           recursiveSum(dtcs, idx + 1);
  };

  std::cout << "14. Recursive Lambda: Total severity (recursive) = "
            << recursiveSum(faultLog, 0) << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 15. LAMBDA RETURNING A LAMBDA (FACTORY PATTERN) LAMBDA DONDUREN LAMBDA (FABRIKA DESENI)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: A Lambda can return ANOTHER Lambda. This is a powerful functional programming pattern —
  // creating specialized filters at runtime.
  // TR: Bir Lambda BAŞKA bir Lambda döndürebilir. Bu güçlü bir fonksiyonel programlama desenidir
  // — çalışma zamanında özelleşmiş filtreler oluşturur.

  auto makeSeverityFilter = [](uint8_t minSev) {
    return [minSev](const DTC& dtc) {
      return dtc.active && dtc.severity >= minSev;
    };
  };

  auto criticalFilter = makeSeverityFilter(4); // fatal only
  auto warningFilter = makeSeverityFilter(2);  // warning and above

  std::cout << "15. Lambda factory:" << std::endl;
  std::cout << "    Oil Pressure Low fatal? "
            << (criticalFilter(faultLog[1]) ? "YES" : "NO") << std::endl;
  std::cout << "    Battery Voltage warning+? "
            << (warningFilter(faultLog[2]) ? "YES" : "NO")
            << " (inactive, so NO)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // SUMMARY / OZET
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "=== LAMBDA CHEAT SHEET ===" << std::endl;
  std::cout << "  [](){}        — Simplest Lambda" << std::endl;
  std::cout << "  [=]           — Capture all by value" << std::endl;
  std::cout << "  [&]           — Capture all by reference" << std::endl;
  std::cout << "  [x, &y]       — Explicit mixed capture" << std::endl;
  std::cout << "  [x=move(v)]   — Init/move capture (C++14)" << std::endl;
  std::cout << "  mutable       — Modify value-captured copies" << std::endl;
  std::cout << "  auto params   — Generic Lambda (C++14)" << std::endl;
  std::cout << "  constexpr     — Compile-time Lambda (C++17)" << std::endl;
  std::cout << "  std::function — Type-erased storage" << std::endl;
  std::cout << "  IILE [](){}() — Immediate invocation" << std::endl;
  std::cout << "\nAll examples use Automotive/Embedded domain data." << std::endl;

  return 0;
}
