# C++ Professional Mastery Learning Plan
# C++ Profesyonel Ustalık Eğitim Planı

Welcome to your journey to becoming a professional C++ developer! This living document will guide us through lessons, exercises, and milestones.
Profesyonel bir C++ geliştiricisi olma yolculuğunuza hoş geldiniz! Bu yaşayan belge bizi dersler, egzersizler ve kilometre taşları boyunca yönlendirecek.

## Resources / Kaynaklar
1. **[cppreference.com](https://en.cppreference.com/)**: The authoritative C++ language and standard library reference. Our PRIMARY technical reference for all modules. *(C++ dili ve standart kütüphanesi için resmi referans. Tüm modüller için BİRİNCİL teknik kaynağımız.)*
2. **[C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)**: Best practices by Bjarne Stroustrup & Herb Sutter. Foundation for Module 8. *(En iyi pratikler kılavuzu. Modül 8'in temeli.)*
3. **[Mike Shah C++ Playlist](https://www.youtube.com/watch?v=LGOgNqkRMs0&list=PLvv0ScY6vfd8j-tlhYVPYgiIyXduu6m-L)**: Video lectures and core concepts. *(Video dersler ve temel kavramlar)*
4. **[Simplify C++!](https://simplifycpp.org/)**: Modern C++ techniques and clean code. *(Modern C++ teknikleri ve temiz kod)*
5. **[CppQuiz](https://cppquiz.org/)**: Technical questions to sharpen our understanding. *(Anlayışımızı keskinleştirmek için teknik sorular)*
6. **[C++ Institute](https://cppinstitute.org/cpp)**: Roadmap for CPA and CPP professional certifications. *(CPA ve CPP profesyonel sertifikaları için yol haritası)*
7. **[ISO C++](https://isocpp.org/)**: Official standards and news. *(Resmi C++ standartları ve güncel haberler)*
8. **[Learn-Cpp.org](https://learn-cpp.org/)**: Interactive basic tutorials. *(İnteraktif temel seviye eğitimler)*

---

## Phase 1: Foundations / Aşama 1: Temeller
- **Goal / Hedef**: Understand basic syntax, memory model, compilation process. *(Temel sözdizimi, bellek modeli ve derleme sürecini yapısal olarak kavramak.)*
- **Actions / Eylemler**:
  - Watch first videos from Mike Shah. *(Mike Shah'dan ilk giriş videolarını izle)*
  - Write first program with Doxygen comments. *(Doxygen yorumları içeren ilk C++ programını yaz)*
  - Review basic concepts on learn-cpp.org. *(learn-cpp.org'daki temel konuları tekrar et)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 10 dosya)*

## Phase 2: Object-Oriented Programming (OOP) / Aşama 2: Nesne Yönelimli Programlama
- **Goal / Hedef**: Classes, inheritance, polymorphism, RAII. *(Sınıflar, kalıtım, çok biçimlilik, RAII prebsipleri.)*
- **Actions / Eylemler**:
  - Deep dive into Mike Shah's OOP videos. *(Mike Shah'ın OOP videolarına derinlemesine in)*
  - Implement a small project using OOP principles. *(OOP prensiplerini kullanarak ufak bir proje geliştir)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 8 dosya)*

## Phase 3: Modern C++ (C++11/14/17/20) / Aşama 3: Modern C++
- **Goal / Hedef**: Smart pointers, auto, lambdas, move semantics. *(Akıllı işaretçiler, auto, lambda ifadeleri, taşıma semantiği.)*
- **Actions / Eylemler**:
  - Read resources from Simplify C++. *(Simplify C++ sitesindeki gelişkin kaynakları analiz et)*
  - Follow ISO C++ standards guidelines. *(ISO C++ standartlarına göre modern kod yazımını uygula)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 6 dosya)*

## Phase 4: Advanced Concepts & Testing / Aşama 4: İleri Düzey Kavramlar ve Testler
- **Goal / Hedef**: Templates, Concurrency, STL, memory management. *(Şablonlar, Eşzamanlılık, STL kütüphaneleri, bellek yönetimi ustuluğu.)*
- **Actions / Eylemler**:
  - Daily challenges on CppQuiz. *(CppQuiz mekaniğinden zorlu testler ve kod okuma alıştırmaları)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 9 dosya)*

## Phase 5: Certification & Professional Prep / Aşama 5: Sertifikasyon ve Profesyonel Hazırlık
- **Goal / Hedef**: Prepare for C++ Institute exams (CPA/CPP). *(C++ Institute sınav standartlarına hazırlan - CPA/CPP)*
- **Actions / Eylemler**:
  - Review exam syllabuses. *(Sınav müfredatı ile bilgi dağarcığını karşılaştır)*
  - Solve sample questions. *(Mülakat ve sertifikasyon soruları çöz)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 7 dosya)*

## Phase 6: Industry Standards & Architecture / Aşama 6: Endüstri Standartları ve Mimari
- **Goal / Hedef**: Learn CMake, Unit Testing (GTest), Concurrency, Debugging/Profiling (GDB/Valgrind). *(CMake, Birim Testleri, Eşzamanlılık ve Hata Ayıklama araçlarına hükmet. Profesyonel üretime geç.)*
- **Actions / Eylemler**:
  - Create a full CMake project. *(Tam donanımlı bir CMake projesi kur)*
  - Write unit tests. *(Gtest ile test yaz)*
  - Prevent memory leaks manually. *(Valgrind ile bellek denetimi sağla)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 11 dosya)*

## Phase 7: Under the Hood / Aşama 7: OS ve Donanım Derinlikleri
- **Goal / Hedef**: Understand vtable memory layout, SIGSEGV/RAII deeper mechanics, fork vs threads. *(vtable bellek düzeni, SIGSEGV/RAII mekanikleri, fork vs thread farkını derinlemesine kavra.)*
- **Actions / Eylemler**:
  - Study virtual table proof with `sizeof`. *(sizeof ile vptr kanıtını incele)*
  - Explore RAII with core dumps and GDB. *(RAII'yi core dump ve GDB ile keşfet)*
  - Compare fork() vs std::thread with shared memory. *(fork() ve std::thread'i ortak bellek testi ile karşılaştır)*
- **CppReference:** [virtual](https://en.cppreference.com/w/cpp/language/virtual), [RAII](https://en.cppreference.com/w/cpp/language/raii), [std::thread](https://en.cppreference.com/w/cpp/thread/thread)
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 11 dosya)*

## Phase 8: C++ Core Guidelines / Aşama 8: Üst Düzey Kurallar
- **Goal / Hedef**: Apply C++ Core Guidelines (P, R, E, CP sections) to write safe, idiomatic code. *(C++ Core Guidelines'ı uygulayarak güvenli, deyimsel kod yazma becerisi kazan.)*
- **Actions / Eylemler**:
  - Study Philosophy & Interfaces (P.1, P.4, P.5). *(Felsefe ve Arayüzler)*
  - Apply Rule of Zero resource management. *(Rule of Zero kaynak yönetimi)*
  - Practice error handling best practices. *(Hata yönetimi en iyi pratikleri)*
  - Learn concurrency guidelines (CP.20, false sharing). *(Eşzamanlılık kuralları)*
- **CppReference:** [Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines), [std::lock_guard](https://en.cppreference.com/w/cpp/thread/lock_guard)
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 9 dosya)*

## Phase 9: Advanced Mechanics (C++20) / Aşama 9: İleri Mekanikler
- **Goal / Hedef**: Master C++20 features: Concepts, consteval/constinit, advanced pointer mechanics, bit manipulation. *(C++20 özelliklerinde ustalık kazan: Concepts, consteval/constinit, ileri pointer mekanikleri, bit manipülasyonu.)*
- **Actions / Eylemler**:
  - Implement bit manipulation with `alignas` and `std::bitset`. *(Bit manipülasyonu)*
  - Practice operator overloading patterns. *(Operatör aşırı yükleme kalıpları)*
  - Study SFINAE → C++20 Concepts evolution. *(SFINAE'den Concepts'e geçiş)*
  - Explore `constexpr` vs `consteval` vs `constinit`. *(Derleme zamanı değerlendirme)*
- **CppReference:** [Concepts](https://en.cppreference.com/w/cpp/language/constraints), [consteval](https://en.cppreference.com/w/cpp/language/consteval), [alignas](https://en.cppreference.com/w/cpp/language/alignas)
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 8 dosya)*

## Phase 10: Design Patterns / Aşama 10: Tasarım Kalıpları
- **Goal / Hedef**: Master GoF design patterns and SOLID principles in modern C++. *(Modern C++'ta GoF tasarım kalıpları ve SOLID prensiplerinde ustalık kazan.)*
- **Actions / Eylemler**:
  - Implement SOLID principles (SRP, OCP, LSP, ISP, DIP). *(SOLID prensiplerini uygula)*
  - Build Creational patterns: Singleton, Factory, Builder. *(Yaratıcı kalıplar)*
  - Apply Behavioral patterns: Observer, Strategy, Command. *(Davranışsal kalıplar)*
  - Use Structural patterns: Adapter, Decorator, Facade. *(Yapısal kalıplar)*
  - Explore testable design: CRTP, Type Erasure, Policy-Based Design. *(Test edilebilir tasarım)*
  - Study embedded & automotive patterns: Memory Pool, Active Object, FSM, Hardware Proxy. *(Gömülü/otomotiv kalıpları)*
- **CppReference:** [CRTP](https://en.cppreference.com/w/cpp/language/crtp), [std::function](https://en.cppreference.com/w/cpp/utility/functional/function)
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 7 dosya)*

## Phase 11: Embedded & Real-Time Systems / Aşama 11: Gömülü ve Gerçek Zamanlı Sistemler
- **Goal / Hedef**: Apply C++ in resource-constrained, real-time embedded environments. *(Kaynak kısıtlı, gerçek zamanlı gömülü ortamlarda C++ uygula.)*
- **Actions / Eylemler**:
  - Understand `volatile` and memory-mapped I/O. *(volatile ve bellek eşlemeli G/Ç)*
  - Implement bitfields and protocol parsing. *(Bitfield'lar ve protokol çözümleme)*
  - Practice no-heap allocation and static buffers. *(Heap'siz bellek yönetimi)*
  - Study RTOS patterns and priority inversion. *(RTOS kalıpları ve öncelik tersliği)*
  - Build thread pools and parallel algorithms. *(Thread havuzları ve paralel algoritmalar)*
- **CppReference:** [volatile](https://en.cppreference.com/w/cpp/language/cv), [std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic), [execution policies](https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag_t)
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 5 dosya)*

## Phase 12: Coroutines / Aşama 12: C++20 Eşyordamlar
- **Goal / Hedef**: Understand C++20 coroutines: co_await, co_yield, co_return, promise_type and custom awaitables. *(C++20 eşyordamlarını kavra: co_await, co_yield, co_return, promise_type ve özel awaitable nesneleri.)*
- **Actions / Eylemler**:
  - Implement Generator and Task coroutine types. *(Generator ve Task eşyordam tiplerini uygula)*
  - Practice lazy evaluation with coroutines. *(Eşyordamlar ile tembel değerlendirme uygula)*
  - Build async scheduler, pipeline, and cancellation patterns. *(Asenkron zamanlayıcı, pipeline ve iptal kalıpları oluştur)*
  - Understand symmetric transfer and exception propagation. *(Simetrik transfer ve istisna yaymayı kavra)*
- **CppReference:** [coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 2 dosya)*

## Phase 13: Networking / Aşama 13: Ağ Programlama
- **Goal / Hedef**: Master BSD sockets, TCP/UDP communication, client-server architecture. *(BSD soketleri, TCP/UDP iletişimi, istemci-sunucu mimarisinde ustalık kazan.)*
- **Actions / Eylemler**:
  - Build TCP echo server and UDP broadcaster. *(TCP echo sunucu ve UDP yayıncı oluştur)*
  - Implement non-blocking I/O with poll()/epoll(). *(poll()/epoll() ile bloklamaz G/Ç uygula)*
  - Study epoll edge-triggered vs level-triggered modes. *(epoll kenar/seviye tetiklemeli modları incele)*
  - Build event-driven multi-client server (Reactor pattern). *(Olay güdümlü çoklu istemci sunucu oluştur)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 2 dosya)*

## Phase 14: Automotive Networks / Aşama 14: Otomotiv Ağları
- **Goal / Hedef**: Deep understanding of CAN FD/XL, LIN, FlexRay, Automotive Ethernet, UDS diagnostics. *(CAN FD/XL, LIN, FlexRay, Otomotiv Ethernet, UDS tanı protokollerinde derin anlayış kazan.)*
- **Actions / Eylemler**:
  - Implement CAN FD frame parser with DBC support. *(DBC destekli CAN FD çerçeve ayrıştırıcı yaz)*
  - Build UDS diagnostic session handler. *(UDS tanı oturumu yöneticisi oluştur)*
  - Study Automotive Ethernet SOME/IP and DoIP. *(SOME/IP ve DoIP protokollerini incele)*
- **CppReference:** [std::bitset](https://en.cppreference.com/w/cpp/utility/bitset), [std::array](https://en.cppreference.com/w/cpp/container/array)
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 6 dosya)*

## Phase 15: HW Interfaces / Aşama 15: Donanım Arayüzleri
- **Goal / Hedef**: Register-level programming: GPIO, ADC/DAC, SPI/I2C/UART, DMA, interrupts, memory technologies. *(Register seviyesinde programlama: GPIO, ADC/DAC, SPI/I2C/UART, DMA, kesme yönetimi, bellek teknolojileri.)*
- **Actions / Eylemler**:
  - Implement memory-mapped register abstractions. *(Bellek eşlemeli register soyutlamaları uygula)*
  - Build DMA transfer manager and ISR handler. *(DMA transfer yöneticisi ve ISR işleyici oluştur)*
  - Study EEPROM, FLASH, FRAM and DDR technologies. *(EEPROM, FLASH, FRAM ve DDR teknolojilerini incele)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 5 dosya)*

## Phase 16: AUTOSAR / Aşama 16: AUTOSAR & Güvenlik
- **Goal / Hedef**: Master AUTOSAR Classic/Adaptive platforms, ISO 26262 functional safety, automotive cybersecurity, MISRA/CERT standards. *(AUTOSAR Classic/Adaptive platformları, ISO 26262 fonksiyonel güvenlik, siber güvenlik, MISRA/CERT standartlarında ustalık kazan.)*
- **Actions / Eylemler**:
  - Study AUTOSAR layered architecture (MCAL, ECU-Abstraction, Services). *(AUTOSAR katmanlı mimariyi incele)*
  - Implement ASIL-compliant patterns. *(ASIL uyumlu kalıpları uygula)*
  - Apply MISRA C++ 2023 rules. *(MISRA C++ 2023 kurallarını uygula)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 5 dosya)*

## Phase 17: SDV / Aşama 17: Yazılım Tanımlı Araç
- **Goal / Hedef**: Understand Software-Defined Vehicle architecture, OTA updates, middleware (SOME/IP, DDS, iceoryx). *(Yazılım Tanımlı Araç mimarisi, OTA güncellemeleri, ara katman yazılımlarını kavra.)*
- **Actions / Eylemler**:
  - Study SDV zonal architecture evolution. *(SDV zonal mimari evrimini incele)*
  - Implement OTA update state machine. *(OTA güncelleme durum makinesi uygula)*
  - Compare SOME/IP, DDS and iceoryx middleware. *(SOME/IP, DDS ve iceoryx karşılaştırması yap)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 3 dosya)*

## Phase 18: Driver Development / Aşama 18: Sürücü Geliştirme
- **Goal / Hedef**: Master driver/HAL/BSP layers, zero-cost abstraction, peripheral drivers, and build a capstone ECU project. *(Driver/HAL/BSP katmanları, sıfır maliyetli soyutlama, çevre birimi sürücüleri ve final ECU projesi.)*
- **Actions / Eylemler**:
  - Build HAL abstraction with CRTP and Policy-Based Design. *(CRTP ve Policy-Based Design ile HAL soyutlama oluştur)*
  - Implement GPIO, UART, SPI peripheral drivers. *(GPIO, UART, SPI çevre birimi sürücüleri yaz)*
  - Complete Body Control Module (BCM) capstone project. *(BCM simülasyonu final projesini tamamla)*
- **Status / Durum**: ✅ Content Written *(İçerik Yazıldı — 4 dosya)*
