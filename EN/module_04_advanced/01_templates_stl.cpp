/**
 * @file module_04_advanced/01_templates_stl.cpp
 * @brief Advanced: Templates and STL
 *
 * @details
 * =============================================================================
 * [THEORY: Why Were Templates Invented?]
 * =============================================================================
 * EN: Imagine you want to find the maximum of two numbers. You write a function `max(int a, int
 * b)`. But tomorrow, you need `max(double a, double b)`. Next week, `max(string a, string b)`.
 * Should you write 100 overloaded functions? NO! In C++, Templates are "Code that writes code".
 * You write ONE template, and the compiler generates the specific versions (int, float, custom
 * class) FOR YOU at Compile-Time. This is called "Template Instantiation". The beauty is that
 * the same logic applies to ANY type that supports the operations used inside the template —
 * this is called "Structural Typing" or "Duck Typing at Compile-Time".
 *
 * There are two kinds of templates:
 * 1. Function Templates: Generic functions like `findMax<T>(T a, T b)`.
 * 2. Class Templates: Generic classes like `Box<T>`, `std::vector<T>`.
 *
 * The compiler performs "Type Deduction" — if you call `findMax(3, 5)` it automatically deduces
 * `T = int`. For class templates (before C++17 CTAD), you must specify the type explicitly:
 * `Box<int> b(42);`
 *
 *
 *
 * b(42);`
 *
 * =============================================================================
 * [THEORY: The Standard Template Library (STL)]
 * =============================================================================
 * EN: The STL is one of the greatest engineering marvels. It has three pillars:
 * 1. Containers (vector, map, set, list, deque) -> Store data.
 * 2. Iterators  (begin(), end(), rbegin())      -> Object-oriented pointers.
 * 3. Algorithms (std::sort, std::find, std::transform) -> Process data.
 *
 * The key insight: Algorithms don't know about containers. Containers don't know about
 * algorithms. They communicate ONLY through iterators! This decoupling means `std::sort` works
 * on vector, array, deque — anything with random-access iterators. And `std::find` works on ANY
 * container with forward iterators.
 *
 * Common STL containers and their use cases:
 *     1. `std::vector<T>`: Dynamic array, contiguous memory, fast random access.
 *        Best for: most general-purpose sequential data.
 *     2. `std::map<K,V>`: Red-black tree, sorted key-value pairs, O(log n) lookup.
 *        Best for: ordered dictionaries.
 *     3. `std::unordered_map<K,V>`: Hash table, O(1) average lookup.
 *        Best for: fast key-value lookups without ordering.
 *     4. `std::set<T>`: Sorted unique elements. Best for: membership testing.
 *     5. `std::deque<T>`: Double-ended queue, fast push/pop at both ends.
 *
 *
 *
 *     3. `std::unordered_map<K,V>`: Hash tablo, ortalama O(1) arama.
 *
 * =============================================================================
 * [THEORY: Template Specialization]
 * =============================================================================
 * EN: Sometimes a generic template doesn't work well for a specific type. For example, comparing
 * C-strings with `>` compares pointer addresses, not content! You can provide a "Template
 * Specialization" — a custom version for a specific type. The compiler will prefer the
 * specialized version when that type is used. This is explicit (full) specialization. There's
 * also "Partial Specialization" for class templates where you specialize some parameters.
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: Template Instantiation & Code Bloat]
 * =============================================================================
 * EN: Templates are NOT real functions. They are blueprints. If you call `findMax` with 50
 * different data types `(int, float, char, SensorData, ECUConfig...)`, the compiler physically
 * generates 50 DIFFERENT functions in the final `.exe` binary. This causes "Code Bloat" (Massive
 * file size). Use them wisely! You can mitigate bloat with:
 *     1. `extern template` declarations (C++11): Tell the compiler NOT to
 *        instantiate a template in this translation unit.
 *     2. Thin template wrappers over non-template implementations.
 *     3. Limiting the number of distinct type arguments.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_templates_stl.cpp -o 01_templates_stl
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

// ─── 1. Function Template ────────────────────────────────────────────────────

/**
 * @brief Generic Template Function
 *
 * EN: A template parameter `typename T` stands in for "any data type". The compiler deduces T
 * from the arguments automatically.
 */
template <typename T> T findMax(T a, T b) {
  return (a > b) ? a : b;
}

// ─── 2. Template Specialization ────────────────────────────────────────────

/**
 * @brief Specialization for C-strings
 *
 * EN: Without this, `findMax("apple", "zebra")` compares pointer addresses! This specialization
 * uses strcmp to compare actual string content.
 */
template <> const char *findMax<const char *>(const char *a, const char *b) {
  return (std::strcmp(a, b) > 0) ? a : b;
}

// ─── 3. Class Template ───────────────────────────────────────────────────────────

/**
 * @class Box
 * @brief Generic Class Template with Multiple Operations
 *
 * EN: A generic box that can hold any type. Demonstrates class templates with member functions,
 * operator overloading, and type-safe containers.
 */
template <typename DataType> class Box {
private:
  DataType content;

public:
  explicit Box(DataType initData) : content(initData) {}
  DataType peek() const { return content; }

  // EN: Comparison operator so we can compare Box objects
  bool operator>(const Box &other) const { return content > other.content; }

  // EN: Stream output operator (friend function)
  friend std::ostream &operator<<(std::ostream &os, const Box &b) {
    os << "Box[" << b.content << "]";
    return os;
  }
};

// ─── 4. Multi-Parameter Template ────────────────────────────────────────

/**
 * @brief Template with multiple type parameters
 *
 * EN: Templates can have more than one type parameter. This Pair class holds two values of
 * potentially different types, like std::pair.
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
  std::cout << "Max of (10, 20): " << findMax<int>(10, 20)
            << "  [explicit <int>]" << std::endl;
  std::cout << "Max of (3.14, 2.71): " << findMax(3.14, 2.71)
            << "  [auto deduced double]" << std::endl;
  std::cout << "Max of strings: "
            << findMax(std::string("Apple"), std::string("Zebra"))
            << "  [auto deduced string]" << std::endl;

  // EN: Template specialization for C-strings in action
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
  std::vector<int> data = {42, 1, 99, 15, -3, 77, 23};
  std::cout << "Original: ";
  for (int v : data) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  // EN: std::sort uses iterators to work with ANY random-access container
  std::sort(data.begin(), data.end());
  std::cout << "Sorted:   ";
  for (int v : data) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  // EN: std::find returns an iterator to the found element or end()
  auto it = std::find(data.begin(), data.end(), 42);
  if (it != data.end()) {
    std::cout << "Found 42 at index: "
              << std::distance(data.begin(), it) << std::endl;
  }

  // EN: std::accumulate sums all elements (from <numeric>)
  int sum = std::accumulate(data.begin(), data.end(), 0);
  std::cout << "Sum of all elements: " << sum << std::endl;

  // EN: std::transform applies a function to each element
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
  std::map<std::string, int> scores;
  scores["Alice"] = 95;
  scores["Bob"] = 87;
  scores["Charlie"] = 92;
  scores["Diana"] = 98;

  // EN: Iterate over map (automatically sorted by key)
  std::cout << "Student scores (sorted by name):" << std::endl;
  for (const auto &[name, score] : scores) {
    std::cout << "  " << name << ": " << score << std::endl;
  }

  // EN: Find a specific key
  auto mapIt = scores.find("Bob");
  if (mapIt != scores.end()) {
    std::cout << "Bob's score: " << mapIt->second << std::endl;
  }
  std::cout << std::endl;

  // ─── 6. STL Algorithm Chaining (Algoritma Zincirleme) ──────────────────────────────────────────
  std::cout << "--- 6. Algorithm Chaining (Algoritma Zincirleme) ---"
            << std::endl;

  // EN: Count elements greater than 20 using std::count_if with a lambda
  std::vector<int> values = {5, 12, 35, 8, 42, 99, 3, 17, 50};
  auto countAbove20 = std::count_if(values.begin(), values.end(),
                                    [](int x) { return x > 20; });
  std::cout << "Elements > 20: " << countAbove20 << std::endl;

  // EN: Partition — move elements satisfying a predicate to the front
  std::partition(values.begin(), values.end(),
                 [](int x) { return x % 2 == 0; });
  std::cout << "After partition (evens first): ";
  for (int v : values) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  // EN: Find min and max in one pass
  auto [minIt, maxIt] = std::minmax_element(values.begin(), values.end());
  std::cout << "Min: " << *minIt << ", Max: " << *maxIt << std::endl;

  return 0;
}
