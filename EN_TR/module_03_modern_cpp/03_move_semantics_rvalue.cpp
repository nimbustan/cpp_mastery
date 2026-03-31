/**
 * @file module_03_modern_cpp/03_move_semantics_rvalue.cpp
 * @brief Modern C++: Move Semantics & R-Values — Taşıma Semantiği ve L-Value vs R-Value (C++11
 * Devrimi)
 *
 * @details
 * =============================================================================
 * [THEORY: Why are C++03 and C++11 practically "different languages"? / TEORİ: Büyük C++
 * Atılımı]
 * =============================================================================
 * EN: Before C++11, returning a HUGE object (like a 4GB Vector) from a function forced the
 * compiler to perfectly COPY all 4GB of data byte by byte to the new variable, and then
 * immediately DESTROY the old 4GB vector inside the function.
 *
 * C++11 introduced "Move Semantics" (std::move). Instead of copying 4GB, it steals the internal
 * pointer of the old vector, gives it to the new one, and leaves the old one empty. Cost: O(N)
 * -> O(1). 4GB data copied in 1 nanosecond!
 *
 * TR: C++11 öncesinde, bir fonksiyonun içinden devasa büyüklükte (örneğin 4 GB'lik) bir `vector`
 * döndürdüğünüzde, derleyici bu 4 GB'ı bayt bayt yeni değişkene KOPYALAR (RAM ikiye katlanır),
 * sonra fonksiyon içindeki o orijinal 4 GB'ı ÇÖPE ATARDI. Bu tam bir performans katliamıydı.
 *
 * C++11 "Move Semantics" (Taşıma Semantiği) getirdi. Artık 4 GB'ı kopyalamıyoruz. "Madem
 * fonksiyonun içindeki nesne birazdan ölecek, onun içindeki işçiyi (pointer'ı) ÇALALIM (Steal),
 * yeni değişkene teslim edelim, ölecek olanın içini boşaltıp (null) cenazesini kaldıralım."
 * Maliyet: O(N)'den -> O(1)'e inmiştir. 1 Nanosaniyede 4 GB veri taşınır!
 *
 * =============================================================================
 * [THEORY: L-Value vs R-Value / TEORİ: Sol Taraf vs Sağ Taraf Değeri]
 * =============================================================================
 * EN:
 *     1. L-Value (Left Value): Has a permanent memory address, a name, and can
 *        sit on the left of an equal sign. (`int x = 5;` -> 'x' is an L-Value).
 *     2. R-Value (Right Value): A temporary, nameless object born to die.
 *        (`5` or the unassigned return of `factory()` is an R-Value).
 *     3. L-Value Reference (`&`): Classic reference.
 *     4. R-Value Reference (`&&`): C++11 feature. Specifically catches TEMPORARY,
 *        DYING objects so you can "Steal/Move" their internals!
 *
 * TR:
 *     1. L-Value (Left Value): RAM'de yer edinen, bir ismi (adresi) olan ve
 *        eşittirin solunda olabilen nesne. (`int x = 5;` -> 'x' bir L-Value'dir).
 *     2. R-Value (Right Value): Geçici, ismi olmayan, şipşak yaratılıp ölen nesne.
 *        (`5` veya `factory()` fonksiyonunun havada kalan dönüşü R-Value'dir).
 *     3. L-Value Reference (`&`): Klasik C++ referansı (Sadece kalıcıları tutar).
 *     4. R-Value Reference (`&&`): C++11 Devrimi! GEÇİCİ, ÖLECEK olan nesneleri
 *        yakalar ve onların donanımlarını "çalmanızı/taşımanızı" sağlar!
 *
 * =============================================================================
 * [CPPREF DEPTH: The Secret of `std::move` - It DOES NOT move anything! / CPPREF DERİNLİK:
 * `std::move`'un Sırrı - Aslında Hiçbir Şeyi TAŞIMAZ!]
 * =============================================================================
 * EN: CppReference warns developers continuously: `std::move()` DOES NOT ACTUALLY MOVE DATA.
 * Yes, really. It is just an unconditional `static_cast<T&&>(obj)`. It merely "re-labels" an
 * L-Value into an R-Value explicitly, so the compiler is "tricked" into calling the Move
 * Constructor instead of the Copy Constructor. The actual moving/stealing is done inside the
 * Move Constructor!
 *
 * TR: CppReference'in en büyük sırrı: `std::move()` ASLA BİR ŞEY TAŞIMAZ! Evet.. Okyanus kadar
 * veriniz olsa da, o sadece şipşak bir `static_cast<T&&>(obje)` çevirimidir. Yaptığı tek iş bir
 * L-Value nesnesinin etiketini söküp "Bu artık R-Value (Ölecek) bir nesnedir" yazmaktır. Böylece
 * derleyici kandırılır ve (Kopya) yerine (Taşıma) fonksiyonunu çağırır. Asıl hırsızlık/taşıma
 * Move Constructor'ın süslü parantezlerinin içinde yapılır!
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_move_semantics_rvalue.cpp -o 03_move_semantics_rvalue
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <utility>
#include <vector>

class HeavyBuffer {
private:
  int size;
  int *data;

public:
  // [1] Normal Constructor / Normal Yapıcı
  HeavyBuffer(int bufferSize) : size(bufferSize) {
    data = new int[size];
    std::cout << "[Constructor] Allocated heavy data of size: " << size << " (Ağır veri üretildi.)"
        << std::endl;
  }

  // [2] DESTRUCTOR / YIKICI
  ~HeavyBuffer() {
    std::cout << "[Destructor] Memory wiped. Size: " << size << " (Bellek yıkıldı.)" << std::endl;
    delete[] data;
  }

  // [3] COPY CONSTRUCTOR (The Old, Slow, Expensive Way / Eski Yöntem - Yavaş ve
  // Pahalı)
  HeavyBuffer(const HeavyBuffer &other) : size(other.size) {
    std::cout << "\033[31m[COPY Constructor]\033[0m COPYING BYTE BY BYTE! Too "
                 "heavy... (Satır satır kopyalanıyor!)"
              << std::endl;
    data = new int[size];
    for (int i = 0; i < size; ++i)
      data[i] = other.data[i];
  }

  // [4] MOVE CONSTRUCTOR (The C++11 Revolution using R-Value Reference '&&')
  // EN: "other" is a temporary object. So we DO NOT make it "const". We will empty its guts
  // (Steal).
  // TR: "other" geçici bir nesnedir. O yüzden "const" YAPMIYORUZ, içini boşaltacağız
  // (Hırsızlık).
  HeavyBuffer(HeavyBuffer &&other) noexcept
      : size(other.size), data(other.data) {

    // [STEP 1]: We stole 'other's internal pointer and adopted it! (Zero
    // Copying) [ADIM 1]: 'other'ın pointerını ('data') çaldık ve kendimize verdik. O(1) hız!
    std::cout << "\033[32m[MOVE Constructor]\033[0m DATA STOLEN (Moved)! O(1) "
                 "Speed... (Veri çalındı!)"
              << std::endl;

    // [STEP 2]: POST-THEFT. We MUST nullify 'other' so when it dies, it doesn't
    // delete OUR stolen memory. [ADIM 2]: HIRSIZLIK SONRASI. 'other'ı nullptr yapmalıyız ki, o
    // yıkılırken bizim çalınan RAM'imizi çökertmesin.
    other.data = nullptr;
    other.size = 0;
  }
};

/**
 * @brief EN: A factory function creating a massive temporary memory chunk.
 * TR: Devasa geçici bir bellek üreten fonksiyon.
 */
HeavyBuffer factory() {
  HeavyBuffer temp(1000000); // 1 Million elements / 1 Milyon eleman
  // EN: Temp is an R-Value! Returns a dying object.
  // TR: Temp fonksiyondan çıkarken ölür! (R-Value)
  return temp;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: PERFECT FORWARDING — Passing arguments without losing their value category
// TR: MÜKEMMEL YÖNLENDİRME — Argümanları değer kategorisini kaybetmeden aktarma
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 1. Helper Overloads for Forwarding Demo ─────────────────────────────────────────────────────
// EN: Two overloads that reveal whether the caller passed an L-value or R-value.
// TR: Çağıranın L-value mi yoksa R-value mi gönderdiğini ortaya çıkaran iki aşırı yükleme.

void process(const std::string& signal) {
    // EN: Binds to L-values (named, persistent objects).
    // TR: L-value'lere (isimli, kalıcı nesnelere) bağlanır.
    std::cout << "  [process] L-value received: " << signal << std::endl;
}

void process(std::string&& signal) {
    // EN: Binds to R-values (temporaries, dying objects).
    // TR: R-value'lere (geçici, ölecek nesnelere) bağlanır.
    std::cout << "  [process] R-value received: " << signal << std::endl;
}

// ─── 2. Forwarding Reference (Universal Reference) ───────────────────────────────────────────────
// EN: T&& in a template context is NOT an rvalue reference — it is a "forwarding reference"
// (also called universal reference). It deduces T as lvalue-ref or rvalue-ref depending on what
// the caller passes. std::forward<T>(arg) preserves the original value category.
// TR: Şablon bağlamında T&& bir rvalue referansı DEĞİLDİR — "yönlendirme referansı" (evrensel
// referans) adını alır. T, çağıranın gönderdiğine göre lvalue-ref veya rvalue-ref olarak
// çıkarılır. std::forward<T>(arg) orijinal değer kategorisini korur.

template <typename T>
void relay(T&& arg) {
    // EN: std::forward keeps lvalue as lvalue, rvalue as rvalue.
    // TR: std::forward, lvalue'yi lvalue, rvalue'yi rvalue olarak korur.
    process(std::forward<T>(arg));
}

// ─── 3. SensorPacket — Variadic Perfect Forwarding ───────────────────────────────────────────────
// EN: Automotive ECU sensor data packet built via factory with zero unnecessary copies.
// TR: Otomotiv ECU sensör veri paketi — fabrika ile sıfır gereksiz kopya ile oluşturulur.

class SensorPacket {
private:
    std::string sensor_id_;
    double value_;
    std::string unit_;

public:
    SensorPacket(std::string id, double val, std::string unit)
        : sensor_id_(std::move(id)), value_(val), unit_(std::move(unit)) {
        std::cout << "  [SensorPacket] Constructed: " << sensor_id_
                  << " = " << value_ << " " << unit_ << std::endl;
    }

    friend std::ostream& operator<<(std::ostream& os, const SensorPacket& sp) {
        os << "[" << sp.sensor_id_ << ": " << sp.value_ << " " << sp.unit_ << "]";
        return os;
    }
};

// EN: Variadic template factory — every argument is perfectly forwarded into the SensorPacket
// constructor. No extra copy or move happens.
// TR: Değişken sayıda argümanlı şablon fabrikası — her argüman SensorPacket yapıcısına mükemmel
// şekilde yönlendirilir. Fazladan kopya/taşıma olmaz.

template <typename... Args>
SensorPacket createSensorPacket(Args&&... args) {
    return SensorPacket(std::forward<Args>(args)...);
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
  std::cout << "=== MODULE 3: MOVE SEMANTICS & R-VALUE (&&) ===\n" << std::endl;

  std::cout << "\n--- TEST 1: THE OLD C++ COPY (Wasted Power/Sokağa Atılan Güç) ---" << std::endl;
  HeavyBuffer bufferOne(50000);

  // EN: bufferOne is an L-Value (has a name). The compiler IS FORCED to perform a deep COPY.
  // TR: bufferOne bir L-Value'dir (ismi var). Derleyici MECBUREN derin Satır Satır Kopyalama
  // Yapar.
  HeavyBuffer copyBuffer = bufferOne;

  std::cout << "\n--- TEST 2: USING STD::MOVE (Forcible move / Zorla Taşıma) ---" << std::endl;
  // EN: Sometimes, even though bufferOne is an L-Value, we want to say "I'm done with you,
  // transfer to someone else". `std::move(bufferOne)` casts it to an R-Value (`&&`), triggering
  // the MOVE Constructor!
  // TR: Bazen bufferOne L-Value olsa da "Öl, donanımını yeniye aktar" demek isteriz.
  // `std::move(bufferOne)`, objeyi zorla R-Value'ye (`&&`) dönüştürür.
  HeavyBuffer movedBuffer = std::move(bufferOne);
  // WARNING (DİKKAT)! bufferOne is now an empty "shell". Its data is stolen. Do not use it
  // again!

  std::cout << "\n--- TEST 3: FACTORY (R-Value Passage / Fabrika R-Value Geçişi) ---" << std::endl;
  // EN: factory() returns a nameless (R-Value) object. Compiler detects this and triggers Move!
  // TR: factory() isimsiz geçici (R-Value) üretir. Derleyici bunu görür ve hemen Move (Taşıma)
  // tetikler!
  HeavyBuffer fastBuffer = factory();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 4 — Perfect Forwarding: relay() dispatches to correct overload
  // TR: Demo 4 — Mükemmel Yönlendirme: relay() doğru aşırı yüklemeyi çağırır
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 4: PERFECT FORWARDING (Mükemmel Yönlendirme) ---" << std::endl;

  std::string canSignal = "CAN_SPEED_0x1A3";

  // EN: canSignal is an L-value → relay deduces T = std::string&
  // TR: canSignal bir L-value → relay, T = std::string& olarak çıkarır
  std::cout << "  Passing L-value:" << std::endl;
  relay(canSignal);

  // EN: std::string{...} produces a temporary R-value → relay deduces T = std::string
  // TR: std::string{...} geçici R-value üretir → relay, T = std::string olarak çıkarır
  std::cout << "  Passing R-value:" << std::endl;
  relay(std::string{"CAN_RPM_0x0C6"});

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 5 — Universal Reference vs Rvalue Reference
  // TR: Demo 5 — Evrensel Referans ile Rvalue Referansı Farkı
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 5: UNIVERSAL REF vs RVALUE REF ---" << std::endl;

  // EN: KEY DISTINCTION: template<typename T> void foo(T&& x);   // <-- UNIVERSAL (forwarding)
  // ref void bar(std::string&& x);               // <-- plain RVALUE reference
  //
  // Universal ref (T&&) can bind to BOTH lvalue AND rvalue. Plain rvalue ref (Type&&) can ONLY
  // bind to rvalue.
  //
  // TR: KRİTİK FARK: template<typename T> void foo(T&& x);   // <-- EVRENSEL referans void
  // bar(std::string&& x);               // <-- düz RVALUE referansı
  //
  // Evrensel referans (T&&) hem lvalue hem rvalue'ya bağlanabilir. Düz rvalue ref (Tip&&)
  // YALNIZCA rvalue'ya bağlanabilir.

  // EN: Demonstrating that relay (universal ref) accepts both:
  // TR: relay (evrensel ref) her ikisini de kabul eder:
  relay(canSignal);                          // L-value OK
  relay(std::string{"TEMP_COOLANT_0xF4"});   // R-value OK

  // EN: A plain rvalue ref function would REJECT canSignal (l-value).
  // TR: Düz rvalue ref fonksiyon canSignal'i (l-value) REDDEDERDİ.
  std::cout << "  (Plain rvalue ref cannot accept L-values — compile error!)" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 6 — Variadic Perfect Forwarding: createSensorPacket factory
  // TR: Demo 6 — Değişken Argümanlı Mükemmel Yönlendirme: Fabrika
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 6: VARIADIC PERFECT FORWARDING (Fabrika) ---" << std::endl;

  // EN: All arguments are forwarded directly into the SensorPacket constructor.
  // TR: Tüm argümanlar doğrudan SensorPacket yapıcısına yönlendirilir.
  auto throttle  = createSensorPacket("THROTTLE_POS", 73.5, "percent");
  auto oilTemp   = createSensorPacket(std::string{"OIL_TEMP"}, 92.1, std::string{"celsius"});

  std::cout << "  Packet 1: " << throttle << std::endl;
  std::cout << "  Packet 2: " << oilTemp  << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 7 — std::forward vs std::move: when to use which?
  // TR: Demo 7 — std::forward vs std::move: hangisi ne zaman kullanılır?
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 7: std::forward vs std::move ---" << std::endl;

  // EN: RULE OF THUMB: • std::move  → Use when you OWN the object and are DONE with it.
  // Unconditionally casts to rvalue. The source becomes empty. • std::forward → Use ONLY inside
  // templates with forwarding references (T&&). Conditionally casts: preserves lvalue/rvalue
  // category.
  //
  // TR: ALTIN KURAL: • std::move  → Nesneye SAHİPSENİZ ve artık İŞİNİZ BİTTİYSE kullanın.
  // Koşulsuz olarak rvalue'ya dönüştürür. Kaynak boşalır. • std::forward → YALNIZCA T&&
  // yönlendirme referanslı şablonlarda kullanın. Koşullu dönüştürme yapar: lvalue/rvalue
  // kategorisini korur.

  std::string ecu_msg = "ECU_DIAG_REQUEST";

  // EN: std::move — we are transferring ownership deliberately.
  // TR: std::move — sahipliği kasıtlı olarak devrediyoruz.
  std::string transferred = std::move(ecu_msg);
  std::cout << "  After std::move, original is empty: '" << ecu_msg << "'" << std::endl;
  std::cout << "  Transferred holds: '" << transferred << "'" << std::endl;

  // EN: std::forward — only meaningful inside relay() / template context. Outside templates,
  // prefer std::move for owned objects.
  // TR: std::forward — yalnızca relay() / şablon bağlamında anlamlıdır. Şablonların dışında,
  // sahip olduğunuz nesneler için std::move tercih edin.
  std::cout << "  (std::forward is used inside relay() — see Demo 4 output)" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 8 — Reference Collapsing Rules / Referans Çökme Kuralları  
  // TR: Demo 8 — Referans Çökme Kuralları  
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 8: REFERENCE COLLAPSING RULES ---" << std::endl;

  // EN: The C++ standard defines reference collapsing rules. When a template T&& is deduced:
  //
  //     ┌───────────────┬──────────────┬──────────────────────────────────────────┐
  //     │ T deduced as  │ T&&  becomes │ Explanation                              │
  //     ├───────────────┼──────────────┼──────────────────────────────────────────┤
  //     │ Widget        │ Widget&&     │ Rvalue ref (caller passed rvalue)        │
  //     │ Widget&       │ Widget&      │ & + && = & (lvalue collapse)             │
  //     │ Widget&&      │ Widget&&     │ && + && = && (rvalue stays)              │
  //     └───────────────┴──────────────┴──────────────────────────────────────────┘
  //
  //     Rule: If EITHER reference is an lvalue reference (&), result is &.
  //           Only && + && = &&.
  //
  // TR: C++ standardı referans çökme (collapsing) kurallarını tanımlar:
  //     Kural: Referanslardan BİRİ lvalue referansı (&) ise sonuç & olur.
  //            Yalnızca && + && = && verir.

  // EN: Demonstrate with type deduction
  // TR: Tip çıkarımı ile gösterim
  auto showRefType = [](auto&& val) {
    // EN: If val binds to lvalue, T=string& → T&&=string& (collapsed)
    //     If val binds to rvalue, T=string  → T&&=string&&
    // TR: val lvalue'ya bağlanırsa, T=string& → T&&=string& (çökme)
    //     val rvalue'ya bağlanırsa, T=string  → T&&=string&&
    if constexpr (std::is_lvalue_reference_v<decltype(val)>) {
      std::cout << "  Received as LVALUE reference (&)" << std::endl;
    } else {
      std::cout << "  Received as RVALUE reference (&&)" << std::endl;
    }
  };

  std::string lval = "I am an lvalue";
  showRefType(lval);                      // T = string&  → T&& = string&   (& + && = &)
  showRefType(std::string{"I am rvalue"}); // T = string   → T&& = string&& (&& stays)
  showRefType(std::move(lval));           // T = string   → T&& = string&& (xvalue → rvalue)

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 9 — Universal Reference vs Rvalue Reference: The Distinction  
  // TR: Demo 9 — Evrensel Referans ile Rvalue Referans Arasındaki Fark  
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 9: UNIVERSAL REF vs RVALUE REF DISTINCTION ---" << std::endl;

  // EN: KEY DISTINCTION — memorize this:
  //
  //     ┌──────────────────────────────┬───────────────────┬────────────────────┐
  //     │ Syntax                       │ What it is        │ Binds to           │
  //     ├──────────────────────────────┼───────────────────┼────────────────────┤
  //     │ void f(Widget&& w)           │ Rvalue reference  │ Rvalues ONLY       │
  //     │ template<class T> f(T&& w)   │ Forwarding ref    │ BOTH lval & rval   │
  //     │ auto&& x = expr;             │ Forwarding ref    │ BOTH lval & rval   │
  //     │ Widget&& w = expr;           │ Rvalue reference  │ Rvalues ONLY       │
  //     │ void f(vector<T>&& v)        │ Rvalue reference  │ Rvalues ONLY       │
  //     └──────────────────────────────┴───────────────────┴────────────────────┘
  //
  //     The ONLY way to get a forwarding reference:
  //     (1) T&& where T is deduced, OR (2) auto&&
  //     vector<T>&& is NOT a forwarding ref because T is not deduced from &&.
  //
  // TR: ANAHTAR AYRIM:
  //     Yönlendirme referansı elde etmenin TEK yolu:
  //     (1) T'nin çıkarıldığı T&& VEYA (2) auto&&
  //     vector<T>&& yönlendirme referansı DEĞİLDİR.

  // EN: auto&& is a forwarding reference — deduces from initializer
  // TR: auto&& bir yönlendirme referansıdır — başlatıcıdan çıkarım yapar
  auto&& fwdRef1 = lval;                           // auto = string& → auto&& = string&
  auto&& fwdRef2 = std::string{"temp"};             // auto = string  → auto&& = string&&
  std::cout << "  auto&& from lvalue: "
            << (std::is_lvalue_reference_v<decltype(fwdRef1)> ? "lvalue ref" : "rvalue ref")
            << std::endl;
  std::cout << "  auto&& from rvalue: "
            << (std::is_lvalue_reference_v<decltype(fwdRef2)> ? "lvalue ref" : "rvalue ref")
            << std::endl;

  std::cout << "\n=== Program Ending (Destructors Will Run / Yıkıcılar "
               "Çalışacak) ===\n"
            << std::endl;

  return 0;
}
