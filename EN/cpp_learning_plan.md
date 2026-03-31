# C++ Professional Mastery Learning Plan

Welcome to your journey to becoming a professional C++ developer! This living document will guide us through lessons, exercises, and milestones.

## Resources
1. **[cppreference.com](https://en.cppreference.com/)**: The authoritative C++ language and standard library reference. Our PRIMARY technical reference for all modules.
2. **[C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)**: Best practices by Bjarne Stroustrup & Herb Sutter. Foundation for Module 8.
3. **[Mike Shah C++ Playlist](https://www.youtube.com/watch?v=LGOgNqkRMs0&list=PLvv0ScY6vfd8j-tlhYVPYgiIyXduu6m-L)**: Video lectures and core concepts.
4. **[Simplify C++!](https://simplifycpp.org/)**: Modern C++ techniques and clean code.
5. **[CppQuiz](https://cppquiz.org/)**: Technical questions to sharpen our understanding.
6. **[C++ Institute](https://cppinstitute.org/cpp)**: Roadmap for CPA and CPP professional certifications.
7. **[ISO C++](https://isocpp.org/)**: Official standards and news.
8. **[Learn-Cpp.org](https://learn-cpp.org/)**: Interactive basic tutorials.

---

## Phase 1: Foundations
- **Goal**: Understand basic syntax, memory model, compilation process.
- **Actions**:
  - Watch first videos from Mike Shah.
  - Write first program with Doxygen comments.
  - Review basic concepts on learn-cpp.org.
- **Status**: ✅ Content Written

## Phase 2: Object-Oriented Programming (OOP)
- **Goal**: Classes, inheritance, polymorphism, RAII.
- **Actions**:
  - Deep dive into Mike Shah's OOP videos.
  - Implement a small project using OOP principles.
- **Status**: ✅ Content Written

## Phase 3: Modern C++ (C++11/14/17/20)
- **Goal**: Smart pointers, auto, lambdas, move semantics.
- **Actions**:
  - Read resources from Simplify C++.
  - Follow ISO C++ standards guidelines.
- **Status**: ✅ Content Written

## Phase 4: Advanced Concepts & Testing
- **Goal**: Templates, Concurrency, STL, memory management.
- **Actions**:
  - Daily challenges on CppQuiz.
- **Status**: ✅ Content Written

## Phase 5: Certification & Professional Prep
- **Goal**: Prepare for C++ Institute exams (CPA/CPP).
- **Actions**:
  - Review exam syllabuses.
  - Solve sample questions.
- **Status**: ✅ Content Written

## Phase 6: Industry Standards & Architecture
- **Goal**: Learn CMake, Unit Testing (GTest), Concurrency, Debugging/Profiling (GDB/Valgrind).
- **Actions**:
  - Create a full CMake project.
  - Write unit tests.
  - Prevent memory leaks manually.
- **Status**: ✅ Content Written

## Phase 7: Under the Hood
- **Goal**: Understand vtable memory layout, SIGSEGV/RAII deeper mechanics, fork vs threads.
- **Actions**:
  - Study virtual table proof with `sizeof`.
  - Explore RAII with core dumps and GDB.
- **CppReference:** [virtual](https://en.cppreference.com/w/cpp/language/virtual), [RAII](https://en.cppreference.com/w/cpp/language/raii), [std::thread](https://en.cppreference.com/w/cpp/thread/thread)
- **Status**: ✅ Content Written

## Phase 8: C++ Core Guidelines
- **Goal**: Apply C++ Core Guidelines (P, R, E, CP sections) to write safe, idiomatic code.
- **Actions**:
  - Study Philosophy & Interfaces (P.1, P.4, P.5).
  - Apply Rule of Zero resource management.
  - Practice error handling best practices.
  - Learn concurrency guidelines (CP.20, false sharing).
- **CppReference:** [Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines), [std::lock_guard](https://en.cppreference.com/w/cpp/thread/lock_guard)
- **Status**: ✅ Content Written

## Phase 9: Advanced Mechanics (C++20)
- **Goal**: Master C++20 features: Concepts, consteval/constinit, advanced pointer mechanics, bit manipulation.
- **Actions**:
  - Implement bit manipulation with `alignas` and `std::bitset`.
  - Practice operator overloading patterns.
  - Study SFINAE → C++20 Concepts evolution.
  - Explore `constexpr` vs `consteval` vs `constinit`.
- **CppReference:** [Concepts](https://en.cppreference.com/w/cpp/language/constraints), [consteval](https://en.cppreference.com/w/cpp/language/consteval), [alignas](https://en.cppreference.com/w/cpp/language/alignas)
- **Status**: ✅ Content Written

## Phase 10: Design Patterns
- **Goal**: Master GoF design patterns and SOLID principles in modern C++.
- **Actions**:
  - Implement SOLID principles (SRP, OCP, LSP, ISP, DIP).
  - Build Creational patterns: Singleton, Factory, Builder.
  - Apply Behavioral patterns: Observer, Strategy, Command.
  - Use Structural patterns: Adapter, Decorator, Facade.
  - Explore testable design: CRTP, Type Erasure, Policy-Based Design.
  - Study embedded & automotive patterns: Memory Pool, Active Object, FSM, Hardware Proxy.
- **CppReference:** [CRTP](https://en.cppreference.com/w/cpp/language/crtp), [std::function](https://en.cppreference.com/w/cpp/utility/functional/function)
- **Status**: ✅ Content Written

## Phase 11: Embedded & Real-Time Systems
- **Goal**: Apply C++ in resource-constrained, real-time embedded environments.
- **Actions**:
  - Understand `volatile` and memory-mapped I/O.
  - Implement bitfields and protocol parsing.
  - Practice no-heap allocation and static buffers.
  - Study RTOS patterns and priority inversion.
  - Build thread pools and parallel algorithms.
- **CppReference:** [volatile](https://en.cppreference.com/w/cpp/language/cv), [std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic), [execution policies](https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag_t)
- **Status**: ✅ Content Written

## Phase 12: Coroutines
- **Goal**: Understand C++20 coroutines: co_await, co_yield, co_return, promise_type and custom awaitables.
- **Actions**:
  - Implement Generator and Task coroutine types.
  - Practice lazy evaluation with coroutines.
  - Build async scheduler, pipeline, and cancellation patterns.
  - Understand symmetric transfer and exception propagation.
- **CppReference:** [coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
- **Status**: ✅ Content Written

## Phase 13: Networking
- **Goal**: Master BSD sockets, TCP/UDP communication, client-server architecture.
- **Actions**:
  - Build TCP echo server and UDP broadcaster.
  - Study epoll edge-triggered vs level-triggered modes.
  - Build event-driven multi-client server (Reactor pattern).
- **Status**: ✅ Content Written

## Phase 14: Automotive Networks
- **Goal**: Deep understanding of CAN FD/XL, LIN, FlexRay, Automotive Ethernet, UDS diagnostics.
- **Actions**:
  - Implement CAN FD frame parser with DBC support.
  - Build UDS diagnostic session handler.
  - Study Automotive Ethernet SOME/IP and DoIP.
- **CppReference:** [std::bitset](https://en.cppreference.com/w/cpp/utility/bitset), [std::array](https://en.cppreference.com/w/cpp/container/array)
- **Status**: ✅ Content Written

## Phase 15: HW Interfaces
- **Goal**: Register-level programming: GPIO, ADC/DAC, SPI/I2C/UART, DMA, interrupts, memory technologies.
- **Actions**:
  - Implement memory-mapped register abstractions.
  - Build DMA transfer manager and ISR handler.
  - Study EEPROM, FLASH, FRAM and DDR technologies.
- **Status**: ✅ Content Written

## Phase 16: AUTOSAR
- **Goal**: Master AUTOSAR Classic/Adaptive platforms, ISO 26262 functional safety, automotive cybersecurity, MISRA/CERT standards.
- **Actions**:
  - Study AUTOSAR layered architecture (MCAL, ECU-Abstraction, Services).
  - Implement ASIL-compliant patterns.
  - Apply MISRA C++ 2023 rules.
- **Status**: ✅ Content Written

## Phase 17: SDV
- **Goal**: Understand Software-Defined Vehicle architecture, OTA updates, middleware (SOME/IP, DDS, iceoryx).
- **Actions**:
  - Study SDV zonal architecture evolution.
  - Implement OTA update state machine.
  - Compare SOME/IP, DDS and iceoryx middleware.
- **Status**: ✅ Content Written

## Phase 18: Driver Development
- **Goal**: Master driver/HAL/BSP layers, zero-cost abstraction, peripheral drivers, and build a capstone ECU project.
- **Actions**:
  - Build HAL abstraction with CRTP and Policy-Based Design.
  - Implement GPIO, UART, SPI peripheral drivers.
  - Complete Body Control Module (BCM) capstone project.
- **Status**: ✅ Content Written
