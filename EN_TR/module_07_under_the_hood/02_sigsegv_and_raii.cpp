/**
 * @file module_07_under_the_hood/02_sigsegv_and_raii.cpp
 * @brief Under the Hood: Segmentation Fault (SIGSEGV) & Prevention via RAII — İşletim Sistemi
 * Seviyesi: SIGSEGV ve RAII
 *
 * @details
 * =============================================================================
 * [THEORY: Why do we get SIGSEGV (Crash)? / TEORİ: Neden SIGSEGV (Çöktü) Hatası Alırız?]
 * =============================================================================
 * EN: The OS allocates specific RAM pages for your C++ program. If you try to read or write to a
 * memory address that the OS DID NOT GIVE YOU (like `nullptr`, or an array index that is out of
 * bounds), the hardware's MMU (Memory Management Unit) sends an interrupt to the OS. The OS
 * immediately ASSASSINATES your program with a signal called `SIGSEGV` (Segmentation Violation).
 *
 * TR: İşletim Sistemi (OS), C++ programınıza sadece belirli bir RAM hafızası (Sayfa) verir. Eğer
 * size ait OLMAYAN bir adrese (Örn: Donanımın adresine veya 'nullptr' içi boş adrese) dokunmaya,
 * okumaya veya yazmaya çalışırsanız, işlemcinin Donanımsal Bellek Yöneticisi (MMU) bunu yakalar.
 * İşletim sistemi anında programınıza suikast düzenler ve `SIGSEGV` (Segmentation Violation)
 * sinyali yollayarak UYGULAMAYI ZORLA KAPATIR!
 *
 * =============================================================================
 * [THEORY: RAII - How it saves lives! / TEORİ: RAII Nedir ve Hayatımızı Nasıl Kurtarır?]
 * =============================================================================
 * EN: RAII = "Resource Acquisition Is Initialization". Manual `new` and `delete` leads to 90% of
 * human errors. If you forget `delete`, you get a Memory Leak. If you delete twice, you get a
 * Double Free Crash. RAII dictates: "Entrust the Resource (RAM/File/Mutex) to a C++ Object. When
 * the object goes out of scope {...}, its Destructor AUTOMATICALLY CLEANS IT UP." (e.g.,
 * std::unique_ptr, std::vector).
 *
 * TR: RAII = "Kaynak Edinimi Başlatmadır". Manuel `new` ve `delete` kullanımı hataların %90'ına
 * yol açar. `delete` yapmayı unutursanız Bellek Sızıntısı (Memory Leak) olur. İki kere
 * silerseniz Çift Silme Hatası (Double Free/SIGSEGV) olur. RAII der ki: "RAM/Dosya/Kilit
 * kaynağını C++ Objelerine emanet et. Nesne süslü parantez dışına çıkıp öldüğünde, KENDİ KENDİNİ
 * OTOMATİK SİLSİN." (Örn: std::unique_ptr, std::vector).
 *
 * =============================================================================
 * [CPPREF DEPTH: OS Core Dumps (Program Cesetleri) / CPPREF DERİNLİK: İşletim Sistemi Bellek
 * Dökümleri (Program Cesetleri)]
 * =============================================================================
 * EN: When SIGSEGV happens, Linux doesn't just kill the program. It violently dumps the entire
 * RAM state (Memory map, registers) into a massive file called a "Core Dump" (The corpse of your
 * program). You can later load this Core Dump into GDB (`gdb ./app core`) to do an autopsy and
 * see exactly which line of C++ triggered the assassination!
 *
 * TR: SIGSEGV olduğunda Linux sadece programı kapatıp geçmez. Çöktüğü andaki tüm İşlemci
 * yazmaçlarını (Registers) ve RAM haritasını "Core Dump" (Çekirdek Dökümü - Programın Cesedi)
 * denen devasa bir dosyaya yazar. Daha sonra GDB hata ayıklayıcısına bu cesedi verip (`gdb
 * ./uygulama core`) otopsi yapabilir, programınızın tam olarak hangi C++ satırından vurulduğunu
 * görebilirsiniz!
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_sigsegv_and_raii.cpp -o 02_sigsegv_and_raii
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <vector>

// EN: Dangerous legacy C-style function. WARNING: SIGSEGV RISK!
// TR: Tehlikeli (Eski C Tarzi) bir fonksiyon. DİKKAT: SIGSEGV RİSKİ!
void dangerousMemoryAccess() {
  int *garbageAddress = nullptr; // Address 0x0000000
  // EN: Suppress warning — demo only
  // TR: Uyarıyı bastır — yalnızca demo
  (void)garbageAddress;

  // EN: BURIED SIGSEGV TRAP: If you uncomment the line below, the program is ASSASSINATED
  // INSTANTLY!
  // TR: GÖMÜLÜ SIGSEGV TUZAĞI: Aşağıdaki satır açılırsa Program ANINDA İNFAZ EDİLİR!
  //
  // *garbageAddress = 55; // SIGSEGV (Segmentation Fault: 11)!!!

  // EN: Why? Because the OS did not grant us permission to write to Address 0.
  // TR: Neden? Cünkü İşletim Sistemi bize 0. (Yokluk) adresine yazmamız için HAK VERMEDİ.
}

// EN: Memory Leak Trap (What happens without RAII?)
// TR: Memory Leak Tuzağı. (RAII kullanılmazsa ne olur?)
void createAndForget() {
  // EN: Stole 2000 Bytes from OS!
  // TR: OS'den 2000 Bayt çaldık!
  int *array = new int[500];

  // EN: Imagine an error in the control loop
  // TR: Kontrol döngüsünde hata çıktı.
  bool randomErrorOccurred = true;

  if (randomErrorOccurred) {
    // EN: WARNING! We escape before reaching 'delete[] array'. RAM is lost forever (Leak).
    // TR: DİKKAT! 'delete[] array;' koduna asla gelemeden fonksiyondan kaçtık! RAM sızdı.
    return;
  }

  // EN: Would have freed it if no error happened...
  // TR: Hata olmasaydı silinecekti...
  delete[] array;
}

// EN: ------ THE ARCHITECTURE OF MODERN C++: RAII ------
// TR: ------ MÜKEMMEL C++ MİMARİSİ: RAII ------
class ActuatorController {
public:
  ActuatorController() {
    std::cout << "[RAII] Actuator initialized (Memory Acquired). / Aktüatör başlatıldı." <<
        std::endl;
  }
  ~ActuatorController() {
    std::cout << "[RAII] Actuator went out of scope! AUTO-RELEASED!!" <<
        "\n       (Aktüatör kapsam dışına çıktı! OTOMATIK SERBEST BIRAKILIYOR!)" << std::endl;
  }
};

int main() {
  std::cout << "=== MODULE 7: SIGSEGV (SEGMENTATION FAULT) & RAII ===\n" << std::endl;

  std::cout << "\n1. Risk of SIGSEGV (SIGSEGV Riski ve Neden Çıkar?)" << std::endl;
  std::cout << "EN: The OS won't let you write to unowned RAM. It shoots you "
               "with SIGSEGV."
            << std::endl;
  std::cout << "TR: İşletim Sistemi, hakkiniz olmayan RAM adresine yazmaniza "
               "anında SIGSEGV vurur."
            << std::endl;

  dangerousMemoryAccess();

  std::cout << "\n2. The RAII Solution (RAII Çözümü)" << std::endl;

  // EN: Creating a Scope { ... }
  // TR: Süslü parantez ile (Scope / Kapsam) yaratıyoruz.
  {
    // EN: STEP 1: Smart Pointer is the backbone of C++14 RAII. NO NEW, NO DELETE.
    // TR: ADIM 1: "Smart Pointer", C++14'ün RAII silahidir. Asla `new` veya `delete`
    // kullanılmaz.
    std::unique_ptr<ActuatorController> safeObject = std::make_unique<ActuatorController>();

    // EN: STEP 2: std::vector is historically a RAII container itself.
    // TR: ADIM 2: std::vector doğuştan bir RAII'dir.
    std::vector<int> smartArray(500, 10); // 2000 Bytes managed automatically!

    std::cout << "  -> Inside the Scope, everything is fine... (Kapsam içindeyiz...)" << std::endl;

    bool suddenCodeCrash = true;
    if (suddenCodeCrash) {
      std::cout << "  -> ERROR BROKE THE FLOW! ESCAPING! (HATA İLE KOD "
                   "KIRILDI! KACIYORUZ!)"
                << std::endl;
      // EN: Without RAII, breaking here would mean severe leaking...
      // TR: Eğer RAII olmasaydı 'return/throw' yüzünden RAM sızacaktı...
    }

  // EN: THIS IS RAII! The moment this bracket closes, EVERYTHING is returned to OS
  // automatically!
  // TR: İşTE RAII! Parantez kapandığı an, İşletim Sisteminden alınan HDD/RAM her şey İADE
  // EDİLİR!
  }

  std::cout << "  -> Returned outside safely with 0 Bytes leaked. (Dışarıya "
               "Sifir Cöple dönüldü.)"
            << std::endl;

  return 0;
}
