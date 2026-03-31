/**
 * @file module_01_basics/08_file_io_and_streams.cpp
 * @brief Basics: File I/O and Stream Operations — Temeller: Dosya G/Ç ve Akış İşlemleri
 *
 * @details
 * =============================================================================
 * [THEORY: C++ I/O Streams / TEORİ: C++ G/Ç Akışları]
 * =============================================================================
 * EN: C++ I/O is built on a stream hierarchy:
 *     1. `std::cin`  → Standard Input  (keyboard).
 *     2. `std::cout` → Standard Output (console).
 *     3. `std::cerr` → Standard Error  (unbuffered, for errors).
 *     4. `std::clog` → Standard Log    (buffered).
 *
 *     For File I/O, `<fstream>` provides:
 *     1. `std::ofstream` → Write to file (Output File Stream).
 *     2. `std::ifstream` → Read from file (Input File Stream).
 *     3. `std::fstream`  → Read AND Write.
 *
 * TR: C++ G/Ç bir akış hiyerarşisi üzerine kurulmuştur:
 *     1. `std::cin`  → Standart Girdi  (klavye).
 *     2. `std::cout` → Standart Çıktı  (konsol).
 *     3. `std::cerr` → Standart Hata   (tamponlanmamış, hatalar için).
 *     4. `std::clog` → Standart Günlük (tamponlanmış).
 *
 *     Dosya G/Ç için `<fstream>` şunları sağlar:
 *     1. `std::ofstream` → Dosyaya yaz (Çıktı Dosya Akışı).
 *     2. `std::ifstream` → Dosyadan oku (Girdi Dosya Akışı).
 *     3. `std::fstream`  → Hem Oku hem de Yaz.
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
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 08_file_io_and_streams.cpp -o 08_file_io_and_streams
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::cout << "=== MODULE 1: FILE I/O & STREAMS ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. CONSOLE OUTPUT FORMATTING / KONSOL ÇIKTI BİÇİMLENDİRME
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. Console Output ---" << std::endl;

  // EN: BUFFERING — std::cout collects characters in an internal buffer and writes
  //     them to the screen in chunks (efficient). `std::endl` does TWO things:
  //         1. Inserts a newline character '\n'.
  //         2. FLUSHES the buffer — forces all pending data to the screen immediately.
  //     Flushing on every line is SLOW (each flush = a system call to the OS).
  //     Plain '\n' only inserts the newline WITHOUT flushing — the buffer is written
  //     when it's full or when the program ends. In performance-critical or embedded
  //     code, prefer '\n' and flush only when you truly need immediate output.
  // TR: TAMPONLAMA — std::cout karakterleri dahili bir tamponda toplar ve ekrana
  //     parçalar halinde yazar (verimli). `std::endl` İKİ şey yapar:
  //         1. '\n' yeni satır karakteri ekler.
  //         2. Tamponu TEMİZLER — bekleyen tüm veriyi hemen ekrana yazar.
  //     Her satırda temizleme YAVAŞTIR (her temizleme = OS'a sistem çağrısı).
  //     Düz '\n' SADECE yeni satır ekler, temizleme YAPMAZ — tampon dolduğunda
  //     veya program bittiğinde yazılır. Performans kritik/gömülü kodda '\n'
  //     tercih edin, yalnızca anında çıktı gerektiğinde flush yapın.
  std::cout << "Line 1 with endl" << std::endl; // Newline + flush (slow)
  std::cout << "Line 2 with \\n\n";              // Newline only, no flush (fast)

  // EN: std::cerr is UNBUFFERED — every character is written immediately, even
  //     if the program crashes right after. This is why errors go to cerr:
  //     if the program segfaults, buffered cout data might be LOST, but cerr
  //     output is guaranteed to have been written. cerr also writes to stderr
  //     (file descriptor 2), which can be redirected separately from stdout.
  //     Example: `./program > output.txt 2> errors.txt`
  // TR: std::cerr TAMPONLANMAMIŞTIR — her karakter hemen yazılır, program hemen
  //     ardından çökse bile. Hataların cerr'e gitmesinin nedeni budur:
  //     program segfault yaparsa, tamponlanmış cout verisi KAYBOLABİLİR, ama
  //     cerr çıktısının yazıldığı GARANTİDİR. cerr ayrıca stderr'e (dosya
  //     tanımlayıcı 2) yazar, stdout'tan ayrı yönlendirilebilir.
  //     Örnek: `./program > output.txt 2> errors.txt`
  std::cerr << "[ERROR] Simulated ECU fault (stderr)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. CONSOLE INPUT (std::cin) / KONSOL GİRDİSİ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. Console Input (std::cin) ---" << std::endl;

  // EN: std::cin reads from stdin (keyboard). The >> operator reads ONE TOKEN
  //     at a time, separated by whitespace (space, tab, newline).
  //     TRAP: If you type "John Doe", `cin >> name` reads only "John"!
  //     The " Doe" part stays in the buffer and pollutes the NEXT read.
  //
  //     Pattern for single value:
  //         int userRpm;
  //         std::cout << "Enter RPM: ";
  //         std::cin >> userRpm;
  //
  //     Pattern for full line (including spaces):
  //         std::string fullLine;
  //         std::getline(std::cin, fullLine);
  //
  //     COMMON BUG: Mixing >> and getline. After `cin >> x`, the '\n' remains
  //     in the buffer. The next `getline()` reads an EMPTY string!
  //     Fix: call `std::cin.ignore()` between >> and getline to discard '\n'.
  //
  // TR: std::cin stdin'den (klavye) okur. >> operatörü boşlukla (boşluk, tab,
  //     satır sonu) ayrılmış seferde TEK TOKEN okur.
  //     TUZAK: "John Doe" yazarsanız, `cin >> name` sadece "John" okur!
  //     " Doe" kısmı tamponda kalır ve SONRAKI okumayı kirletir.
  //
  //     Tek değer kalıbı:
  //         int userRpm;
  //         std::cout << "Enter RPM: ";
  //         std::cin >> userRpm;
  //
  //     Tam satır kalıbı (boşluk dahil):
  //         std::string fullLine;
  //         std::getline(std::cin, fullLine);
  //
  //     YAYGIN HATA: >> ile getline karıştırma. `cin >> x`'ten sonra '\n'
  //     tamponda kalır. Sonraki `getline()` BOŞ string okur!
  //     Çözüm: >> ile getline arasında `std::cin.ignore()` çağırarak '\n' atın.
  //
  std::cout << "[SKIPPED] Interactive cin disabled for automated build.\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. WRITE TO FILE (std::ofstream) / DOSYAYA YAZMA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Write to File (std::ofstream) ---" << std::endl;

  // EN: std::ofstream (Output File Stream) opens a file for WRITING.
  //     DEFAULT BEHAVIOR: If the file exists, it is ERASED and overwritten.
  //     If the file doesn't exist, it is CREATED. This is equivalent to
  //     opening with `std::ios::out | std::ios::trunc` (truncate).
  //
  //     The `{ }` braces create a SCOPE BLOCK. We put the ofstream inside it
  //     so that when we reach the closing `}`, the ofstream destructor runs
  //     and AUTOMATICALLY closes the file. This is RAII (Resource Acquisition
  //     Is Initialization) — the most important C++ idiom for resource safety.
  //     No need to call `.close()` manually — the destructor handles it.
  //
  // TR: std::ofstream (Çıkış Dosya Akışı) bir dosyayı YAZMA için açar.
  //     VARSAYILAN DAVRANIŞ: Dosya varsa SİLİNİR ve üzerine yazılır.
  //     Dosya yoksa OLUŞTURULUR. Bu, `std::ios::out | std::ios::trunc`
  //     (kesme) ile açmaya eşdeğerdir.
  //
  //     `{ }` süslü parantezler bir KAPSAM BLOĞU oluşturur. ofstream'i içine
  //     koyarız — kapanış `}` satırına ulaştığımızda ofstream yıkıcısı çalışır
  //     ve dosyayı OTOMATİK KAPATIR. Bu RAII'dir (Kaynak Edinimi Başlatmadır)
  //     — kaynak güvenliği için en önemli C++ deyimidir.
  //     `.close()` çağırmaya gerek yok — yıkıcı halleder.
  const std::string filename = "/tmp/ecu_dtc_log.txt";
  {
    std::ofstream outFile(filename);

    // EN: ALWAYS check is_open() after creating a file stream! Opening can
    //     FAIL SILENTLY for many reasons: bad path, no write permission,
    //     disk full, directory doesn't exist. If you write to a failed stream,
    //     the data simply VANISHES — no crash, no error, just silent data loss.
    // TR: Dosya akışı oluşturduktan sonra HER ZAMAN is_open() kontrol edin!
    //     Açma birçok nedenle SESSİZCE BAŞARISız olabilir: geçersiz yol, yazma
    //     izni yok, disk dolu, dizin mevcut değil. Başarısız akışa yazarsanız
    //     veri sessizce KAYBOLUR — çökme yok, hata yok, sadece sessiz veri kaybı.
    if (!outFile.is_open()) {
      std::cerr << "[ERROR] Cannot open file for writing: " << filename
                << std::endl;
      return 1;
    }

    // EN: The << operator works exactly like std::cout — because both inherit
    //     from std::ostream. Anything you can print to console, you can write
    //     to a file with the same syntax.
    // TR: << operatörü aynen std::cout gibi çalışır — çünkü ikisi de
    //     std::ostream'den türer. Konsola yazdırabildiğiniz her şeyi aynı
    //     sözdizimi ile dosyaya yazabilirsiniz.
    outFile << "=== ECU DTC LOG ===" << std::endl;
    outFile << "DTC: P0300 — Random/Multiple Cylinder Misfire Detected"
            << std::endl;
    outFile << "DTC: P0171 — System Too Lean (Bank 1)" << std::endl;
    outFile << "DTC: P0420 — Catalyst Efficiency Below Threshold" << std::endl;
    outFile << "DTC: P0442 — EVAP System Leak Detected (Small)" << std::endl;
    outFile << "Total DTCs: 4" << std::endl;

    std::cout << "Written 4 DTCs to: " << filename << std::endl;

    // EN: When this closing brace `}` is reached, `outFile` goes out of scope.
    //     Its destructor runs → calls close() → flushes buffer → releases file handle.
    //     This is RAII in action: you CANNOT forget to close the file.
    // TR: Bu kapanış süslü parantezine `}` ulaşıldığında `outFile` kapsam dışına çıkar.
    //     Yıkıcısı çalışır → close() çağırır → tamponu temizler → dosya tanıtıcısını serbest bırakır.
    //     Bu RAII'nin uygulamasıdır: dosyayı kapatmayı UNUTAMAZSINIZ.
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. READ FROM FILE (std::ifstream) / DOSYADAN OKUMA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 4. Read from File (std::ifstream) ---" << std::endl;

  {
    // EN: std::ifstream (Input File Stream) opens a file for READING.
    //     If the file doesn't exist, is_open() returns false — no exception thrown.
    // TR: std::ifstream (Girdi Dosya Akışı) bir dosyayı OKUMA için açar.
    //     Dosya yoksa is_open() false döner — istisna fırlatılmaz.
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
      std::cerr << "[ERROR] Cannot open file for reading: " << filename
                << std::endl;
      return 1;
    }

    // EN: std::getline(stream, string) reads characters until it finds '\n'.
    //     It stores everything EXCEPT the '\n' into `line`, then discards '\n'.
    //     When the stream reaches EOF (End Of File), getline returns false
    //     and the while loop stops.
    //
    //     WHY getline INSTEAD OF >>?
    //     `inFile >> word` reads ONE WORD (stops at whitespace).
    //     `getline(inFile, line)` reads the ENTIRE LINE (including spaces).
    //     For log files, config files, CSV — you almost always want getline.
    //
    // TR: std::getline(akış, string) '\n' bulana kadar karakterleri okur.
    //     '\n' HARİÇ her şeyi `line`'a kaydeder, sonra '\n'i atar.
    //     Akış EOF'a (Dosya Sonu) ulaşınca getline false döner ve while döngüsü durur.
    //
    //     NEDEN >> YERİNE getline?
    //     `inFile >> word` TEK KELİME okur (boşlukta durur).
    //     `getline(inFile, line)` TÜM SATIRI okur (boşluklar dahil).
    //     Log, config, CSV dosyaları için neredeyse her zaman getline istersiniz.
    std::string line;
    int lineNum = 1;
    while (std::getline(inFile, line)) {
      std::cout << "  [L" << lineNum << "] " << line << std::endl;
      ++lineNum;
    }

    std::cout << "Read " << (lineNum - 1) << " lines from: " << filename
              << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. APPEND TO FILE / DOSYAYA EKLEME
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 5. Append to File ---" << std::endl;

  {
    // EN: The second parameter controls the OPEN MODE. Important modes:
    //     std::ios::out   → Write (default for ofstream). ERASES existing content.
    //     std::ios::app   → Append. Writes are always added to the END of the file.
    //                       Existing content is PRESERVED. File is created if absent.
    //     std::ios::ate   → Open and seek to End, but you CAN seek back (unlike app).
    //     std::ios::in    → Read (default for ifstream).
    //     std::ios::binary→ Binary mode (no text translation of \n ↔ \r\n).
    //     Modes can be combined with `|`: `std::ios::out | std::ios::binary`
    //
    //     KEY DIFFERENCE — app vs ate:
    //     app: Every write goes to EOF. Even if you seekp() backward, the write
    //          still appends. This is SAFE for concurrent logging.
    //     ate: Opens at EOF, but you CAN seekp() to any position and overwrite.
    //
    // TR: İkinci parametre AÇMA MODUNU kontrol eder. Önemli modlar:
    //     std::ios::out   → Yaz (ofstream varsayılanı). Mevcut içeriği SİLER.
    //     std::ios::app   → Ekle. Yazılar her zaman dosya SONUNA eklenir.
    //                       Mevcut içerik KORUNUR. Dosya yoksa oluşturulur.
    //     std::ios::ate   → Aç ve Sona git, ama geri seek YAPABİLİRSİNİZ (app'ten farkı).
    //     std::ios::in    → Oku (ifstream varsayılanı).
    //     std::ios::binary→ İkili mod ('\n' ↔ '\r\n' metin çevrimi yok).
    //     Modlar `|` ile birleştirilebilir: `std::ios::out | std::ios::binary`
    //
    //
    //     KRİTİK FARK — app vs ate:
    //     app: Her yazma EOF'a gider. seekp() geriye gitseler bile yazma yine ekler.
    //          Eşzamanlı loglama için GÜVENLİDİR.
    //     ate: EOF'ta açılır ama seekp() ile herhangi bir konuma GİDİP üzerine yazabilirsiniz.
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

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. RE-READ TO VERIFY APPEND / EKLEME DOĞRULAMA İÇİN TEKRAR OKU
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 6. Verify Append ---" << std::endl;

  // EN: Re-read the entire file to confirm that Section 5's append worked.
  //     The file should now contain the original 4 DTCs PLUS the 1 we appended.
  //     This pattern (write → close → re-read) is common in testing/validation.
  // TR: Bölüm 5'teki eklemenin çalıştığını doğrulamak için dosyanın tamamını tekrar oku.
  //     Dosya artık orijinal 4 DTC ARTI eklediğimiz 1 DTC'yi içermelidir.
  //     Bu kalıp (yaz → kapat → tekrar oku) test/doğrulamada yaygındır.
  {
    std::ifstream verifyFile(filename);
    if (verifyFile.is_open()) {
      std::string line;
      while (std::getline(verifyFile, line)) {
        std::cout << "  " << line << std::endl;
      }
    }
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 7. FILE STATUS CHECKS / DOSYA DURUM KONTROLLERİ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 7. File Status Checks ---" << std::endl;

  // EN: Every stream has a set of STATE FLAGS you can query:
  //     good()    → True if NO errors at all. The stream is ready for I/O.
  //     fail()    → True if a logical error occurred (wrong type read, open failed).
  //                 Also true when badbit is set.
  //     bad()     → True if an IRRECOVERABLE I/O error occurred (disk failure).
  //     eof()     → True if the stream reached End Of File.
  //     is_open() → True if the underlying file was successfully associated.
  //
  //     A stream used in a boolean context (e.g., `if (inFile)`) checks !fail().
  //     This is why `while (std::getline(inFile, line))` stops at EOF — getline
  //     sets eofbit on the stream, which makes fail() return true.
  //
  // TR: Her akışın sorgulayabileceğiniz DURUM BAYRAKLARI vardır:
  //     good()    → Hiç hata yoksa true. Akış G/Ç'ye hazır.
  //     fail()    → Mantıksal hata oluştuysa true (yanlış tip okundu, açma başarısız).
  //                 badbit ayarlıysa da true döner.
  //     bad()     → KURTARILAMAZ G/Ç hatası oluştuysa true (disk arızası).
  //     eof()     → Akış Dosya Sonuna ulaştığında true.
  //     is_open() → Alttaki dosya başarıyla ilişkilendirildiyse true.
  //
  //     Boolean bağlamda bir akış (ör. `if (inFile)`) !fail() kontrol eder.
  //     `while (std::getline(inFile, line))` EOF'ta durmasının nedeni budur —
  //     getline akışta eofbit ayarlar, bu fail()'in true dönmesini sağlar.

  // EN: Test with a file that does NOT exist:
  // TR: VAR OLMAYAN bir dosya ile test:
  std::ifstream badFile("/tmp/non_existent_file_xyz.txt");
  std::cout << "Bad file is_open(): " << badFile.is_open() << std::endl;
  std::cout << "Bad file good()   : " << badFile.good() << std::endl;
  std::cout << "Bad file fail()   : " << badFile.fail() << std::endl;

  // EN: Test with a file that DOES exist (we created it in Section 3):
  // TR: VAR OLAN bir dosya ile test (Bölüm 3'te oluşturduk):
  std::ifstream goodFile(filename);
  std::cout << "Good file is_open(): " << goodFile.is_open() << std::endl;
  std::cout << "Good file good()   : " << goodFile.good() << std::endl;

  return 0;
}
