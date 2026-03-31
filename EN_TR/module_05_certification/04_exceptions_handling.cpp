/**
 * @file module_05_certification/04_exceptions_handling.cpp
 * @brief CPA/CPP Prep: Exception Handling in C++ — Sertifikasyon: İstisnai Durum (Hata) Yönetimi
 *
 * @details
 * =============================================================================
 * [THEORY: if-else Error Checking vs Exceptions / TEORİ: if-else Hata Kontrolü vs İstisnalar]
 * =============================================================================
 * EN: In old C systems, when an error occurred, functions returned `-1` or `false`. The danger
 * was "Ignoring the Error Value". A developer might forget to check it.
 *
 * In C++, the `try / catch / throw` mechanism exists. If an error is thrown (`throw`), the
 * program starts tearing itself back down the Stack (Stack Unwinding) until it finds a `catch`
 * block. If you never catch it, it CRASHES (`std::terminate`).
 *
 * TR: Eski C sistemlerinde bir hata oluştuğunda fonksiyon `-1` veya `false` döndürürdü. Bu
 * yöntem "Hata değerinin yoksayılması / unutulması (Ignore)" riskini taşırdı.
 *
 * C++'ta `try / catch / throw` mekanizması vardır. Bir hata fırlatılırsa (`throw`), o hatayı
 * Tutan (`catch`) bir yer bulana kadar program kendi kendini Stack'ten (Yığın) geriye doğru
 * söker (Buna "Stack Unwinding" denir). Yakalamazsanız program tamamen GÖÇER (std::terminate).
 *
 * =============================================================================
 * [THEORY: The `noexcept` Keyword (Interview Question) / TEORİ: noexcept Anahtar Kelimesi]
 * =============================================================================
 * EN: C++11 introduced `noexcept` (The guarantee of NOT throwing). It tells the compiler: "I
 * swear this function is perfect. It will NEVER throw." The compiler therefore removes all
 * background try-catch safety netting, making the code INCREDIBLY FAST! Often used in 'Move
 * Constructors'.
 *
 * TR: C++11 ile `noexcept` (Exception fırlatmama sözü/garantisi) geldi. Derleyiciye: "Söz
 * veriyorum, bu fonksiyonum hatasız çalışacak, hiç hata fırlatmayacak" der. Derleyici bunu
 * görünce tüm hata kontrol (try-catch) koruma zırhlarını kaldırır ve KODU İNANILMAZ HIZLANDIRIR!
 * Genellikle 'Move Constructor' (Taşıma Yıkıcıları) ve 'Destructor' (Yıkıcılar) için yazılır.
 *
 * DİKKAT: Eğer `noexcept` verip içinde gizlice `throw` yaparsanız (sözünüzü bozduğunuz an), C++
 * programı ANINDA infaz eder (terminate).
 *
 * =============================================================================
 * [CPPREF DEPTH: The 4 Levels of Exception Safety Guarantees / CPPREF DERİNLİK: 4 İstisna
 * Güvenliği Garanti Seviyesi]
 * =============================================================================
 * EN: CppReference expects professional C++ code to provide one of these guarantees:
 *   1. Nothrow Guarantee: Will NEVER throw (like `noexcept`).
 *   2. Strong Guarantee: If an operation fails, the state is rolled back to exactly what it was
 *      before the call (Commit or Rollback - like Databases).
 *   3. Basic Guarantee: If it fails, no memory is leaked, but the object might be in an altered
 *      (but valid) state.
 *   4. No Guarantee: Fails and corrupts/leaks memory. (Unacceptable in modern C++).
 *
 * TR: CppReference, profesyonel kodun şu 4 Özel Güvenlik Seviyesinden birini vadetmesini bekler:
 *   1. Nothrow: Asla hata fırlatmaz (`noexcept`).
 *   2. Güçlü Teminat (Strong): İşlem başarısız olursa, her şey işlem öncesi haline geri döner.
 *      Hiç dokunulmamış gibi olur (Veritabanlarındaki Rollback gibi).
 *   3. Temel Teminat (Basic): Hata olursa RAM sızdırmaz, çökmeleri engeller ama nesne o anki
 *      bozuk (fakat güvenli) bir halde kalabilir.
 *   4. Garantisiz: Hata olursa RAM sızdırır ve programı çökertir (Modern C++'ta Yasak!)
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_exceptions_handling.cpp -o 04_exceptions_handling
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
// EN: Contains many built-in standard exceptions
// TR: Birçok yerleşik standart hata sınıfını içerir
#include <stdexcept>

// EN: 1. Custom Exception Class
// TR: 1. Özel Hata Sınıfı
// EN: Usually inherited from std::exception.
// TR: Genelde std::exception'dan miras alınarak yapılır.
class BankException : public std::exception {
public:
  // EN: Overriding what() to provide a custom message. Must be `noexcept`.
  // TR: what() fonksiyonunu ezerek kendi mesajımızı veriyoruz. (`noexcept` zorunludur).
  const char *what() const noexcept override {
    return "Insufficient Funds! (Bakiye Yetersiz!)";
  }
};

class BankAccount {
private:
  int balance;

public:
  BankAccount(int initialBalance) : balance(initialBalance) {}

  // EN: 2. A function that THROWS an Exception
  // TR: 2. Hata fırlatan fonksiyon.
  void deposit(int amount) {
    if (amount < 0) {
      // EN: Throw! Execution stops instantly. Program searches for 'catch'.
      // TR: Fırlat (throw)! Program akışı ANINDA KESİLİR, en yakın 'catch' bloğu aranır.
      throw std::invalid_argument( "Cannot deposit negative money! (Negatif para yatırılamaz!)");
    }
    balance += amount;
  }

  void withdraw(int amount) {
    if (amount > balance) {
      // EN: Throwing our CUSTOM Object!
      // TR: Kendi Özel Objemizi fırlattık.
      throw BankException();
    }
    balance -= amount;
  }

  // EN: 3. NOEXCEPT GUARANTEE (Performance Boost)
  // TR: 3. PERFORMANS ARTIŞI
  int getBalance() const noexcept {
    // EN: So simple, it's impossible to throw. Compiler optimizes it to O(1) raw speed.
    // TR: Bu o kadar basit bír işlem ki, hata fırlatması ihtimali SIFIR. Derleyici güvenlik
    // ağlarını siler, en hızlı şekilde (O(1)) C gibi derler.
    return balance;
  }
};

int main() {
  std::cout << "=== MODULE 5: EXCEPTION HANDLING (try/catch/noexcept) ===\n" << std::endl;

  BankAccount myAccount(100);

  // EN: RULE: Trap risky zones (where a throw might happen) inside a 'try' block!
  // TR: KURAL: Riskli bölgeleri (throw olma ihtimali olanları) 'try' içine hapset!
  try {
    std::cout << "Transaction 1 in progress (İşlem 1 yazılıyor)..." << std::endl;
    myAccount.deposit(50); // Balance: 150

    std::cout << "Transaction 2 in progress (İşlem 2 yapılıyor)..." << std::endl;
    // CRASH! / HATA FIRLIYOR! (Aşağıdaki işlemler iptal)
    myAccount.withdraw( 1000);

    std::cout << "You will never see this line! (Bu satırı göremeyeceksiniz! "
                 "Kod kırıldı.)"
              << std::endl;
    myAccount.deposit(-5);

  }
  // EN: catch blocks are Polymorphic. Put the most specific ones FIRST, the generic ones LAST.
  // TR: catch blokları 'Polimorfik'tir. En spesifik (özel) hatayı başa, en geneli sona
  // (Fallback) yazın.

  catch (const BankException &e) {
    // EN: We caught our custom exception by reference (&).
    // TR: Özel Banka sınıfımızı referans(&) ile yakaladık.
    std::cout << "\n[ERROR CAUGHT - BANK]: " << e.what() << std::endl;
  } catch (const std::invalid_argument &e) {
    // EN: Standard argument error.
    // TR: std içinden gelen Argüman hatası.
    std::cout << "\n[ERROR CAUGHT - ARGUMENT]: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    // EN: The ultimate fallback catcher for any other std::exception.
    // TR: Yukarıdaki özel durumlara uymayan başka BİLİNMEYEN std hataları EN SON BURAYA düşer.
    std::cout << "\n[ERROR CAUGHT - GENERIC]: " << e.what() << std::endl;
  }

  std::cout << "\nProgram did not crash! Life goes on! (Program çökmedi!)" << std::endl;
  std::cout << "Current Balance [noexcept] (Mevcut Bakiye): " << myAccount.getBalance() <<
      std::endl;

  return 0;
}
