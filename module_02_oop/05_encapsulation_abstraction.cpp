/**
 * @file module_02_oop/05_encapsulation_abstraction.cpp
 * @brief OOP Part 5: Encapsulation & Abstraction / OOP Kısım 5: Kapsülleme ve Soyutlama
 *
 * @details
 * =============================================================================
 * [THEORY: The 4 Pillars of OOP / TEORİ: 4 Büyük OOP Sütunu]
 * =============================================================================
 * EN: 1. Inheritance -> Prevents code duplication (DRY). 2. Polymorphism -> Provides dynamic
 * flexibility through the V-Table. 3. Encapsulation -> Data hiding and security shield. 4.
 * Abstraction -> Hides the "How" and only shows the "What".
 *
 * TR: 1. Kalıtım (Inheritance) -> Kod tekrarını önler (DRY). 2. Çok Biçimlilik (Polymorphism) ->
 * V-Table ile dinamik esneklik sağlar. 3. Kapsülleme (Encapsulation) -> Veri gizleme ve güvenlik
 * kalkanıdır. 4. Soyutlama (Abstraction) -> Sadece "Neyin" yapılacağını gösterir, "Nasıl"
 * yapıldığını gizler.
 *
 * =============================================================================
 * [THEORY: Why is Encapsulation INDISPENSABLE? / TEORİ: Kapsülleme Neden Vazgeçilmezdir?]
 * =============================================================================
 * EN: Imagine a 'BankAccount' class. If the 'balance' variable is public, ANYONE from anywhere
 * in the code can do: `account.balance = 1000000;`. This breaks reality! Money should only
 * change through `deposit()` or `withdraw()`. Encapsulation hides the data (`private`) and
 * exposes safety checks (`public` methods).
 *
 * TR: Bir 'BankaHesabi' sınıfı düşünün. Eğer 'bakiye' değişkeni 'public' (açık) olursa, kodun
 * herhangi bir yerinden herhangi biri şunu yazabilir: `hesap.bakiye = 1000000;`. Mevcut
 * gerçekliği yıkar! Para SADECE yasal yollarla `paraYatir()` veya `paraCek()` fonksiyonları
 * üzerinden değişmelidir. Kapsülleme, veriyi (`private`) gizler ve dünyaya sadece bizim
 * yazdığımız güvenlik fonksiyonlarını (`public`) sunar (Getter ve Setter mantığı).
 *
 * =============================================================================
 * [THEORY: What is Abstraction? / TEORİ: Soyutlama Nedir?]
 * =============================================================================
 * EN: When you drive a car, do you need to know the piston angles or spark plug timings? No. You
 * just press the gas pedal (Interface). That's Abstraction. "Hide the complex internal
 * mechanism, expose a simple interface." "Interface Classes" created with Pure Virtual Functions
 * (= 0) are the peak of this.
 *
 * TR: Bir araba kullanırken "Motorun piston açılarını" veya "Bujilerin ateşleme milisaniyesini"
 * bilmek zorunda mısınız? Hayır. Sadece gaza (Interface/Arayüz) basarsınız. Abstraction
 * (Soyutlama) budur. "Karmaşık iç mekanizmayı gizle, sadece kullanımı basit olan dış yüzü
 * göster." Saf Sanal Fonksiyonlar (= 0) ile yaratılan "Interface Sınıflar" bunun zirvesidir.
 *
 * [CPPREF DEPTH: Access Specifiers and Friend — Compiler Enforcement vs Runtime / CPPREF
 * DERİNLİK: Erişim Belirteçleri ve Friend — Derleyici Zorlama vs Çalışma Zamanı]
 * =============================================================================
 * EN: Access specifiers (`public`, `private`, `protected`) are COMPILE-TIME ONLY checks. The
 * compiler rejects `obj.privateVar` in source code, but at runtime ALL memory is flat and
 * accessible — a raw pointer cast can read any byte. This is by design: C++ trusts the
 * programmer after compilation. Security comes from the type system, not from memory hiding.
 * `friend` is not a design flaw — it is a CONTROLLED GATE. When two classes are tightly coupled
 * by design (e.g., an iterator and its container, or `operator<<` for a class), `friend` grants
 * precise access without exposing internals to the entire world via public getters. The PIMPL
 * (Pointer to IMPLementation) idiom achieves ABI-stable encapsulation: the public header
 * contains only a forward-declared Impl struct and a `std::unique_ptr<Impl>`. Changing Impl's
 * members does NOT change the public header's sizeof — downstream code need not recompile. This
 * is critical in shared libraries and automotive ECU software where ABI breaks cause fleet-wide
 * update cascades. Note: `protected` is weaker than `private` — any derived class in any
 * translation unit can access protected members, making it a wider trust boundary than many
 * developers realize.
 *
 * TR: Erişim belirteçleri (`public`, `private`, `protected`) YALNIZCA DERLEME ZAMANI
 * kontrolleridir. Derleyici kaynak kodda `obj.privateVar` erişimini reddeder, ancak çalışma
 * zamanında TÜM bellek düzdür ve erişilebilirdir — ham bir işaretçi dönüşümü herhangi bir baytı
 * okuyabilir. Bu tasarım gereğidir: C++ derleme sonrasında programcıya güvenir. `friend` bir
 * tasarım hatası değildir — KONTROLLÜ bir GEÇİTTİR. İki sınıf tasarım gereği sıkı bağlıysa (örn.
 * yineleyici ve konteyneri veya bir sınıf için `operator<<`), `friend` tüm dünyaya public
 * getter'lar ile iç yapıyı açmadan kesin erişim sağlar. PIMPL deyimi ABI-kararlı kapsülleme
 * sağlar: public başlık yalnızca ileri bildirilmiş bir Impl yapısı ve `std::unique_ptr<Impl>`
 * içerir. Impl'in üyelerini değiştirmek public başlığın sizeof değerini DEĞİŞTİRMEZ — bağımlı
 * kodun yeniden derlenmesi gerekmez. Not: `protected`, `private`'den daha zayıftır — herhangi
 * bir çeviri birimindeki türetilmiş sınıf protected üyelere erişebilir.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/access
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

// EN: --- ENCAPSULATION EXAMPLE ---
// TR: --- KAPSÜLLEME ÖRNEĞİ ---
class BankAccount {
private: // DATA HIDING / VERİ GİZLEME
  std::string ownerName;
  double balance;

  // EN: Internal private helper function. CANNOT be called from outside. (Abstraction)
  // TR: Özel bir yardımcı fonksiyondur. Dışarıdan ÇAĞIRILAMAZ. (Soyutlama içerir)
  bool isTransactionValid(double amount) const { return amount > 0; }

public:
  BankAccount(std::string name, double initialDeposit)
      : ownerName(name), balance(initialDeposit) {}

  // EN: GETTER method retrieving the encapsulated real data (Read-Only access):
  // TR: Kapsüllenen veriyi dışarı (sadece OKUMAK için) çıkaran GETTER metodu:
  double getBalance() const {
    // [Look, but don't touch! / Göster, ama dokundurma!]
    return balance;
  }

  // EN: SETTER method modifying the encapsulated data through rules:
  // TR: Kapsüllenen veriyi kurallarıyla değiştiren SETTER / İşlem metodu:
  void deposit(double amount) {
    if (isTransactionValid(amount)) {
      balance += amount;
      std::cout << "[Encapsulation] Safe deposit successful. Amount: " << amount <<
          " / Güvenli para yatırma işlemi başarılı." << std::endl;
    } else {
      std::cout << "[ERROR/HATA] Cannot deposit zero or negative funds! (Sıfır "
                   "veya eksi yatırılamaz!)"
                << std::endl;
    }
  }
};

// --- ABSTRACTION EXAMPLE / SOYUTLAMA ÖRNEĞİ ---

/**
 * @class IEspressoMachine
 * @brief EN: 'I' prefix indicates it is an Interface.
 * TR: 'I' ön eki Interface (Arayüz) olduğunu belirtir.
 *
 * EN: Contains a "Pure Virtual" function (= 0). This class has NO body. You CANNOT create an
 * object like 'new IEspressoMachine()'. It is just a "CONTRACT".
 *
 * TR: Saf Sanal Fonksiyon (Pure Virtual) içerir. Yani bu sınıfın BEDENİ yoktur. Ondan 'new
 * IEspressoMachine()' diye nesne ÜRETEMEZSİNİZ. O sadece bir "TAAHHÜTNAME"dir.
 */
class IEspressoMachine {
public:
  virtual ~IEspressoMachine() = default;

  // EN: '= 0' means just show the rule (interface), no body!
  // TR: '= 0' demek SADECE kuralı (arayüzü) göster, gövdesi yok demektir!
  virtual void makeCoffee() = 0;
};

// EN: The concrete class implementing the real mechanism (the 'How' details).
// TR: Gerçek mekanizmayı uygulayan (Nasıl çalıştığını yazan) Sınıf.
class ModernCoffeeMaker : public IEspressoMachine {
private:
  void grindBeans() {
    std::cout << "1. Grinding beans (0.3mm)... / Çekirdekler öğütülüyor..." << std::endl;
  }
  void boilWater() {
    std::cout << "2. Boiling water to 94C... / Su 94 dereceye isitiliyor..." << std::endl;
  }
  void filterEspresso() {
    std::cout << "3. Filtering with 9 Bars... / 9 Bar basınç ile süzülüyor!" << std::endl;
  }

public:
  void makeCoffee() override {
    // EN: The Core of Abstraction! User ONLY calls makeCoffee, they don't see the complex 3
    // steps.
    // TR: Soyutlamanın Çekirdeği! Kullanıcı SADECE makeCoffee çağırır, şu 3 karmaşık adımı
    // BİLMEZ/GÖRMEZ.
    grindBeans();
    boilWater();
    filterEspresso();
    std::cout << "[Result / Sonuç] Your espresso is ready! (Espresso hazır!)\n" << std::endl;
  }
};

int main() {
  std::cout << "=== MODULE 2: ENCAPSULATION & ABSTRACTION ===\n" << std::endl;

  // EN: 1. Encapsulation Test
  // TR: 1. Kapsülleme Testi
  std::cout << "--- 1. ENCAPSULATION (Kapsülleme) ---" << std::endl;
  BankAccount myAccount("Alice", 1000.0);

  // myAccount.balance = 9999999; // ERROR! private veriye asla dokunamazsın.
  std::cout << "Current Balance (Mevcut Para): " << myAccount.getBalance() << std::endl;

  // EN: Security shield engages!
  // TR: Sistem güvenlik kalkanını devreye sokar!
  myAccount.deposit(-500);

  // 2. Abstraction Test (Soyutlama Testi)
  std::cout << "\n--- 2. ABSTRACTION (Soyutlama) ---" << std::endl;

  // EN: We use the coffee machine through the IEspressoMachine interface. We don't worry about
  // pistons, pressure, or temp. We just press the button.
  // TR: IEspressoMachine arayüzü ile makineyi kullanırız. Pistonlar veya sıcaklık bizi
  // ilgilendirmez. Sadece düğmeye basarız.

  IEspressoMachine *myMachine = new ModernCoffeeMaker();
  myMachine->makeCoffee();

  delete myMachine;

  return 0;
}
