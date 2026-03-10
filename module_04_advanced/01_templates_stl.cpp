/**
 * @file module_04_advanced/01_templates_stl.cpp
 * @brief Advanced: Templates and STL / İleri Seviye: Şablonlar (Derleme Zamanı Büyüsü) ve STL
 *
 * @details
 * =============================================================================
 * [THEORY: Why Were Templates Invented? / TEORİ: Şablonlar (Templates) Neden Yaratıldı?]
 * =============================================================================
 * EN: Imagine you want to find the maximum of two numbers. You write a function `max(int a, int
 * b)`. But tomorrow, you need `max(double a, double b)`. Next week, `max(string a, string b)`.
 * Should you write 100 overloaded functions? NO! In C++, Templates are "Code that writes code".
 * You write ONE template, and the compiler generates the specific versions (int, float, custom
 * class) FOR YOU at Compile-Time. This is called "Template Instantiation". The beauty is that
 * the same logic applies to ANY type that supports the operations used inside the template —
 * this is called "Structural Typing" or "Duck Typing at Compile-Time".
 *
 * There are two kinds of templates: 1. Function Templates: Generic functions like `findMax<T>(T
 * a, T b)`. 2. Class Templates: Generic classes like `Box<T>`, `std::vector<T>`.
 *
 * The compiler performs "Type Deduction" — if you call `findMax(3, 5)` it automatically deduces
 * `T = int`. For class templates (before C++17 CTAD), you must specify the type explicitly:
 * `Box<int> b(42);`
 *
 * TR: İki sayının maksimumunu bulan bir `max(int a, int b)` yazdın. Yarın `max(double, double)`
 * gerekti, haftaya `max(string, string)`. 100 farklı fonksiyon mu yazacaksın? HAYIR! C++'da
 * Şablonlar (Templates) "Kod Yazan Kodlardır". Sen sadece tek bir T (Zaman/Tip) şablonu
 * yazarsın, DERLEYİCİ program derlenirken (Compile-Time) arka planda senin için o tiplere özel
 * fonksiyonları (veya sınıfları) fiziksel olarak üretir. Buna "Şablon Somutlaştırma (Template
 * Instantiation)" denir. Şablon içinde kullanılan operasyonları destekleyen HER tip ile çalışır
 * — buna derleme zamanı "Ördek Tipleme" denir (Duck Typing at Compile-Time).
 *
 * İki çeşit şablon vardır: 1. Fonksiyon Şablonları: `findMax<T>(T a, T b)` gibi jenerik
 * fonksiyonlar. 2. Sınıf Şablonları: `Box<T>`, `std::vector<T>` gibi jenerik sınıflar.
 *
 * Derleyici "Tip Çıkarımı (Type Deduction)" yapar — `findMax(3, 5)` çağrısında otomatik olarak
 * `T = int` anlar. Sınıf şablonlarında (C++17 CTAD öncesi) tipi açıkça belirtmelisin: `Box<int>
 * b(42);`
 *
 * =============================================================================
 * [THEORY: The Standard Template Library (STL) / TEORİ: Standart Kütüphane ve Algoritması]
 * =============================================================================
 * EN: The STL is one of the greatest engineering marvels. It has three pillars: 1. Containers
 * (vector, map, set, list, deque) -> Store data. 2. Iterators  (begin(), end(), rbegin())     
 * -> Object-oriented pointers. 3. Algorithms (std::sort, std::find, std::transform) -> Process
 * data.
 *
 * The key insight: Algorithms don't know about containers. Containers don't know about
 * algorithms. They communicate ONLY through iterators! This decoupling means `std::sort` works
 * on vector, array, deque — anything with random-access iterators. And `std::find` works on ANY
 * container with forward iterators.
 *
 * Common STL containers and their use cases: - `std::vector<T>`: Dynamic array, contiguous
 * memory, fast random access. Best for: most general-purpose sequential data. - `std::map<K,V>`:
 * Red-black tree, sorted key-value pairs, O(log n) lookup. Best for: ordered dictionaries. -
 * `std::unordered_map<K,V>`: Hash table, O(1) average lookup. Best for: fast key-value lookups
 * without ordering. - `std::set<T>`: Sorted unique elements. Best for: membership testing. -
 * `std::deque<T>`: Double-ended queue, fast push/pop at both ends.
 *
 * TR: STL (Standard Template Library) dünyanın en harika mühendislik ürünlerinden biridir. Üç
 * temel bacağı vardır: 1. Konteynerler (Containers: vector, map, set, list, deque -> Veriyi
 * depolar) 2. İteratörler  (Iterators: begin(), end(), rbegin() -> Nesne odaklı ptr) 3.
 * Algoritmalar (Algorithms: std::sort, std::find, std::transform)
 *
 * Kilit bilgi: Algoritmalar konteynerleri bilmez. Konteynerler algoritmaları bilmez. Sadece
 * İteratörler aracılığıyla iletişim kurarlar! Bu ayrışma sayesinde `std::sort` vector, array,
 * deque üzerinde — random-access iteratörü olan her şeyde çalışır. `std::find` ise forward
 * iteratörü olan HER konteynerde çalışır.
 *
 * Yaygın STL konteynerleri ve kullanım alanları: - `std::vector<T>`: Dinamik dizi, bitişik
 * bellek, hızlı rastgele erişim. - `std::map<K,V>`: Kırmızı-siyah ağaç, sıralı anahtar-değer,
 * O(log n). - `std::unordered_map<K,V>`: Hash tablo, ortalama O(1) arama. - `std::set<T>`:
 * Sıralı benzersiz elemanlar. Üyelik testi için ideal. - `std::deque<T>`: Çift uçlu kuyruk, her
 * iki uçta hızlı push/pop.
 *
 * =============================================================================
 * [THEORY: Template Specialization / TEORİ: Şablon Özelleştirme]
 * =============================================================================
 * EN: Sometimes a generic template doesn't work well for a specific type. For example, comparing
 * C-strings with `>` compares pointer addresses, not content! You can provide a "Template
 * Specialization" — a custom version for a specific type. The compiler will prefer the
 * specialized version when that type is used. This is explicit (full) specialization. There's
 * also "Partial Specialization" for class templates where you specialize some parameters.
 *
 * TR: Bazen jenerik bir şablon belirli bir tip için düzgün çalışmaz. Örneğin, C-string'leri `>`
 * ile karşılaştırmak pointer adreslerini karşılaştırır, içeriği değil! "Şablon Özelleştirme
 * (Template Specialization)" ile belirli bir tipe özel versiyon yazabilirsin. Derleyici o tip
 * kullanıldığında özel versiyonu tercih eder. Bu "tam (full) özelleştirme"dir. Sınıf
 * şablonlarında bazı parametreleri özelleştiren "Kısmi Özelleştirme (Partial Specialization)" de
 * vardır.
 *
 * =============================================================================
 * [CPPREF DEPTH: Template Instantiation & Code Bloat / CPPREF DERİNLİK: Şablon Somutlaştırma ve
 * Kod Şişmesi]
 * =============================================================================
 * EN: Templates are NOT real functions. They are blueprints. If you call `findMax` with 50
 * different data types `(int, float, char, SensorData, ECUConfig...)`, the compiler physically
 * generates 50 DIFFERENT functions in the final `.exe` binary. This causes "Code Bloat" (Massive
 * file size). Use them wisely! You can mitigate bloat with: - `extern template` declarations
 * (C++11): Tell the compiler NOT to instantiate a template in this translation unit. - Thin
 * template wrappers over non-template implementations. - Limiting the number of distinct type
 * arguments.
 *
 * TR: Şablonlar (Templates) gerçek fonksiyon DEĞİLDİR. Eğer siz `findMax` metodunu 50 farklı
 * veri tipiyle çağırırsanız, derleyici arkaplanda 50 FARKLI fiziksel fonksiyon kopyası
 * (Instantiations) oluşturur ve `.exe` dosyanızın boyutu devasa şişer! (Buna Code Bloat denir).
 * Şişmeyi azaltmak için: - `extern template` bildirimleri (C++11): Derleyiciye bu çeviri
 * biriminde şablonu somutlaştırma demeyi sağlar. - Şablon olmayan uygulamalar üzerine ince
 * şablon sarmalayıcılar yazmak. - Farklı tip argüman sayısını sınırlamak.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm> // EN: std::sort, std::find, std::transform
#include <cstring>   // EN: strcmp for C-string specialization
#include <iostream>
#include <map>
#include <numeric> // EN: std::accumulate
#include <string>
#include <vector>

// ─── 1. Function Template (Fonksiyon Şablonu) ────────────────────────────────────────────────────

/**
 * @brief Generic Template Function / Jenerik Şablon Fonksiyonu
 *
 * EN: A template parameter `typename T` stands in for "any data type". The compiler deduces T
 * from the arguments automatically.
 * TR: T, "herhangi bir veri tipi" anlamına gelen kılıftır (Placeholder). Derleyici T'yi
 * argümanlardan otomatik çıkarır.
 */
template <typename T> T findMax(T a, T b) {
  return (a > b) ? a : b;
}

// ─── 2. Template Specialization (Şablon Özelleştirme) ────────────────────────────────────────────

/**
 * @brief Specialization for C-strings / C-string İçin Özelleştirme
 *
 * EN: Without this, `findMax("apple", "zebra")` compares pointer addresses! This specialization
 * uses strcmp to compare actual string content.
 * TR: Bu olmazsa `findMax("apple", "zebra")` pointer adreslerini karşılaştırır! Bu özelleştirme
 * strcmp ile gerçek string içeriğini karşılaştırır.
 */
template <> const char *findMax<const char *>(const char *a, const char *b) {
  return (std::strcmp(a, b) > 0) ? a : b;
}

// ─── 3. Class Template (Sınıf Şablonu) ───────────────────────────────────────────────────────────

/**
 * @class Box
 * @brief Generic Class Template with Multiple Operations
 *
 * EN: A generic box that can hold any type. Demonstrates class templates with member functions,
 * operator overloading, and type-safe containers.
 * TR: Herhangi bir tipi tutabilen jenerik bir kutu. Sınıf şablonlarını üye fonksiyonlar,
 * operatör yükleme ve tip-güvenli konteynerlerle gösterir.
 */
template <typename DataType> class Box {
private:
  DataType content;

public:
  explicit Box(DataType initData) : content(initData) {}
  DataType peek() const { return content; }

  // EN: Comparison operator so we can compare Box objects
  // TR: Box nesnelerini karşılaştırabilmek için karşılaştırma operatörü
  bool operator>(const Box &other) const { return content > other.content; }

  // EN: Stream output operator (friend function)
  // TR: Akış çıktı operatörü (arkadaş fonksiyon)
  friend std::ostream &operator<<(std::ostream &os, const Box &b) {
    os << "Box[" << b.content << "]";
    return os;
  }
};

// ─── 4. Multi-Parameter Template (Çok Parametreli Şablon) ────────────────────────────────────────

/**
 * @brief Template with multiple type parameters / Çoklu tip parametreli şablon
 *
 * EN: Templates can have more than one type parameter. This Pair class holds two values of
 * potentially different types, like std::pair.
 * TR: Şablonlar birden fazla tip parametresine sahip olabilir. Bu Pair sınıfı, std::pair gibi
 * farklı tipte iki değer tutar.
 */
template <typename First, typename Second> struct Pair {
  First first;
  Second second;

  Pair(First f, Second s) : first(f), second(s) {}

  void print() const {
    std::cout << "(" << first << ", " << second << ")";
  }
};

int main() {
  std::cout << "=== ADVANCED C++: TEMPLATES & STL DEEP DIVE ===\n"
            << std::endl;

  // ─── 1. Function Templates with Type Deduction ─────────────────────────────────────────────────
  std::cout << "--- 1. Function Templates (Sablon Fonksiyonlar) ---"
            << std::endl;

  // EN: Explicit type specification vs auto deduction
  // TR: Açık tip belirtme vs otomatik çıkarım
  std::cout << "Max of (10, 20): " << findMax<int>(10, 20)
            << "  [explicit <int>]" << std::endl;
  std::cout << "Max of (3.14, 2.71): " << findMax(3.14, 2.71)
            << "  [auto deduced double]" << std::endl;
  std::cout << "Max of strings: "
            << findMax(std::string("Apple"), std::string("Zebra"))
            << "  [auto deduced string]" << std::endl;

  // EN: Template specialization for C-strings in action
  // TR: C-string şablon özelleştirmesi iş başında
  const char *result = findMax("apple", "zebra");
  std::cout << "Max of C-strings: " << result
            << "  [specialization with strcmp]" << std::endl;
  std::cout << std::endl;

  // ─── 2. Class Templates ────────────────────────────────────────────────────────────────────────
  std::cout << "--- 2. Class Templates (Sablon Siniflar) ---" << std::endl;

  Box<int> intBox(999);
  Box<std::string> strBox("Secret Message / Gizli Mesaj");
  Box<double> dblBox(3.14159);

  std::cout << "Int box: " << intBox << std::endl;
  std::cout << "String box: " << strBox << std::endl;
  std::cout << "Double box: " << dblBox << std::endl;

  // EN: Using findMax with Box objects (operator> is defined)
  // TR: Box nesneleriyle findMax kullanımı (operator> tanımlı)
  Box<int> boxA(42);
  Box<int> boxB(99);
  std::cout << "Max box: " << findMax(boxA, boxB) << std::endl;
  std::cout << std::endl;

  // ─── 3. Multi-Parameter Templates ──────────────────────────────────────────────────────────────
  std::cout << "--- 3. Multi-Parameter Pair (Coklu Tip Sablonu) ---"
            << std::endl;

  Pair<std::string, int> student("Ali", 95);
  Pair<int, double> measurement(42, 98.6);
  std::cout << "Student: ";
  student.print();
  std::cout << std::endl;
  std::cout << "Measurement: ";
  measurement.print();
  std::cout << "\n" << std::endl;

  // ─── 4. STL Container + Iterator + Algorithm Combo ─────────────────────────────────────────────
  std::cout << "--- 4. STL: Container + Iterator + Algorithm ---" << std::endl;

  // EN: Vector container with algorithms
  // TR: Vector konteyneri ve algoritmalar
  std::vector<int> data = {42, 1, 99, 15, -3, 77, 23};
  std::cout << "Original: ";
  for (int v : data) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  // EN: std::sort uses iterators to work with ANY random-access container
  // TR: std::sort iteratörleri kullanarak rastgele erişimli HER konteynerde çalışır
  std::sort(data.begin(), data.end());
  std::cout << "Sorted:   ";
  for (int v : data) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  // EN: std::find returns an iterator to the found element or end()
  // TR: std::find bulunan elemana iterator döndürür, yoksa end()
  auto it = std::find(data.begin(), data.end(), 42);
  if (it != data.end()) {
    std::cout << "Found 42 at index: "
              << std::distance(data.begin(), it) << std::endl;
  }

  // EN: std::accumulate sums all elements (from <numeric>)
  // TR: std::accumulate tüm elemanları toplar (<numeric>'ten)
  int sum = std::accumulate(data.begin(), data.end(), 0);
  std::cout << "Sum of all elements: " << sum << std::endl;

  // EN: std::transform applies a function to each element
  // TR: std::transform her elemana bir fonksiyon uygular
  std::vector<int> doubled(data.size());
  std::transform(data.begin(), data.end(), doubled.begin(),
                 [](int x) { return x * 2; });
  std::cout << "Doubled:  ";
  for (int v : doubled) {
    std::cout << v << " ";
  }
  std::cout << "\n" << std::endl;

  // ─── 5. STL Map (Associative Container) ────────────────────────────────────────────────────────
  std::cout << "--- 5. STL Map (Iliskisel Konteyner) ---" << std::endl;

  // EN: std::map stores sorted key-value pairs (Red-Black Tree)
  // TR: std::map sıralı anahtar-değer çiftleri saklar (Kırmızı-Siyah Ağaç)
  std::map<std::string, int> scores;
  scores["Alice"] = 95;
  scores["Bob"] = 87;
  scores["Charlie"] = 92;
  scores["Diana"] = 98;

  // EN: Iterate over map (automatically sorted by key)
  // TR: Map üzerinde döngü (anahtara göre otomatik sıralı)
  std::cout << "Student scores (sorted by name):" << std::endl;
  for (const auto &[name, score] : scores) {
    std::cout << "  " << name << ": " << score << std::endl;
  }

  // EN: Find a specific key
  // TR: Belirli bir anahtarı bul
  auto mapIt = scores.find("Bob");
  if (mapIt != scores.end()) {
    std::cout << "Bob's score: " << mapIt->second << std::endl;
  }
  std::cout << std::endl;

  // ─── 6. STL Algorithm Chaining (Algoritma Zincirleme) ──────────────────────────────────────────
  std::cout << "--- 6. Algorithm Chaining (Algoritma Zincirleme) ---"
            << std::endl;

  // EN: Count elements greater than 20 using std::count_if with a lambda
  // TR: Lambda ile 20'den büyük elemanları say (std::count_if)
  std::vector<int> values = {5, 12, 35, 8, 42, 99, 3, 17, 50};
  auto countAbove20 = std::count_if(values.begin(), values.end(),
                                    [](int x) { return x > 20; });
  std::cout << "Elements > 20: " << countAbove20 << std::endl;

  // EN: Partition — move elements satisfying a predicate to the front
  // TR: Partition — koşulu sağlayan elemanları öne taşı
  std::partition(values.begin(), values.end(),
                 [](int x) { return x % 2 == 0; });
  std::cout << "After partition (evens first): ";
  for (int v : values) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  // EN: Find min and max in one pass
  // TR: Tek geçişte min ve max bul
  auto [minIt, maxIt] = std::minmax_element(values.begin(), values.end());
  std::cout << "Min: " << *minIt << ", Max: " << *maxIt << std::endl;

  return 0;
}
