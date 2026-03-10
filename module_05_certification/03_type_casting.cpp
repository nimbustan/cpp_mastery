/**
 * @file module_05_certification/03_type_casting.cpp
 * @brief CPA/CPP Prep: Type Casting / Sertifikasyon: C++ Tip Dönüşüm Operatörleri
 *
 * @details
 * =============================================================================
 * [THEORY: Why are C-Style Casts (int) banned in Modern C++? / TEORİ: Eski Cast Neden Yasak?]
 * =============================================================================
 * EN: In C, you could force any type into any other type using `(int) x` or `(char*) ptr`. This
 * tells the compiler: "Shut up, I know what I'm doing." But it's violent. It breaks constness,
 * converts pointers blindly, and causes severe run-time crashes. C++ introduced 4 safe,
 * intentional, and searchable cast operators.
 *
 * TR: C dilinde, `(int)x` diyerek her türlü veriyi (örneğin bir kedi nesnesini integer'a) zorla
 * dönüştürebilirsiniz. Derleyiciye "Sus, ne yaptığımı biliyorum" demektir. Ama bu bir
 * felakettir; nesne dilimlenmelerine, `const` zırhının kırılmasına yol açar. C++, derleyicinin
 * "niyetinizi anlaması" ve sizi koruması için 4 ayrı dönüştürücü çıkardı. C-Stili dönüşüm
 * kullananlar mülakatlarda genellikle anında eksi puan alır!
 *
 * =============================================================================
 * [THEORY: The 4 Modern C++ Casts / TEORİ: 4 Modern C++ Dönüştürücü]
 * =============================================================================
 *
 * EN: C++ replaced the dangerous C-style `(int)x` cast with 4 explicit, searchable, and
 * intentional cast operators. Each has a specific purpose:
 *
 * TR: C++, tehlikeli C-stili `(int)x` dönüşümünü 4 açık, aranabilir ve niyeti belirten
 * dönüştürücü operatörle değiştirdi. Her birinin amacı farklıdır:
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * 1. static_cast<Type>(val)
 * ─────────────────────────────────────────────────────────────────────────────
 * EN: The "logical/ideal" cast. Used for well-defined conversions like float → int, int → enum,
 * base* → derived* (when you know the type). Checked at COMPILE-TIME. Zero runtime cost. This is
 * the cast you use 95% of the time.
 *
 * TR: "Mantıklı/ideal" dönüşüm. float → int, int → enum, base* → derived* gibi iyi tanımlanmış
 * dönüşümler için kullanılır (tipi bildiğinizde). DERLEME ZAMANINDA kontrol edilir. Çalışma
 * zamanı maliyeti sıfırdır. Zamanın %95'inde bu cast'i kullanırsınız.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * 2. dynamic_cast<Type>(val)
 * ─────────────────────────────────────────────────────────────────────────────
 * EN: The "safe polymorphism" cast. Used to safely downcast a base class pointer/reference to a
 * derived class. Checks the V-Table at RUN-TIME (RTTI). If the cast is invalid, returns nullptr
 * (pointer) or throws std::bad_cast (reference). Slower than static_cast but SAFE.
 *
 * TR: "Güvenli polimorfizm" dönüşümü. Üst sınıf pointer/referansını alt sınıfa güvenle
 * dönüştürmek için kullanılır. ÇALIŞMA ZAMANINDA V-Table üzerinden kontrol eder (RTTI). Geçersiz
 * dönüşümde nullptr (pointer) veya std::bad_cast (referans) döner. static_cast'ten yavaş ama
 * GÜVENLİDİR.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * 3. const_cast<Type>(val)
 * ─────────────────────────────────────────────────────────────────────────────
 * EN: Strips away `const` or `volatile` qualifiers. Used ONLY when interfacing with old C APIs
 * that forgot to mark parameters as `const`. Modifying a truly const object through const_cast
 * is UNDEFINED BEHAVIOR. Use with extreme caution!
 *
 * TR: `const` veya `volatile` niteleyicilerini kaldırır. SADECE `const` parametreyi unutmuş eski
 * C API'leriyle çalışırken kullanılır. Gerçekten const olan bir nesneyi const_cast ile
 * değiştirmek TANIMSIZ DAVRANISTIR (UB). Son derece dikkatli kullanın!
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * 4. reinterpret_cast<Type>(val)
 * ─────────────────────────────────────────────────────────────────────────────
 * EN: The "brute force" cast. Reinterprets the raw bit pattern of one type as another. No
 * checks, no safety net. Used almost exclusively in low-level hardware/driver programming and
 * memory-mapped I/O. Casting unrelated pointer types and dereferencing them triggers Strict
 * Aliasing UB. Only safe targets: `char*`, `unsigned char*`, `std::byte*`. FORBIDDEN in
 * application-level code!
 *
 * TR: "Kaba kuvvet" dönüşümü. Bir tipin ham bit desenini başka bir tip olarak yorumlar. Kontrol
 * yok, güvenlik ağı yok. Neredeyse yalnızca düşük seviye donanım/sürücü programlama ve
 * bellek-eşlemeli I/O'da kullanılır. İlişkisiz pointer tiplerini dönüştürüp okumak Strict
 * Aliasing UB tetikler. Güvenli hedefler: `char*`, `unsigned char*`, `std::byte*`. Uygulama
 * seviyesi kodda YASAKTIR!
 *
 * =============================================================================
 * [CPPREF DEPTH: `dynamic_cast` Cost & `reinterpret_cast` Strict Aliasing UB / CPPREF DERİNLİK:
 * `dynamic_cast` Maliyeti ve `reinterpret_cast` Sıkı Örtüşme UB]
 * =============================================================================
 * EN: `dynamic_cast` performs a string-search on the Run-Time Type Information (RTTI) tree,
 * making it EXTREMELY SLOW compared to `static_cast`. Use it sparingly! Also, `reinterpret_cast`
 * frequently triggers "Strict Aliasing" Undefined Behavior if you cast two unrelated pointers
 * and dereference them. Only cast to `char*` or `std::byte*`.
 *
 * TR: `dynamic_cast`, asıl kimliği bulmak için RTTI ağacında text araması yapar. Bu yüzden
 * `static_cast`e kıyasla DEHŞET YAVAŞTIR (Oyun döngüsünde anlık FPS düşüşü yaratır). Ek olarak,
 * `reinterpret_cast` ile dönüştürülmüş uyumsuz tipleri `*ptr` okumak Strict Aliasing kuralını
 * kırıp UB (Tanımsız Davranış) çökmelerine sebep olur. Sadece `char*` veya `std::byte*` ile
 * güvenle donanım okuyabilirsiniz.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

class Organism {
public:
  virtual ~Organism() {}
// EN: Virtual Destructor REQUIRED for V-Table (Polymorphism)
// TR: V-Table için şart!
};

class Human : public Organism {
public:
  void run() { std::cout << "Human is running! (İnsan koşuyor!)" << std::endl; }
};

class Plant : public Organism {};

void cStyle_VS_staticCast() {
  double pi = 3.14159;

  // EN: Old C-way (Bad Habit) - Unclear intention and unsearchable.
  // TR: Eski C Yöntemi (Kötü Alışkanlık) - Güvenliği belirsiz.
  int oldInt = (int)pi;

  // EN: Modern C++ way (static_cast) - Explicit intention. Compiler verifies it.
  // TR: Modern C++ yöntemi (static_cast) - Niyet belli. Derleyici onaylar.
  int modernInt = static_cast<int>(pi);

  std::cout << "C-Style cast: " << oldInt << " | static_cast: " << modernInt << std::endl;
}

void unlockDynamicCast() {
  Organism *unknownEntity1 = new Human();
  Organism *unknownEntity2 = new Plant();

  std::cout << "\n--- DYNAMIC CAST (Polymorphic Safety / Güvenlik) ---" << std::endl;

  // EN: [INTERVIEW QUESTION]: I have an Organism*. If it's truly a Human, I want to make it run.
  // If it's a Plant, I do nothing. How do I do this without crashing?
  //
  // TR: [MÜLAKAT SORUSU]: Elimde Organism* var, ama "İnsan" ise koşturacağım. "Bitki" ise hiçbir
  // şey yapmayacağım. Nasıl güvenle kontrol ederim?

  // EN: dynamic_cast queries the V-Table! If it matches `Human*`, it returns the pointer. Else,
  // `nullptr`.
  // TR: dynamic_cast gider V-Table'dan sorar! İnsansa verir, değilse "nullptr" döndürür! Çökme
  // engellenir.

  Human *isReallyHuman1 = dynamic_cast<Human *>(unknownEntity1);
  // EN: C++17 allows "if (auto p = dynamic_cast...)"
  // TR: C++17, "if (auto p = dynamic_cast...)" kullanımına izin verir.
  if (isReallyHuman1 != nullptr) {
    std::cout << "[SUCCESS / BAŞARI] 1st Entity is Human!" << std::endl;
    isReallyHuman1->run();
  }

  Human *isReallyHuman2 = dynamic_cast<Human *>(unknownEntity2);
  if (isReallyHuman2 == nullptr) {
    std::cout << "[REJECTED / REDDEDİLDİ] 2nd Entity is NOT Human (Probably Plant). "
           "CRASH PREVENTED! (Çöküş önlendi!)"
        << std::endl;
  }

  delete unknownEntity1;
  delete unknownEntity2;
}

int main() {
  std::cout << "=== MODULE 5: MODERN TYPE CASTING ===\n" << std::endl;

  unlockDynamicCast();

  std::cout << "\n--- reinterpret_cast (Danger Zone / Tehlikeli Bölge) ---" << std::endl;
  // EN: Do not use this unless hacking hardware/drivers. It tells the OS to treat a meaningless
  // number as a RAM address.
  // TR: Donanım programlama harici YASAKTIR. Integer bir sayıyı, RAM adresi imiş gibi yutturmaya
  // denir.

  long hardwareAddress = 0x7FFAABBCCDD;
  int *fakePointer = reinterpret_cast<int *>(hardwareAddress);

  std::cout << "Integer masked as a Pointer (Sayı Pointer yapıldı): " << fakePointer <<
      " (If you dereference this, PROGRAM CRASHES! / İçini okursanız ÇÖKER!)" << std::endl;

  return 0;
}
