/**
 * @file module_08_core_guidelines/02_resource_management_rule_of_zero.cpp
 * @brief C++ Core Guidelines: Resource Management & Rule of Zero — Kaynak Yönetimi Mimarisi
 *
 * @details
 * =============================================================================
 * [INTRODUCTION: Rule of Zero & RAII / GİRİŞ: Sıfır Kuralı ve RAII]
 * =============================================================================
 * EN: The Guidelines say: "Manage resources automatically using resource handles and RAII."
 *
 * What is the "Rule of Zero"? If your class only manages standard modern resources (like
 * `std::string`, `std::vector`, `std::unique_ptr`), you SHOULD NOT write a Destructor
 * (`~MyClass()`), Copy Constructor, or Assignment Operator. The compiler's default automatically
 * handles memory perfectly. ZERO custom memory-management code = ZERO memory leak bugs!
 *
 * TR: Çekirdek Kurallar der ki: "Kaynakları (Belleği) otomatik olarak RAII kılıflarıyla yönet!"
 *
 * "Sıfır Kuralı (Rule of Zero)" Nedir? Eğer sizin yazdığınız sınıf, belleği kendi yöneten akıllı
 * nesneler (std::vector, std::string) kuruyorsa, BAŞKA HİÇBİR ŞEY YAZMAYIN! Derleyicinin
 * üreteceği otomatik Yıkıcı/Kurucu kodlar zaten KUSURSUZ çalışır. O (Sıfır) kod yazarsanız = 0
 * (Sıfır) Bellek Kaçağı üretirsiniz!
 *
 * =============================================================================
 * [R.10: Avoid malloc() and free() / R.10: Malloc ve Free KESİNLİKLE YASAK]
 * =============================================================================
 * EN: `malloc()` does not call Constructors! It grabs raw, dumb bytes from RAM. `free()` does
 * not call Destructors! It leaves sub-resources orphaned. NEVER use them.
 *
 * TR: Eski C fonksiyonu `malloc()`, C++ Sınıflarını "Constructor" çağırmadan ucube gibi yaşama
 * döndürür. `free()` ise Sınıf içindeki işaretçileri umursamadan öldürür ve RAM çöp olur.
 *
 * [CPPREF DEPTH: Rule of Zero — Let the Compiler Write Your Special Members / CPPREF DERİNLİK:
 * Sıfır Kuralı — Özel Üyeleri Derleyiciye Bırakın]
 * =============================================================================
 * EN: If every data member is an RAII type (std::string, std::vector, std::unique_ptr,
 * std::shared_ptr), the compiler-generated destructor, copy constructor, copy assignment, move
 * constructor, and move assignment are all correct — you need to write NONE of them. This is the
 * Rule of Zero. The moment you define ANY special member (even a destructor), the compiler
 * suppresses implicit generation of the move operations. Writing `= default` explicitly requests
 * compiler generation and documents your intent. The Rule of Zero dramatically reduces bugs
 * because hand-written special members are a top source of resource leaks, double-frees, and
 * slicing errors. Reserve the Rule of Five for low-level resource handles only.
 *
 * TR: Tüm veri üyeleri RAII tipi (std::string, std::vector, std::unique_ptr, std::shared_ptr)
 * ise derleyicinin ürettiği yıkıcı, kopyalama kurucusu, kopyalama ataması, taşıma kurucusu ve
 * taşıma ataması zaten doğrudur — HİÇBİRİNİ yazmanız gerekmez (Sıfır Kuralı). HERHANGİ bir özel
 * üye tanımladığınız anda (yıkıcı bile olsa) derleyici taşıma işlemlerini otomatik üretmez. `=
 * default` derleyici üretimini açıkça talep eder. Sıfır Kuralı hataları büyük ölçüde azaltır
 * çünkü elle yazılan özel üyeler kaynak sızıntısı, çift serbest bırakma ve dilimleme hatalarının
 * en büyük kaynağıdır.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_resource_management_rule_of_zero.cpp -o 02_resource_management_rule_of_zero
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// -------------------------------------------------------------------------------------------------
// [C.20, R.1] THE RULE OF ZERO (Sıfır Kuralı Mimarisi)
// -------------------------------------------------------------------------------------------------
// EN: This class flawlessly manages its own memory thanks to the Rule of Zero.
// TR: Bu sınıf, 'Sıfır Kuralı' sayesinde kendi belleğini kusursuz yönetir.
class VehicleConfigManager {
private:
  std::string vehicleId;                  // EN: Self-managed (RAII)
  std::vector<std::string> ecuModuleNames; // EN: Self-managed (RAII)

public:
  // Simple constructor to initialize data.
  // TR: Constructor sadece ilk değer ataması içindir.
  VehicleConfigManager(const std::string &id) : vehicleId(id) {
    std::cout << "[VehicleConfig] Initialized: " << vehicleId << std::endl;
  }

  void addECU(const std::string &name) { ecuModuleNames.push_back(name); }

  // EN: NOTICE WHAT IS MISSING? - NO `~VehicleConfigManager()` Destructor! - NO `new` or
  // `delete` anywhere! -> When this object naturally dies, `std::vector` and `std::string` clean
  // themselves up. 100% Leak-Proof!
  //
  // TR: FARK ETTİNİZ Mİ NE EKSİK? - Yıkıcı `~VehicleConfigManager()` fonksiyonu YOK! - El ile
  // yazılmış `new` veya `delete` YOK! -> Obje ölürken, içindeki objeler (vector, string) C++
  // mantığıyla OS'ye iade edilir.
};

int main() {
  std::cout << "=== MODULE 8: C++ CORE GUIDELINES - RULE OF ZERO & RESOURCE "
               "MGMT ===\n"
            << std::endl;

  {
    std::cout << "-> Entering local scope... (Kapsama giriliyor...)" << std::endl;

    // 1. Applying Rule of Zero
    VehicleConfigManager myCar("VIN_WBAPH5C55BA271234");
    myCar.addECU("BCM");
    myCar.addECU("Powertrain");

    // 2. Applying R.11 (Avoid new and delete explicitly)
    // EN: NEVER write -> VehicleConfigManager* v = new VehicleConfigManager(...);
    // TR: ASLA yazma -> VehicleConfigManager* v = new VehicleConfigManager(...);

    std::unique_ptr<VehicleConfigManager> smartConfig =
        std::make_unique<VehicleConfigManager>("VIN_JH4DB7660SS001234");

  } // EN: BOTH 'myMatch' and 'smartMatchLocation' auto-destruct HERE safely.
    // TR: Buradaki '}' anında, akıllı işaretçi ve stack objesi muazzam bir sessizlikle iade
    // edilir.

  std::cout << "-> Left local scope safely. No manual memory management required!\n"
         "-> (Sıfır sızıntı! Amale gibi delete[] kontrolü yapmaya gerek "
         "kalmadı!)\n"
      << std::endl;

  std::cout << "\n[ARCHITECTURAL CONCLUSION / MİMARİ SONUÇ]:" << std::endl;
  std::cout << "- EN: C++ Core Guidelines states: 'Rule of Zero'. Fall back "
               "entirely on std::vector and Smart Pointers."
            << std::endl;
  std::cout << "- TR: (Manuel bellek işine hiç girmemeyi (Rule of Zero) emreder!)" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: R.1 — Manage resources automatically using RAII
  // TR: R.1 — Kaynakları RAII kullanarak otomatik yönet
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ─── 1. RAII FileHandle wrapper ────────────────────────────────────────────────────────────────
  // EN: Constructor acquires, Destructor releases. No manual close needed.
  // TR: Constructor edinir, Destructor serbest bırakır. Manuel kapatma yok.
  struct FileHandle {
    std::string filename;
    bool isOpen = false;

    explicit FileHandle(const std::string& fname)
        : filename(fname), isOpen(true) {
      std::cout << "[R.1 RAII] Opened file: " << filename << std::endl;
    }

    ~FileHandle() {
      if (isOpen) {
        isOpen = false;
        std::cout << "[R.1 RAII] Auto-closed file: " << filename << std::endl;
      }
    }

    // EN: Delete copy to enforce single ownership (move-only resource).
    // TR: Tek sahiplik için kopyalamayı sil (sadece taşınabilir kaynak).
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
  };

  // ─── 2. Demo: automatic cleanup even on early return ───────────────────────────────────────────
  // EN: Even if we exit the scope early, destructor is guaranteed to run.
  // TR: Kapsamdan erken çıksak bile, yıkıcı çalışması garanti edilir.
  {
    FileHandle logFile("telemetry_log.csv");
    std::cout << "[R.1 Demo] Processing file: " << logFile.filename << std::endl;

    bool earlyExit = true;
    if (earlyExit) {
      std::cout << "[R.1 Demo] Early return from scope!" << std::endl;
      // EN: ~FileHandle() runs automatically RIGHT HERE at '}'
      // TR: ~FileHandle() otomatik olarak '}' noktasında çalışır
    }
  } // EN: Destructor fires here — file is auto-closed, no leak.
    // TR: Yıkıcı burada çalışır — dosya otomatik kapatılır, sızıntı yok.

  std::cout << "[R.1 Demo] Scope exited — FileHandle cleaned up automatically!" << std::endl;

  return 0;
}
