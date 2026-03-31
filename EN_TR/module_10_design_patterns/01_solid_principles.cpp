/**
 * @file module_10_design_patterns/01_solid_principles.cpp
 * @brief Design Patterns & Architecture: SOLID Principles — Tasarım Kalıpları ve Mimari: SOLID
 * Prensipleri
 *
 * @details
 * =============================================================================
 * [THEORY: What is SOLID? / TEORİ: SOLID Nedir?]
 * =============================================================================
 * EN: SOLID is a set of five design principles that make software easier to maintain, extend,
 * and test. Coined by Robert C. Martin ("Uncle Bob"), these principles are the FOUNDATION of
 * professional OOP design. Every senior C++ developer is expected to know and apply them daily.
 *
 * TR: SOLID, yazılımı bakımı kolay, genişletilebilir ve test edilebilir kılan beş tasarım
 * prensibidir. Robert C. Martin ("Bob Amca") tarafından formüle edilen bu prensipler profesyonel
 * OOP tasarımının TEMELİDİR. Her kıdemli C++ geliştiricisinin bunları bilmesi ve günlük
 * uygulaması beklenir.
 *
 * =============================================================================
 * [THEORY: S — Single Responsibility Principle (SRP) / TEORİ: S — Tek Sorumluluk Prensibi]
 * =============================================================================
 * EN: A class should have ONE and ONLY ONE reason to change. If a class handles file I/O,
 * business logic, AND formatting — it has THREE reasons to change. Split it into three focused
 * classes. The key question: "What is this class's single job?"
 *
 * TR: Bir sınıfın değişmek için TEK ve YALNIZCA TEK bir nedeni olmalıdır. Eğer bir sınıf dosya
 * G/Ç, iş mantığı VE biçimlendirme yapıyorsa — üç değişim nedeni vardır. Üç odaklı sınıfa bölün.
 * Anahtar soru: "Bu sınıfın tek görevi nedir?"
 *
 * =============================================================================
 * [THEORY: O — Open/Closed Principle (OCP) / TEORİ: O — Açık/Kapalı Prensibi]
 * =============================================================================
 * EN: Software entities should be OPEN for extension but CLOSED for modification. Add new
 * behavior by creating NEW classes (inheritance or composition), NOT by editing existing working
 * code. This prevents introducing bugs into stable code while still allowing growth.
 *
 * TR: Yazılım varlıkları genişlemeye AÇIK, değişikliğe KAPALI olmalıdır. Yeni davranış eklemek
 * için mevcut çalışan kodu DEĞİŞTİRMEK yerine YENİ sınıflar (kalıtım veya bileşim) oluşturun.
 * Bu, kararlı kodda hata tanıtmayı önlerken büyümeye izin verir.
 *
 * =============================================================================
 * [THEORY: L — Liskov Substitution Principle (LSP) / TEORİ: L — Liskov Yerine Geçme Prensibi]
 * =============================================================================
 * EN: Objects of a derived class MUST be substitutable for objects of its base class WITHOUT
 * altering the correctness of the program. If `Square` inherits from `Rectangle` but breaks
 * `setWidth()/setHeight()` behavior, it violates LSP. The derived class must honor the BASE
 * class's contract.
 *
 * TR: Türetilmiş sınıf nesneleri, programın doğruluğunu bozmadan temel sınıf nesnelerinin yerine
 * GEÇEBİLMELİDİR. Eğer `Kare` sınıfı `Dikdörtgen`'den miras alır ama `setWidth()/setHeight()`
 * davranışını bozarsa, LSP'yi ihlal eder. Türetilmiş sınıf, TEMEL sınıfın sözleşmesine
 * uymalıdır.
 *
 * =============================================================================
 * [THEORY: I — Interface Segregation Principle (ISP) / TEORİ: I — Arayüz Ayrımı Prensibi]
 * =============================================================================
 * EN: No client should be FORCED to depend on methods it does not use. Instead of one "God
 * Interface" with 20 methods, split into small focused interfaces. A `Printer` shouldn't be
 * forced to implement `scan()` and `fax()` if it only prints. Each interface represents ONE
 * capability.
 *
 * TR: Hiçbir istemci, kullanmadığı yöntemlere bağımlı olmaya ZORLANMAMALIDIR. 20 metodlu tek bir
 * "Tanrı Arayüzü" yerine küçük odaklı arayüzlere bölün. Bir `Yazıcı`, yalnızca yazdırıyorsa
 * `tarama()` ve `faks()` uygulamaya zorlanmamalıdır. Her arayüz TEK bir yeteneği temsil eder.
 *
 * =============================================================================
 * [THEORY: D — Dependency Inversion Principle (DIP) / TEORİ: D — Bağımlılık Tersine Çevirme
 * Prensibi]
 * =============================================================================
 * EN: High-level modules should NOT depend on low-level modules. Both should depend on
 * ABSTRACTIONS (interfaces). A `GameEngine` shouldn't know about `OpenGLRenderer` directly — it
 * should depend on an `IRenderer` interface. This makes swapping implementations trivial and
 * enables testing with mocks.
 *
 * TR: Üst seviye modüller, alt seviye modüllere bağımlı OLMAMALIDIR. İkisi de SOYUTLAMALARA
 * (arayüzler) bağımlı olmalıdır. Bir `OyunMotoru` doğrudan `OpenGLRenderer`'ı bilmemelidir — bir
 * `IRenderer` arayüzüne bağımlı olmalıdır. Bu, uygulama değiştirmeyi kolay ve mock ile test
 * etmeyi mümkün kılar.
 *
 * =============================================================================
 * [CPPREF DEPTH: Pure Virtual Functions and Abstract Classes / CPPREF DERİNLİK: Saf Sanal
 * Fonksiyonlar ve Soyut Sınıflar]
 * =============================================================================
 * EN: SOLID heavily relies on abstract classes (interfaces). In C++, a class with at least one
 * `= 0` pure virtual function is abstract and CANNOT be instantiated. The derived class MUST
 * override all pure virtuals or it also becomes abstract. A virtual destructor is essential in
 * base classes to avoid undefined behavior when deleting through base pointers.
 * cppreference.com/w/cpp/language/abstract_class
 *
 * TR: SOLID, soyut sınıflara (arayüzler) yoğun şekilde dayanır. C++'ta en az bir `= 0` saf sanal
 * fonksiyonu olan sınıf soyuttur ve ÖRNEKLENEMez. Türetilmiş sınıf tüm saf sanal fonksiyonları
 * geçersiz kılMALIDIR aksi takdirde kendisi de soyut olur. Temel sınıflarda, temel işaretçi
 * aracılığıyla silerken tanımsız davranıştan kaçınmak için sanal yıkıcı şarttır.
 * cppreference.com/w/cpp/language/abstract_class
 *
 * =============================================================================
 * [CPPREF DEPTH: std::unique_ptr and Ownership Transfer / CPPREF DERİNLİK: std::unique_ptr ve
 * Sahiplik Devri]
 * =============================================================================
 * EN: DIP implementations typically use `std::unique_ptr<IInterface>` to hold injected
 * dependencies. `unique_ptr` enforces single ownership — the object is destroyed when the
 * pointer goes out of scope. Transfer ownership with `std::move()`. For shared ownership, use
 * `std::shared_ptr`. Never use raw `new`/`delete` in modern C++ — smart pointers are the RAII
 * standard. cppreference.com/w/cpp/memory/unique_ptr
 *
 * TR: DIP uygulamaları tipik olarak enjekte edilen bağımlılıkları tutmak için
 * `std::unique_ptr<IArayüz>` kullanır. `unique_ptr` tek sahipliği zorlar — nesne, işaretçi
 * kapsam dışına çıktığında yok edilir. Sahipliği `std::move()` ile devredin. Paylaşımlı sahiplik
 * için `std::shared_ptr` kullanın. Modern C++'ta asla ham `new`/`delete` kullanmayın — akıllı
 * işaretçiler RAII standardıdır. cppreference.com/w/cpp/memory/unique_ptr
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_solid_principles.cpp -o 01_solid_principles
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [SRP DEMO: Single Responsibility Principle]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: BAD: One class doing everything. GOOD: Split into focused classes.
// TR: KÖTÜ: Her şeyi yapan tek sınıf. İYİ: Odaklı sınıflara böl.

// --- BAD DESIGN (SRP violation) ---
class BadReport {
public:
  std::string generateContent() { return "Q4 Sales: $1,200,000"; }
  void saveToFile(const std::string &content) {
    // EN: Mixes business logic with I/O!
    // TR: İş mantığını G/Ç ile karıştırır!
    (void)content;
    std::cout << "[BAD] Saving to file... (mixed responsibility)" << std::endl;
  }
};

// --- GOOD DESIGN (SRP applied) ---
class ReportGenerator {
public:
  std::string generate() { return "Q4 Sales: $1,200,000"; }
};

class ReportSaver {
public:
  void save(const std::string &content) {
    (void)content;
    std::cout << "[GOOD-SRP] Report saved by dedicated saver." << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [OCP DEMO: Open/Closed Principle]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Add new shapes WITHOUT modifying existing code.
// TR: Mevcut kodu DEĞİŞTİRMEDEN yeni şekiller ekle.

class Shape {
public:
  virtual ~Shape() = default;
  virtual double area() const = 0;
  virtual std::string name() const = 0;
};

class Circle : public Shape {
  double radius_;

public:
  explicit Circle(double r) : radius_(r) {}
  double area() const override { return 3.14159 * radius_ * radius_; }
  std::string name() const override { return "Circle"; }
};

class Rectangle : public Shape {
  double w_, h_;

public:
  Rectangle(double w, double h) : w_(w), h_(h) {}
  double area() const override { return w_ * h_; }
  std::string name() const override { return "Rectangle"; }
};

// EN: NEW shape added without touching Circle or Rectangle!
// TR: Circle veya Rectangle'a dokunmadan YENİ şekil eklendi!
class Triangle : public Shape {
  double base_, height_;

public:
  Triangle(double b, double h) : base_(b), height_(h) {}
  double area() const override { return 0.5 * base_ * height_; }
  std::string name() const override { return "Triangle"; }
};

// EN: This function works with ANY shape — open for extension.
// TR: Bu fonksiyon HERHANGİ bir şekille çalışır — genişlemeye açık.
void printArea(const Shape &s) {
  std::cout << "[OCP] " << s.name() << " area = " << s.area() << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [ISP DEMO: Interface Segregation Principle]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Split "God interface" into small focused interfaces.
// TR: "Tanrı arayüzü"nü küçük odaklı arayüzlere böl.

class IPrintable {
public:
  virtual ~IPrintable() = default;
  virtual void print() = 0;
};

class IScannable {
public:
  virtual ~IScannable() = default;
  virtual void scan() = 0;
};

// EN: SimplePrinter only implements IPrintable — not forced to implement scan!
// TR: SimplePrinter yalnızca IPrintable uygular — scan uygulamaya zorlanmaz!
class SimplePrinter : public IPrintable {
public:
  void print() override {
    std::cout << "[ISP] SimplePrinter printing..." << std::endl;
  }
};

class MultiFunctionDevice : public IPrintable, public IScannable {
public:
  void print() override {
    std::cout << "[ISP] MultiFunctionDevice printing..." << std::endl;
  }
  void scan() override {
    std::cout << "[ISP] MultiFunctionDevice scanning..." << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DIP DEMO: Dependency Inversion Principle]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: High-level DiagnosticsEngine depends on ICommunicationBus interface, not a concrete
// protocol class.
// TR: Üst seviye DiagnosticsEngine, somut sınıfa değil ICommunicationBus arayüzüne bağımlıdır.

class ICommunicationBus {
public:
  virtual ~ICommunicationBus() = default;
  virtual void sendFrame(const std::string &frameData) = 0;
};

class CANBusProtocol : public ICommunicationBus {
public:
  void sendFrame(const std::string &frameData) override {
    std::cout << "[DIP] CAN Bus transmitting: " << frameData << std::endl;
  }
};

class FlexRayProtocol : public ICommunicationBus {
public:
  void sendFrame(const std::string &frameData) override {
    std::cout << "[DIP] FlexRay transmitting: " << frameData << std::endl;
  }
};

// EN: DiagnosticsEngine doesn't know or care which bus protocol it uses!
// TR: DiagnosticsEngine hangi veri yolu protokolünü kullandığını bilMEZ!
class DiagnosticsEngine {
  std::unique_ptr<ICommunicationBus> bus_;

public:
  explicit DiagnosticsEngine(std::unique_ptr<ICommunicationBus> b)
      : bus_(std::move(b)) {}
  void transmitDTC(const std::string &dtcCode) { bus_->sendFrame(dtcCode); }
};

int main() {
  std::cout << "=== MODULE 10: SOLID PRINCIPLES ===\n" << std::endl;

  // --- SRP Demo ---
  std::cout << "--- SRP: Single Responsibility ---\n" << std::endl;
  ReportGenerator gen;
  ReportSaver saver;
  std::string content = gen.generate();
  saver.save(content);
  std::cout << "Report content: " << content << "\n" << std::endl;

  // --- OCP Demo ---
  std::cout << "--- OCP: Open/Closed ---\n" << std::endl;
  Circle c(5.0);
  Rectangle r(4.0, 6.0);
  Triangle t(3.0, 8.0);
  printArea(c);
  printArea(r);
  printArea(t);
  std::cout << std::endl;

  // --- LSP Demo (via OCP shapes — all substitutable) ---
  std::cout << "--- LSP: Liskov Substitution ---\n" << std::endl;
  std::vector<std::unique_ptr<Shape>> shapes;
  shapes.push_back(std::make_unique<Circle>(3.0));
  shapes.push_back(std::make_unique<Rectangle>(2.0, 5.0));
  shapes.push_back(std::make_unique<Triangle>(4.0, 6.0));
  for (const auto &s : shapes) {
    std::cout << "[LSP] " << s->name() << " -> " << s->area() << std::endl;
  }
  std::cout << std::endl;

  // --- ISP Demo ---
  std::cout << "--- ISP: Interface Segregation ---\n" << std::endl;
  SimplePrinter sp;
  MultiFunctionDevice mfd;
  sp.print();
  mfd.print();
  mfd.scan();
  std::cout << std::endl;

  // --- DIP Demo ---
  std::cout << "--- DIP: Dependency Inversion ---\n" << std::endl;
  DiagnosticsEngine engine1(std::make_unique<CANBusProtocol>());
  DiagnosticsEngine engine2(std::make_unique<FlexRayProtocol>());
  engine1.transmitDTC("P0300 - Random Misfire Detected");
  engine2.transmitDTC("C0035 - Left Front Wheel Speed Sensor");

  std::cout << "\n=> All 5 SOLID principles demonstrated!" << std::endl;

  return 0;
}
