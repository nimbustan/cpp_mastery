/**
 * @file module_01_basics/08_file_io_and_streams.cpp
 * @brief Basics: File I/O and Stream Operations / Temeller: Dosya G/Ç ve Akış İşlemleri
 *
 * @details
 * =============================================================================
 * [THEORY: C++ I/O Streams / TEORİ: C++ G/Ç Akışları]
 * =============================================================================
 * EN: C++ I/O is built on a stream hierarchy: - `std::cin`  → Standard Input  (keyboard). -
 * `std::cout` → Standard Output (console). - `std::cerr` → Standard Error  (unbuffered, for
 * errors). - `std::clog` → Standard Log    (buffered).
 *
 * For File I/O, `<fstream>` provides: - `std::ofstream` → Write to file (Output File Stream). -
 * `std::ifstream` → Read from file (Input File Stream). - `std::fstream`  → Read AND Write.
 *
 * TR: C++ G/Ç bir akış hiyerarşisi üzerine kurulmuştur: - `std::cin`  → Standart Girdi 
 * (klavye). - `std::cout` → Standart Çıktı  (konsol). - `std::cerr` → Standart Hata  
 * (tamponlanmamış, hatalar için). - `std::clog` → Standart Günlük (tamponlanmış).
 *
 * Dosya G/Ç için `<fstream>` şunları sağlar: - `std::ofstream` → Dosyaya yaz (Çıktı Dosya
 * Akışı). - `std::ifstream` → Dosyadan oku (Girdi Dosya Akışı). - `std::fstream`  → Hem Oku hem
 * de Yaz.
 *
 * =============================================================================
 * [CPPREF DEPTH: RAII and File Handles / CPPREF DERİNLİK: RAII ve Dosya Tanıtıcıları]
 * =============================================================================
 * EN: `std::ofstream` and `std::ifstream` use RAII: the file is automatically closed when the
 * object goes out of scope (destructor runs). You do NOT need to call `.close()` explicitly if
 * the object is about to be destroyed. Calling `.close()` is only needed if you want to reopen
 * or reuse the stream.
 *
 * The `is_open()` check is critical — opening a file can FAIL silently (bad path, permissions),
 * and writing to a failed stream is Undefined Behavior.
 *
 * TR: `std::ofstream` ve `std::ifstream` RAII kullanır: nesne kapsam dışına çıktığında dosya
 * otomatik kapatılır (yıkıcı çalışır). Nesne yok edilecekse `.close()` çağırmanıza GEREK YOKTUR.
 * `.is_open()` kontrolü kritiktir — dosya açma sessizce başarısız olabilir.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/io/basic_ofstream
 * ⚠  Ref: https://en.cppreference.com/w/cpp/io/basic_ifstream
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::cout << "=== MODULE 1: FILE I/O & STREAMS ===\n" << std::endl;

  // ===============================================================================================
  // 1. CONSOLE OUTPUT FORMATTING / KONSOL ÇIKTI BİÇİMLENDİRME
  // ===============================================================================================
  std::cout << "--- 1. Console Output ---" << std::endl;

  // EN: std::endl flushes the buffer (slow). '\n' does not (fast). In embedded: prefer '\n' for
  // performance.
  // TR: std::endl tamponu temizler (yavaş). '\n' temizlemez (hızlı).
  std::cout << "Line 1 with endl" << std::endl; // Flushes
  std::cout << "Line 2 with \\n\n";              // No flush (faster)

  // EN: std::cerr is unbuffered — used for critical error messages.
  // TR: std::cerr tamponlanmamıştır — kritik hata mesajları için kullanılır.
  std::cerr << "[ERROR] Simulated ECU fault (stderr)\n" << std::endl;

  // ===============================================================================================
  // 2. CONSOLE INPUT (std::cin) / KONSOL GİRDİSİ
  // ===============================================================================================
  std::cout << "--- 2. Console Input (std::cin) ---" << std::endl;

  // EN: We skip interactive cin in automated builds. Showing the pattern:
  // TR: Otomatik derlemelerde interaktif cin atlıyoruz. Kalıbı gösteriyoruz:
  //
  // int userRpm; std::cout << "Enter RPM: "; std::cin >> userRpm; std::cout << "You entered: "
  // << userRpm << std::endl;
  //
  // EN: For reading full lines (including spaces):
  // TR: Boşluk içeren satır okumak için: std::string fullLine; std::getline(std::cin, fullLine);
  // std::cout << "Full line: " << fullLine << std::endl;
  //
  std::cout << "[SKIPPED] Interactive cin disabled for automated build.\n"
            << std::endl;

  // ===============================================================================================
  // 3. WRITE TO FILE (std::ofstream) / DOSYAYA YAZMA
  // ===============================================================================================
  std::cout << "--- 3. Write to File (std::ofstream) ---" << std::endl;

  // EN: Create/overwrite a DTC log file. RAII: auto-closes on scope exit.
  // TR: Bir DTC log dosyası oluştur/üzerine yaz. RAII: kapsam çıkışında otomatik kapanır.
  const std::string filename = "/tmp/ecu_dtc_log.txt";
  {
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
      std::cerr << "[ERROR] Cannot open file for writing: " << filename
                << std::endl;
      return 1;
    }

    // EN: Writing DTC records to file.
    // TR: DTC kayıtlarını dosyaya yazma.
    outFile << "=== ECU DTC LOG ===" << std::endl;
    outFile << "DTC: P0300 — Random/Multiple Cylinder Misfire Detected"
            << std::endl;
    outFile << "DTC: P0171 — System Too Lean (Bank 1)" << std::endl;
    outFile << "DTC: P0420 — Catalyst Efficiency Below Threshold" << std::endl;
    outFile << "DTC: P0442 — EVAP System Leak Detected (Small)" << std::endl;
    outFile << "Total DTCs: 4" << std::endl;

    std::cout << "Written 4 DTCs to: " << filename << std::endl;
    // EN: outFile is auto-closed here (RAII destructor).
    // TR: outFile burada otomatik kapatılır (RAII yıkıcı).
  }

  // ===============================================================================================
  // 4. READ FROM FILE (std::ifstream) / DOSYADAN OKUMA
  // ===============================================================================================
  std::cout << "\n--- 4. Read from File (std::ifstream) ---" << std::endl;

  {
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
      std::cerr << "[ERROR] Cannot open file for reading: " << filename
                << std::endl;
      return 1;
    }

    // EN: Read line by line using std::getline.
    // TR: std::getline ile satır satır oku.
    std::string line;
    int lineNum = 1;
    while (std::getline(inFile, line)) {
      std::cout << "  [L" << lineNum << "] " << line << std::endl;
      ++lineNum;
    }

    std::cout << "Read " << (lineNum - 1) << " lines from: " << filename
              << std::endl;
  }

  // ===============================================================================================
  // 5. APPEND TO FILE / DOSYAYA EKLEME
  // ===============================================================================================
  std::cout << "\n--- 5. Append to File ---" << std::endl;

  {
    // EN: std::ios::app opens the file in Append mode (doesn't erase).
    // TR: std::ios::app dosyayı Ekleme modunda açar (silmez).
    std::ofstream appendFile(filename, std::ios::app);

    if (!appendFile.is_open()) {
      std::cerr << "[ERROR] Cannot open file for appending." << std::endl;
      return 1;
    }

    appendFile << "DTC: P0128 — Coolant Temperature Below Thermostat Range"
               << std::endl;
    appendFile << "Total DTCs: 5 (updated)" << std::endl;

    std::cout << "Appended 1 more DTC to: " << filename << std::endl;
  }

  // ===============================================================================================
  // 6. RE-READ TO VERIFY APPEND / EKLEME DOĞRULAMA İÇİN TEKRAR OKU
  // ===============================================================================================
  std::cout << "\n--- 6. Verify Append ---" << std::endl;

  {
    std::ifstream verifyFile(filename);
    if (verifyFile.is_open()) {
      std::string line;
      while (std::getline(verifyFile, line)) {
        std::cout << "  " << line << std::endl;
      }
    }
  }

  // ===============================================================================================
  // 7. FILE STATUS CHECKS / DOSYA DURUM KONTROLLERİ
  // ===============================================================================================
  std::cout << "\n--- 7. File Status Checks ---" << std::endl;

  std::ifstream badFile("/tmp/non_existent_file_xyz.txt");
  std::cout << "Bad file is_open(): " << badFile.is_open() << std::endl;
  std::cout << "Bad file good()   : " << badFile.good() << std::endl;
  std::cout << "Bad file fail()   : " << badFile.fail() << std::endl;

  std::ifstream goodFile(filename);
  std::cout << "Good file is_open(): " << goodFile.is_open() << std::endl;
  std::cout << "Good file good()   : " << goodFile.good() << std::endl;

  return 0;
}
