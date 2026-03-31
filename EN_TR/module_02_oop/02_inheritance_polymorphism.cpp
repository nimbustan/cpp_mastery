/**
 * @file module_02_oop/02_inheritance_polymorphism.cpp
 * @brief OOP Part 2: Architecture of Inheritance and Polymorphism — OOP Kısım 2: Kalıtım ve Çok
 * Biçimliliğin Mimarisi
 *
 * @details
 * =============================================================================
 * [THEORY: What is Inheritance? Why use it? / TEORİ: Kalıtım Nedir ve Neden Kullanılır?]
 * =============================================================================
 * EN: Software developers hate repeating code (DRY Principle - Don't Repeat Yourself). If we
 * have a 'Dog', a 'Cat', and a 'Bird', they all have an 'age', 'weight', and an 'eat()'
 * function. Instead of writing these properties 3 times, we create a "Base Class" called
 * 'Animal'. The Dog and Cat "inherit" everything from Animal, and only add their unique features
 * (like bark for Dog).
 *
 * TR: Yazılım geliştiriciler aynı kodu tekrar yazmaktan nefret eder (DRY Prensibi). Bir 'Köpek',
 * 'Kedi' ve 'Kuş' sınıfımız varsa, hepsinin bir 'yaş', 'ağırlık' değişkeni ve 'yemekYe()'
 * fonksiyonu olacaktır. Bunları 3 defa ayrı ayrı yazmak yerine 'Hayvan' (Animal) adında bir
 * "Temel Sınıf" (Base Class) yaratırız. Köpek ve Kedi, Hayvan sınıfındaki her şeyi miras alır,
 * üzerine sadece köpeğe özgü şeyleri (havlama gibi) ekleriz.
 *
 * =============================================================================
 * [THEORY: Polymorphism and V-Table Architecture / TEORİ: Çok Biçimlilik ve V-Table]
 * =============================================================================
 * EN: "Give a command, let the object react according to its true nature." If we have an
 * 'Animal' pointer, but it actually points to a 'Dog', which sound will play if we call
 * `makeSound()`? This is where 'virtual' comes in. 'virtual' tells the compiler to do "Late
 * Binding / Dynamic Dispatch". "Look at the object's REAL type at runtime (using the V-Table)
 * and call the Dog's function."
 *
 * TR: "Bir komut ver, arkadaki nesne kendi doğasına göre tepki versin." Eğer 'Hayvan' tipinde
 * bir işaretçimiz (pointer) varsa, ama bu işaretçi gerçekte bir 'Köpek' nesnesini tutuyorsa,
 * `sesÇıkar()` dediğimizde kimin sesi çıkacak? İşte 'virtual' (sanal) anahtar kelimesi devreye
 * girer. 'virtual', derleyiciye "Program çalışırken nesnenin gerçek tipine bak (V-Table / Sanal
 * Tablo üzerinden) ve Köpeğin fonksiyonunu çağır (Geç Bağlama)" der. Eğer Polymorphism
 * olmasaydı, karşımızdaki nesne gerçekte bir Köpek olsa bile, işaretçi tipi Hayvan olduğu için
 * sıradan bir Hayvan sesi çıkardı.
 *
 * =============================================================================
 * [CPPREF DEPTH: Object Slicing & V-Table Performance Penalty / CPPREF DERİNLİK: Nesne
 * Dilimlemesi ve V-Tablo Performans Cezası]
 * =============================================================================
 * EN: "Object Slicing" occurs when you pass a Derived object by VALUE to a Base. The compiler
 * literally "slices off" the extra data! ALWAYS pass polymorphic objects by Pointer (*) or
 * Reference (&). Also, `virtual` functions have a slight CPU cost because they require an extra
 * pointer jump (vptr -> vtable) at runtime, which blocks the compiler from 'inlining' the
 * function.
 *
 * TR: Eğer bir Köpek nesnesini `void feed(Animal a)` şeklinde Değer Kopyası ile gönderirseniz
 * "Nesne Dilimlenmesi (Object Slicing)" yaşanır, köpeklik özellikleri kesilip atılır!
 * Polimorfizm SADECE pointer/referans `feed(Animal& a)` ile çalışır. Ayrıca `virtual`
 * fonksiyonlar oyun anında ekstra bir pointer araması (vptr) yaptıkları için derleyicinin
 * "inlining" yapmasını engeller (Ufak Hız Kaybı).
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_inheritance_polymorphism.cpp -o 02_inheritance_polymorphism
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <vector>

/**
 * @class Animal
 * @brief Base class (Temel Sınıf). Represents generic animal traits. — Genel hayvan
 * özelliklerini barındırır.
 */
class Animal {
public:
  Animal() { std::cout << "[Animal] Created. (Hayvan Doğdu)" << std::endl; }

  /**
   * @brief [THEORY — TEORİ]: The Vital Importance of Virtual Destructor / Virtual Yıkıcının
   * Hayati Önemi
   *
   * EN: If you're building a base class to be inherited from, its DESTRUCTOR MUST BE VIRTUAL!
   * Why? If we say `Animal* pet = new Dog();` and later `delete pet;`, without 'virtual', only
   * the Animal's destructor is called. The Dog's memory becomes a LEAK!
   *
   * TR: Eğer miras alınacak bir temel sınıf yapıyorsanız YIKICI FONKSİYON MUTLAKA VIRTUAL
   * OLMALIDIR. Neden? Eğer biz "Animal* pet = new Dog();" dersek ve ardından "delete pet;"
   * yaparsak komut sınıfı silmeye 'Animal'dan başlar. Eğer virtual demezek, Dog sınıfının
   * yıkıcısı ASLA çağrılmaz ve köpeğe ayırdığımız hafıza sızıntıya (memory leak) uğrar. O yüzden
   * 'virtual' deriz.
   */
  virtual ~Animal() {
    std::cout << "[Animal] Destroyed. (Hayvan Yok Oldu)" << std::endl;
  }

  /**
   * @brief Polymorphic function — Çok biçimli fonksiyon.
   *
   * EN: 'virtual' allows derived classes to override this behavior.
   * TR: 'virtual', bu sınıftan üretilen alt sınıfların bu fonksiyonu ezmesine (override) izin
   * verir.
   */
  virtual void makeSound() const {
    std::cout << "(Silence... Sessizlik...)" << std::endl;
  }
};

/**
 * @class Dog
 * @brief Derived class (Alt — Türetilmiş Sınıf). Inherits from Animal.
 */
// EN: Inheritance syntax
// TR: Kalıtım sözdizimi
class Dog : public Animal {
public:
  Dog() {
    std::cout << "[Dog] Born. (Köpek Hayata Gözlerini Açtı)" << std::endl;
  }

  ~Dog() override {
    std::cout << "[Dog] Destroyed. (Köpek Yok Oldu)" << std::endl;
  }

  /**
   * @brief Overrides the base class functionality — Temel sınıftaki fonksiyonu Köpeklerin
   * doğasına göre ezer.
   *
   * EN: 'override' is optional but HIGHLY RECOMMENDED. It tells the compiler to verify our
   * override.
   * TR: 'override' yazmak derleyicinin bizi denetlemesini sağlar, hata yaparsak uyarır.
   */
  void makeSound() const override {
    std::cout << "Dog says: Woof! Woof! (Hav! Hav!)" << std::endl;
  }
};

/**
 * @class Cat
 * @brief Another Derived class — Başka bir türetilmiş sınıf.
 */
class Cat : public Animal {
public:
  Cat() { std::cout << "[Cat] Born. (Kedi Doğdu)" << std::endl; }
  ~Cat() override {
    std::cout << "[Cat] Destroyed. (Kedi Yok Oldu)" << std::endl;
  }

  void makeSound() const override {
    std::cout << "Cat says: Meow! (Miyav!)" << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: IDiagnosticService — Pure Abstract Interface (Automotive Diagnostics)
// TR: IDiagnosticService — Saf Soyut Arayüz (Otomotiv Teşhis)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

/**
 * @class IDiagnosticService
 * @brief EN: Pure abstract interface for automotive diagnostic protocols. Any class implementing
 * this MUST provide all three methods.
 * TR: Otomotiv teşhis protokolleri için saf soyut arayüz. Bu arayüzü uygulayan her sınıf üç
 * metodu da sağlamalıdır.
 */
class IDiagnosticService {
public:
    // EN: Pure virtual — every implementation must provide a service name
    // TR: Saf sanal — her uygulama bir servis adı sağlamalı
    virtual std::string getName() const = 0;

    // EN: Pure virtual — runs a diagnostic check and returns pass/fail
    // TR: Saf sanal — teşhis kontrolü çalıştırır ve geçti/kaldı döner
    virtual bool runDiagnostic() = 0;

    // EN: Virtual destructor defaulted — required for safe polymorphic deletion
    // TR: Varsayılan sanal yıkıcı — güvenli polimorfik silme için gerekli
    virtual ~IDiagnosticService() = default;
};

// ─── OBD2Service — Concrete Implementation ───────────────────────────────────────────────────────
// EN: OBD-II (On-Board Diagnostics) standard used in all modern vehicles
// TR: OBD-II (Araç İçi Teşhis) tüm modern araçlarda kullanılan standart

class OBD2Service : public IDiagnosticService {
private:
    std::string vehicleVIN_;

public:
    explicit OBD2Service(const std::string& vin) : vehicleVIN_(vin) {
        std::cout << "[OBD2Service] Initialized for VIN: " << vehicleVIN_ << std::endl;
    }

    std::string getName() const override {
        return "OBD-II Diagnostic (VIN: " + vehicleVIN_ + ")";
    }

    bool runDiagnostic() override {
        std::cout << "[OBD2] Scanning DTCs (Diagnostic Trouble Codes) for "
                  << vehicleVIN_ << "..." << std::endl;
        std::cout << "[OBD2] Result: No fault codes detected. PASS." << std::endl;
        return true;
    }
};

// ─── CANDiagService — Another Concrete Implementation ────────────────────────────────────────────
// EN: CAN Bus (Controller Area Network) diagnostics for vehicle ECU networks
// TR: CAN Bus (Denetleyici Alan Ağı) araç ECU ağları için teşhis

class CANDiagService : public IDiagnosticService {
private:
    int busSpeed_;

public:
    explicit CANDiagService(int busSpeedKbps) : busSpeed_(busSpeedKbps) {
        std::cout << "[CANDiagService] Initialized at " << busSpeed_
                  << " kbps" << std::endl;
    }

    std::string getName() const override {
        return "CAN Bus Diagnostic (" + std::to_string(busSpeed_) + " kbps)";
    }

    bool runDiagnostic() override {
        std::cout << "[CAN] Probing CAN bus at " << busSpeed_
                  << " kbps..." << std::endl;
        std::cout << "[CAN] Result: Bus load 42%, all nodes responsive. PASS." << std::endl;
        return true;
    }
};

// ─── FinalConfig — Demonstrates 'final' keyword on a class ───────────────────────────────────────
// EN: A class marked 'final' cannot be inherited from. Useful for sealed configuration objects
// that must not be extended.
// TR: 'final' ile işaretlenen bir sınıftan miras alınamaz. Genişletilmemesi gereken kapalı
// yapılandırma nesneleri için kullanışlıdır.

class FinalConfig final {
private:
    std::string configName_;
    int configValue_;

public:
    FinalConfig(const std::string& name, int value)
        : configName_(name), configValue_(value) {}

    void print() const {
        std::cout << "Config: " << configName_ << " = " << configValue_ << std::endl;
    }
};

// EN: The following would cause a compile error:
// TR: Aşağıdaki derleme hatası verir: class DerivedConfig : public FinalConfig {};  // ERROR:
// cannot derive from 'final' class

// ─── Shape / Circle / FixedCircle — override and final on methods ────────────────────────────────
// EN: Demonstrates 'override' for correctness and 'final' to prevent further overriding
// TR: Doğruluk için 'override' ve daha fazla ezmeyi önlemek için 'final' gösterir

class Shape {
public:
    virtual double area() const {
        return 0.0;
    }
    virtual ~Shape() = default;
};

class Circle : public Shape {
protected:
    double radius_;

public:
    explicit Circle(double r) : radius_(r) {}

    // EN: 'override' ensures we are truly overriding a base virtual method. If we misspelled
    // 'area' as 'Area', the compiler catches it.
    // TR: 'override', gerçekten bir temel sanal metodu ezdiğimizi garanti eder. 'area' yerine
    // 'Area' yazarsak derleyici yakalar.
    double area() const override {
        return 3.14159265358979 * radius_ * radius_;
    }
};

class FixedCircle final : public Circle {
public:
    explicit FixedCircle(double r) : Circle(r) {}

    // EN: 'final' on this method means NO class inheriting from FixedCircle can override area()
    // again. (But FixedCircle itself is also 'final', so no class can inherit from it at all.)
    // TR: Bu metod üzerindeki 'final', FixedCircle'dan miras alan hiçbir sınıfın area()'yı
    // tekrar ezemeyeceği anlamına gelir.
    double area() const final {
        return 3.14159265358979 * radius_ * radius_;
    }
};

int main() {
  std::cout << "=== OOP Part 2: Polymorphism (Çok Biçimlilik) ===\n" << std::endl;

  // EN: Pointers are Base (Animal), but objects are Derived.
  // TR: İşaretçimiz Base (Animal), ama işaret etiğimiz şeyler gerçek nesneler.
  Animal *pet1 = new Dog();
  std::cout << "---" << std::endl;
  Animal *pet2 = new Cat();

  std::cout << "\n[POLYMORPHISM IN ACTION / ÇOK BİÇİMLİLİK DEVREDE]" << std::endl;

  // EN: Thanks to the V-Table, the REAL types will speak!
  // TR: V-Table sayesinde gerçek tiplerine göre ses çıkaracaklar!
  pet1->makeSound();
  pet2->makeSound();

  std::cout << "\n[CLEANUP / TEMİZLİK]" << std::endl;
  // EN: Virtual Destructor guarantees perfect teardown. Memory Leak = 0!
  // TR: Virtual Destructor sayesinde MÜKEMMEL bir yıkım sırası izlenecek. Memory Leak 0!
  delete pet1;
  std::cout << "---" << std::endl;
  delete pet2;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: ADVANCED POLYMORPHISM — Automotive Diagnostic Service Demos
  // TR: GELİŞMİŞ POLİMORFİZM — Otomotiv Teşhis Servisi Gösterimleri
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  std::cout << "\n\n=== ADVANCED POLYMORPHISM: AUTOMOTIVE DIAGNOSTICS ===\n" << std::endl;

  // ─── 1. Abstract Interface — IDiagnosticService ────────────────────────────────────────────────
  // EN: We cannot instantiate IDiagnosticService directly (it has pure virtual methods). We
  // create concrete implementations through base pointers.
  // TR: IDiagnosticService doğrudan örneklenemez (saf sanal metotları var). Temel işaretçiler
  // aracılığıyla somut uygulamalar oluştururuz.
  std::cout << "--- Demo: Abstract Interface — IDiagnosticService ---" << std::endl;
  {
      IDiagnosticService* svc1 = new OBD2Service("WVWZZZ3CZWE123456");
      IDiagnosticService* svc2 = new CANDiagService(500);

      std::cout << "\nService 1: " << svc1->getName() << std::endl;
      svc1->runDiagnostic();

      std::cout << "\nService 2: " << svc2->getName() << std::endl;
      svc2->runDiagnostic();

      // EN: Virtual destructor ensures proper cleanup through base pointer
      // TR: Sanal yıkıcı, temel işaretçi aracılığıyla doğru temizliği sağlar
      delete svc1;
      delete svc2;
  }
  std::cout << std::endl;

  // ─── 2. final Class — FinalConfig ──────────────────────────────────────────────────────────────
  // EN: FinalConfig is marked 'final' — attempting to inherit from it would produce a
  // compile-time error. This is useful for sealed ECU configuration objects that must remain
  // unextended.
  // TR: FinalConfig 'final' olarak işaretlenmiş — ondan miras almaya çalışmak derleme zamanı
  // hatası üretir. Genişletilmemesi gereken kapalı ECU yapılandırma nesneleri için
  // kullanışlıdır.
  std::cout << "--- Demo: final Class — FinalConfig ---" << std::endl;
  {
      FinalConfig ecuConfig("MaxRPM", 7500);
      ecuConfig.print();

      FinalConfig brakeConfig("ABSThreshold_mbar", 4200);
      brakeConfig.print();

      // EN: Uncommenting the following line would cause: error: cannot derive from 'final' base
      // 'FinalConfig'
      // TR: Aşağıdaki satırın yorumunu kaldırmak şu hataya neden olur: error: 'final' temel
      // 'FinalConfig'ten türetilemez class ExtendedConfig : public FinalConfig {};  // COMPILE
      // ERROR!
  }
  std::cout << std::endl;

  // ─── 3. override Keyword — Shape / Circle ──────────────────────────────────────────────────────
  // EN: 'override' is a C++11 keyword that tells the compiler: "I intend to override a virtual
  // function in the base class. If I'm wrong (e.g., wrong signature), give me a compile error."
  // This prevents silent bugs.
  // TR: 'override', derleyiciye "Temel sınıftaki bir sanal fonksiyonu ezmeyi amaçlıyorum.
  // Yanlışsam derleme hatası ver" der. Sessiz hataları önler.
  std::cout << "--- Demo: override Keyword — Shape/Circle ---" << std::endl;
  {
      Shape baseShape;
      Circle wheel(17.5);
      FixedCircle hubcap(7.0);

      // EN: Polymorphism through base pointer — correct area() called each time
      // TR: Temel işaretçi aracılığıyla polimorfizm — her seferinde doğru area() çağrılır
      Shape* shapes[] = { &baseShape, &wheel, &hubcap };
      const char* names[] = { "Base Shape", "Wheel (Circle r=17.5)", "Hubcap (FixedCircle r=7.0)" };

      for (int i = 0; i < 3; ++i) {
          std::cout << names[i] << " area = " << shapes[i]->area() << std::endl;
      }
  }
  std::cout << std::endl;

  // ─── 4. Polymorphic Container — vector<unique_ptr<IDiagnosticService>> ─────────────────────────
  // EN: The idiomatic C++ way to manage polymorphic objects: store them in a vector of
  // unique_ptr to the base interface. No manual delete needed. This is how modern automotive
  // middleware (AUTOSAR Adaptive) manages service instances.
  // TR: Polimorfik nesneleri yönetmenin idiomatik C++ yolu: temel arayüze unique_ptr vektöründe
  // saklamak. Manuel delete gerekmez. Modern otomotiv ara yazılımı (AUTOSAR Adaptive) servis
  // örneklerini böyle yönetir.
  std::cout << "--- Demo: Polymorphic Container — vector<unique_ptr> ---" << std::endl;
  {
      std::vector<std::unique_ptr<IDiagnosticService>> diagnostics;

      diagnostics.push_back(std::make_unique<OBD2Service>("TMBJB9NE1L0123456"));
      diagnostics.push_back(std::make_unique<CANDiagService>(250));
      diagnostics.push_back(std::make_unique<OBD2Service>("WBA3A5C55CF256789"));
      diagnostics.push_back(std::make_unique<CANDiagService>(1000));

      std::cout << "\nRunning all diagnostics in polymorphic container:" << std::endl;
      std::cout << "================================================" << std::endl;

      int passCount = 0;
      int totalCount = 0;
      for (const auto& svc : diagnostics) {
          std::cout << "\n>> " << svc->getName() << std::endl;
          if (svc->runDiagnostic()) {
              ++passCount;
          }
          ++totalCount;
      }

      std::cout << "\n================================================" << std::endl;
      std::cout << "Diagnostic Summary: " << passCount << "/" << totalCount
                << " passed." << std::endl;

      // EN: When 'diagnostics' vector goes out of scope, unique_ptr automatically deletes each
      // service. Zero memory leaks.
      // TR: 'diagnostics' vektörü kapsam dışına çıkınca unique_ptr her servisi otomatik siler.
      // Sıfır bellek sızıntısı.
  }

  return 0;
}
