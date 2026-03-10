# C++ Mastery — Detaylı Müfredat / Detailed Curriculum

**11 Modül · 72 Kaynak Dosya · 20.281 Satır · C++17/C++20**

Bu müfredat; cppreference.com, C++ Core Guidelines, ISO C++ standartları, Mike Shah Serisi ve CppQuiz kaynaklarına dayanılarak hazırlanmıştır. Her dosya Doxygen formatında İngilizce/Türkçe ikili teori, cppreference derinlik bölümleri ve derlenebilir kod örnekleri içerir.

---

## Modül 1: Basics — Temeller ve Hafıza
📁 `module_01_basics` · C++17 · **9 dosya · 1.659 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_variables_and_memory.cpp` | Değişkenler, Veri Tipleri, Stack/Heap/Static Allocation, UB |
| 2 | `02_pointers_and_references.cpp` | Raw Pointers, Referanslar, Dangling Pointer |
| 3 | `03_control_flow_and_functions.cpp` | If-Else, Döngüler, Fonksiyon Overloading, Inline |
| 4 | `04_operators_and_expressions.cpp` | Operatörler, İfadeler, Öncelik Kuralları |
| 5 | `05_control_flow_deepdive.cpp` | Switch-Case, Ternary, Goto, Range-For Derinlemesine |
| 6 | `06_strings_and_arrays.cpp` | std::string, C-String, Diziler, std::array |
| 7 | `07_enums_type_conversions.cpp` | enum/enum class, auto, typedef/using, Tip Dönüşümleri |
| 8 | `08_file_io_and_streams.cpp` | Dosya G/Ç, fstream, stringstream, Format |
| 9 | `09_scope_namespace_recursion.cpp` | Kapsam, Ömür, Namespace, Özyineleme |

**Temel Kazanımlar:** Bellek modeli anlayışı, pointer aritmetiği, fonksiyon tasarımı, dosya işlemleri.

---

## Modül 2: OOP — Nesne Yönelimli Programlama
📁 `module_02_oop` · C++17 · **8 dosya · 2.344 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_classes_objects.cpp` | Struct vs Class, Memory Layout, Padding, Access Specifiers |
| 2 | `02_inheritance_polymorphism.cpp` | Kalıtım, virtual, Dynamic Dispatch, vtable |
| 3 | `03_rule_of_three_five.cpp` | Copy/Move Constructor, Rule of 3/5/0, Memory Leak |
| 4 | `04_multiple_inheritance.cpp` | Çoklu Kalıtım, Diamond Problem, Virtual Inheritance |
| 5 | `05_encapsulation_abstraction.cpp` | Kapsülleme, Soyutlama, Pure Virtual Functions |
| 6 | `06_static_and_friends.cpp` | Static Üyeler, Friend Functions/Classes |
| 7 | `07_nested_and_local_classes.cpp` | İç İçe Sınıflar, Yerel Sınıflar |
| 8 | `08_rtti_typeid.cpp` | RTTI, typeid, dynamic_cast |

**Temel Kazanımlar:** RAII prensibi, polimorfizm, nesne yaşam döngüsü yönetimi.

---

## Modül 3: Modern C++ — Güvenlik ve Performans
📁 `module_03_modern_cpp` · C++17 · **5 dosya · 1.652 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_smart_pointers.cpp` | unique_ptr, shared_ptr, weak_ptr, make_shared/make_unique |
| 2 | `02_lambdas.cpp` | Lambda Expressions, Capture Semantiği, Generic Lambdas |
| 3 | `03_move_semantics_rvalue.cpp` | Rvalue References, std::move, Perfect Forwarding |
| 4 | `04_modern_types.cpp` | std::optional, std::variant, std::any, std::string_view |
| 5 | `05_structured_bindings_optional.cpp` | Structured Bindings, C++17 Vocabulary Types |

**Temel Kazanımlar:** Manuel bellek yönetiminden kurtulma, Modern C++ idiomları.

---

## Modül 4: Templates & STL — Şablonlar ve STL
📁 `module_04_advanced` · C++17 · **8 dosya · 3.105 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_templates_stl.cpp` | Function/Class Templates, Type Deduction, CTAD |
| 2 | `02_functors_callbacks.cpp` | Functors, std::function, Callback Mekanizması |
| 3 | `03_stl_containers_deepdive.cpp` | vector, map, unordered_map, Big-O Analizi |
| 4 | `04_iterators_and_adaptors.cpp` | Iterator Kategorileri, Adaptörler, reverse/insert |
| 5 | `05_algorithms_masterclass.cpp` | sort, find_if, accumulate, transform, partition |
| 6 | `06_associative_containers.cpp` | set, map, multimap, unordered Konteynerler |
| 7 | `07_string_operations.cpp` | String İşlemleri, string_view, Performans |
| 8 | `08_ranges_and_views.cpp` | C++20 Ranges & Views (C++17 Uyumlu Implementasyon) |

**Temel Kazanımlar:** Generic programlama, STL veri yapıları, algoritma karmaşıklığı.

---

## Modül 5: Certification Prep — Sertifikasyon Hazırlığı
📁 `module_05_certification` · C++17 · **6 dosya · 1.319 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_undefined_behavior.cpp` | UB Kategorileri, Dangling Pointers, Evaluation Order |
| 2 | `02_object_slicing_virtual.cpp` | Object Slicing, RTTI, Polimorfik Geçiş |
| 3 | `03_type_casting.cpp` | static_cast, dynamic_cast, const_cast, reinterpret_cast |
| 4 | `04_exceptions_handling.cpp` | Exception Safety, noexcept, Custom Exceptions |
| 5 | `05_const_correctness.cpp` | const Doğruluğu, Mutable, const Üye Fonksiyonlar |
| 6 | `06_interview_gotchas.cpp` | Mülakat Tuzakları, Tricky Davranışlar |

**Temel Kazanımlar:** CPA/CPP sınav hazırlığı, edge case farkındalığı, mülakat yetkinliği.

---

## Modül 6: Industry Standards — Endüstri Standartları
📁 `module_06_industry_standards` · C++17 · **6 dosya · 1.775 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_concurrency_threads.cpp` | std::thread, std::mutex, lock_guard, Race Condition |
| 2 | `02_cmake_basics.txt` | CMake Build Sistemi, CMakeLists.txt Yazımı |
| 3 | `03_unit_testing_gtest.cpp` | Google Test (TDD) ile Birim Test Yazımı |
| 4 | `04_valgrind_profiling.cpp` | Memory Leak Tespiti, Valgrind, Profiling |
| 5 | `05_async_futures.cpp` | std::async, std::future, std::promise, Launch Policies |
| 6 | `06_debugging_techniques.cpp` | GDB, Defensive Programming, Assert, Sanitizers |

**Temel Kazanımlar:** Profesyonel araç zinciri, test yazımı, hata ayıklama, build sistemi.

---

## Modül 7: Under the Hood — OS Derinlikleri
📁 `module_07_under_the_hood` · C++17 · **6 dosya · 1.497 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_vtable_memory_layout.cpp` | Virtual Table, vptr, sizeof Farkı, final/devirtualization |
| 2 | `02_sigsegv_and_raii.cpp` | SIGSEGV, Core Dump, RAII ile Güvenli Kaynak Yönetimi |
| 3 | `03_fork_vs_thread_pool.cpp` | POSIX fork() vs std::thread, Shared Memory, Thread Pool |
| 4 | `04_memory_model_and_cache.cpp` | CPU Cache Hiyerarşisi, False Sharing, Cache-Friendly Tasarım |
| 5 | `05_compilation_pipeline.cpp` | Preprocessing → Compilation → Assembly → Linking Pipeline |
| 6 | `06_abi_and_name_mangling.cpp` | ABI, Name Mangling, extern "C", Plugin API'leri |

**Temel Kazanımlar:** Donanım seviyesi anlayış, bellek erişim optimizasyonu, OS mekanizmaları.

---

## Modül 8: C++ Core Guidelines — Üst Düzey Kurallar
📁 `module_08_core_guidelines` · C++17 · **6 dosya · 1.393 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_philosophy_and_interfaces.cpp` | P.1/P.4/P.5, constexpr, static_assert, STL Algoritmaları |
| 2 | `02_resource_management_rule_of_zero.cpp` | Rule of Zero, R.10/R.11, RAII Tipleri |
| 3 | `03_error_handling.cpp` | E.2/E.3/E.15/E.16, noexcept, std::terminate |
| 4 | `04_concurrency_and_performance.cpp` | CP.20, lock_guard, False Sharing, std::atomic |
| 5 | `05_smart_pointer_guidelines.cpp` | R.20-R.37, Ownership Semantiği, Passing Pointers |
| 6 | `06_class_design_guidelines.cpp` | C.2/C.4/C.9, Sınıf Tasarım Kuralları, Invariant |

**Temel Kazanımlar:** ISO C++ Core Guidelines uyumlu profesyonel kod yazımı.

---

## Modül 9: Advanced Mechanics — İleri Mekanikler
📁 `module_09_advanced_mechanics` · **C++20** · **7 dosya · 1.570 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_bit_manipulation_and_alignment.cpp` | Bit Maskeleme, alignas, std::bitset, C++20 <bit> |
| 2 | `02_operator_overloading.cpp` | +, ==, (), << Overloading, friend, C++20 <=> |
| 3 | `03_advanced_pointers.cpp` | Pointer to Member, std::invoke, std::mem_fn |
| 4 | `04_constant_evaluation.cpp` | constexpr vs consteval vs constinit |
| 5 | `05_type_traits_and_concepts.cpp` | SFINAE, enable_if → C++20 Concepts, requires |
| 6 | `06_sfinae_and_tag_dispatch.cpp` | SFINAE, Tag Dispatch, if constexpr |
| 7 | `07_crtp_pattern.cpp` | CRTP — Curiously Recurring Template Pattern |

**Temel Kazanımlar:** Düşük seviye mekanikler, C++20 yenilikleri, derleme zamanı programlama.

---

## Modül 10: Design Patterns — Tasarım Kalıpları
📁 `module_10_design_patterns` · **C++20** · **6 dosya · 2.415 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_solid_principles.cpp` | SOLID Prensipleri (SRP, OCP, LSP, ISP, DIP) |
| 2 | `02_creational_patterns.cpp` | Singleton, Factory, Builder, Prototype |
| 3 | `03_behavioral_patterns.cpp` | Observer, Strategy, Command, State |
| 4 | `04_structural_patterns.cpp` | Adapter, Decorator, Facade, Composite, Proxy |
| 5 | `05_testable_design.cpp` | CRTP, Type Erasure, Policy-Based Design |
| 6 | `06_pattern_decision_guide.cpp` | Kalıp Karar Rehberi, Anti-Pattern, Proxy, Prototype |

**Temel Kazanımlar:** GoF tasarım kalıpları, SOLID mimari, test edilebilir kod tasarımı, kalıp seçim kararları.

---

## Modül 11: Embedded & Real-Time — Gömülü Sistemler
📁 `module_11_embedded_systems` · **C++20** · **5 dosya · 1.552 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_volatile_hardware.cpp` | volatile, Memory-Mapped I/O, ISR Kavramları |
| 2 | `02_bitfields_protocols.cpp` | Bitfield'lar, Protokol Parsing, Packed Struct |
| 3 | `03_embedded_constraints.cpp` | No-Heap Allocation, Static Buffer, constexpr Lookup |
| 4 | `04_rtos_concurrency.cpp` | RTOS Patterns, Priority Inversion, Lock-Free |
| 5 | `05_advanced_parallelism.cpp` | Thread Pool, C++17 Parallel Algorithms, Work Stealing |

**Temel Kazanımlar:** Gömülü sistemlerde C++ kullanımı, kaynak kısıtlı ortamlar, gerçek zamanlı programlama.

---

## Derleme Bilgisi

```bash
# Modül 1-8 (C++17):
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow \
    -Wconversion -Wsign-conversion -Wnull-dereference -Wformat=2 \
    <dosya>.cpp -o output

# Modül 9-11 (C++20):
g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow \
    -Wconversion -Wsign-conversion -Wnull-dereference -Wformat=2 \
    <dosya>.cpp -o output

# Concurrency dosyaları (-pthread gerekli):
# module_05/05, module_06/01, module_06/05, module_07/03,
# module_07/04, module_08/04, module_11/04, module_11/05
```

Tüm 72 dosya yukarıdaki sıkı flag'lerle **0 warning, 0 error** olarak derlenir.

---

## Referanslar

- [cppreference.com](https://en.cppreference.com/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [ISO C++](https://isocpp.org/)
- [Google Test](https://google.github.io/googletest/)
- [CMake Documentation](https://cmake.org/documentation/)

---

**Hazırlayan:** Murat Mecit KAHRAMANLI · Mart 2026
