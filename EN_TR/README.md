# C++ Mastery

**Professional C++ Learning Curriculum — From Fundamentals to Automotive & Embedded Systems**

Kapsamlı bir C++ eğitim müfredatı: 18 modül, 118 kaynak dosya, ~55.000 satır kod ve açıklama.
Her dosya Doxygen formatında İngilizce/Türkçe ikili teori, cppreference.com derinlik bölümleri ve derlenebilir kod örnekleri içerir.

**Primary Reference:** [cppreference.com](https://en.cppreference.com/) — Every file includes relevant cppreference links in its `[CPPREF DEPTH]` section.

---

## Project Stats

| | |
|---|---|
| **Modules** | 18 |
| **Source Files** | 118 (114 `.cpp` + 2 `.txt` + 2 `.md`) |
| **Total Lines** | ~55,000 |
| **Standards** | C++17 / C++20 / C++23 |
| **Languages** | English + Turkish (bilingual theory) |
| **Output** | PDF Book (933 pages) · DOCX · Doxygen HTML |

---

## Module Overview

| # | Module | Topic | Std | Files |
|---|--------|-------|-----|-------|
| 1 | Basics | Temeller ve Hafıza | C++17 | 10 |
| 2 | OOP | Nesne Yönelimli Programlama | C++17 | 8 |
| 3 | Modern C++ | Güvenlik ve Performans | C++17/C++23 | 6 |
| 4 | Templates & STL | Şablonlar ve STL | C++17 | 9 |
| 5 | Certification Prep | Sertifikasyon Hazırlığı | C++17 | 7 |
| 6 | Industry Standards | Endüstri Standartları | C++17 | 11 |
| 7 | Under the Hood | OS Derinlikleri | C++17/C++20 | 11 |
| 8 | Core Guidelines | Üst Düzey Kurallar | C++17 | 9 |
| 9 | Advanced Mechanics | İleri Mekanikler | C++20 | 8 |
| 10 | Design Patterns | Tasarım Kalıpları | C++20 | 7 |
| 11 | Embedded Systems | Gömülü Sistemler | C++20 | 5 |
| 12 | Coroutines | C++20 Eşyordamlar | C++20 | 2 |
| 13 | Networking | Ağ Programlama | C++20 | 2 |
| 14 | Automotive Networks | Otomotiv Ağları | C++20 | 6 |
| 15 | HW Interfaces | Donanım Arayüzleri | C++20 | 5 |
| 16 | AUTOSAR | AUTOSAR & Güvenlik | C++20 | 5 |
| 17 | SDV | Yazılım Tanımlı Araç | C++20 | 3 |
| 18 | Driver Development | Sürücü Geliştirme | C++20 | 4 |

---

## Module 1: Basics — Temeller ve Hafıza
📁 `module_01_basics` · C++17 · 10 files · 2,962 lines

| File | Topic |
|------|-------|
| `01_variables_and_memory.cpp` | Variables, Data Types, and Memory Model |
| `02_pointers_and_references.cpp` | Pointers, References, and Memory Addresses |
| `03_functions_and_overloading.cpp` | Functions, Overloading, and Inline |
| `04_operators_and_expressions.cpp` | Operators, Expressions & Precedence |
| `05_control_flow_deepdive.cpp` | Control Flow Deep Dive |
| `06_strings_and_arrays.cpp` | Strings and Arrays |
| `07_enums_type_conversions.cpp` | Enumerations, auto, typedef/using & Type Conversions |
| `08_file_io_and_streams.cpp` | File I/O and Stream Operations |
| `09_scope_namespace_recursion.cpp` | Scope, Lifetime, Namespace & Recursion |
| `10_filesystem_and_binary_io.cpp` | std::filesystem & Binary I/O |

## Module 2: OOP — Nesne Yönelimli Programlama
📁 `module_02_oop` · C++17 · 8 files · 2,898 lines

| File | Topic |
|------|-------|
| `01_classes_objects.cpp` | Classes, Objects, and Access Specifiers |
| `02_inheritance_polymorphism.cpp` | Inheritance and Polymorphism |
| `03_rule_of_three_five.cpp` | Memory Leaks and Rule of 3/5/0 |
| `04_multiple_inheritance.cpp` | Multiple Inheritance & Diamond Problem |
| `05_encapsulation_abstraction.cpp` | Encapsulation & Abstraction |
| `06_static_and_friends.cpp` | Static Members & Friend Functions |
| `07_nested_and_local_classes.cpp` | Nested & Local Classes |
| `08_rtti_typeid.cpp` | RTTI & typeid |

## Module 3: Modern C++ — Güvenlik ve Performans
📁 `module_03_modern_cpp` · C++17/C++23 · 6 files · 2,258 lines

| File | Topic |
|------|-------|
| `01_smart_pointers.cpp` | Smart Pointers and RAII |
| `02_lambdas.cpp` | Lambda Expressions — From Zero to Mastery |
| `03_move_semantics_rvalue.cpp` | Move Semantics & R-Values |
| `04_modern_types.cpp` | Modern Data Types: optional, variant, any, string_view |
| `05_structured_bindings_optional.cpp` | Structured Bindings & C++17 Vocabulary Types |
| `06_cpp23_features.cpp` | C++23 Features: std::expected, std::print, Deducing this |

## Module 4: Templates & STL — Şablonlar ve STL
📁 `module_04_advanced` · C++17 · 9 files · 4,268 lines

| File | Topic |
|------|-------|
| `01_templates_stl.cpp` | Templates and STL |
| `02_functors_callbacks.cpp` | Functors and Callbacks |
| `03_stl_containers_deepdive.cpp` | Container Architectures & Big-O |
| `04_iterators_and_adaptors.cpp` | Iterators, Categories and Adaptors |
| `05_algorithms_masterclass.cpp` | Algorithm Masterclass |
| `06_associative_containers.cpp` | Associative & Unordered Containers |
| `07_string_operations.cpp` | String Operations & string_view |
| `08_ranges_and_views.cpp` | C++20 Ranges & Views |
| `09_json_and_serialization.cpp` | JSON Parsing & Serialization |

## Module 5: Certification Prep — Sertifikasyon Hazırlığı
📁 `module_05_certification` · C++17 · 7 files · 1,924 lines

| File | Topic |
|------|-------|
| `01_undefined_behavior.cpp` | Undefined Behavior & Tricky Questions |
| `02_object_slicing_virtual.cpp` | Object Slicing and Virtual Tables |
| `03_type_casting.cpp` | Type Casting (static/dynamic/const/reinterpret) |
| `04_exceptions_handling.cpp` | Exception Handling |
| `05_const_correctness.cpp` | const Correctness |
| `06_interview_gotchas.cpp` | Interview Gotchas & Tricky Behavior |
| `07_error_handling_advanced.cpp` | Advanced Error Handling Patterns |

## Module 6: Industry Standards — Endüstri Standartları
📁 `module_06_industry_standards` · C++17 · 11 files · 4,746 lines

| File | Topic |
|------|-------|
| `01_concurrency_threads.cpp` | Threading and Concurrency |
| `02_cmake_basics.txt` | CMake Build System |
| `03_unit_testing_gtest.cpp` | Unit Testing (TDD) with Google Test |
| `04_valgrind_profiling.cpp` | Memory Profiling and Valgrind |
| `05_async_futures.cpp` | std::async, std::future, std::promise |
| `06_debugging_techniques.cpp` | Debugging Techniques & Defensive Programming |
| `07_git_for_cpp_devs.md` | Git Workflow for C++ Developers |
| `08_cmake_deepdive.txt` | CMake Deep Dive — Targets, Generators, FetchContent |
| `09_mutex_lock_deepdive.cpp` | Mutex & Lock Deep Dive |
| `10_advanced_logging.cpp` | Advanced Logging Patterns |
| `11_gtest_advanced.cpp` | Google Test — Advanced TDD & Mocking |

## Module 7: Under the Hood — OS Derinlikleri
📁 `module_07_under_the_hood` · C++17/C++20 · 11 files · 4,390 lines

| File | Topic |
|------|-------|
| `01_vtable_memory_layout.cpp` | V-Table Memory Cost Proof |
| `02_sigsegv_and_raii.cpp` | SIGSEGV & Prevention via RAII |
| `03_fork_vs_thread_pool.cpp` | POSIX fork() vs std::thread & Thread Pooling |
| `04_memory_model_and_cache.cpp` | CPU Cache Hierarchy, False Sharing, Cache-Friendly Design |
| `05_compilation_pipeline.cpp` | C++ Compilation Pipeline — Source to Executable |
| `06_abi_and_name_mangling.cpp` | ABI and C++ Name Mangling |
| `07_inline_assembly.cpp` | Inline Assembly & GCC Extended asm |
| `08_simd_intrinsics.cpp` | SIMD Intrinsics (SSE/AVX) |
| `09_cpp20_modules.cpp` | C++20 Modules |
| `10_linking_deepdive.cpp` | Static & Dynamic Linking Deep Dive |
| `11_low_level_optimization.cpp` | Low-Level Optimization Techniques |

## Module 8: C++ Core Guidelines — Üst Düzey Kurallar
📁 `module_08_core_guidelines` · C++17 · 9 files · 2,578 lines

| File | Topic |
|------|-------|
| `01_philosophy_and_interfaces.cpp` | Philosophy & Interfaces (P.1/P.4/P.5) |
| `02_resource_management_rule_of_zero.cpp` | Resource Management & Rule of Zero |
| `03_error_handling.cpp` | Error Handling (E.2/E.3/E.15/E.16) |
| `04_concurrency_and_performance.cpp` | Concurrency & Performance Rules (CP.20) |
| `05_smart_pointer_guidelines.cpp` | Smart Pointer Ownership Rules |
| `06_class_design_guidelines.cpp` | Class Design Rules |
| `07_api_design.cpp` | API Design — Clean Interfaces |
| `08_maintainable_cpp.cpp` | Maintainable C++ — Code Quality |
| `09_clean_code_reference.md` | Clean Code Reference Guide |

## Module 9: Advanced Mechanics — İleri Mekanikler
📁 `module_09_advanced_mechanics` · **C++20** · 8 files · 2,065 lines

| File | Topic |
|------|-------|
| `01_bit_manipulation_and_alignment.cpp` | Bit Manipulation & Memory Alignment |
| `02_operator_overloading.cpp` | Operator Overloading |
| `03_advanced_pointers.cpp` | Pointers to Member Functions & std::invoke |
| `04_constant_evaluation.cpp` | constexpr vs consteval vs constinit |
| `05_type_traits_and_concepts.cpp` | Type Traits (SFINAE) & C++20 Concepts |
| `06_sfinae_and_tag_dispatch.cpp` | SFINAE, Tag Dispatch & if constexpr |
| `07_crtp_pattern.cpp` | CRTP — Static Polymorphism |
| `08_template_metaprogramming.cpp` | Template Metaprogramming |

## Module 10: Design Patterns — Tasarım Kalıpları
📁 `module_10_design_patterns` · **C++20** · 7 files · 3,434 lines

| File | Topic |
|------|-------|
| `01_solid_principles.cpp` | SOLID Principles |
| `02_creational_patterns.cpp` | Creational — Singleton, Factory, Builder, Prototype |
| `03_behavioral_patterns.cpp` | Behavioral — Observer, Strategy, Command, State |
| `04_structural_patterns.cpp` | Structural — Adapter, Decorator, Facade, Composite, Proxy |
| `05_testable_design.cpp` | Testable Design — CRTP, Type Erasure, Policy-Based |
| `06_pattern_decision_guide.cpp` | Pattern Decision Guide — Anti-Patterns, Comparisons, Proxy, Prototype |
| `07_embedded_automotive_patterns.cpp` | Embedded & Automotive Patterns — Memory Pool, Active Object, FSM, Hardware Proxy |

## Module 11: Embedded & Real-Time — Gömülü Sistemler
📁 `module_11_embedded_systems` · **C++20** · 5 files · 1,608 lines

| File | Topic |
|------|-------|
| `01_volatile_hardware.cpp` | volatile Keyword, Memory-Mapped I/O, ISR |
| `02_bitfields_protocols.cpp` | Bitfields, Protocol Parsing, Packed Structs |
| `03_embedded_constraints.cpp` | No-Heap Allocation, Static Buffers, constexpr |
| `04_rtos_concurrency.cpp` | RTOS Patterns, Priority Inversion, Lock-Free |
| `05_advanced_parallelism.cpp` | Thread Pools, C++17 Parallel Algorithms, Work Stealing |

## Module 12: Coroutines — C++20 Eşyordamlar
📁 `module_12_coroutines` · **C++20** · 2 files · 1,759 lines

| File | Topic |
|------|-------|
| `01_coroutine_fundamentals.cpp` | C++20 Coroutines: Fundamentals and Mechanics |
| `02_async_coroutine_patterns.cpp` | Async Coroutine Patterns — Scheduler, Pipeline, Cancellation, Symmetric Transfer |

## Module 13: Networking — Ağ Programlama
📁 `module_13_networking` · **C++20** · 2 files · 1,848 lines

| File | Topic |
|------|-------|
| `01_socket_fundamentals.cpp` | BSD Sockets, TCP/UDP Network Programming |
| `02_nonblocking_io_epoll.cpp` | Non-Blocking I/O, poll, epoll — Event-Driven Server, Reactor Pattern |

## Module 14: Automotive Networks — Otomotiv Ağları
📁 `module_14_automotive_networks` · **C++20** · 6 files · 5,127 lines

| File | Topic |
|------|-------|
| `01_can_fd_and_xl.cpp` | CAN FD & CAN XL — Modern CAN Protocols |
| `02_lin_bus.cpp` | LIN Bus — Low-Cost Automotive Sub-Network |
| `03_flexray.cpp` | FlexRay — High-Reliability Automotive Bus |
| `04_automotive_ethernet.cpp` | Automotive Ethernet — 100BASE-T1, SOME/IP, DoIP, TSN |
| `05_inter_mcu_communication.cpp` | Inter-MCU Communication — SPI, I2C, UART, Shared Memory |
| `06_uds_diagnostics.cpp` | UDS — Unified Diagnostic Services (ISO 14229) |

## Module 15: HW Interfaces — Donanım Arayüzleri
📁 `module_15_hw_interfaces` · **C++20** · 5 files · 3,664 lines

| File | Topic |
|------|-------|
| `01_gpio_and_registers.cpp` | GPIO & Register-Level Programming |
| `02_adc_dac.cpp` | ADC & DAC — Analog-to-Digital / Digital-to-Analog |
| `03_serial_protocols_deep.cpp` | SPI/I2C/UART — Register-Level Deep Dive |
| `04_dma_and_interrupts.cpp` | DMA Engines & Interrupt Handling |
| `05_memory_types.cpp` | Memory Technologies — EEPROM, FLASH, FRAM, DDR |

## Module 16: AUTOSAR — AUTOSAR & Güvenlik
📁 `module_16_autosar` · **C++20** · 5 files · 4,095 lines

| File | Topic |
|------|-------|
| `01_classic_platform.cpp` | AUTOSAR Classic Platform — Layered Architecture |
| `02_adaptive_platform.cpp` | AUTOSAR Adaptive Platform — Modern C++ |
| `03_functional_safety.cpp` | Functional Safety — ISO 26262 & ASIL |
| `04_automotive_cybersecurity.cpp` | Automotive Cybersecurity — SecOC, HSM, ISO 21434 |
| `05_misra_cert_coding_standard.cpp` | MISRA C++ 2023 & CERT C++ Coding Standards |

## Module 17: SDV — Yazılım Tanımlı Araç
📁 `module_17_sdv` · **C++20** · 3 files · 2,232 lines

| File | Topic |
|------|-------|
| `01_sdv_architecture.cpp` | Software-Defined Vehicle Architecture |
| `02_ota_updates.cpp` | OTA (Over-The-Air) Update Mechanism |
| `03_middleware_someip_dds.cpp` | Middleware: SOME/IP, DDS, iceoryx |

## Module 18: Driver Development — Sürücü Geliştirme
📁 `module_18_driver_development` · **C++20** · 4 files · 3,101 lines

| File | Topic |
|------|-------|
| `01_driver_hal_bsp.cpp` | Driver / HAL / BSP Layers |
| `02_zero_cost_abstraction.cpp` | CRTP & Policy-Based Design — Zero-Cost Abstraction |
| `03_peripheral_driver.cpp` | Peripheral Driver — GPIO, UART, SPI, ISR |
| `04_mini_ecu_project.cpp` | Capstone Project — Body Control Module (BCM) Simulation |

---

## File Format

Every `.cpp` file follows a consistent Doxygen-documented structure:

```
/**
 * @file    module_XX_name/NN_filename.cpp
 * @brief   EN Title — TR Title
 * @details
 * ==============================================
 * [THEORY: Topic / TEORİ: Konu]
 * ==============================================
 * EN: English theory paragraph...
 * TR: Türkçe teori paragrafı...
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion file.cpp -o output
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI
 * @license Code: Apache 2.0 | Book: CC BY-NC-SA 4.0
 */

// ═══════════════════════════════════════════════
// PART N: Section Title / Bölüm Başlığı
// ═══════════════════════════════════════════════
// Compilable C++ code with bilingual inline comments
int main() { ... }
```

---

## Compilation

```bash
# All modules (C++20 — backward compatible with C++17 content):
g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    <file>.cpp -o output

# C++23 files (module_03/06_cpp23_features.cpp):
g++ -std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    <file>.cpp -o output

# Concurrency files (add -pthread):
# module_06/01, module_06/05, module_06/09, module_07/03,
# module_07/04, module_08/04, module_11/04, module_11/05
g++ -std=c++20 ... -pthread <file>.cpp -o output

# fork() example (POSIX only — Linux/macOS):
g++ -std=c++20 -pthread module_07_under_the_hood/03_fork_vs_thread_pool.cpp -o fork_demo
```

All 114 `.cpp` source files compile with **0 warnings, 0 errors** using the strict flags above (except `06_cpp23_features.cpp` which requires `-std=c++23`).

---

## Book Output

The curriculum is also available as a professionally formatted book:

- **PDF** — `C++_Mastery_Book.pdf` (936 pages, clickable TOC, artistic cover)

---

## References

- [cppreference.com](https://en.cppreference.com/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [ISO C++](https://isocpp.org/)
- [Google Test](https://google.github.io/googletest/)
- [CMake Documentation](https://cmake.org/documentation/)

---

## License

This project uses a **dual-license** model:

| Component | License |
|---|---|
| **Source code** (`.cpp` files) | [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0) |
| **Book content** (text, explanations, PDF layout) | [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) |

### Apache License 2.0 — Source Code

You may freely use, modify, and distribute the source code examples, including for commercial purposes. Requirements:
- **Attribution** — Retain the original copyright notice and license
- **State Changes** — Clearly mark any modifications you make
- **Include License** — Distribute a copy of the Apache 2.0 license with your work
- **Patent Grant** — Contributors grant a patent license for their contributions

### CC BY-NC-SA 4.0 — Book Content

The educational text, theory sections, bilingual comments, and PDF book layout are licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0:
- **Attribution** — Give appropriate credit and link to the license
- **NonCommercial** — No commercial use of the content
- **ShareAlike** — Distribute derivatives under the same license

See [LICENSE](LICENSE) for full details.

---

**Author:** Murat Mecit KAHRAMANLI · March 2026

**Copyright** © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
