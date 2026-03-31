/**
 * @file module_04_advanced/03_stl_containers_deepdive.cpp
 * @brief Advanced STL: Container Architectures & Big-O — İleri STL: Konteyner Mimarileri ve
 * Big-O Analizi
 *
 * @details
 * =============================================================================
 * [THEORY: When to Use Which Container? (Software Engineering Interviews) / TEORİ: Hangi
 * Konteyner Ne Zaman?]
 * =============================================================================
 * EN: Choosing the wrong STL container can make a program 10,000x slower.
 * - std::vector : Fast random access [ O(1) ]. Slow to insert at the front [ O(N) ]. Under
 *   the hood, it's just a dynamic array. Best default choice because of CPU CACHE.
 * - std::list   : Doubly Linked List. Fast to insert anywhere [ O(1) ]. TERRIBLE at searching
 *   or random access [ O(N) ]. Nodes are scattered in RAM.
 * - std::map / unordered_map : Key-Value stores. map uses a Red-Black Tree [ O(logN) ].
 *   unordered_map uses a Hash Table [ O(1) ].
 *
 * TR: Yazılım Mimarisi Mülakatlarında asıl soru "Neyi yazdığın" değil, "Veriyi nereye
 * koyduğun"dur.
 * - std::vector : %90 ihtimalle doğru cevaptır. RAM'de yan yana (Contiguous) durur. CPU Cache
 *   (Önbellek) dostudur. Okuması roket hızındadır O(1). Ama dizinin başına eleman
 *   İTERSENİZ tüm dizi kayacağı için çok yavaştır O(N).
 * - std::list   : Bağlı Listelerdir (Linked List). Elemanlar RAM'de darmadağın durur. Araya
 *   eleman sokuşturmak harikadır O(1). Ama 50. elemanı bulmak O(N)'dir.
 * - std::map vs std::unordered_map : Sözlük yapıları (Key-Value/Anahtar-Değer). map: Veriyi
 *   sürekli alfabetik/sayısal sıralayarak (Kırmızı-Siyah Ağaç) tutar O(logN). unordered_map:
 *   Sırasız, Hash (Şifreleme) tablosudur. Rokettir O(1).
 *
 * =============================================================================
 * [THEORY: Vector Capacity vs Size Confusion / TEORİ: Kapasite vs Boyut Kargaşası]
 * =============================================================================
 * EN: `size` is the actual elements inside (e.g., 5 elements). `capacity` is the pre-allocated
 * RAM hardware given by OS (e.g., 8 spaces). If capacity fills up, Vector doubles it `(8*2 = 16)
 * ` and physically MOVES to a new RAM address (Reallocation). This is costly! Solution:
 * `reserve()`.
 *
 * TR: Vector'ün Boyutu (Size) içindeki gerçek eleman sayısıdır. Capacity (Kapasite) ise işletim
 * sisteminden çekrilmiş RAM potansiyelidir. Eğer Kapasite dolar ve siz itmeye devam ederseniz,
 * Vector kendi kapasitesini 2 KATINA ÇIKARARAK yep yeni bir fiziksel RAM adresine kopyalanır ve
 * Taşınır (Reallocation). Bu korkunç yavaştır! Çözüm: `reserve()` fonksiyonu ile baştan yer
 * ayırtmak.
 *
 * =============================================================================
 * [CPPREF DEPTH: The Dreaded Iterator Invalidation HACK! / CPPREF DERİNLİK: Korkunç İteratör
 * Geçersizleşmesi (İteratör Çöküşü)!]
 * =============================================================================
 * EN: What happens if you have a pointer (Iterator) looking at `vector[5]`, but you push a new
 * element and the vector reallocates to a new RAM address? CppReference says: ALL ITERATORS ARE
 * INVALIDATED! Your pointer is now looking at dead memory (Garbage). Same happens if you
 * `erase()` from the middle of a Vector. ALWAYS check cppreference for the "Iterator
 * invalidation" table before modifying a container while looping over it!
 *
 * TR: Ya eğer elinizde `vector`'ün 5. elemanını gösteren bir pointer/İteratör varsa, ve siz yeni
 * eleman ekleyip vector'ün kapasitesini doldurduğunuz için vector komple YENİ BİR RAM ADRESİNE
 * taşınırsa? CppReference açıkça yazar: "TÜM İTERATÖRLER GEÇERSİZ KALIR (Invalidated)". Sizin o
 * 5. elemana bakan pointerınız çöpte sallanıp (Dangling) oyunu çökertir (SIGSEGV). Bir STL kabı
 * içinde döngü yaparken eleman siliyorsanız, daima CppRef Invalidation tablosuna bakmalısınız!
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_stl_containers_deepdive.cpp -o 03_stl_containers_deepdive
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <memory_resource>
#include <numeric>
#include <tuple>
#include <unordered_map>
#include <vector>

int main() {
  std::cout << "=== MODULE 4: STL CONTAINERS DEEP DIVE ===\n" << std::endl;

  // --- 1. VECTOR CAPACITY & REALLOCATION COST (Vektor Kapasite Çilesi) ---
  std::cout << "--- 1. std::vector Capacity & Reallocation ---" << std::endl;
  std::vector<int> numbers;

  // EN: Cost of not using reserve():
  // TR: reserve() kullanmamanın "Yeniden Taşınma (Realloc)" maliyeti:
  int migrationCount = 0;
  size_t oldCapacity = numbers.capacity();

  for (int i = 0; i < 20; i++) {
    numbers.push_back(i);
    // EN: If capacity changes, it means the entire vector moved to a new memory block!
    // TR: Eğer kapasite değişirse, tüm vector donanımsal olarak yeni bir RAM alanına fırlatıldı
    // demektir!
    if (numbers.capacity() != oldCapacity) {
      std::cout << "Reallocation (Veri Taşındı)! New Capacity (Yeni Kapasite): " <<
          numbers.capacity() << std::endl;
      oldCapacity = numbers.capacity();
      migrationCount++;
    }
  }

  std::cout << "For just 20 elements, Vector physically moved in RAM " << migrationCount <<
      " times!" << std::endl;
  std::cout << "> SOLUTION (ÇÖZÜM): `numbers.reserve(20);` to prepare RAM at "
               "the start.\n"
            << std::endl;

  // --- 2. MAP (Tree) vs UNORDERED_MAP (Hash Table) ---
  std::cout << "--- 2. Dictionary (Key-Value) Selection ---" << std::endl;

  // EN: Backed by Red-Black Tree. Maintains order. O(logN).
  // TR: Arkasında Kırmızı-Siyah Ağaç yatar. Sürekli veriyi sıralı tutar. O(logN).
  std::map<std::string, int> orderedDictionary;
  orderedDictionary["Zebra"] = 1;
  orderedDictionary["Bear"] = 2;
  orderedDictionary["Cat"] = 3;

  std::cout << "std::map (Ordered Tree / Sıralı Ağaç - O(logN) ) :" << std::endl;
  for (auto const &[key, val] : orderedDictionary) {
    // EN: Output will AUTOMATICALLY be Alphabetical! (Bear, Cat, Zebra)
    // TR: Çıktı OTOMATİK olarak Alfabetik sıralıdır! (Bear, Cat, Zebra)
    std::cout << key << ":" << val << " ";
  }

  // EN: Backed by Hash Table. Unpredictable order. O(1) Speed!
  // TR: Arkasında Hash C-Tabloları yatar. Sıralama garantisi YOKTUR. O(1) Roket Hızı!
  std::cout << "\n\nstd::unordered_map (Hash Table / Sırasız - O(1) ) :" << std::endl;
  std::unordered_map<std::string, int> hashDictionary;
  hashDictionary["Zebra"] = 1;
  hashDictionary["Bear"] = 2;
  hashDictionary["Cat"] = 3;

  for (auto const &[key, val] : hashDictionary) {
    // EN: Output order is NOT guaranteed. It only focuses on speed.
    // TR: Çıktı sıralama garantisi VERMEZ. Sadece hıza odaklanır.
    std::cout << key << ":" << val << " ";
  }
  std::cout << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 3 — std::array: Fixed-Size Wheel Speed Sensors
  // TR: Demo 3 — std::array: Sabit Boyutlu Tekerlek Hız Sensörleri
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. std::array — Fixed-Size Wheel Speed Sensors ---" << std::endl;

  // EN: std::array<T, N> lives entirely on the stack — zero heap allocation.
  // TR: std::array<T, N> tamamen stack üzerinde yaşar — heap tahsisi sıfırdır.
  std::array<double, 4> wheelSpeedKmh = {82.5, 81.9, 83.1, 80.7};

  // ─── 3a. Compile-Time Size ─────────────────────────────────────────────────────────────────────
  // EN: Size is known at compile time — no runtime overhead.
  // TR: Boyut derleme zamanında bilinir — çalışma zamanı ek maliyeti yoktur.
  std::cout << "Wheel count (compile-time): " << wheelSpeedKmh.size() << std::endl;
  static_assert(wheelSpeedKmh.size() == 4, "ECU expects exactly 4 wheel sensors");

  // ─── 3b. Bounds Checking with .at() ────────────────────────────────────────────────────────────
  // EN: .at() throws std::out_of_range if index is invalid — safer than [].
  // TR: .at() geçersiz indekste std::out_of_range fırlatır — []'den güvenlidir.
  std::cout << "Front-Left  (FL) speed: " << wheelSpeedKmh.at(0) << " km/h" << std::endl;
  std::cout << "Front-Right (FR) speed: " << wheelSpeedKmh.at(1) << " km/h" << std::endl;
  std::cout << "Rear-Left   (RL) speed: " << wheelSpeedKmh.at(2) << " km/h" << std::endl;
  std::cout << "Rear-Right  (RR) speed: " << wheelSpeedKmh.at(3) << " km/h" << std::endl;

  // ─── 3c. STL Algorithm Compatibility ───────────────────────────────────────────────────────────
  // EN: std::array is fully compatible with STL algorithms (sort, find, etc.).
  // TR: std::array, STL algoritmalarıyla (sort, find vb.) tam uyumludur.
  std::sort(wheelSpeedKmh.begin(), wheelSpeedKmh.end());
  std::cout << "Sorted speeds (ascending): ";
  for (const auto &spd : wheelSpeedKmh) {
    std::cout << spd << " ";
  }
  std::cout << std::endl;

  auto it = std::find(wheelSpeedKmh.begin(), wheelSpeedKmh.end(), 83.1);
  if (it != wheelSpeedKmh.end()) {
    std::cout << "Found 83.1 km/h at index: "
              << std::distance(wheelSpeedKmh.begin(), it) << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 4 — std::tuple: Heterogeneous ECU Diagnostic Data
  // TR: Demo 4 — std::tuple: Heterojen ECU Arıza Teşhis Verisi
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. std::tuple — ECU Diagnostic Packet ---" << std::endl;

  // EN: Tuple groups different types together — perfect for multi-field ECU frames.
  // TR: Tuple farklı tipleri bir arada gruplar — çoklu alan içeren ECU çerçeveleri için
  // idealdir.
  std::tuple<int, std::string, double> ecuDiag(0x1A3F, "Throttle Sensor Drift", 4.87);

  // ─── 4a. Access with std::get<> ────────────────────────────────────────────────────────────────
  // EN: std::get<Index> retrieves the element at compile-time index.
  // TR: std::get<Index> derleme zamanı indeksiyle elemana erişir.
  std::cout << "Error Code : 0x" << std::hex << std::get<0>(ecuDiag) << std::dec << std::endl;
  std::cout << "Description: " << std::get<1>(ecuDiag) << std::endl;
  std::cout << "Voltage    : " << std::get<2>(ecuDiag) << " V" << std::endl;

  // ─── 4b. Unpacking with std::tie ───────────────────────────────────────────────────────────────
  // EN: std::tie binds tuple elements to existing variables by reference.
  // TR: std::tie, tuple elemanlarını mevcut değişkenlere referansla bağlar.
  int errCode{};
  std::string errDesc;
  double voltage{};
  std::tie(errCode, errDesc, voltage) = ecuDiag;
  std::cout << "[tie] Code=0x" << std::hex << errCode << std::dec
            << "  Desc=" << errDesc << "  V=" << voltage << std::endl;

  // ─── 4c. Structured Bindings (C++17) ───────────────────────────────────────────────────────────
  // EN: Structured bindings provide the cleanest syntax — one-liner unpacking.
  // TR: Yapısal bağlamalar en temiz sözdizimini sunar — tek satırda açma.
  auto [code, desc, volt] = ecuDiag;
  std::cout << "[structured binding] Code=0x" << std::hex << code << std::dec
            << "  Desc=" << desc << "  V=" << volt << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 5 — Container Selection Guide (Automotive Context)
  // TR: Demo 5 — Konteyner Seçim Rehberi (Otomotiv Bağlamı)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 5. Container Selection Guide ---" << std::endl;
  std::cout << R"(
  +-------------------+------------------+--------------------------------------+
  | Container         | Complexity       | Automotive Use Case                  |
  +-------------------+------------------+--------------------------------------+
  | std::vector       | O(1) access      | Sensor stream buffers, CAN frames    |
  | std::array        | O(1), stack only | Fixed wheel/axle sensor arrays       |
  | std::map          | O(logN) ordered  | DTC code registry (sorted by ID)     |
  | std::unordered_map| O(1) hash        | Real-time signal lookup by name      |
  | std::tuple        | compile-time     | Grouping heterogeneous ECU fields    |
  +-------------------+------------------+--------------------------------------+
)" << std::endl;

  // ─── Note: std::span (C++20) ───────────────────────────────────────────────────────────────────
  // EN: std::span<T> is a non-owning, lightweight view over contiguous memory (e.g., a raw
  // array, std::vector, or std::array). It does NOT copy data. Think of it as a "window" into an
  // existing buffer — ideal for passing sensor data slices to processing functions without
  // ownership transfer. Requires C++20: compile with -std=c++20.
  // TR: std::span<T>, bitişik bellekteki veriye sahiplenmeyen hafif bir görünümdür (raw dizi,
  // std::vector veya std::array üzerinde). Veriyi KOPYALAMAZ. Mevcut bir tampona açılan bir
  // "pencere" gibi düşünün — sensör veri dilimlerini sahiplik devretmeden işleme fonksiyonlarına
  // geçirmek için idealdir. C++20 gerektirir: -std=c++20 ile derleyin.

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 6 — std::deque: Double-Ended Queue Internals
  // TR: Demo 6 — std::deque: Çift Uçlu Kuyruk İç Yapısı
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 6. std::deque: Double-Ended Queue ---" << std::endl;
  {
    // EN: deque stores data in CHUNKS (blocks). Not contiguous like vector.
    //     O(1) push_front AND push_back (vector has O(n) push_front).
    //     Iterators invalidated only on insert/erase in the middle.
    // TR: deque veriyi PARÇALAR (bloklar) halinde saklar. vector gibi bitişik değil.
    //     O(1) push_front VE push_back (vector'ün push_front'u O(n)).
    std::deque<std::string> canBus;

    // EN: Efficient at both ends — ideal for sliding window / FIFO buffer
    // TR: Her iki uçta verimli — kayan pencere / FIFO tampon için ideal
    canBus.push_back("FRAME_0x100");
    canBus.push_back("FRAME_0x101");
    canBus.push_back("FRAME_0x102");
    canBus.push_front("FRAME_0x0FF");  // O(1) — vector can't do this efficiently

    std::cout << "  Front: " << canBus.front()
              << ", Back: " << canBus.back()
              << ", Size: " << canBus.size() << std::endl;

    canBus.pop_front();  // O(1) dequeue
    std::cout << "  After pop_front, Front: " << canBus.front() << std::endl;

    // EN: Random access — same as vector, O(1)
    // TR: Rastgele erişim — vector ile aynı, O(1)
    std::cout << "  canBus[1] = " << canBus[1] << std::endl;

    // EN: Memory layout: array of pointers to fixed-size chunks
    //     ┌─────────┐     ┌──────────┐
    //     │ chunk 0 │────>│ A B C D  │
    //     │ chunk 1 │────>│ E F G H  │
    //     │ chunk 2 │────>│ I _ _ _  │
    //     └─────────┘     └──────────┘
    //     Unlike vector, no full reallocation on growth — just add a new chunk.
    // TR: Bellek yerleşimi: sabit boyutlu parçalara işaretçi dizisi.
    //     vector'den farklı olarak, büyümede tam yeniden ayırma yok.
    std::cout << "  (deque: O(1) both ends, random access, chunk-based memory)\n" << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 7 — std::pmr: Polymorphic Memory Resources
  // TR: Demo 7 — std::pmr: Polimorfik Bellek Kaynakları
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 7. std::pmr: Polymorphic Memory Resources ---" << std::endl;
  {
    // EN: std::pmr containers use a runtime-configurable memory resource.
    //     Use cases: arena allocators, stack buffers, embedded systems.
    //     Key benefit: swap allocator at runtime without changing container type.
    // TR: std::pmr konteynerleri çalışma zamanında yapılandırılabilir bellek kaynağı kullanır.
    //     Kullanım alanları: arena ayırıcılar, yığın tamponları, gömülü sistemler.

    // EN: Stack-based buffer — zero heap allocations for small data
    // TR: Yığın tabanlı tampon — küçük veriler için sıfır heap ayırma
    char stackBuffer[1024];
    std::pmr::monotonic_buffer_resource pool{stackBuffer, sizeof(stackBuffer)};

    std::pmr::vector<int> sensorReadings{&pool};
    for (int i = 0; i < 20; ++i) {
      sensorReadings.push_back(i * 10);
    }
    std::cout << "  pmr::vector with stack buffer: " << sensorReadings.size()
              << " elements, sum=" << std::accumulate(sensorReadings.begin(),
                                                      sensorReadings.end(), 0)
              << std::endl;

    // EN: pmr::string uses same allocator
    // TR: pmr::string aynı bellek yöneticisini kullanır
    std::pmr::string msg{"ECU_DIAGNOSTIC_OK", &pool};
    std::cout << "  pmr::string: " << msg << " (length=" << msg.size() << ")" << std::endl;

    // EN: Advantages over std::allocator:
    //     1. No template parameter differences — pmr::vector<int> is always same type
    //     2. Arena/pool allocation — super fast for many short-lived objects
    //     3. No heap fragmentation — monotonic_buffer_resource just bumps a pointer
    //     4. Embedded-friendly — can use static buffers
    // TR: std::allocator'a göre avantajları:
    //     1. Şablon parametresi farkı yok — pmr::vector<int> her zaman aynı tip
    //     2. Arena/havuz ayırma — çok sayıda kısa ömürlü nesne için süper hızlı
    //     3. Heap parçalanması yok
    //     4. Gömülü dostu — statik tamponlar kullanabilir
    std::cout << "  (pmr: runtime allocator, arena-friendly, zero-heap possible)\n" << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 8 — C++23 flat_map/flat_set Reference
  // TR: Demo 8 — C++23 flat_map/flat_set Referansı
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 8. C++23 flat_map / flat_set Reference ---" << std::endl;
  {
    // EN: std::flat_map and std::flat_set (C++23, <flat_map> / <flat_set>) store
    //     elements in sorted CONTIGUOUS memory (vector-backed). Benefits:
    //     - Cache-friendly iteration (unlike std::map's tree nodes)
    //     - Lower memory overhead (no node pointers)
    //     - Better for read-heavy, small-to-medium sized maps
    //
    //     #include <flat_map>
    //     std::flat_map<int, std::string> dtc_codes;
    //     dtc_codes[0x100] = "Engine Overheat";
    //     dtc_codes[0x200] = "Low Oil Pressure";
    //     // Iteration is FAST — contiguous memory, cache-line friendly
    //     for (const auto& [code, desc] : dtc_codes) { ... }
    //
    //     ┌──────────────┬──────────────┬──────────────┬──────────────────────┐
    //     │ Container    │ Insert O()   │ Lookup O()   │ Iteration            │
    //     ├──────────────┼──────────────┼──────────────┼──────────────────────┤
    //     │ std::map     │ O(log N)     │ O(log N)     │ Slow (tree nodes)    │
    //     │ flat_map     │ O(N) amort.  │ O(log N)     │ FAST (contiguous)    │
    //     │ unordered_map│ O(1) amort.  │ O(1) amort.  │ Unordered            │
    //     └──────────────┴──────────────┴──────────────┴──────────────────────┘
    //
    // TR: std::flat_map ve std::flat_set (C++23) elemanları sıralı BİTİŞİK
    //     bellekte (vector-destekli) saklar. Önbellek dostu iterasyon.

    // EN: Simulate flat_map behavior with sorted vector of pairs
    // TR: flat_map davranışını sıralı çift vektörüyle simüle et
    std::vector<std::pair<int, std::string>> flatDtc = {
        {0x100, "Engine Overheat"},
        {0x150, "Coolant Low"},
        {0x200, "Low Oil Pressure"},
        {0x300, "ABS Fault"}
    };
    std::sort(flatDtc.begin(), flatDtc.end());

    // EN: Binary search — O(log N), same as flat_map::find
    // TR: İkili arama — O(log N), flat_map::find ile aynı
    auto found = std::lower_bound(flatDtc.begin(), flatDtc.end(),
                               std::make_pair(0x200, std::string{}),
                               [](const auto& a, const auto& b) {
                                   return a.first < b.first;
                               });
    if (found != flatDtc.end() && found->first == 0x200) {
        std::cout << "  Found 0x200: " << found->second << std::endl;
    }

    std::cout << "  Flat iteration (contiguous): ";
    for (const auto& [dtcCode, dtcDesc] : flatDtc) {
        (void)dtcDesc;
        std::cout << "0x" << std::hex << dtcCode << std::dec << " ";
    }
    std::cout << "\n  (C++23 std::flat_map — use sorted vector until compiler supports it)\n"
              << std::endl;
  }

  return 0;
}
