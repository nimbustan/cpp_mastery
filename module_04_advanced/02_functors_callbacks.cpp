/**
 * @file module_04_advanced/02_functors_callbacks.cpp
 * @brief Advanced STL: Functors and Callbacks / İleri STL: Obje Fonksiyonlar (Functors) ve Geri
 * Çağırmalar
 *
 * @details
 * =============================================================================
 * [THEORY: Why Are Standard Functions Insufficient? (What is a Functor?) / TEORİ: Neden Sıradan
 * Fonksiyonlar Yetersiz Kalır?]
 * =============================================================================
 * EN: A regular function holds NO state (no memory of past calls). A "Functor" (Function Object)
 * is a Class that acts like a function by overloading `operator()`. It can have internal
 * variables (state), a Constructor, and can be passed to STL algorithms like `std::sort` or
 * `std::for_each`.
 *
 * TR: Alışageldiğimiz sıradan fonksiyonların "Hafızası" (State/Durum) yoktur. Kaç kere
 * çağırıldığı, içine konan parametrelerin geçmişi vs silinir. "Functor" (Fonksiyon Objesi),
 * sanki bir fonksiyonmuş gibi davranan ama aslında bir "Sınıf" (Class) olan yapılardır. Bunu
 * `operator()` ezerek (overload) yapar. İçinde kendi "private" hafızasını tutabilir! Özel bir
 * devrimdir.
 *
 * =============================================================================
 * [THEORY: Callbacks and std::function / TEORİ: Callbacks ve std::function]
 * =============================================================================
 * EN: In HMI (Head-Up Display) or Networking, when a button is pressed, you don't know exactly
 * which logic should run at compile time. You "donate" a function to that button. This is called
 * a "Callback". Using `#include <functional>`, `std::function` acts as a universal vessel
 * capable of storing ANY callable (Lambdas, Functors, raw functions) like a variable!
 *
 * TR: Bir butona tıklandığında (UI programlama) HANGİ fonksiyonun çalışacağını önceden
 * bilemezsiniz. O butona bir fonksiyon "bağışlarsınız". Buna "Callback" (Geri Çağırma) denir.
 * `<functional>` kütüphanesi ile `std::function` kullanarak her türlü fonksiyonu (lambda,
 * functor, normal fonk) bir C++ değişkeni gibi saklayabilirsiniz.
 *
 * =============================================================================
 * [CPPREF DEPTH: `std::function` CPU Penalty & Type Erasure (Tip Silinmesi) / CPPREF DERİNLİK:
 * `std::function` CPU Cezası ve Tip Silinmesi]
 * =============================================================================
 * EN: `std::function` achieves this magic using "Type Erasure". However, if the lambda/functor
 * you store inside it is too large (usually > 16-32 bytes), it FAILS the "Small Object
 * Optimization (SOO)" and silently calls `new` to allocate heap memory! In a rendering loop (60
 * FPS), this silent allocation can murder system performance!
 *
 * TR: `std::function` bu esnekliği "Type Erasure (Tip Silinmesi)" ile yapar. Fakat içine
 * koyduğunuz Lambda objesi çok büyükse (Örneğin bir sürü şeyi [=] ile hapsettiyse), "Küçük Nesne
 * Optimizasyonunu (SOO)" aşar ve o okyanus kadar objeyi saklamak için arkaplanda gizlice `new`
 * ile Heap(RAM) ayırır! Saniyede 60 kere çalışan bir ECU kontrol döngüsünde `std::function`
 * yüzünden gizlice RAM alınıp silinmesi performansı FELÇ eder!
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>
#include <functional> // EN: std::function
#include <iostream>
#include <vector>

/**
 * @class DeductorFunctor
 * @brief EN: A Stateful Functor evaluating ongoing calculations.
 * TR: Özel bir durum (State) tutan Obje Fonksiyon (Functor). Her çağırılışta kendisinden önceki
 * bakiyeyi HATIRLAR (Stateful)!
 */
class DeductorFunctor {
private:
  // EN: STATE (Memory of past calls)
  // TR: STATE (Durum - Fonksiyon Hafızası)
  int balance;
public:
  DeductorFunctor(int initialBalance) : balance(initialBalance) {}

  // EN: Overloading the parenthesis () operator! Now this CLASS can be called like a FUNCTION!
  // TR: Parantez () operatörü aşırı yüklendi! (Overloaded). Artık bu SINIF, bir FONKSİYON gibi
  // `banka(10);` şeklinde ÇAĞRILABİLİR!
  void operator()(int amount) {
    balance -= amount;
    std::cout << amount << " spent (harcandı). Remaining Balance (Kalan bakiye): " << balance <<
        std::endl;
  }
};

/**
 * @brief EN: GUI or Networking Simulation. "Give me a function to execute later!"
 * TR: GUI veya Ağ simülasyonu. "Bana çalışması bitince bir fonksiyon ver, çağıracağım!"
 * @param callback Çalıştırılacak olay. `std::function` sayesinde Lambda, Functor fark etmez!
 */
void executeEvent(std::function<void(int)> callback) {
  std::cout << "\n[SYSTEM] Event triggered! Starting Callback... (Olay "
               "tetiklendi! Geri çağırma başlıyor...)"
            << std::endl;
  // EN: Running the injected function internally.
  // TR: Bize verilen fonksiyonu kendi içimizde çalıştırdık.
  callback(50);
}

int main() {
  std::cout << "=== MODULE 4: FUNCTORS & CALLBACKS (std::function) ===\n" << std::endl;

  // 1. Functor Test (Stateful Function Magic / Hafızalı Fonksiyon Büyüsü)
  std::cout << "--- 1. FUNCTORS (Stateful Functions) ---" << std::endl;
  DeductorFunctor bankSystem(100);

  std::cout << ">> Used exactly like a function, but it's an Object! "
               "(Fonksiyon gibi çağrılıyor):"
            << std::endl;
  // EN: 1st Call: Balance is 80. (It remembers!)
  // TR: 1. Çağrı: Bakiye 80. (Hatırlıyor!)
  bankSystem( 20);
  // EN: 2nd Call: Balance is 65.
  // TR: 2. Çağrı: Bakiye 65.
  bankSystem(15);

  // 2. Callbacks Test
  std::cout << "\n--- 2. CALLBACKS (std::function) ---" << std::endl;

  // EN: Lambda Carrier
  // TR: Sıradan fonk yerine LAMBDA taşıyıcısı (std::function) kullanıyoruz.
  std::function<void(int)> myLambda = [](int x) {
    std::cout << "Lambda executed! Given value (Lambda çalıştı, değer): " << x << std::endl;
  };

  executeEvent(myLambda);

  // EN: Callback via Functor
  // TR: Functor ile callback (Sınıfı veriyoruz, operator() sayesinde çağırılıyor)
  executeEvent(bankSystem);

  return 0;
}
