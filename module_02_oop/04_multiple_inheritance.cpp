/**
 * @file module_02_oop/04_multiple_inheritance.cpp
 * @brief OOP Part 4: Multiple Inheritance & The Diamond Problem / Çoklu Kalıtım ve Elmas
 * Problemi
 *
 * @details
 * =============================================================================
 * [THEORY: Why is Multiple Inheritance Banned in Many Languages? / TEORİ: Çoklu Kalıtım Neden
 * Yasaktır?]
 * =============================================================================
 * EN: C++ is one of the very few languages that allows a class to have TWO or more Base classes.
 * `class FlyingCar : public Car, public Airplane`. This sounds cool but causes a deadly issue
 * known as the "Diamond Problem". Java and C# banned this entirely, relying on Interfaces
 * instead.
 *
 * TR: C++, bir sınıfın AYNI ANDA İKİ farklı temel sınıftan (Base) miras almasına olanak tanıyan
 * nadir dillerdendir. Örneğin Uçan Arabamız (FlyingCar) var; hem KaradaGiden hem de HavadaUcan
 * sınıflarından kalıtım alır. Ancak bu harika görünen özellik, "Elmas Problemi (Diamond
 * Problem)" denen felaketi doğurur. Java veya C# bunu tamamen yasaklamış, yerine `Interface`
 * (Arayüz) zorunluluğu getirmiştir.
 *
 * =============================================================================
 * [THEORY: The Diamond Problem & Virtual Inheritance / TEORİ: Elmas Problemi ve Virtual Kalıtım]
 * =============================================================================
 * EN: Imagine: A Base class 'Device' exists (with an int 'power'). - 'Printer' inherits from
 * Device. - 'Scanner' inherits from Device. - 'Copier' inherits from BOTH Printer and Scanner!
 * Result: 'Copier' now contains TWO sets of 'power'! A waste of RAM and ambiguous. C++ solves
 * this using `virtual public` inheritance: "Only make 1 copy of Device."
 *
 * TR: Düşünün: 'Alet' (Device) adında bir Ana sınıf var (içinde 'int power' eklentisi olsun). -
 * 'Yazici' (Printer), Alet'ten miras alır. - 'Tarayici' (Scanner), Alet'ten miras alır. -
 * 'HepsiBirArada' (Copier), hem Yazici'dan hem Tarayici'dan miras alır!
 *
 * Sonuç: HepsiBirArada nesnesinin içinde İKİ ADET 'power' değişkeni oluşur! Bir RAM israfı ve
 * Belirsizlik! C++ bunu çözmek için `virtual public` kalıtımını icat etti. "Güç değişkeninden
 * sadece 1 adet kopyala."
 *
 * [CPPREF DEPTH: The Diamond Problem and Virtual Base Classes — Memory Layout / CPPREF DERİNLİK:
 * Elmas Problemi ve Sanal Temel Sınıflar — Bellek Düzeni]
 * =============================================================================
 * EN: With normal (non-virtual) multiple inheritance, each path to a shared base class creates a
 * separate sub-object. If both Printer and Scanner inherit from Device without `virtual`, Copier
 * contains TWO Device sub-objects — sizeof(Copier) includes both. Virtual inheritance changes
 * the ABI layout fundamentally. Instead of embedding the base sub-object directly, each
 * virtual-inheriting class stores a VBASE OFFSET (or vbase pointer) inside its vtable. This
 * offset tells the runtime where the single shared virtual base resides relative to the current
 * sub-object. The shared Device is placed at the END of the most-derived object's memory layout.
 * Construction order with virtual bases: ALL virtual bases are constructed FIRST, in
 * left-to-right, depth-first order — and only by the MOST DERIVED class's constructor (not
 * intermediate classes). Then non-virtual bases are constructed left-to-right, then the class
 * itself. sizeof() increases with virtual inheritance due to the hidden vptr or vbase-offset
 * pointers added by the compiler. On a typical 64-bit ABI, each virtual inheritance level adds 8
 * bytes per sub-object for the vptr. This is why `dynamic_cast` across virtual bases is more
 * expensive — it must traverse offsets stored in the vtable to locate the target.
 *
 * TR: Normal (sanal olmayan) çoklu kalıtımda, paylaşılan bir temel sınıfa giden her yol ayrı bir
 * alt nesne oluşturur. Printer ve Scanner, Device'tan `virtual` olmadan miras alırsa, Copier İKİ
 * Device alt nesnesi içerir. Sanal kalıtım ABI düzenini temelden değiştirir. Temel alt nesneyi
 * doğrudan gömmek yerine, sanal miras alan her sınıf vtable'ında bir VBASE OFFSET (sanal taban
 * ofseti) saklar. Bu ofset, paylaşılan sanal tabanın geçerli alt nesneye göre nerede bulunduğunu
 * çalışma zamanında belirtir. Paylaşılan Device, en türetilmiş nesnenin bellek düzeninin SONUNA
 * yerleştirilir. Sanal tabanlarda yapılandırma sırası: TÜM sanal tabanlar İLK ÖNCE, soldan sağa
 * ve derinlik öncelikli sırada — yalnızca EN TÜRETİLMİŞ sınıfın yapıcısı tarafından oluşturulur.
 * Sonra sanal olmayan tabanlar soldan sağa yapılandırılır, ardından sınıfın kendisi. sizeof()
 * sanal kalıtımla artar çünkü derleyici gizli vptr veya vbase-ofset işaretçileri ekler. Tipik
 * bir 64-bit ABI'de her sanal kalıtım seviyesi alt nesne başına 8 bayt ekler.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/derived_class
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

// EN: 1. TOP LEVEL ROOT CLASS
// TR: 1. KÖK SINIF
class Device {
public:
  int powerConsumption;
  Device() : powerConsumption(100) {
    std::cout << "[Device] Root created. / Kök üretildi." << std::endl;
  }
};

// EN: 2. SUBCLASS 1 (Virtual Inheritance!)
// TR: 2. ALT SINIF 1 (Virtual Kalıtım!)
// EN: If we do not say 'virtual' here, the final child class will copy 'powerConsumption' twice!
// TR: Eğer 'virtual' demezsek, en aşağıdaki sentez cihaz kendi içinde 'powerConsumption'dan İKİ
// KOPYA üretir.
class Printer : virtual public Device {
public:
  Printer() {
    std::cout << "[Printer] Print capability added. / Yazdırma yeteneği eklendi." << std::endl;
  }
  void print() { std::cout << "Printing... / Baskı yapılıyor..." << std::endl; }
};

// EN: 3. SUBCLASS 2 (Virtual Inheritance!)
// TR: 3. ALT SINIF 2 (Virtual Kalıtım!)
class Scanner : virtual public Device {
public:
  Scanner() {
    std::cout << "[Scanner] Scan capability added. / Tarama yeteneği eklendi." << std::endl;
  }
  void scan() { std::cout << "Scanning... / Belge taranıyor..." << std::endl; }
};

// EN: 4. THE NIGHTMARE: MULTIPLE INHERITANCE
// TR: 4. KABUS: ÇOKLU KALITIM
class AllInOneCopier : public Printer, public Scanner {
public:
  AllInOneCopier() {
    std::cout << "[AllInOneCopier] Synthesis ready! / Sentez cihaz hazır!" << std::endl;
  }

  void showPower() {
    // EN: If we hadn't used 'virtual' inheritance above, 'powerConsumption' would be AMBIGUOUS
    // and the code WOULD NOT COMPILE. (It wouldn't know which 'power' to access).
    //
    // TR: Eğer yukarıda 'virtual' kullanmasaydık, 'powerConsumption' AMBIGUOUS (Belirsiz)
    // olacaktı ve kod DERLENMEYECEKTİ (Hangisinin 'power'ına ulaşmak istediğimizi
    // bilemeyecekti).
    std::cout << "Power Consumption (Güç Tüketimi): " << powerConsumption <<
        " Watt (Diamond Problem Solved / Problem çözüldü!)" << std::endl;
  }
};

int main() {
  std::cout << "=== MODULE 2: MULTIPLE INHERITANCE (DIAMOND PROBLEM) ===\n" << std::endl;

  AllInOneCopier machine;

  std::cout << "\nMachine Tests / Makine Testleri:" << std::endl;
  machine.print();
  machine.scan();
  machine.showPower();

  return 0;
}
