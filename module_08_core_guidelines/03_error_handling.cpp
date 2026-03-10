/**
 * @file module_08_core_guidelines/03_error_handling.cpp
 * @brief C++ Core Guidelines: Error Handling / Hata Yönetimi
 *
 * @details
 * =============================================================================
 * [E.2 & E.3: Use Exceptions for Errors / Hatalar için Exception Kullanın]
 * =============================================================================
 * EN: "Throw an exception to signal that a function can't perform its assigned task." C++ Core
 * Guidelines heavily prefers Exceptions (`throw std::runtime_error`) over returning Error Codes
 * (like `-1` or `false`). Error codes get ignored, leading to silent bugs. Exceptions CANNOT be
 * ignored unless explicitly caught (`catch`).
 *
 * TR: "Eğer bir fonksiyon işini yapamıyorsa (Örn: Dosya bulunamadı), Exception (İstisna)
 * fırlat." C++ Mimarisi, eski C-tarzı "-1" veya "false" döndüren hata kodlarından nefret eder.
 * Çünkü hata kodlarını yazılımcılar kolayca görmezden gelir. Exception (`throw`) ise
 * yakalanmazsa (`catch`) programı durdurur. Sessiz (Zombie) bug'lara izin vermez!
 *
 * =============================================================================
 * [E.15: Throw by value, catch by reference / Değer ile Fırlat, Referans ile Yakala]
 * =============================================================================
 * EN: ALWAYS throw standard exceptions by value (`throw MyError();`) and catch them by const
 * reference (`catch (const MyError& e)`). This prevents "Object Slicing".
 *
 * TR: Exception fırlatırken obje oluşturarak (değer) fırlat: `throw
 * std::invalid_argument("hata");` Yakalarken ASLA değer olarak yakalama, her zaman **Referans**
 * ile yakala: `catch(const std::exception& e)` Bu sayede "Object Slicing" engellenir ve C++
 * polimorfizmi çalışır.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// EN: BAD PRACTICE: Using Error Codes (C-Style)
// TR: KÖTÜ PRATİK: C-Tarzı Hata Kodları (Return ile kontrol). Yazılımcı bunu unutabilir!
int openDatabaseOldWay(const std::string &address) {
  if (address.empty())
    return -1; // -1 means FATAL ERROR (Bulunamadı)
  return 0;    // 0 means Success
}

// EN: GOOD PRACTICE: Using Exceptions (C++ Core Guidelines E.2)
// TR: MÜKEMMEL PRATİK: Exception! Sessizce geçilmesine / unutulmasına C++ derleyicisi asla izin
// vermez!
void openDatabaseModern(const std::string &address) {
  if (address.empty()) {
    // EN: E.15: Throw by value! (Mimar kuralı: Sadece objeyi fırlat)
    throw std::invalid_argument(
        "FATAL: Database address cannot be empty!"
        " (Db adresi bos olamaz!)");
  }
  std::cout << "[Database] Successfully connected to: " << address << std::endl;
}

// EN: [[nodiscard]] makes the compiler WARN if the return value is ignored.
// TR: [[nodiscard]] dönüş değeri göz ardı edilirse derleyiciyi UYARIR.
[[nodiscard]] int getErrorCode() {
  return -1; // EN: Simulated error code
             // TR: Simüle edilmiş hata kodu
}

int main() {
  std::cout << "=== MODULE 8: C++ CORE GUIDELINES - ERROR HANDLING ===\n" << std::endl;

  // 1. DANGER OF ERROR CODES (Sessiz Zombileşen Hatalar)
  // EN: I forgot to check the return value (-1). My program continues
  openDatabaseOldWay( "");
           // BROKEN! TR: Gelen -1 değerini `if` içine almayı unuttum. Program ZOMBİ olarak devam
           // ediyor!

  // 2. SAFETY OF EXCEPTIONS (Exception Güvenliği, Fail-Fast Yaklasimi)
  try {
    // EN: This will immediately blast an error and JUMP to catch! TR: Bu fonksiyon anında
    // fırlatma yapar ve Alt satırları okumadan 'catch' bloğuna DÜŞER!
    openDatabaseModern("");
    std::cout << "This line will NEVER be printed." << std::endl;
  }
  // EN: CABLE-RULE (E.15): CATCH BY CONST REFERENCE! (Referans ile Yakala)
  // TR: Object Slicing olmasın diye her zaman 'const &' ile polimorfik olarak yakalıyoruz!
  catch (const std::invalid_argument &e) {
    std::cerr << "[Core Guidelines Exception Catcher] -> " << e.what() << std::endl;
  }

  // ===============================================================================================
  // [E.12 & E.16: Destructors and noexcept / Yıkıcılar ve noexcept Güvencesi]
  // ===============================================================================================
  // EN: Destructors (`~MyClass()`) and memory deallocation functions MUST NEVER THROW
  // EXCEPTIONS! If an exception happens inside a destructor during another exception unwinding,
  // the C++ runtime immediately terminates (`std::terminate`).
  //
  // TR: Yıkıcı fonksiyonlarda (`~MyClass`), ASLA Exception fırlatılamaz. Eğer fırlatılırsa C++
  // sistemi o saniye İntihar/İptal eder (`std::terminate`). Modern C++'da tüm yıkıcılar
  // varsayılan olarak `noexcept` (Hata Fırlatmaz) güvencesindedir.
  // ===============================================================================================
  //
  // ===============================================================================================
    // PPREF DEPTH: The Brutality of `std::terminate()` / CPPREF DERİNLİK: `std::terminate()`
  // Acımasızlığı]
  // ===============================================================================================
  // EN: `std::terminate()` is the absolute end of the line. It doesn't call destructors, it
  // doesn't free memory, it doesn't close files. It violently kills the process from the OS
  // level (`abort()`). This happens if: 1. You throw an exception and no `catch` block exists
  // anywhere to catch it. 2. An exception throws INSIDE a destructor while another exception is
  // currently flying (Stack Unwinding collision). 3. A `noexcept` function explicitly throws.
  //
  // TR: `std::terminate()` bir programın yaşayabileceği en vahşi ölümdür. Açık olan dosyaları
  // kapatmaz, RAM'i temizlemez (Destructor çağırmadan fişi çeker). 3 durumda ortaya çıkar: 1.
  // Fırlattığınız hatayı kimse (catch ile) yakalamazsa, 2. Bir hata havada uçarken (Stack geri
  // sarılırken), Destructor'ın içinden İKİNCİ bir hata daha fırlatırsanız (Çarpışma), 3.
  // `noexcept` verdiğiniz fonksiyonun içinde gizlice hata fırlatırsanız. Program anında OS
  // seviyesinde vurulur (`abort()`).
  // ===============================================================================================

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: E.6 — Use RAII to prevent leaks in error paths
  // TR: E.6 — Hata yollarında sızıntıları önlemek için RAII kullanın
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ─── 1. BAD: Raw pointer leaks on exception ────────────────────────────────────────────────────
  // EN: If process() throws, the delete never runs → memory leak.
  // TR: process() hata fırlatırsa delete çalışmaz → bellek sızıntısı.
  //
  // int* data = new int[1024]; process(data);      // If this throws... delete[] data;      //
  // ...this line is NEVER reached! LEAK!

  // ─── 2. GOOD: unique_ptr + exception = no leak ─────────────────────────────────────────────────
  // EN: unique_ptr guarantees cleanup even when exceptions fly.
  // TR: unique_ptr, exception fırlatılsa bile temizliği garanti eder.
  try {
    auto safeData = std::make_unique<int[]>(1024);
    safeData[0] = 42;
    std::cout << "[E.6 GOOD] safeData[0] = " << safeData[0] << std::endl;

    // EN: Simulate an error after allocation
    // TR: Tahsis sonrası bir hata simüle et
    throw std::runtime_error("Simulated processing error!");

    // EN: This line is skipped, but unique_ptr still frees memory.
    // TR: Bu satır atlanır ama unique_ptr yine de belleği serbest bırakır.
  } catch (const std::runtime_error& e) {
    std::cerr << "[E.6 Caught] " << e.what()
              << " (unique_ptr freed memory automatically!)" << std::endl;
  }

  // ─── 3. [[nodiscard]]: Force callers to check return values ────────────────────────────────────
  // EN: [[nodiscard]] makes the compiler WARN if the return value is ignored.
  // TR: [[nodiscard]] dönüş değeri göz ardı edilirse derleyiciyi UYARIR. (getErrorCode() defined
  // above main as a [[nodiscard]] free function)

  // EN: Good — caller checks the result. Compiler is happy.
  // TR: İyi — çağıran sonucu kontrol eder. Derleyici mutludur.
  int errCode = getErrorCode();
  if (errCode != 0) {
    std::cout << "[nodiscard Demo] Error code received: " << errCode << std::endl;
  }

  // EN: Bad — ignoring the return would trigger a compiler warning:
  // TR: Kötü — dönüşü yok saymak derleyici uyarısı tetikler: getErrorCode();  // WARNING:
  // ignoring return value with [[nodiscard]]

  return 0;
}
