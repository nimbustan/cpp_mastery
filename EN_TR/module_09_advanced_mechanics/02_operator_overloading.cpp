/**
 * @file module_09_advanced_mechanics/02_operator_overloading.cpp
 * @brief Advanced Mechanics: Operator Overloading — İleri Teknikler: Operatör Aşırı Yükleme
 *
 * @details
 * =============================================================================
 * [Operator Overloading (Operatörlerin Ezilmesi / Aşırı Yüklenmesi)]
 * =============================================================================
 * EN: C++ allows us to modify what standard operators (`+`, `-`, `==`, `<<`, `()`) actually do
 * when used with our custom Classes. This allows our custom Types to feel and act exactly like
 * built-in native C++ types (`int`, `float`).
 *
 * TR: C++ dili, kendi yarattığımız devasa Sınıfları (Objects) tıpkı bir `int` veya `float` gibi
 * toplama `+`, çıkarma `-`, karılaştırma `==` operatörleriyle kullanabilmemiz için Operatör Ezme
 * (Aşırı Yükleme) yetkisi verir.
 *
 * =============================================================================
 * [Why overload `<<`? / Neden Sol Kaydırma `<<` ezilir?]
 * =============================================================================
 * EN: If you type `std::cout << myObject;`, the compiler throws a FATAL ERROR. Because it
 * doesn't know how to "Print" your custom object. By overloading `operator<<` as a `friend`
 * function, we teach `std::ostream` how to read your object.
 *
 * TR: Eğer bir objeye `std::cout << obje` derseniz derleyici küfür eder. Çünkü objeyi ekrana
 * nasıl basacağını bilemez. `operator<<` fonksiyonunu sınıfın içine bir `friend` (dost
 * fonksiyon) olarak aşılarsak kütüphane objemizi şakır şakır okur.
 *
 * [CPPREF DEPTH: Operator Overloading — ADL, Hidden Friends, and the Spaceship / CPPREF
 * DERİNLİK: Operatör Aşırı Yükleme — ADL, Gizli Dostlar ve Uzay Gemisi]
 * =============================================================================
 * EN: Argument-Dependent Lookup (ADL) finds `operator<<` in the namespace of the operand type,
 * even without a `using` directive. The "hidden friend" idiom defines `friend operator==` inline
 * inside the class — it is found only via ADL and prevents implicit conversions on the LHS
 * operand. C++20 `operator<=>` (the "spaceship operator") auto-generates all six comparison
 * operators (`<`, `>`, `<=`, `>=`, `==`, `!=`). Return types: `std::strong_ordering` (total
 * order, substitutable), `std::weak_ordering` (total order, not substitutable), and
 * `std::partial_ordering` (permits incomparable values, e.g., NaN).
 *
 * TR: Argümana Bağlı Arama (ADL), `using` direktifi olmadan bile `operator<<` fonksiyonunu
 * operandın ad alanında bulur. "Gizli dost" deyimi, sınıf içinde `friend operator==` tanımlar —
 * yalnızca ADL ile bulunur ve sol operandda örtük dönüşümleri engeller. C++20 `operator<=>`
 * ("uzay gemisi operatörü") altı karşılaştırma operatörünü otomatik üretir. Dönüş türleri:
 * `std::strong_ordering` (tam sıralama, ikame edilebilir), `std::weak_ordering` (tam sıralama,
 * ikame edilemez) ve `std::partial_ordering` (karşılaştırılamaz değerlere izin verir, ör: NaN).
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_operator_overloading.cpp -o 02_operator_overloading
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <tuple>

// EN: A 2D Math Vector (Not std::vector array, but a mathematical Coordinate System) TR: 2D
// Matematik Vektörü (Dizilerdeki vector değil, fizikteki XYZ vektörü).
class Vector2D {
private:
  float x;
  float y;

public:
  Vector2D(float inputX = 0, float inputY = 0) : x(inputX), y(inputY) {}

  // -----------------------------------------------------------------------------------------------
  // [1] Operator Overloading for Addition `+` (Toplama Operatörü)
  // -----------------------------------------------------------------------------------------------
  // EN: Tells C++ what happens when we do: vectorA + vectorB
  // TR: vecA + vecB yaparsak arka planda tam olarak ne olacağını C++'a öğretiyoruz.
  Vector2D operator+(const Vector2D &other) const {
    return Vector2D(this->x + other.x, this->y + other.y);
  }

  // -----------------------------------------------------------------------------------------------
  // [2] Operator Overloading for Equality `==` (Eşitlik Operatörü)
  // -----------------------------------------------------------------------------------------------
  bool operator==(const Vector2D &other) const {
    // EN: Note: Float comparison requires an epsilon gap. simplified here.
    // TR: Normalde float kıyası risklidir (1.000001), ama eğitim için basit tutuyoruz.
    return (this->x == other.x && this->y == other.y);
  }

  // -----------------------------------------------------------------------------------------------
  // [3] Operator Overloading for Function Call `()` (Functor Mimarisinin
  // Temeli)
  // -----------------------------------------------------------------------------------------------
  // EN: Allows an OBJECT to be called exactly like a FUNCTION. `myObject()`
  // TR: Bir OBJEYİ, sanki sıradan bir fonksiyonmuş gibi çağırmayı (`obje()`) sağlar.
  void operator()() const {
    std::cout << "[Functor Call] Vector coordinates: (" << x << ", " << y << ")" << std::endl;
  }

  // -----------------------------------------------------------------------------------------------
  // [4] STREAM OPERATOR OVERLOADING (`<<`) -> Using `friend`
  // -----------------------------------------------------------------------------------------------
  // EN: Stream operators CANNOT be normal member functions! They must be global or friends. TR:
  // Cout/Ostream fonksiyonları dışarıdan geldiği için `friend` anahtar kelimesi ile Sınıfımızın
  // 'Mahrem (Private)' bölgelerine erişim izni vermemiz gerekir!
  friend std::ostream &operator<<(std::ostream &out, const Vector2D &vec) {
    out << "[" << vec.x << ", " << vec.y << "]";
    // EN: We return out to chain calls: `cout << v1 << v2;` (Zincirleme)
    // TR: Zincirleme çağrılar için out döndürürüz: `cout << v1 << v2;`
    return out;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: SensorReading — All 6 comparison operators via std::tie idiom
// TR: SensorReading — std::tie deyimiyle 6 karşılaştırma operatörünün tamamı
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class SensorReading {
private:
    double timestamp_;
    double value_;
    std::string sensor_id_;

public:
    SensorReading(double ts, double val, std::string id)
        : timestamp_(ts), value_(val), sensor_id_(std::move(id)) {}

    // ─── 1. Equality == ──────────────────────────────────────────────────────────────────────────
    // EN: Two readings are equal when both timestamp and value match.
    // TR: Zaman damgası ve değer eşleştiğinde iki okuma eşittir.
    bool operator==(const SensorReading& o) const {
        return std::tie(timestamp_, value_) == std::tie(o.timestamp_, o.value_);
    }

    // ─── 2. Inequality != ────────────────────────────────────────────────────────────────────────
    // EN: Defined in terms of == for consistency.
    // TR: Tutarlılık için == ile tanımlanır.
    bool operator!=(const SensorReading& o) const {
        return !(*this == o);
    }

    // ─── 3. Less-than < ──────────────────────────────────────────────────────────────────────────
    // EN: Lexicographic comparison via std::tie — first by timestamp, then value.
    // TR: std::tie ile sözlüksel karşılaştırma — önce zaman damgası, sonra değer.
    bool operator<(const SensorReading& o) const {
        return std::tie(timestamp_, value_) < std::tie(o.timestamp_, o.value_);
    }

    // ─── 4. Greater-than > ───────────────────────────────────────────────────────────────────────
    // EN: Flip the operands of < for > .
    // TR: > için < operandları ters çevrilir.
    bool operator>(const SensorReading& o) const {
        return o < *this;
    }

    // ─── 5. Less-or-equal <= ─────────────────────────────────────────────────────────────────────
    // EN: Not greater means less-or-equal.
    // TR: Büyük değilse küçük-eşittir.
    bool operator<=(const SensorReading& o) const {
        return !(o < *this);
    }

    // ─── 6. Greater-or-equal >= ──────────────────────────────────────────────────────────────────
    // EN: Not less means greater-or-equal.
    // TR: Küçük değilse büyük-eşittir.
    bool operator>=(const SensorReading& o) const {
        return !(*this < o);
    }

    // ─── Stream output operator ──────────────────────────────────────────────────────────────────
    // EN: Prints sensor reading in automotive diagnostic format.
    // TR: Sensör okumasını otomotiv teşhis formatında yazdırır.
    friend std::ostream& operator<<(std::ostream& os, const SensorReading& sr) {
        os << "[" << sr.sensor_id_ << " @" << sr.timestamp_
           << "s : " << sr.value_ << "]";
        return os;
    }

    // ─── C++20 Spaceship Operator (Reference Only) ───────────────────────────────────────────────
    // EN: In C++20, a SINGLE declaration replaces all 6 comparison operators: auto
    // operator<=>(const SensorReading&) const = default; The compiler auto-generates ==, !=, <,
    // >, <=, >= from this one line. It performs member-wise lexicographic comparison in
    // declaration order.
    // TR: C++20'de TEK bir bildirim 6 karşılaştırma operatörünün hepsini değiştirir: auto
    // operator<=>(const SensorReading&) const = default; Derleyici bu tek satırdan ==, !=, <, >,
    // <=, >= operatörlerini üretir. Üye bazında, bildirim sırasına göre sözlüksel karşılaştırma
    // yapar.
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: EventCounter — Prefix and Postfix increment operators
// TR: EventCounter — Önek ve Sonek artırma operatörleri
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class EventCounter {
private:
    int count_;
    std::string event_name_;

public:
    EventCounter(std::string name, int initial = 0)
        : count_(initial), event_name_(std::move(name)) {}

    // ─── Prefix ++counter (returns reference to incremented self) ────────────────────────────────
    // EN: Prefix increment — increments first, then returns the new value.
    // TR: Önek artırma — önce artırır, sonra yeni değeri döndürür.
    EventCounter& operator++() {
        ++count_;
        return *this;
    }

    // ─── Postfix counter++ (int dummy distinguishes from prefix) ─────────────────────────────────
    // EN: Postfix increment — the dummy 'int' parameter tells the compiler this is postfix.
    // Returns the OLD value, then increments.
    // TR: Sonek artırma — 'int' kukla parametresi derleyiciye sonek olduğunu bildirir. ESKİ
    // değeri döndürür, sonra artırır.
    EventCounter operator++(int) {
        EventCounter old = *this;
        ++count_;
        return old;
    }

    friend std::ostream& operator<<(std::ostream& os, const EventCounter& ec) {
        os << "[" << ec.event_name_ << ": " << ec.count_ << "]";
        return os;
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
  std::cout << "=== MODULE 9: OPERATOR OVERLOADING ===\n" << std::endl;

  Vector2D pointVelocity_A(5.5f, -2.0f);
  Vector2D pointVelocity_B(2.0f, 4.0f);

  // 1. Addition Overload (O-Overload of +)
  // TR: Sanki int veya float imiş gibi doğal toplama yapar!
  Vector2D result = pointVelocity_A + pointVelocity_B;

  // 2. Stream Overload (O-Overload of <<)
  // TR: `<<` ezildiği (overload edildiği) için obje anında string algısı yaratır!
  std::cout << "Velocity A : " << pointVelocity_A << std::endl;
  std::cout << "Velocity B : " << pointVelocity_B << std::endl;
  std::cout << "Result (A+B): " << result << std::endl;

  // 3. Equality Overload (O-Overload of ==)
  if (pointVelocity_A == pointVelocity_B) {
    std::cout << "Both vectors are totally equal!" << std::endl;
  } else {
    std::cout << "Vectors are pointing to different directions." << std::endl;
  }

  // 4. Function Overload / Functor Mimari Testi (O-Overload of ())
  std::cout << "\nTriggering Functor Object Call:" << std::endl;
  // EN: Looks like a function, but it is actually the OBJECT acting like a function.
  // TR: Fonksiyon gibi görünür ama aslında NESNE fonksiyon gibi davranır.
  result();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 5 — All 6 comparison operators on SensorReading
  // TR: Demo 5 — SensorReading üzerinde 6 karşılaştırma operatörü
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 5: SENSOR READING COMPARISONS ---" << std::endl;

  SensorReading rpm_early(1.00, 3200.0, "RPM_SENSOR");
  SensorReading rpm_late (2.50, 3200.0, "RPM_SENSOR");
  SensorReading rpm_same (1.00, 3200.0, "RPM_SENSOR");

  std::cout << "  A: " << rpm_early << std::endl;
  std::cout << "  B: " << rpm_late  << std::endl;
  std::cout << "  C: " << rpm_same  << std::endl;

  // EN: Testing all 6 comparison operators.
  // TR: 6 karşılaştırma operatörünün tümünü test ediyoruz.
  std::cout << "  A == C : " << (rpm_early == rpm_same  ? "true" : "false") << std::endl;
  std::cout << "  A != B : " << (rpm_early != rpm_late  ? "true" : "false") << std::endl;
  std::cout << "  A <  B : " << (rpm_early <  rpm_late  ? "true" : "false") << std::endl;
  std::cout << "  B >  A : " << (rpm_late  >  rpm_early ? "true" : "false") << std::endl;
  std::cout << "  A <= C : " << (rpm_early <= rpm_same  ? "true" : "false") << std::endl;
  std::cout << "  B >= A : " << (rpm_late  >= rpm_early ? "true" : "false") << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 6 — Prefix vs Postfix increment on EventCounter
  // TR: Demo 6 — EventCounter üzerinde önek vs sonek artırma
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 6: PREFIX vs POSTFIX INCREMENT ---" << std::endl;

  EventCounter brakeEvents("BRAKE_PRESS", 0);
  std::cout << "  Initial : " << brakeEvents << std::endl;

  // EN: Prefix — increments then returns the NEW value.
  // TR: Önek — artırır, sonra YENİ değeri döndürür.
  std::cout << "  ++counter: " << ++brakeEvents << std::endl;
  std::cout << "  After    : " << brakeEvents   << std::endl;

  // EN: Postfix — returns OLD value, then increments.
  // TR: Sonek — ESKİ değeri döndürür, sonra artırır.
  std::cout << "  counter++: " << brakeEvents++ << std::endl;
  std::cout << "  After    : " << brakeEvents   << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 7 — Stream output operator for SensorReading
  // TR: Demo 7 — SensorReading için akış çıkış operatörü
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- DEMO 7: STREAM OUTPUT FOR SENSOR READING ---" << std::endl;

  SensorReading coolant(0.50, 87.3,  "COOLANT_TEMP");
  SensorReading battery(0.50, 13.8,  "BATTERY_VOLT");
  SensorReading oil    (1.20, 102.5, "OIL_PRESSURE");

  // EN: The overloaded << operator prints in automotive diagnostic format.
  // TR: Aşırı yüklenmiş << operatörü otomotiv teşhis formatında yazdırır.
  std::cout << "  " << coolant << std::endl;
  std::cout << "  " << battery << std::endl;
  std::cout << "  " << oil     << std::endl;

  // EN: Chained stream output — works because operator<< returns ostream&.
  // TR: Zincirleme akış çıkışı — operator<< ostream& döndürdüğü için çalışır.
  std::cout << "  Chained: " << coolant << " | " << battery << " | " << oil << std::endl;

  return 0;
}
