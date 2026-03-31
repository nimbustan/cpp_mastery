# C++ Mastery — Detaylı Müfredat / Detailed Curriculum

**18 Modül · 118 Kaynak Dosya · ~55.000 Satır · C++17/C++20/C++23**

Bu müfredat; cppreference.com, C++ Core Guidelines, ISO C++ standartları, Mike Shah Serisi ve CppQuiz kaynaklarına dayanılarak hazırlanmıştır. Her dosya Doxygen formatında İngilizce/Türkçe ikili teori, cppreference derinlik bölümleri ve derlenebilir kod örnekleri içerir.

---

## Modül 1: Basics — Temeller ve Hafıza
📁 `module_01_basics` · C++17 · **10 dosya · 2.962 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_variables_and_memory.cpp` | Değişkenler, Veri Tipleri, Stack/Heap/Static Allocation, UB |
| 2 | `02_pointers_and_references.cpp` | Raw Pointers, Referanslar, Dangling Pointer |
| 3 | `03_functions_and_overloading.cpp` | Fonksiyonlar, Overloading, Inline |
| 4 | `04_operators_and_expressions.cpp` | Operatörler, İfadeler, Öncelik Kuralları |
| 5 | `05_control_flow_deepdive.cpp` | Switch-Case, Ternary, Goto, Range-For Derinlemesine |
| 6 | `06_strings_and_arrays.cpp` | std::string, C-String, Diziler, std::array |
| 7 | `07_enums_type_conversions.cpp` | enum/enum class, auto, typedef/using, Tip Dönüşümleri |
| 8 | `08_file_io_and_streams.cpp` | Dosya G/Ç, fstream, stringstream, Format |
| 9 | `09_scope_namespace_recursion.cpp` | Kapsam, Ömür, Namespace, Özyineleme |
| 10 | `10_filesystem_and_binary_io.cpp` | std::filesystem, İkili G/Ç, Binary Serialization |

**Temel Kazanımlar:** Bellek modeli anlayışı, pointer aritmetiği, fonksiyon tasarımı, dosya işlemleri.

---

## Modül 2: OOP — Nesne Yönelimli Programlama
📁 `module_02_oop` · C++17 · **8 dosya · 2.898 satır**

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
📁 `module_03_modern_cpp` · C++17/C++23 · **6 dosya · 2.258 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_smart_pointers.cpp` | unique_ptr, shared_ptr, weak_ptr, make_shared/make_unique |
| 2 | `02_lambdas.cpp` | Lambda Expressions, Capture Semantiği, Generic Lambdas |
| 3 | `03_move_semantics_rvalue.cpp` | Rvalue References, std::move, Perfect Forwarding |
| 4 | `04_modern_types.cpp` | std::optional, std::variant, std::any, std::string_view |
| 5 | `05_structured_bindings_optional.cpp` | Structured Bindings, C++17 Vocabulary Types |
| 6 | `06_cpp23_features.cpp` | C++23: std::expected, std::print, Deducing this |

**Temel Kazanımlar:** Manuel bellek yönetiminden kurtulma, Modern C++ idiomları.

---

## Modül 4: Templates & STL — Şablonlar ve STL
📁 `module_04_advanced` · C++17 · **9 dosya · 4.268 satır**

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
| 9 | `09_json_and_serialization.cpp` | JSON Parsing & Serialization |

**Temel Kazanımlar:** Generic programlama, STL veri yapıları, algoritma karmaşıklığı.

---

## Modül 5: Certification Prep — Sertifikasyon Hazırlığı
📁 `module_05_certification` · C++17 · **7 dosya · 1.924 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_undefined_behavior.cpp` | UB Kategorileri, Dangling Pointers, Evaluation Order |
| 2 | `02_object_slicing_virtual.cpp` | Object Slicing, RTTI, Polimorfik Geçiş |
| 3 | `03_type_casting.cpp` | static_cast, dynamic_cast, const_cast, reinterpret_cast |
| 4 | `04_exceptions_handling.cpp` | Exception Safety, noexcept, Custom Exceptions |
| 5 | `05_const_correctness.cpp` | const Doğruluğu, Mutable, const Üye Fonksiyonlar |
| 6 | `06_interview_gotchas.cpp` | Mülakat Tuzakları, Tricky Davranışlar |
| 7 | `07_error_handling_advanced.cpp` | İleri Seviye Hata Yönetimi, Exception Hierarchies |

**Temel Kazanımlar:** CPA/CPP sınav hazırlığı, edge case farkındalığı, mülakat yetkinliği.

---

## Modül 6: Industry Standards — Endüstri Standartları
📁 `module_06_industry_standards` · C++17 · **11 dosya · 4.746 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_concurrency_threads.cpp` | std::thread, std::mutex, lock_guard, Race Condition |
| 2 | `02_cmake_basics.txt` | CMake Build Sistemi, CMakeLists.txt Yazımı |
| 3 | `03_unit_testing_gtest.cpp` | Google Test (TDD) ile Birim Test Yazımı |
| 4 | `04_valgrind_profiling.cpp` | Memory Leak Tespiti, Valgrind, Profiling |
| 5 | `05_async_futures.cpp` | std::async, std::future, std::promise, Launch Policies |
| 6 | `06_debugging_techniques.cpp` | GDB, Defensive Programming, Assert, Sanitizers |
| 7 | `07_git_for_cpp_devs.md` | Git Workflow, Branching, CI/CD Entegrasyonu |
| 8 | `08_cmake_deepdive.txt` | CMake İleri Seviye, Toolchain, Cross-Compile |
| 9 | `09_mutex_lock_deepdive.cpp` | Mutex & Lock Mekanizmaları Derinlemesine |
| 10 | `10_advanced_logging.cpp` | İleri Seviye Günlükleme, spdlog, Custom Sink |
| 11 | `11_gtest_advanced.cpp` | İleri GTest Desenleri, Parametrized Tests |

**Temel Kazanımlar:** Profesyonel araç zinciri, test yazımı, hata ayıklama, build sistemi.

---

## Modül 7: Under the Hood — OS Derinlikleri
📁 `module_07_under_the_hood` · C++17 · **11 dosya · 4.390 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_vtable_memory_layout.cpp` | Virtual Table, vptr, sizeof Farkı, final/devirtualization |
| 2 | `02_sigsegv_and_raii.cpp` | SIGSEGV, Core Dump, RAII ile Güvenli Kaynak Yönetimi |
| 3 | `03_fork_vs_thread_pool.cpp` | POSIX fork() vs std::thread, Shared Memory, Thread Pool |
| 4 | `04_memory_model_and_cache.cpp` | CPU Cache Hiyerarşisi, False Sharing, Cache-Friendly Tasarım |
| 5 | `05_compilation_pipeline.cpp` | Preprocessing → Compilation → Assembly → Linking Pipeline |
| 6 | `06_abi_and_name_mangling.cpp` | ABI, Name Mangling, extern "C", Plugin API'leri |
| 7 | `07_inline_assembly.cpp` | Inline Assembly, Doğrudan CPU ile Konuşma |
| 8 | `08_simd_intrinsics.cpp` | SIMD, Vektörel İşleme, SSE/AVX |
| 9 | `09_cpp20_modules.cpp` | C++20 Modules, #include'ın Sonu |
| 10 | `10_linking_deepdive.cpp` | Static/Dynamic Linking, LTO Derinlemesine |
| 11 | `11_low_level_optimization.cpp` | Düşük Seviye Optimizasyon, PGO, Branch Prediction |

**Temel Kazanımlar:** Donanım seviyesi anlayış, bellek erişim optimizasyonu, OS mekanizmaları.

---

## Modül 8: C++ Core Guidelines — Üst Düzey Kurallar
📁 `module_08_core_guidelines` · C++17 · **9 dosya · 2.578 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_philosophy_and_interfaces.cpp` | P.1/P.4/P.5, constexpr, static_assert, STL Algoritmaları |
| 2 | `02_resource_management_rule_of_zero.cpp` | Rule of Zero, R.10/R.11, RAII Tipleri |
| 3 | `03_error_handling.cpp` | E.2/E.3/E.15/E.16, noexcept, std::terminate |
| 4 | `04_concurrency_and_performance.cpp` | CP.20, lock_guard, False Sharing, std::atomic |
| 5 | `05_smart_pointer_guidelines.cpp` | R.20-R.37, Ownership Semantiği, Passing Pointers |
| 6 | `06_class_design_guidelines.cpp` | C.2/C.4/C.9, Sınıf Tasarım Kuralları, Invariant |
| 7 | `07_api_design.cpp` | Modern C++'da API Tasarımı, Arayüz Prensipler |
| 8 | `08_maintainable_cpp.cpp` | Büyük Projelerde Sürdürülebilir C++ |
| 9 | `09_clean_code_reference.md` | Clean Code Referans Rehberi |

**Temel Kazanımlar:** ISO C++ Core Guidelines uyumlu profesyonel kod yazımı.

---

## Modül 9: Advanced Mechanics — İleri Mekanikler
📁 `module_09_advanced_mechanics` · **C++20** · **8 dosya · 2.065 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_bit_manipulation_and_alignment.cpp` | Bit Maskeleme, alignas, std::bitset, C++20 <bit> |
| 2 | `02_operator_overloading.cpp` | +, ==, (), << Overloading, friend, C++20 <=> |
| 3 | `03_advanced_pointers.cpp` | Pointer to Member, std::invoke, std::mem_fn |
| 4 | `04_constant_evaluation.cpp` | constexpr vs consteval vs constinit |
| 5 | `05_type_traits_and_concepts.cpp` | SFINAE, enable_if → C++20 Concepts, requires |
| 6 | `06_sfinae_and_tag_dispatch.cpp` | SFINAE, Tag Dispatch, if constexpr |
| 7 | `07_crtp_pattern.cpp` | CRTP — Curiously Recurring Template Pattern |
| 8 | `08_template_metaprogramming.cpp` | Şablon Metaprogramlama Derinlemesine |

**Temel Kazanımlar:** Düşük seviye mekanikler, C++20 yenilikleri, derleme zamanı programlama.

---

## Modül 10: Design Patterns — Tasarım Kalıpları
📁 `module_10_design_patterns` · **C++20** · **7 dosya · 3.434 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_solid_principles.cpp` | SOLID Prensipleri (SRP, OCP, LSP, ISP, DIP) |
| 2 | `02_creational_patterns.cpp` | Singleton, Factory, Builder, Prototype |
| 3 | `03_behavioral_patterns.cpp` | Observer, Strategy, Command, State |
| 4 | `04_structural_patterns.cpp` | Adapter, Decorator, Facade, Composite, Proxy |
| 5 | `05_testable_design.cpp` | CRTP, Type Erasure, Policy-Based Design |
| 6 | `06_pattern_decision_guide.cpp` | Kalıp Karar Rehberi, Anti-Pattern, Proxy, Prototype |
| 7 | `07_embedded_automotive_patterns.cpp` | Gömülü & Otomotiv Kalıpları — Memory Pool, Active Object, FSM, Hardware Proxy |

**Temel Kazanımlar:** GoF tasarım kalıpları, SOLID mimari, test edilebilir kod tasarımı, kalıp seçim kararları, gömülü/otomotiv özel kalıplar.

---

## Modül 11: Embedded & Real-Time — Gömülü Sistemler
📁 `module_11_embedded_systems` · **C++20** · **5 dosya · 1.608 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_volatile_hardware.cpp` | volatile, Memory-Mapped I/O, ISR Kavramları |
| 2 | `02_bitfields_protocols.cpp` | Bitfield'lar, Protokol Parsing, Packed Struct |
| 3 | `03_embedded_constraints.cpp` | No-Heap Allocation, Static Buffer, constexpr Lookup |
| 4 | `04_rtos_concurrency.cpp` | RTOS Patterns, Priority Inversion, Lock-Free |
| 5 | `05_advanced_parallelism.cpp` | Thread Pool, C++17 Parallel Algorithms, Work Stealing |

**Temel Kazanımlar:** Gömülü sistemlerde C++ kullanımı, kaynak kısıtlı ortamlar, gerçek zamanlı programlama.

---

## Modül 12: Coroutines — Eşyordamlar
📁 `module_12_coroutines` · **C++20** · **2 dosya · 1.759 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_coroutine_fundamentals.cpp` | C++20 Coroutines: co_await, co_yield, co_return, Promise Type |
| 2 | `02_async_coroutine_patterns.cpp` | Asenkron Coroutine Kalıpları: Scheduler, Pipeline, Cancellation, Symmetric Transfer |

**Temel Kazanımlar:** Eşyordam mekanizması, lazy generator, asenkron pipeline, coroutine handle, simetrik transfer, iptal token.

---

## Modül 13: Networking — Ağ Programlama
📁 `module_13_networking` · **C++20** · **2 dosya · 1.848 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_socket_fundamentals.cpp` | BSD Sockets, TCP/UDP, POSIX Ağ Programlama |
| 2 | `02_nonblocking_io_epoll.cpp` | Bloklamayan I/O, poll, epoll — Olay Güdümlü Sunucu, Reaktör Kalıbı |

**Temel Kazanımlar:** Soket programlama, istemci-sunucu mimarisi, TCP/UDP farkları, ağ protokolleri, epoll ile yüksek performanslı I/O.

---

## Modül 14: Automotive Networks — Otomotiv Ağları
📁 `module_14_automotive_networks` · **C++20** · **6 dosya · 5.127 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_can_fd_and_xl.cpp` | CAN FD & CAN XL — Modern CAN Protokolleri |
| 2 | `02_lin_bus.cpp` | LIN Bus — Düşük Maliyetli Otomotiv Alt Ağı |
| 3 | `03_flexray.cpp` | FlexRay — Yüksek Güvenilirlikli Otomotiv Bus |
| 4 | `04_automotive_ethernet.cpp` | Automotive Ethernet — 100BASE-T1, SOME/IP, DoIP, TSN |
| 5 | `05_inter_mcu_communication.cpp` | Inter-MCU Communication — SPI, I2C, UART, Shared Memory |
| 6 | `06_uds_diagnostics.cpp` | UDS — ISO 14229 & ISO-TP (ISO 15765) |

**Temel Kazanımlar:** Otomotiv iletişim protokolleri, araç içi ağ mimarileri, teşhis servisleri.

---

## Modül 15: HW Interfaces — Donanım Arayüzleri
📁 `module_15_hw_interfaces` · **C++20** · **5 dosya · 3.664 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_gpio_and_registers.cpp` | GPIO & Register-Level Programming, Memory-Mapped I/O |
| 2 | `02_adc_dac.cpp` | ADC & DAC — Analog/Dijital Dönüşüm |
| 3 | `03_serial_protocols_deep.cpp` | SPI/I2C/UART — Register-Level Deep Dive |
| 4 | `04_dma_and_interrupts.cpp` | DMA Engines & Interrupt Handling, NVIC, ISR |
| 5 | `05_memory_types.cpp` | EEPROM, FLASH, FRAM, RAM, DDR, NAND, NVMe |

**Temel Kazanımlar:** Donanım seviyesi programlama, register erişimi, kesme yönetimi, bellek teknolojileri.

---

## Modül 16: AUTOSAR — Otomotiv Yazılım Mimarisi
📁 `module_16_autosar` · **C++20** · **5 dosya · 4.095 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_classic_platform.cpp` | AUTOSAR Classic Platform — Katmanlı Mimari |
| 2 | `02_adaptive_platform.cpp` | AUTOSAR Adaptive Platform — Modern C++ |
| 3 | `03_functional_safety.cpp` | Fonksiyonel Güvenlik — ISO 26262 & ASIL |
| 4 | `04_automotive_cybersecurity.cpp` | Otomotiv Siber Güvenlik — SecOC, HSM, ISO 21434 |
| 5 | `05_misra_cert_coding_standard.cpp` | MISRA C++ 2023 & CERT C++ Kodlama Standartları |

**Temel Kazanımlar:** AUTOSAR platform mimarisi, fonksiyonel güvenlik, siber güvenlik, kodlama standartları.

---

## Modül 17: SDV — Yazılım Tanımlı Araç
📁 `module_17_sdv` · **C++20** · **3 dosya · 2.232 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_sdv_architecture.cpp` | SDV Mimarisi & C++ Rolü |
| 2 | `02_ota_updates.cpp` | OTA (Over-The-Air) Güncelleme Mekanizması |
| 3 | `03_middleware_someip_dds.cpp` | Middleware: SOME/IP, DDS, iceoryx |

**Temel Kazanımlar:** Yazılım tanımlı araç konsepti, OTA güncelleme, otomotiv middleware.

---

## Modül 18: Driver Development — Sürücü Geliştirme
📁 `module_18_driver_development` · **C++20** · **4 dosya · 3.101 satır**

| # | Dosya | Konu |
|---|-------|------|
| 1 | `01_driver_hal_bsp.cpp` | Driver / HAL / BSP Katmanları — C vs C++ |
| 2 | `02_zero_cost_abstraction.cpp` | CRTP & Policy-Based Design — Zero-Cost Abstraction |
| 3 | `03_peripheral_driver.cpp` | Peripheral Driver: GPIO, UART, SPI, ISR, State Machine |
| 4 | `04_mini_ecu_project.cpp` | Capstone Project — Body Control Module (BCM) Simülasyonu |

**Temel Kazanımlar:** HAL/BSP tasarımı, sıfır maliyetli soyutlamalar, sürücü geliştirme, mini ECU projesi.

---

## Derleme Bilgisi

```bash
# Birleşik derleme (C++20 — tüm modüller):
g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow \
    -Wconversion -Wsign-conversion -Wnull-dereference -Wformat=2 \
    <dosya>.cpp -o output

# C++23 gerektiren dosyalar (module_03/06_cpp23_features.cpp):
g++ -std=c++23 -Wall -Wextra -Wpedantic -Wshadow \
    -Wconversion -Wsign-conversion -Wnull-dereference -Wformat=2 \
    <dosya>.cpp -o output

# Concurrency dosyaları (-pthread gerekli):
# module_05/05, module_06/01, module_06/05, module_06/09,
# module_07/03, module_07/04, module_08/04,
# module_11/04, module_11/05, module_12/01
```

Tüm 118 dosya yukarıdaki sıkı flag'lerle **0 warning, 0 error** olarak derlenir.

---

## Referanslar

- [cppreference.com](https://en.cppreference.com/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [ISO C++](https://isocpp.org/)
- [Google Test](https://google.github.io/googletest/)
- [CMake Documentation](https://cmake.org/documentation/)

---

**Hazırlayan:** Murat Mecit KAHRAMANLI · Mart 2026
