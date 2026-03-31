/**
 * @file module_02_oop/03_rule_of_three_five.cpp
 * @brief OOP Part 3: Memory Leaks and "Rule of 3 — 5" — OOP Kisim 3: Bellek Sizintilari ve
 * 3'lu/5'li Kurali
 *
 * @details
 * =============================================================================
 * [THEORY: Why does the Rule of 0/3/5 exist? / TEORİ: 0/3/5 Kuralı neden var? (Sığ Kopya Krizi)]
 * =============================================================================
 * EN: When you create a basic Class, the C++ compiler automatically generates 3 things for you:
 *   1. A default Destructor
 *   2. A default Copy Constructor (Sensor s2 = s1;)
 *   3. A default Copy Assignment Operator (p2 = p1;)
 *
 * This is fine for simple variables (ints, floats). BUT! If your class has a POINTER that
 * allocates memory dynamically (using `new` or `malloc`), the default copy performs a "Shallow
 * Copy". Both objects will point to the SAME memory! When they are destroyed, they will "Double
 * Free" and crash!
 *
 * TR: Temel bir sınıf yazdığınızda, C++ derleyicisi otomatik olarak 3 şey üretir:
 *   1. Varsayılan Yıkıcı (Destructor)
 *   2. Varsayılan Kopya Yapıcı (Copy Constructor) (Sensor s2 = s1;)
 *   3. Varsayılan Kopya Atama Operatörü (Copy Assignment) (p2 = p1;)
 *
 * Basit değişkenler (int, float) için bu yeterlidir. AMA! Sınıfınızda dinamik bellek tahsis eden
 * (`new` veya `malloc` kullanan) bir POINTER varsa, varsayılan kopya "Sığ Kopya (Shallow Copy)"
 * yapar. Her iki nesne de AYNI belleği işaret eder! Yıkıldıklarında "Double Free" olur ve
 * program çöker!
 *
 * =============================================================================
 * [RULE OF 3 / 3'LU KURAL]
 * =============================================================================
 * EN: "If a class needs a user-defined Destructor (i.e. manual `delete`), then you MUST also
 * define these two yourself:"
 *
 *   1. Destructor
 *   2. Copy Constructor
 *   3. Copy Assignment Operator
 *
 * TR: "Eğer bir sınıfın kullanıcı tanımlı bir Yıkıcıya (yani manuel `delete`) ihtiyacı varsa, şu
 * ikisini de KENDİNİZ tanımlamalısınız:"
 *
 *   1. Yıkıcı (Destructor)
 *   2. Kopya Yapıcı (Copy Constructor)
 *   3. Kopya Atama Operatörü (Copy Assignment Operator)
 *
 * =============================================================================
 * [CPPREF DEPTH: The Rule of 5 and `noexcept` Move / CPPREF DERİNLİK: 5'li Kural ve `noexcept`
 * Taşıma]
 * =============================================================================
 * EN: C++11 introduced Move Constructors and Move Assignment (Rule of 5). CppReference strongly
 * mandates: YOU MUST mark your Move functions as `noexcept`! If `noexcept` is missing,
 * `std::vector` reallocation will refuse to Move your objects and will fall back to painfully
 * slow Deep Copies!
 *
 * TR: C++11 ile "Move (Tasima)" mimarisi geldi ve Kural 5'e cikti. CppReference der ki: Tasima
 * (Move) metotlarinizi KESINLIKLE `noexcept` (Hatalara Kapali) olarak isaretlemelisiniz! Eger
 * eksikse, `std::vector` kapasitesini artirirken korkuya kapilir ve Tasima yapmak yerine
 * objelerinizi tek tek Kopyalar (Program %70 yavaslar)!
 *
 * @note C++11 ile beraber "Move Semantics" gelince buralara "Move" olan 2 ozellik daha eklendi
 * ve kuralin adi "Rule of 5" oldu.
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_rule_of_three_five.cpp -o 03_rule_of_three_five
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstring>
#include <iostream>
#include <utility>

/**
 * @class StringBox
 * @brief EN: Custom primitive string class that manages dynamic memory.
 * TR: Dinamik bellek yoneten ilkel metin sinifi.
 */
class StringBox {
private:
  // EN: Dynamically allocated text
  // TR: Dinamik RAM
  char *data;
  size_t length;

public:
  // 1. Constructor
  StringBox(const char *sourceText) {
    length = std::strlen(sourceText);
    // EN: Dynamic heap allocation
    // TR: Heap uzerinde RAM
    data = new char[length +
                    1];
    std::strcpy(data, sourceText);
    std::cout << "[Constructor] Memory allocated for: " << data << " (Hafiza ayrildi)" << std::endl;
  }

  // EN: --- RULE OF 3 BEGINS! DESTRUCTOR is user-defined ---
  // TR: --- 3'LÜ KURAL BAŞLIYOR! YIKICI (DESTRUCTOR) kullanıcı tanımlıdır ---
  // EN: [1/3] RULE 1: DESTRUCTOR
  // TR: [1/3] KURAL 1: YIKICI (DESTRUCTOR)
  ~StringBox() {
    std::cout << "[Destructor] Deleting: " << data << " (Siliniyor)" << std::endl;
    delete[] data;
  }

  // EN: --- IF WE WROTE DELETE ABOVE, WE MUST HAND-WRITE THESE ---
  // TR: --- YUKARIDA DELETE YAZDIYSAN, BUNLARI DA KENDİN YAZMAK ZORUNDASIN ---

  // EN: [2/3] RULE 2: COPY CONSTRUCTOR (Deep Copy)
  // TR: [2/3] KURAL 2: KOPYA OLUŞTURUCU (Derin Kopya)
  // EN: Example: StringBox B = A;
  // TR: Ornek: StringBox B = A;
  StringBox(const StringBox &other) {
    length = other.length;
    std::cout << "[Copy Constructor] Deep copying: " << other.data << " (Derin kopyalandi)" <<
        std::endl;

    // EN: NEVER do 'data = other.data' (Shallow Copy disaster)! Allocate NEW RAM!
    // TR: Asla 'data = other.data' yapmayin! Yeni RAM ac!
    data = new char[length + 1];
    std::strcpy(data, other.data);
  }

  // EN: [3/3] RULE 3: COPY ASSIGNMENT OPERATOR
  // TR: [3/3] KURAL 3: KOPYA ATAMA OPERATÖRÜ
  // EN: Example: B = A; (B already exists, must free old content first!)
  // TR: Ornek: B = A; (B zaten yasiyorken eski icerigi silinmeli!)
  StringBox &operator=(const StringBox &other) {
    // EN: Self-assignment guard
    // TR: Kendine atama korumasi
    if (this == &other)
      return *this;

    std::cout << "[Copy Assignment] Overwriting with: " << other.data << " (Uzerine yazildi)" <<
        std::endl;

    // EN: Free old memory first
    // TR: Eski bellek once silinmeli
    delete[] data;

    length = other.length;
    data = new char[length + 1];
    std::strcpy(data, other.data);

    return *this;
  }

  void print() const {
    std::cout << "Box Content (Kutu Icerigi): " << data << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: ECUFirmwareBuffer — Full Rule of 5 Demonstration (Automotive Domain)
// TR: ECUFirmwareBuffer — Tam 5'li Kural Gösterimi (Otomotiv Alanı)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

/**
 * @class ECUFirmwareBuffer
 * @brief EN: Simulates a firmware data buffer for an automotive ECU (Electronic Control Unit).
 * Demonstrates Rule of 5 with move semantics.
 * TR: Otomotiv ECU (Elektronik Kontrol Ünitesi) için firmware veri tamponunu simüle eder. Move
 * semantics ile 5'li Kuralı gösterir.
 */
class ECUFirmwareBuffer {
private:
    char* data_;
    size_t size_;
    std::string label_;

public:
    // EN: Parameterized Constructor — allocates firmware buffer on the heap
    // TR: Parametreli Yapıcı — firmware tamponunu heap üzerinde ayırır
    ECUFirmwareBuffer(size_t size, const std::string& label)
        : data_(new char[size]), size_(size), label_(label) {
        std::memset(data_, 0, size_);
        std::cout << "[Constructor] ECU buffer '" << label_
                  << "' allocated (" << size_ << " bytes)" << std::endl;
    }

    // ─── 1. Destructor ───────────────────────────────────────────────────────────────────────────
    // EN: [1/5] Destructor — releases the heap-allocated firmware buffer
    // TR: [1/5] Yıkıcı — heap'te ayrılan firmware tamponunu serbest bırakır
    ~ECUFirmwareBuffer() {
        if (data_) {
            std::cout << "[Destructor] Deleting ECU buffer '" << label_
                      << "' (" << size_ << " bytes freed)" << std::endl;
            delete[] data_;
        } else {
            std::cout << "[Destructor] ECU buffer '" << label_
                      << "' was moved-from, nothing to free" << std::endl;
        }
    }

    // ─── 2. Copy Constructor (Deep Copy) ─────────────────────────────────────────────────────────
    // EN: [2/5] Copy Constructor — performs a deep copy of the firmware buffer
    // TR: [2/5] Kopya Yapıcı — firmware tamponunun derin kopyasını yapar
    ECUFirmwareBuffer(const ECUFirmwareBuffer& other)
        : data_(new char[other.size_]), size_(other.size_), label_(other.label_ + "_copy") {
        std::memcpy(data_, other.data_, size_);
        std::cout << "[Copy Constructor] Deep copied '" << other.label_
                  << "' -> '" << label_ << "' (" << size_ << " bytes)" << std::endl;
    }

    // ─── 3. Copy Assignment Operator (Copy-and-Swap Idiom) ───────────────────────────────────────
    // EN: [3/5] Copy Assignment — uses copy-and-swap for exception safety
    // TR: [3/5] Kopya Atama — istisna güvenliği için copy-and-swap kullanır
    ECUFirmwareBuffer& operator=(const ECUFirmwareBuffer& other) {
        if (this == &other) return *this;
        std::cout << "[Copy Assignment] Overwriting '" << label_
                  << "' with '" << other.label_ << "'" << std::endl;

        // EN: Create a temporary deep copy, then swap resources
        // TR: Geçici derin kopya oluştur, ardından kaynakları takas et
        ECUFirmwareBuffer temp(other);
        std::swap(data_, temp.data_);
        std::swap(size_, temp.size_);
        std::swap(label_, temp.label_);
        return *this;
    }

    // ─── 4. Move Constructor (C++11) ─────────────────────────────────────────────────────────────
    // EN: [4/5] Move Constructor — steals resources from source, sets it to null
    // TR: [4/5] Taşıma Yapıcı — kaynağın kaynaklarını çalar, kaynağı null yapar
    // EN: noexcept is CRITICAL: without it, std::vector refuses to move and falls back to slow
    // deep copies during reallocation!
    // TR: noexcept KRİTİKTİR: olmadan std::vector taşıma yapmayı reddeder ve yeniden tahsiste
    // yavaş derin kopyalara döner!
    ECUFirmwareBuffer(ECUFirmwareBuffer&& other) noexcept
        : data_(other.data_), size_(other.size_), label_(std::move(other.label_)) {
        std::cout << "[Move Constructor] Moved resources from '" << label_
                  << "' (" << size_ << " bytes stolen)" << std::endl;
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // ─── 5. Move Assignment Operator (C++11) ─────────────────────────────────────────────────────
    // EN: [5/5] Move Assignment — swaps/steals resources from source
    // TR: [5/5] Taşıma Atama — kaynağın kaynaklarını takas eder/çalar
    ECUFirmwareBuffer& operator=(ECUFirmwareBuffer&& other) noexcept {
        std::cout << "[Move Assignment] Moving '" << other.label_
                  << "' into '" << label_ << "'" << std::endl;
        if (this != &other) {
            // EN: Release our current resources first
            // TR: Önce mevcut kaynaklarımızı serbest bırak
            delete[] data_;

            // EN: Steal resources from the source
            // TR: Kaynaklarını kaynaktan çal
            data_ = other.data_;
            size_ = other.size_;
            label_ = std::move(other.label_);

            // EN: Nullify the source so its destructor is safe
            // TR: Kaynağı null yap ki yıkıcısı güvenli olsun
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    // EN: Print current buffer info
    // TR: Mevcut tampon bilgisini yazdır
    void print() const {
        std::cout << "ECU Buffer Label: " << label_
                  << " | Size: " << size_
                  << " | Data ptr: " << static_cast<const void*>(data_) << std::endl;
    }
};

// EN: Factory function — may trigger RVO (Return Value Optimization) or move
// TR: Fabrika fonksiyonu — RVO (Dönüş Değeri Optimizasyonu) veya taşıma tetikler
ECUFirmwareBuffer createECUBuffer(size_t size, const std::string& label) {
    ECUFirmwareBuffer buf(size, label);
    return buf;
}

int main() {
  std::cout << "=== MODULE 2: RULE OF 3 / 5 AND MEMORY MANAGEMENT ===\n" << std::endl;

  StringBox originalFile("Top_Secret_Password");
  originalFile.print();

  // EN: Copy Constructor is invoked here! If we hadn't written Rule of 3 and the compiler's
  // default Shallow Copy ran, 'copyFile' would only steal the pointer (address) of 'Top_Secret'.
  // When main() exits and 'originalFile' is destroyed, that address becomes invalid. Then
  // 'copyFile' tries to delete the SAME address again (Double Free) and OS assassinates the
  // program with SIGSEGV!
  // TR: Copy Constructor burada cagrildi!
  StringBox copyFile = originalFile;

  std::cout << "----------------------" << std::endl;
  copyFile.print();

  // EN: Both are safely destroyed as separate physical memory blocks.
  // TR: Ikisi de ayri fiziksel bellek bloklari olarak guvenle yikildi.

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: RULE OF 5 — ECUFirmwareBuffer Demonstrations
  // TR: 5'Lİ KURAL — ECUFirmwareBuffer Gösterimleri
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  std::cout << "\n\n=== RULE OF 5: ECU FIRMWARE BUFFER DEMOS ===\n" << std::endl;

  // ─── 1. Move Construction ──────────────────────────────────────────────────────────────────────
  // EN: The Move Constructor is invoked when initializing from a temporary.
  // TR: Geçici bir nesneden başlatılırken Taşıma Yapıcı çağrılır.
  std::cout << "--- Demo: Rule of 5 — Move Construction ---" << std::endl;
  {
      ECUFirmwareBuffer engineECU(1024, "EngineECU_v3.2");
      engineECU.print();

      // EN: Move constructor steals engineECU's guts — engineECU becomes empty
      // TR: Taşıma yapıcı engineECU'nun içini çalar — engineECU boş kalır
      ECUFirmwareBuffer transferredECU(std::move(engineECU));
      std::cout << "After move construction:" << std::endl;
      transferredECU.print();
      engineECU.print();
  }
  std::cout << std::endl;

  // ─── 2. Move Assignment ────────────────────────────────────────────────────────────────────────
  // EN: Move Assignment transfers ownership between two existing objects.
  // TR: Taşıma Atama, iki mevcut nesne arasında sahipliği aktarır.
  std::cout << "--- Demo: Rule of 5 — Move Assignment ---" << std::endl;
  {
      ECUFirmwareBuffer absModule(512, "ABS_Controller_v1.0");
      ECUFirmwareBuffer escModule(256, "ESC_Module_v2.1");
      absModule.print();
      escModule.print();

      // EN: Move assignment — escModule steals absModule's resources
      // TR: Taşıma atama — escModule, absModule'un kaynaklarını çalar
      escModule = std::move(absModule);
      std::cout << "After move assignment:" << std::endl;
      escModule.print();
      absModule.print();
  }
  std::cout << std::endl;

  // ─── 3. std::move Explicit Transfer ────────────────────────────────────────────────────────────
  // EN: std::move does NOT move anything — it merely casts an lvalue to an rvalue reference,
  // enabling the move constructor/assignment to be called.
  // TR: std::move hiçbir şeyi taşımaz — yalnızca bir lvalue'yu rvalue referansına dönüştürerek
  // taşıma yapıcının/atamanın çağrılmasını sağlar.
  std::cout << "--- Demo: Rule of 5 — std::move Explicit Transfer ---" << std::endl;
  {
      ECUFirmwareBuffer transmissionECU(2048, "Transmission_TCU_v4.0");
      transmissionECU.print();

      // EN: After std::move, transmissionECU is in a valid-but-unspecified state
      // TR: std::move sonrası transmissionECU geçerli-ama-belirtilmemiş durumda
      ECUFirmwareBuffer newOwner = std::move(transmissionECU);
      std::cout << "New owner after explicit std::move:" << std::endl;
      newOwner.print();

      // EN: transmissionECU is now hollow — data_ is nullptr
      // TR: transmissionECU artık boş — data_ nullptr
      std::cout << "Original after std::move (moved-from state):" << std::endl;
      transmissionECU.print();
  }
  std::cout << std::endl;

  // ─── 4. Factory Function (RVO / Move) ──────────────────────────────────────────────────────────
  // EN: When returning a local object, the compiler may apply RVO (Return Value Optimization)
  // and construct directly in the caller's space. If RVO is not applied, the move constructor is
  // used instead of a deep copy.
  // TR: Yerel bir nesne döndürülürken, derleyici RVO (Dönüş Değeri Optimizasyonu) uygulayabilir
  // ve doğrudan çağıranın alanında oluşturur. RVO uygulanmazsa, derin kopya yerine taşıma yapıcı
  // kullanılır.
  std::cout << "--- Demo: Rule of 5 — Factory Function (RVO/Move) ---" << std::endl;
  {
      ECUFirmwareBuffer airbagECU = createECUBuffer(4096, "Airbag_SRS_v5.1");
      std::cout << "Factory-created buffer:" << std::endl;
      airbagECU.print();
  }

  return 0;
}
