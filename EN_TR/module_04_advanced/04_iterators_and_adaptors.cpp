/**
 * @file module_04_advanced/04_iterators_and_adaptors.cpp
 * @brief Templates & STL: Iterators, Categories and Adaptors — Şablonlar ve STL: İteratörler,
 * Kategoriler ve Adaptörler
 *
 * Konu: Iterators, Categories, Custom Iterators & Iterator Adaptors / İteratörler, Kategorileri,
 * Özel İteratörler ve İteratör Adaptörleri
 *
 * Standart: C++17 Derleme: g++ -std=c++17 -Wall -Wextra -Wpedantic 04_iterators_and_adaptors.cpp
 *
 * @details
 * =====================================================================
 * [THEORY: Iterator Categories — The 5 Ranks of STL Access / TEORİ: İteratör Kategorileri — STL
 * Erişiminin 5 Kademesi]
 * =====================================================================
 *
 * EN: 1. Input Iterator
 * Read-only, single-pass, forward-only. Like reading sensor data from a CAN bus — once you
 * read a frame, it's gone. You can only move forward and read each value exactly once.
 * Supports: ++it, *it (read), it == it2, it != it2
 * 2. Output Iterator
 * Write-only, single-pass, forward-only. Like writing commands to an actuator — you push
 * data out, you can't read it back. Supports: ++it, *it = val (write)
 * 3. Forward Iterator
 * Read+Write, multi-pass, forward-only. Like iterating over a linked list of ECU diagnostic
 * trouble codes (DTCs). Supports: Everything Input + multi-pass guarantee
 * 4. Bidirectional Iterator
 * Forward + backward traversal. Like scrolling through a vehicle's event log — you can go
 * forward and backward through entries.
 * Supports: Everything Forward + --it
 * Used by: std::list, std::set, std::map
 * 5. Random Access Iterator
 * Full pointer arithmetic. Like accessing any sensor value in an array by index — instant
 * O(1) jump to any position.
 * Supports: Everything Bidirectional + it+n, it-n, it[n], it<it2
 * Used by: std::vector, std::deque, std::array, raw pointers
 *
 * TR: 1. Giriş İteratörü
 * Salt okunur, tek geçişli, sadece ileri. CAN bus'tan sensör verisi okumak gibi — bir kare
 * okunduktan sonra gider. Sadece ileri gidip her değeri tam olarak bir kez okuyabilirsiniz.
 * Destekler: ++it, *it (okuma), it == it2, it != it2
 * 2. Çıkış İteratörü
 * Salt yazılır, tek geçişli, sadece ileri. Bir aktüatöre komut yazmak gibi — veri
 * gönderirsiniz, geri okuyamazsınız. Destekler: ++it, *it = val (yazma)
 * 3. İleri İteratörü
 * Okuma+Yazma, çok geçişli, sadece ileri. ECU arıza kodları (DTC) bağlı listesinde gezinmek
 * gibi. Destekler: Input'un tamamı + çok geçiş garantisi
 * 4. Çift Yönlü İteratör
 * İleri + geri gezinme. Araç olay günlüğünde gezinmek gibi — ileri ve geri gidebilirsiniz.
 * Destekler: Forward'ın tamamı + --it
 * Kullanan: std::list, std::set, std::map
 * 5. Rastgele Erişim İteratörü
 * Tam pointer aritmetiği. Bir dizideki herhangi bir sensör değerine indeksle erişmek gibi —
 * herhangi bir konuma anında O(1) atlama.
 * Destekler: Bidirectional'ın tamamı + it+n, it-n, it[n], it<it2
 * Kullanan: std::vector, std::deque, std::array, ham pointerlar
 *
 * =====================================================================
 * [HIERARCHY / HİYERARŞİ]
 * =====================================================================
 *
 * EN: Random Access ⊃ Bidirectional ⊃ Forward ⊃ Input ⊃ Output
 * Each higher category includes all capabilities of those below it.
 * std::sort    → Requires Random Access
 * std::find    → Input is sufficient
 * std::reverse → Requires Bidirectional
 *
 * TR: Random Access ⊃ Bidirectional ⊃ Forward ⊃ Input ⊃ Output
 * Her üst kategori, altındakilerin tüm yeteneklerini içerir.
 * std::sort    → Random Access gerektirir
 * std::find    → Input yeterlidir
 * std::reverse → Bidirectional gerektirir
 *
 * =====================================================================
 *
 * [CPPREF DEPTH: Iterator Categories and Their Guarantees / CPPREF DERİNLİK: İteratör
 * Kategorileri ve Garantileri]
 * =============================================================================
 * EN: The C++ standard defines 5 iterator categories, each a superset of the previous:
 * 1. InputIterator   — single-pass read  (==, !=, *, ++)
 * 2. OutputIterator  — single-pass write  (*it = val, ++)
 * 3. ForwardIterator — multi-pass read/write (can revisit elements)
 * 4. BidirectionalIterator — adds backward traversal (--)
 * 5. RandomAccessIterator  — O(1) jump: it+n, it-n, it[n], <, >
 * `std::distance(first, last)` is O(1) for RandomAccess iterators (pointer subtraction) but
 * O(n) for all others (incremental ++). `std::advance(it, n)` similarly dispatches on
 * category. Iterator invalidation rules vary critically by container:
 * • vector: insert/push_back may invalidate ALL iterators (realloc).
 * • deque: insert at front/back invalidates iterators but not refs.
 * • list/set/map: insert NEVER invalidates existing iterators.
 * C++20 introduces `std::contiguous_iterator` — guarantees elements are adjacent in memory
 * (e.g., vector, array, string).
 *
 * TR: C++ standardı 5 iteratör kategorisi tanımlar, her biri öncekinin üst kümesidir:
 * 1. InputIterator   — tek geçişli okuma  (==, !=, *, ++)
 * 2. OutputIterator  — tek geçişli yazma  (*it = val, ++)
 * 3. ForwardIterator — çok geçişli okuma/yazma (tekrar ziyaret edilebilir)
 * 4. BidirectionalIterator — geri gidiş eklenir (--)
 * 5. RandomAccessIterator  — O(1) atlama: it+n, it-n, it[n], <, >
 * `std::distance(first, last)` RandomAccess için O(1)'dir (işaretçi çıkarma), diğerleri için
 * O(n)'dir (sıralı ++). İteratör geçersizleşme kuralları konteynere göre değişir:
 * • vector: insert/push_back TÜM iteratörleri geçersiz kılabilir (realloc).
 * • deque: baş/sona ekleme iteratörleri geçersiz kılar ama referansları değil.
 * • list/set/map: ekleme mevcut iteratörleri ASLA geçersiz kılmaz.
 * C++20 `std::contiguous_iterator` ekler — elemanların bellekte bitişik olduğunu garanti eder
 * (ör. vector, array, string).
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_iterators_and_adaptors.cpp -o 04_iterators_and_adaptors
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>    // EN: STL algorithms
#include <deque>        // EN: Double-ended queue
#include <forward_list> // EN: Singly-linked list
#include <iostream>     // EN: Standard I/O
#include <iterator>     // EN: Iterator adaptors
#include <list>         // EN: Doubly-linked list
#include <numeric>      // EN: std::iota, std::accumulate
#include <set>          // EN: Ordered set
#include <sstream>      // EN: String stream
#include <string>       // EN: std::string
#include <vector>       // EN: Dynamic array

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [PRODUCTION CODE / ÜRETİM KODU]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 1. Demonstrating Iterator Categories ────────────────────────────────────────────────────────

// EN: A function that REQUIRES Random Access Iterators (like std::sort).
// TR: Random Access İteratör GEREKTİREN bir fonksiyon (std::sort gibi).
template <typename RandomIt>
void printSorted(RandomIt first, RandomIt last) {
    std::vector<typename std::iterator_traits<RandomIt>::value_type> temp(first, last);
    std::sort(temp.begin(), temp.end());
    for (const auto& val : temp) {
        std::cout << val << " ";
    }
    std::cout << "\n";
}

// EN: A function that works with ANY Input Iterator (minimum requirement).
// TR: Herhangi bir Input İteratör ile çalışan fonksiyon (minimum gereksinim).
template <typename InputIt>
typename std::iterator_traits<InputIt>::value_type
sumRange(InputIt first, InputIt last) {
    typename std::iterator_traits<InputIt>::value_type total{};
    while (first != last) {
        total += *first;
        ++first;
    }
    return total;
}

// ─── 2. Custom Iterator — SensorRange ────────────────────────────────────────────────────────────

// EN: A custom forward iterator that generates sensor readings in a range. Simulates reading
// sequential sensor addresses (e.g., temperature registers 0x100, 0x104, 0x108 ... on an ECU
// memory map).
// TR: Belirli bir aralıkta sensör okumaları üreten özel forward iterator. Sıralı sensör
// adreslerini okumayı simüle eder (örn. ECU bellek haritasında 0x100, 0x104, 0x108 ... sıcaklık
// kaydedicileri).
class SensorRange {
   public:
    // EN: Forward Iterator implementation
    // TR: Forward İteratör uygulaması
    class Iterator {
       public:
        // EN: Required type aliases for std::iterator_traits
        // TR: std::iterator_traits için gerekli tip tanımları
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = const int*;
        using reference = const int&;

        explicit Iterator(int val) : current_(val) {}

        reference operator*() const { return current_; }
        pointer operator->() const { return &current_; }

        Iterator& operator++() {
            // EN: 4-byte aligned sensor addresses
            // TR: 4-byte hizalı sensör adresleri
            current_ += 4;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return current_ >= other.current_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

       private:
        int current_;
    };

    SensorRange(int start, int end) : start_(start), end_(end) {}

    Iterator begin() const { return Iterator(start_); }
    Iterator end() const { return Iterator(end_); }

   private:
    int start_;
    int end_;
};

// ─── 3. Container Adaptor Demonstrations ─────────────────────────────────────────────────────────

// EN: Collect diagnostic trouble codes (DTCs) using back_inserter. back_inserter automatically
// calls push_back on the target container.
// TR: Arıza kodlarını (DTC) back_inserter ile topla. back_inserter hedef konteynerde otomatik
// push_back çağırır.
std::vector<std::string> collectActiveDTCs(
    const std::vector<std::string>& allCodes,
    const std::string& prefix) {
    std::vector<std::string> result;
    std::copy_if(allCodes.begin(), allCodes.end(),
                 std::back_inserter(result),
                 [&prefix](const std::string& code) {
                     return code.substr(0, prefix.size()) == prefix;
                 });
    return result;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [EXAMPLES / ÖRNEKLER]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== MODULE 4: ITERATORS & ADAPTORS ===\n"
              << "=== MODÜL 4: İTERATÖRLER VE ADAPTÖRLER ===\n\n";

    // ─── 1. Iterator Categories in Action ────────────────────────────────────────────────────────
    {
        std::cout << "--- 1. Iterator Categories ---\n";

        std::vector<int> speeds = {120, 85, 200, 60, 150};

        // EN: vector iterators are Random Access — supports std::sort
        // TR: vector iteratörleri Random Access — std::sort destekler
        std::cout << "Sorted speeds: ";
        printSorted(speeds.begin(), speeds.end());

        // EN: sumRange works with ANY iterator (Input is enough)
        // TR: sumRange herhangi bir iteratör ile çalışır (Input yeterli)
        std::cout << "Sum of speeds: " << sumRange(speeds.begin(), speeds.end()) << "\n";

        // EN: list iterators are Bidirectional — can reverse but NOT random access
        // TR: list iteratörleri Bidirectional — ters gidebilir ama rastgele erişim YOK
        std::list<int> rpms = {3000, 1500, 4500, 2200};
        // EN: list has its own sort (merge sort) — doesn't need Random Access
        // TR: list'in kendi sıralaması var (merge sort) — Random Access gerektirmez
        rpms.sort();
        std::cout << "Sorted RPMs (list::sort): ";
        for (const auto& r : rpms) std::cout << r << " ";
        std::cout << "\n";

        // EN: forward_list is Forward only — single direction, no size()
        // TR: forward_list sadece Forward — tek yön, size() yok
        std::forward_list<int> temps = {72, 88, 65, 91};
        temps.sort();
        std::cout << "Sorted temps (forward_list): ";
        for (const auto& t : temps) std::cout << t << " ";
        std::cout << "\n\n";
    }

    // ─── 2. Custom Iterator — SensorRange ────────────────────────────────────────────────────────
    {
        std::cout << "--- 2. Custom SensorRange Iterator ---\n";

        // EN: Iterate over sensor addresses 0x100 to 0x120 (4-byte steps)
        // TR: 0x100'den 0x120'ye sensör adresleri üzerinde gezin (4-byte adım)
        SensorRange sensors(0x100, 0x120);

        std::cout << "Sensor addresses: ";
        for (int addr : sensors) {
            std::cout << "0x" << std::hex << addr << " ";
        }
        std::cout << std::dec << "\n";

        // EN: Works with STL algorithms because it satisfies ForwardIterator
        // TR: ForwardIterator'ı karşıladığı için STL algoritmaları ile çalışır
        std::cout << "Count of addresses: "
                  << std::distance(sensors.begin(), sensors.end()) << "\n\n";
    }

    // ─── 3. back_inserter — Dynamic Collection ───────────────────────────────────────────────────
    {
        std::cout << "--- 3. back_inserter ---\n";

        std::vector<int> source = {10, 20, 30, 40, 50};
        std::vector<int> dest;

        // EN: back_inserter creates an Output Iterator that calls push_back
        // TR: back_inserter push_back çağıran bir Output İteratör oluşturur
        std::copy(source.begin(), source.end(), std::back_inserter(dest));
        std::cout << "Copied via back_inserter: ";
        for (int v : dest) std::cout << v << " ";
        std::cout << "\n";

        // EN: Transform + back_inserter — double each value into a new vector
        // TR: Transform + back_inserter — her değeri iki katına çıkar
        std::vector<int> doubled;
        std::transform(source.begin(), source.end(),
                       std::back_inserter(doubled),
                       [](int x) { return x * 2; });
        std::cout << "Doubled: ";
        for (int v : doubled) std::cout << v << " ";
        std::cout << "\n\n";
    }

    // ─── 4. inserter — Middle Insertion ──────────────────────────────────────────────────────────
    {
        std::cout << "--- 4. inserter (mid-point insertion) ---\n";

        std::vector<int> data = {1, 2, 5, 6};
        std::vector<int> toInsert = {3, 4};

        // EN: Insert {3,4} at position 2 (before element '5')
        // TR: {3,4}'ü 2. pozisyona ekle ('5' elemanından önce)
        auto insertPos = data.begin() + 2;
        std::copy(toInsert.begin(), toInsert.end(),
                  std::inserter(data, insertPos));

        std::cout << "After inserter: ";
        for (int v : data) std::cout << v << " ";
        std::cout << "\n\n";
    }

    // ─── 5. ostream_iterator — Direct Output ─────────────────────────────────────────────────────
    {
        std::cout << "--- 5. ostream_iterator ---\n";

        std::vector<std::string> ecuNames = {"ECM", "TCM", "BCM", "ABS", "TPMS"};

        // EN: Write directly to cout using an output iterator — no loop needed!
        // TR: Output iterator ile doğrudan cout'a yaz — döngü gerekmez!
        std::cout << "ECU modules: ";
        std::copy(ecuNames.begin(), ecuNames.end(),
                  std::ostream_iterator<std::string>(std::cout, ", "));
        std::cout << "\n\n";
    }

    // ─── 6. istream_iterator — Parse from String Stream ──────────────────────────────────────────
    {
        std::cout << "--- 6. istream_iterator (from stringstream) ---\n";

        // EN: Simulate reading space-separated sensor values from a data stream
        // TR: Veri akışından boşlukla ayrılmış sensör değerlerini okumayı simüle et
        std::istringstream sensorData("72 85 91 68 77");
        std::vector<int> readings{
            std::istream_iterator<int>(sensorData),
            std::istream_iterator<int>()};

        std::cout << "Parsed readings: ";
        for (int r : readings) std::cout << r << " ";
        std::cout << "\n";
        std::cout << "Average: "
                  << sumRange(readings.begin(), readings.end()) /
                         static_cast<int>(readings.size())
                  << "\n\n";
    }

    // ─── 7. Reverse Iterators ────────────────────────────────────────────────────────────────────
    {
        std::cout << "--- 7. Reverse Iterators ---\n";

        std::vector<int> faultLog = {101, 203, 305, 407, 509};

        // EN: rbegin()/rend() traverse the container backwards
        // TR: rbegin()/rend() konteyneri geriye doğru gezinir
        std::cout << "Fault log (newest first): ";
        for (auto it = faultLog.rbegin(); it != faultLog.rend(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // EN: Find last element > 300 using reverse iterators
        // TR: Ters iteratör ile > 300 olan son elemanı bul
        auto rit = std::find_if(faultLog.rbegin(), faultLog.rend(),
                                [](int code) { return code > 300; });
        if (rit != faultLog.rend()) {
            std::cout << "Last fault > 300: " << *rit << "\n";
        }
        std::cout << "\n";
    }

    // ─── 8. std::move_iterator ───────────────────────────────────────────────────────────────────
    {
        std::cout << "--- 8. move_iterator ---\n";

        std::vector<std::string> logs = {"[INFO] Start", "[WARN] Temp high",
                                         "[ERR] Overheat", "[INFO] Shutdown"};

        // EN: Move (not copy!) strings from logs into archive
        // TR: Stringleri logs'tan archive'a taşı (kopyalama değil!)
        std::vector<std::string> archive(
            std::make_move_iterator(logs.begin()),
            std::make_move_iterator(logs.end()));

        std::cout << "Archived " << archive.size() << " entries\n";
        std::cout << "Original logs[0] after move: \""
                  << logs[0] << "\" (empty — moved!)\n\n";
    }

    // ─── 9. DTC Filter with back_inserter ────────────────────────────────────────────────────────
    {
        std::cout << "--- 9. Automotive DTC Filter ---\n";

        std::vector<std::string> allDTCs = {
            "P0301", "P0420", "B0100", "C0035",
            "P0171", "U0100", "P0442", "B0200"};

        // EN: Filter only Powertrain codes (start with 'P')
        // TR: Sadece Motor/Şanzıman kodlarını filtrele ('P' ile başlayan)
        auto powertrainDTCs = collectActiveDTCs(allDTCs, "P");

        std::cout << "Powertrain DTCs: ";
        std::copy(powertrainDTCs.begin(), powertrainDTCs.end(),
                  std::ostream_iterator<std::string>(std::cout, " "));
        std::cout << "\n";

        // EN: Filter Body codes (start with 'B')
        // TR: Gövde kodlarını filtrele ('B' ile başlayan)
        auto bodyDTCs = collectActiveDTCs(allDTCs, "B");
        std::cout << "Body DTCs: ";
        std::copy(bodyDTCs.begin(), bodyDTCs.end(),
                  std::ostream_iterator<std::string>(std::cout, " "));
        std::cout << "\n\n";
    }

    // ─── 10. std::iota + Iterators ───────────────────────────────────────────────────────────────
    {
        std::cout << "--- 10. std::iota + Iterator Arithmetic ---\n";

        std::vector<int> sensorIDs(10);
        // EN: Fill with sequential IDs: 100, 101, 102, ...
        // TR: Sıralı ID'ler ile doldur: 100, 101, 102, ...
        std::iota(sensorIDs.begin(), sensorIDs.end(), 100);

        // EN: Random Access: jump to 5th element directly
        // TR: Random Access: doğrudan 5. elemana atla
        auto it = sensorIDs.begin() + 4;
        std::cout << "5th sensor ID: " << *it << "\n";

        // EN: Iterator arithmetic: distance between two positions
        // TR: İteratör aritmetiği: iki pozisyon arası mesafe
        auto first = std::find(sensorIDs.begin(), sensorIDs.end(), 103);
        auto last = std::find(sensorIDs.begin(), sensorIDs.end(), 108);
        std::cout << "Distance 103→108: "
                  << std::distance(first, last) << " elements\n";

        // EN: advance() moves an iterator by N steps
        // TR: advance() bir iteratörü N adım ilerletir
        auto walker = sensorIDs.begin();
        std::advance(walker, 7);
        std::cout << "After advance(7): " << *walker << "\n";

        // EN: next()/prev() return a NEW iterator without modifying the original
        // TR: next()/prev() orijinali değiştirmeden YENİ iteratör döndürür
        auto n = std::next(sensorIDs.begin(), 3);
        auto p = std::prev(sensorIDs.end(), 2);
        std::cout << "next(begin,3)=" << *n << "  prev(end,2)=" << *p << "\n";
    }

    std::cout << "\n=== ITERATORS & ADAPTORS COMPLETE ===\n";
    return 0;
}
