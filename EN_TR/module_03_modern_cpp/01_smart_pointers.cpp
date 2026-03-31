/**
 * @file module_03_modern_cpp/01_smart_pointers.cpp
 * @brief Modern C++: Smart Pointers and RAII — Akıllı İşaretçiler ve RAII (Resource Acquisition
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
 * 1. std::unique_ptr: UNIQUE Ownership. Cannot be copied (only moved).
 *    "I am the sole owner of this memory. If I die, it dies."
 * 2. std::shared_ptr: SHARED Ownership. Counts how many people hold the memory
 *    (Reference Counting). When the count hits 0, the memory vanishes.
 * 3. std::weak_ptr: A "watcher" for shared_ptr that doesn't increment
 *    the count, preventing circular reference loops!
 *
 * TR: C++'ın belkemiği prensibi. "Bir kaynak (bellek, dosya, ağ soketi) alındığı anda
 * (Acquisition) onu yerel/stack'te yaşayan bir nesneye bağla. O nesne yaşam alanından ({...}
 * scope) çıktığında otomatik olarak Yıkıcısı (Destructor) çalışsın ve kaynağı güvenle serbest
 * bıraksın." Akıllı işaretçiler tam olarak RAII'dir. Arka planda senin için sessizce
 * `delete` çağırırlar.
 * 1. std::unique_ptr: Sahipliği TEKTİR. Kopyalanamaz, sadece taşınabilir.
 *    "Bu belleğin tek sahibi benim. Ben ölürsem, o da ölür."
 * 2. std::shared_ptr: Sahipliği PAYLAŞILIR. Belleği kaç kişinin tuttuğunu sayar
 *    (Referans Sayacı). Sayaç 0 olunca belleği uçurur.
 * 3. std::weak_ptr: shared_ptr sayacını artırmadan gözcülük yapan, dairesel
 *    referans kilitlenmelerini çözen türdür.
 *
 * =============================================================================
 * [CPPREF DEPTH: `std::make_shared` vs `new` & The Control Block / CPPREF DERİNLİK:
 * `std::make_shared` vs `new` ve Kontrol Bloğu]
 * =============================================================================
 * EN: CppReference dictates: Always use `std::make_shared` instead of `std::shared_ptr<T>(new
 * T())`. Why? Because `shared_ptr` needs TWO things:
 *   1. The actual object.
 *   2. A "Control Block" (holds the counter).
 * Using `new` allocates these separately (2 RAM jumps). `make_shared` allocates them in ONE
 * single contiguous memory block (Faster Cache Hits, No memory leaps).
 *
 * TR: CppReference emreder: Daima `std::make_shared` kullanın. Çünkü `shared_ptr` RAM'de 2 şeye
 * ihtiyaç duyar:
 *   1. Asıl Nesne.
 *   2. Sayaç Bloğu (Control Block).
 * Eğer `new` derseniz işletim sistemi iki farklı yere gider (Yavaş). `make_shared` ise Nesne'yi
 * ve Sayacı BİTİŞİK TEK RAM BLOĞUNA yazar (CPU Önbellek, Cache Hit Hızı %100).
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_smart_pointers.cpp -o 01_smart_pointers
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdio>
#include <string>
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

  // ═══════════════════════════════════════════════════════════════════════════
  // PART 4: Custom Deleter / Özel Silici
  // ═══════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 4. Custom Deleter (unique_ptr with FILE* / FILE* ile Özel Silici) ---"
            << std::endl;
  {
    // EN: unique_ptr can manage non-new resources with a custom deleter.
    //     Classic example: C FILE* must be closed with fclose(), not delete.
    // TR: unique_ptr, özel silici ile new olmayan kaynakları yönetebilir.
    //     Klasik örnek: C FILE* fclose() ile kapatılmalı, delete ile değil.
    auto fileDeleter = [](FILE* fp) {
      if (fp) {
        std::fclose(fp);
        std::cout << "  Custom deleter: FILE* closed / Özel silici: FILE* kapatıldı" << std::endl;
      }
    };

    {
      std::unique_ptr<FILE, decltype(fileDeleter)> filePtr(
          std::fopen("/tmp/smart_ptr_test.txt", "w"), fileDeleter);
      if (filePtr) {
        std::fputs("Hello from unique_ptr with custom deleter!\n", filePtr.get());
        std::cout << "  Written to /tmp/smart_ptr_test.txt" << std::endl;
      }
    } // EN: fileDeleter auto-called here — fclose(), not delete!
      // TR: fileDeleter burada otomatik çağrılır — fclose(), delete değil!

    // EN: Another pattern: array deleter (C++11 style, C++14 has make_unique<T[]>)
    // TR: Diğer desen: dizi silici (C++11 stili, C++14'te make_unique<T[]> var)
    auto arrayDeleter = [](int* p) {
      std::cout << "  Custom deleter: array deleted / Özel silici: dizi silindi" << std::endl;
      delete[] p;
    };
    {
      std::unique_ptr<int, decltype(arrayDeleter)> arr(new int[5]{10, 20, 30, 40, 50},
                                                       arrayDeleter);
      std::cout << "  Array[2] = " << arr.get()[2] << std::endl;
    } // EN: arrayDeleter auto-called — delete[], not delete!
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // PART 5: enable_shared_from_this
  // ═══════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 5. enable_shared_from_this ---" << std::endl;
  {
    // EN: Problem: Inside a method, you need a shared_ptr to `this`.
    //     BAD:  shared_ptr<Foo>(this)  — creates SECOND control block → double free!
    //     GOOD: inherit enable_shared_from_this<Foo>, call shared_from_this().
    // TR: Sorun: Bir metod içinde `this`'e shared_ptr gerekiyor.
    //     KÖTÜ:  shared_ptr<Foo>(this) — İKİNCİ kontrol bloğu oluşturur → çift silme!
    //     İYİ:  enable_shared_from_this<Foo>'dan miras al, shared_from_this() çağır.

    struct EventEmitter : public std::enable_shared_from_this<EventEmitter> {
      std::string name;

      explicit EventEmitter(const std::string& n) : name(n) {}

      std::shared_ptr<EventEmitter> getShared() {
        return shared_from_this();
      }

      void describe() const {
        std::cout << "  EventEmitter: " << name << std::endl;
      }
    };

    auto emitter = std::make_shared<EventEmitter>("SensorHub");
    std::cout << "  use_count before: " << emitter.use_count() << std::endl;

    {
      // EN: Safe! Same control block, count incremented.
      // TR: Güvenli! Aynı kontrol bloğu, sayaç artırıldı.
      auto alias = emitter->getShared();
      std::cout << "  use_count with alias: " << emitter.use_count() << std::endl;
      alias->describe();
    }

    std::cout << "  use_count after alias dies: " << emitter.use_count() << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // PART 6: Aliasing Constructor / Takma Ad Yapıcı
  // ═══════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 6. Aliasing Constructor (shared_ptr to member / Üye'ye shared_ptr) ---"
            << std::endl;
  {
    // EN: The aliasing constructor creates a shared_ptr that shares ownership
    //     with another shared_ptr, but points to a DIFFERENT object (e.g., a member).
    //     Signature: shared_ptr<T>(shared_ptr<U> owner, T* ptr)
    //     The owner keeps the parent alive; ptr is what you dereference.
    // TR: Takma ad yapıcı, başka bir shared_ptr ile sahipliği paylaşan ama
    //     FARKLI bir nesneye (örn. üyeye) işaret eden shared_ptr oluşturur.

    struct Engine {
      int horsepower = 450;
    };

    struct Car {
      std::string model = "M3 GTR";
      Engine engine;
    };

    auto car = std::make_shared<Car>();
    std::cout << "  car use_count: " << car.use_count() << std::endl;

    // EN: Aliasing: enginePtr shares ownership with car, but points to car->engine.
    //     As long as enginePtr is alive, the entire Car stays alive.
    // TR: Takma ad: enginePtr sahipliği car ile paylaşır, ama car->engine'e işaret eder.
    std::shared_ptr<Engine> enginePtr(car, &car->engine);
    std::cout << "  car use_count after alias: " << car.use_count() << std::endl;
    std::cout << "  enginePtr->horsepower: " << enginePtr->horsepower << std::endl;

    car.reset(); // EN: We release car, but enginePtr keeps it alive
                 // TR: car'ı bırakıyoruz ama enginePtr hayatta tutuyor
    std::cout << "  After car.reset(), enginePtr->horsepower: "
              << enginePtr->horsepower << std::endl;
    std::cout << "  (Car still alive because enginePtr holds ownership)"
              << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // PART 7: weak_ptr — Breaking Circular References
  // ═══════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 7. weak_ptr (Circular Reference Breaker / Dairesel Referans Kırıcı) ---"
            << std::endl;
  {
    // EN: Without weak_ptr, two objects holding shared_ptr to each other
    //     create a circular reference — neither is ever freed (memory leak).
    // TR: weak_ptr olmadan, birbirine shared_ptr tutan iki nesne dairesel
    //     referans oluşturur — ikisi de asla serbest bırakılmaz (bellek sızıntısı).

    struct Node {
      std::string name;
      std::weak_ptr<Node> partner;  // EN: weak, not shared! / TR: shared değil, weak!

      explicit Node(const std::string& n) : name(n) {
        std::cout << "  Node " << name << " created" << std::endl;
      }
      ~Node() {
        std::cout << "  Node " << name << " destroyed" << std::endl;
      }
    };

    {
      auto nodeA = std::make_shared<Node>("A");
      auto nodeB = std::make_shared<Node>("B");

      nodeA->partner = nodeB;  // EN: weak reference, count NOT incremented
      nodeB->partner = nodeA;  // TR: weak referans, sayaç ARTTIRILMAZ

      std::cout << "  A use_count: " << nodeA.use_count()
                << ", B use_count: " << nodeB.use_count() << std::endl;

      // EN: Access partner through weak_ptr — must lock() first
      // TR: partner'a weak_ptr üzerinden erişim — önce lock() çağrılmalı
      if (auto locked = nodeA->partner.lock()) {
        std::cout << "  A's partner: " << locked->name << std::endl;
      }

      // EN: Check if partner is still alive
      // TR: Ortağın hâlâ hayatta olup olmadığını kontrol et
      std::cout << "  B's partner expired? " << std::boolalpha
                << nodeA->partner.expired() << std::endl;
    } // EN: Both destroyed! No circular leak because weak_ptr doesn't hold ownership.
      // TR: İkisi de yok edildi! weak_ptr sahiplik tutmadığı için dairesel sızıntı yok.
  }

  return 0;
}
