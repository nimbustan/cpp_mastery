/**
 * @file module_03_modern_cpp/01_smart_pointers.cpp
 * @brief Modern C++: Smart Pointers and RAII / Akıllı İşaretçiler ve RAII (Resource Acquisition
 * Is Initialization)
 *
 * @details
 * =============================================================================
 * [THEORY: Why Do We Need Smart Pointers? / TEORİ: Neden Akıllı İşaretçilere İhtiyacımız Var?]
 * =============================================================================
 * EN: In old C++ (before C++11), developers managed memory dynamically using `new` and `delete`.
 * This was a nightmare. If a function threw an exception or returned early, `delete` was skipped
 * -> Memory Leak! If we deleted a pointer twice -> Double Free Crash!
 *
 * TR: Eski C++'da (C++11 öncesi), geliştiriciler belleği `new` ve `delete` ile tamamen manuel
 * yönetirdi. Bu büyük bir kabustu. Bir fonksiyon hata fırlatır (exception) veya erken `return`
 * ederse, `delete` komutu atlanırdı -> Bellek Sızıntısı (Memory Leak). Aynı pointer'ı
 * yanlışlıkla iki defa silersek -> Çift Silme Çöküşü (Double Free Crash).
 *
 * =============================================================================
 * [THEORY: What is RAII? / TEORİ: RAII Nedir?]
 * =============================================================================
 * EN: The backbone principle of C++. "When a resource (memory, file, socket) is Acquired, bind
 * it to a local/stack Object. When that Object goes out of scope ({...}), its Destructor
 * automatically runs and safely frees the resource." Smart Pointers are pure RAII. They silently
 * call `delete` for you.
 *
 * TR: C++'ın belkemiği prensibi. "Bir kaynak (bellek, dosya, ağ soketi) alındığı anda
 * (Acquisition) onu yerel/stack'te yaşayan bir nesneye bağla. O nesne yaşam alanından ({...}
 * scope) çıktığında otomatik olarak Yıkıcısı (Destructor) çalışsın ve kaynağı güvenle serbest
 * bıraksın."
 *
 * Akıllı işaretçiler (Smart Pointers) tam olarak RAII'dir. Arka planda senin için sessizce
 * `delete` çağırırlar.
 *
 * 1. std::unique_ptr: UNIQUE Ownership. Cannot be copied (only moved). "I am the sole owner of
 * this memory. If I die, it dies." (Sahipliği TEKTİR. Kopyalanamaz, sadece taşınabilir). 2.
 * std::shared_ptr: SHARED Ownership. Counts how many people hold the memory (Reference
 * Counting). When the count hits 0, the memory vanishes. (Sahipliği PAYLAŞILIR. Referans sayacı
 * 0 olunca belleği uçurur). 3. std::weak_ptr: A "watcher" for shared_ptr that doesn't increment
 * the count, preventing circular reference loops! (shared_ptr sayacını artırmadan gözcülük
 * yapan, dairesel referans kilitlenmelerini çözen türdür).
 *
 * =============================================================================
 * [CPPREF DEPTH: `std::make_shared` vs `new` & The Control Block / CPPREF DERİNLİK:
 * `std::make_shared` vs `new` ve Kontrol Bloğu]
 * =============================================================================
 * EN: CppReference dictates: Always use `std::make_shared` instead of `std::shared_ptr<T>(new
 * T())`. Why? Because `shared_ptr` needs TWO things: 1. The actual object. 2. A "Control Block"
 * (holds the counter). Using `new` allocates these separately (2 RAM jumps). `make_shared`
 * allocates them in ONE single contiguous memory block (Faster Cache Hits, No memory leaps).
 *
 * TR: CppReference emreder: Daima `std::make_shared` kullanın. Çünkü `shared_ptr` RAM'de 2 şeye
 * ihtiyaç duyar: 1. Asıl Nesne, 2. Sayaç Bloğu (Control Block). Eğer `new` derseniz işletim
 * sistemi iki farklı yere gider (Yavaş). `make_shared` ise Nesne'yi ve Sayacı BİTİŞİK TEK RAM
 * BLOĞUNA yazar (CPU Önbellek, Cache Hit Hızı %100).
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
// EN: Required for smart pointers
// TR: Akıllı işaretçiler için zorunlu
#include <memory>

/**
 * @class HeavyResource
 * @brief EN: Represents a memory-heavy object like a 4K texture or a gigabyte DB. TR: 4K doku
 * veya megabaytlık bir veritabanı gibi ağır nesneleri temsil eder.
 */
class HeavyResource {
public:
  HeavyResource() {
    std::cout << "[HeavyResource] Acquired from OS. (Kaynak İşletim "
                 "Sisteminden Alındı)"
              << std::endl;
  }
  ~HeavyResource() {
    std::cout << "[HeavyResource] Destroyed safely! (Kaynak Güvenle Yok "
                 "Edildi!) -> NO LEAK"
              << std::endl;
  }
  void render() {
    std::cout << "Rendering 4K Textures... / 4K Dokular İşleniyor..." << std::endl;
  }
};

int main() {
  std::cout << "=== MODERN C++: SMART POINTERS & RAII ===\n" << std::endl;

  std::cout << "--- 1. Raw Pointer (Legacy System - Bad Practice / Eski Sistem "
               "- Kötü Pratik) ---"
            << std::endl;
  HeavyResource *rawPointer = new HeavyResource();

  // EN: If an exception happens here, 'delete' is skipped -> Memory Leak!
  // TR: Eğer burada hata fırlayıp delete adımını atlarsak RAM dolar taşar!
  delete rawPointer;

  std::cout << "\n--- 2. std::unique_ptr (C++14 Way - Best Practice / C++14 "
               "Yolu - En İyi Pratik) ---"
            << std::endl;
  {
    // EN: Scope begins! `std::make_unique()` is faster and SAFER than calling `new`.
    // TR: Scope Başlıyor! `std::make_unique()` kullanmak, doğrudan `new` kullanmaktan çok daha
    // GÜVENLİDİR.
    std::unique_ptr<HeavyResource> uniquePtr = std::make_unique<HeavyResource>();
    uniquePtr->render();

    // auto uniquePtr2 = uniquePtr; // ERROR! Compiler forbids it. Ownership is unique. / HATA!
    // KOPYALANAMAZ.

    // EN: Allowed! Ownership transferred via Move Semantics.
    // TR: İzin verilir. Sahiplik devredildi (Taşıma Semantiği).
    std::unique_ptr<HeavyResource> movedPtr = std::move(uniquePtr);

  } // EN: Scope ends. movedPtr dies. Destructor AUTO-RUNS!
    // TR: Süslü parantez bitti. SCOPE DIŞINA çıktık. movedPtr YOK EDİLDİ. Yıkıcı OTO-ÇALIŞTI!

  std::cout << "\n--- 3. std::shared_ptr (Shared Ownership / Paylaşılan Sahiplik) ---" << std::endl;
  {
    // Count = 1 (Sayaç 1)
    std::shared_ptr<HeavyResource> shared1 = std::make_shared<HeavyResource>();
    std::cout << "Owners (Sahip Sayısı): " << shared1.use_count() << std::endl;

    {
      // EN: Copied! Count = 2
      // TR: Kopya ALINABİLİR! Sayaç 2
      std::shared_ptr<HeavyResource> shared2 = shared1;
      std::cout << "Owners in lower scope (Alt Scope Sahip Sayısı): " << shared1.use_count() <<
          std::endl;
    } // EN: shared2 dies. Count drops to 1. Object still lives.
      // TR: shared2 öldü. Sayaç tekrar 1'e düştü (Nesne halen yaşıyor).

    std::cout << "Owners after lower scope (Alt Scope Bittiğinde Sahip Sayısı): " <<
        shared1.use_count() << std::endl;
  } // EN: shared1 dies. Count becomes 0. OBJECT DESTROYED!
    // TR: shared1 öldü. Sayaç 0 oldu. NESNE TAMAMEN YOK EDİLDİ!

  return 0;
}
