/**
 * @file module_05_certification/02_object_slicing_virtual.cpp
 * @brief CPA/CPP Prep: Object Slicing and Virtual Tables / Nesne Dilimlenmesi ve Sanal Tablolar
 *
 * @details
 * =============================================================================
 * [THEORY: What is Object Slicing? / TEORİ: Nesne Dilimlenmesi Nedir?]
 * =============================================================================
 * EN: Polymorphism (Dynamic Binding) ONLY works over Pointers (*) or References (&). If you
 * directly assign a Derived object to a Base object (Pass by Value), the compiler literally
 * "slices off" all the new variables and overridden functions that belong to the Derived class.
 * The object becomes a pure Base object!
 *
 * TR: Çok biçimlilik (Polimorfizm) SADECE İşaretçiler (*) veya Referanslar (&) üzerinden
 * çalışır. Eğer türetilmiş (Derived) bir nesneyi doğrudan temel (Base) bir nesneye ("Değer ile",
 * yani Pass by Value) atarsanız, derleyici Alt sınıfa ait olan TÜM yeni değişkenleri ve ezilmiş
 * (overridden) fonksiyonları kelimenin tam anlamıyla "bıçakla kesip / dilimleyip" çöpe atar. O
 * nesne artık saf bir Base nesnesine dönüşür!
 *
 * =============================================================================
 * [THEORY: Why does it slice? (Memory Architecture) / TEORİ: Neden Dilimleniyor?]
 * =============================================================================
 * EN: In C++, objects are contiguous blocks of memory. - Base Class: 4 Bytes - Derived Class: 8
 * Bytes If you say `Base b = Derived();`, the compiler cannot fit 8 Bytes into a 4 Byte box. It
 * HAS to SLICE the overflowing parts off.
 *
 * TR: C++'ta nesneler RAM'de yan yana duran kutucuklardır. - Base Sınıf: 4 Bayt (sadece yaş) -
 * Derived Sınıf: 8 Bayt (hem yaş hem maaş) Eğer `Base b = Derived();` derseniz, derleyici 4
 * baytlık küçük kutunun içine, 8 baytlık dev nesneyi sığdıramaz. Mecburen taflan kısmı kesip
 * atar (Dilimler)!
 *
 * =============================================================================
 * [CPPREF DEPTH: Slicing Destroys RTTI / CPPREF DERİNLİK: Dilimleme RTTI'yı Yok Eder (Yazılımsal
 * Kimlik Katliamı)]
 * =============================================================================
 * EN: Slicing doesn't just cut off memory; it permanently DESTROYS the object's dynamic type
 * identity (typeid / RTTI pointer). The sliced object's hidden `vptr` is forcibly rewritten to
 * point to the Base class's `vtable`. It is completely unrecoverable!
 *
 * TR: Dilimleme sadece fazla byte'ları kesmez; nesnenin dinamik RTTI kimliğini (typeid) SONSUZA
 * DEK YOK EDER. Gizli Sanal İşaretçi (`vptr`), zorla Base sınıfın `vtable`'ına (sanal tablosuna)
 * bağlanacak şekilde değiştirilir. Orijinal nesneyi bir daha asla geri getiremezsiniz!
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

class Document {
public:
  std::string name;
  Document(std::string docName) : name(docName) {}

  // EN: Virtual function is the key to polymorphism.
  // TR: Sanal (Virtual) fonksiyon, polimorfizmin anahtarıdır.
  virtual void print() const {
    std::cout << "[Base] Printing generic Document: " << name << std::endl;
  }
};

class PDFDocument : public Document {
public:
  // EN: Specific to subclass.
  // TR: Alt sınıfa Özel bir değişken.
  int securityProtocol;

  PDFDocument(std::string docName, int proto)
      : Document(docName), securityProtocol(proto) {}

  // EN: Overriding the base function.
  // TR: Fonksiyonu ezmek (Override).
  void print() const override {
    std::cout << "[Derived] Printing PDF: " << name << " | Security Protocol (Güvenlik Protokolü): "
        << securityProtocol << std::endl;
  }
};

// EN: 1. UNSAFE: Pass by Value -> SLICED!
// TR: 1. GÜVENSİZ: Değer ile Geçiş -> DİLİMLENİR!
void sloppyPrint(Document doc) {
  // EN: This object is no longer a PDF. It is just a regular Document.
  // TR: Bu nesne artık bir PDF değil. Sadece sıradan bir Document (Dilimlenmiş).
  doc.print();
}

// EN: 2. SAFE: Pass by Reference -> NO SLICING!
// TR: 2. GÜVENLİ: Referans ile Geçiş -> Polimorfizm!
void securePrint(const Document &doc) {
  // EN: The V-Table is consulted. The true owner (PDF) method is called.
  // TR: V-Table'a (Sanal Tabloya) bakılarak asıl sahibin (PDF) metodu çağırılır.
  doc.print();
}

int main() {
  std::cout << "=== MODULE 5: OBJECT SLICING (NESNE DİLİMLENMESİ) ===\n" << std::endl;

  PDFDocument topSecretFile("NuclearCodes.pdf", 256);

  // --- TEST 1: THE SLICING DISASTER / DİLİMLENME FELAKETİ ---
  std::cout << "1. Pass by Value (Sliced Object / Dilimlenen Nesne):" << std::endl;
  std::cout << "-> INTERVIEW TRAP: Compiler gives NO ERROR, but calls the "
               "wrong function!"
            << std::endl;
  std::cout << "-> MÜLAKAT TUZAĞI: Derleyici hata (Error) VERMEZ ancak yanlış "
               "fonksiyonu çağırır!"
            << std::endl;
  sloppyPrint(topSecretFile);

  std::cout << "\n-------------------------------------------------\n" << std::endl;

  // --- TEST 2: CORRECT & SAFE POLYMORPHISM / DOĞRU POLİMORFİZM ---
  std::cout << "2. Pass by Reference (Real Polymorphism / Gerçek Polimorfizm):" << std::endl;
  std::cout << "-> INTACT OBJECT: The V-Table recognized the true owner!" << std::endl;
  std::cout << "-> NESNE BÜTÜN: Sanal Tablo (V-Table) asıl sahibini tanıdı!" << std::endl;
  securePrint(topSecretFile);

  std::cout << "\n-------------------------------------------------\n" << std::endl;

  // EN: A classic CppQuiz Question:
  // TR: Klasik CppQuiz Sorusu:
  std::cout << "3. Direct Assignment (Doğrudan Atama / Slicing):" << std::endl;
  Document standardDoc = topSecretFile;
  standardDoc.print();
  // std::cout << standardDoc.securityProtocol; // ERROR! Won't compile because it was sliced
  // off! (Erişilemez)

  return 0;
}
