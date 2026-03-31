/**
 * @file module_04_advanced/05_algorithms_masterclass.cpp
 * @brief Templates & STL: Algorithm Masterclass — Şablonlar ve STL: Algoritma Ustalık Rehberi
 *
 * Konu: STL Algorithms — Sorting, Searching, Modifying, Numeric & Partitioning
 * STL Algoritmaları — Sıralama, Arama, Değiştirme, Sayısal ve Bölümleme
 *
 * Standart: C++17
 * Derleme: g++ -std=c++17 -Wall -Wextra -Wpedantic 05_algorithms_masterclass.cpp
 *
 * @details
 * =====================================================================
 * [THEORY: STL Algorithms — The Power of Generic Programming / TEORİ: STL Algoritmaları —
 * Jenerik Programlamanın Gücü]
 * =====================================================================
 *
 * EN: The STL provides 100+ algorithms in <algorithm> and <numeric>. They operate on ITERATORS,
 * not containers — making them universal. This is the "separation of concerns" principle:
 * containers STORE data, algorithms PROCESS data, iterators CONNECT them.
 *
 * Key algorithm families:
 * ┌─────────────────────────────────────────────────────────────┐
 * │ Non-modifying  │ find, count, all_of, any_of, none_of,      │
 * │                │ for_each, mismatch, equal, search          │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Modifying      │ copy, move, transform, fill, generate,     │
 * │                │ replace, remove, unique, reverse, rotate   │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Sorting        │ sort, stable_sort, partial_sort,           │
 * │                │ nth_element, is_sorted                     │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Binary Search  │ lower_bound, upper_bound, equal_range,     │
 * │                │ binary_search (requires sorted range!)     │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Partitioning   │ partition, stable_partition,               │
 * │                │ is_partitioned                             │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Numeric        │ accumulate, inner_product, partial_sum,    │
 * │                │ iota, reduce (C++17), transform_reduce     │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Min/Max        │ min, max, minmax, min_element,             │
 * │                │ max_element, minmax_element, clamp (C++17) │
 * └─────────────────────────────────────────────────────────────┘
 *
 * TR: STL, <algorithm> ve <numeric> başlıklarında 100'den fazla algoritma sunar. Konteynerler
 * üzerinde değil, İTERATÖRLER üzerinde çalışırlar — bu onları evrensel kılar. Bu
 * "sorumlulukların ayrılması" ilkesidir: konteynerler veri DEPOLAR, algoritmalar veri İŞLER,
 * iteratörler onları BAĞLAR.
 *
 * Temel algoritma aileleri:
 * ┌─────────────────────────────────────────────────────────────┐
 * │ Değiştirmeyen  │ find, count, all_of, any_of, none_of,      │
 * │                │ for_each, mismatch, equal, search          │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Değiştiren     │ copy, move, transform, fill, generate,     │
 * │                │ replace, remove, unique, reverse, rotate   │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Sıralama       │ sort, stable_sort, partial_sort,           │
 * │                │ nth_element, is_sorted                     │
 * ├─────────────────────────────────────────────────────────────┤
 * │ İkili Arama    │ lower_bound, upper_bound, equal_range,     │
 * │                │ binary_search (sıralı aralık gerektirir!)  │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Bölümleme      │ partition, stable_partition,               │
 * │                │ is_partitioned                             │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Sayısal        │ accumulate, inner_product, partial_sum,    │
 * │                │ iota, reduce (C++17), transform_reduce     │
 * ├─────────────────────────────────────────────────────────────┤
 * │ Min/Maks       │ min, max, minmax, min_element,             │
 * │                │ max_element, minmax_element, clamp (C++17) │
 * └─────────────────────────────────────────────────────────────┘
 * =====================================================================
 *
 * [CPPREF DEPTH: Algorithm Complexity Guarantees and Exception Safety / CPPREF DERİNLİK:
 * Algoritma Karmaşıklık Garantileri ve İstisna Güvenliği]
 * =============================================================================
 * EN: STL algorithms provide strict worst-case complexity guarantees:
 *     • `std::sort`         — O(n log n) comparisons. Typically introsort
 *       (quicksort + heapsort fallback), guaranteed worst-case O(n log n).
 *     • `std::stable_sort`  — O(n log²n) in-place, or O(n log n) if extra
 *       memory is available (merge sort with buffer).
 *     • `std::nth_element`  — O(n) average (introselect). After the call,
 *       element at nth position is what it would be in a sorted range.
 *     • `std::partial_sort` — O(n log k) where k = middle - first.
 *
 * Exception safety: algorithms use `std::move_if_noexcept` internally.
 * If the element's move constructor is noexcept, it moves; otherwise it
 * copies to maintain the strong exception guarantee. This is why marking
 * move constructors `noexcept` is critical for performance.
 * Algorithms that write to output iterators (e.g., `std::transform`)
 * provide the basic guarantee: no leaks, but output may be partial.
 * `std::for_each` returns its function object — useful for stateful
 * functors accumulating results during traversal.
 *
 * TR: STL algoritmaları katı en-kötü-durum karmaşıklık garantisi sunar:
 *     • `std::sort`         — O(n log n) karşılaştırma. Genelde introsort
 *       (quicksort + heapsort yedek), en kötü durum O(n log n) garantili.
 *     • `std::stable_sort`  — Yerinde O(n log²n), veya ek bellekle
 *       O(n log n) (tamponlu merge sort).
 *     • `std::nth_element`  — ortalama O(n) (introselect). Çağrı sonrası
 *       n. pozisyondaki eleman, sıralı dizideki yerindedir.
 *     • `std::partial_sort` — O(n log k), k = middle - first.
 *
 * İstisna güvenliği: algoritmalar dahili olarak `std::move_if_noexcept`
 * kullanır. Elemanın taşıma yapıcısı noexcept ise taşır; değilse güçlü
 * istisna garantisini korumak için kopyalar. Bu nedenle taşıma
 * yapıcılarını `noexcept` işaretlemek performans için kritiktir.
 * Çıkış iteratörlerine yazan algoritmalar (ör. `std::transform`) temel
 * garanti sunar: sızıntı olmaz, ama çıktı kısmi kalabilir.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_algorithms_masterclass.cpp -o 05_algorithms_masterclass
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>   // EN: STL algorithms
#include <cmath>       // EN: std::abs
#include <functional>  // EN: std::greater, std::multiplies
#include <iostream>    // EN: Standard I/O
#include <iterator>    // EN: ostream_iterator, back_inserter
#include <numeric>     // EN: accumulate, iota, inner_product, partial_sum, reduce
#include <string>      // EN: std::string
#include <vector>      // EN: std::vector

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [PRODUCTION CODE / ÜRETİM KODU]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Automotive sensor reading structure
// TR: Otomotiv sensör okuma yapısı
struct SensorReading {
    std::string name;
    double value;
    // EN: 1=critical, 5=info
    // TR: 1=kritik, 5=bilgi
    int priority;

    friend std::ostream& operator<<(std::ostream& os, const SensorReading& s) {
        return os << s.name << "(" << s.value << ",P" << s.priority << ")";
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [EXAMPLES / ÖRNEKLER]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== MODULE 4: STL ALGORITHMS MASTERCLASS ===\n"
              << "=== MODÜL 4: STL ALGORİTMA USTALIK REHBERİ ===\n\n";

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [SORTING FAMILY / SIRALAMA AİLESİ]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 1. std::sort — Introsort O(N log N) ─────────────────────────────────────────────────────
    {
        std::cout << "--- 1. std::sort ---\n";

        std::vector<int> speeds = {120, 85, 200, 60, 150, 95, 180};

        // EN: Default ascending sort
        // TR: Varsayılan artan sıralama
        std::sort(speeds.begin(), speeds.end());
        std::cout << "Ascending: ";
        for (int s : speeds) std::cout << s << " ";

        // EN: Descending sort with std::greater
        // TR: std::greater ile azalan sıralama
        std::sort(speeds.begin(), speeds.end(), std::greater<int>());
        std::cout << "\nDescending: ";
        for (int s : speeds) std::cout << s << " ";
        std::cout << "\n\n";
    }

    // ─── 2. std::stable_sort — Preserves Equal Order ─────────────────────────────────────────────
    {
        std::cout << "--- 2. std::stable_sort (preserves relative order) ---\n";

        std::vector<SensorReading> alerts = {
            {"OilTemp", 95.0, 2},    {"Coolant", 102.0, 1},
            {"TireFL", 32.0, 3},     {"BrakeFL", 450.0, 1},
            {"Battery", 12.1, 3},    {"FuelLvl", 25.0, 2}};

        // EN: stable_sort by priority — equal priorities keep original order
        // TR: Önceliğe göre stable_sort — eşit öncelikler sırasını korur
        std::stable_sort(alerts.begin(), alerts.end(),
                         [](const SensorReading& a, const SensorReading& b) {
                             return a.priority < b.priority;
                         });

        std::cout << "By priority (stable): ";
        for (const auto& s : alerts) std::cout << s << " ";
        std::cout << "\n\n";
    }

    // ─── 3. std::partial_sort — Top-N ────────────────────────────────────────────────────────────
    {
        std::cout << "--- 3. std::partial_sort (Top-3 fastest) ---\n";

        std::vector<int> lapTimes = {92, 88, 95, 85, 91, 87, 98, 83};

        // EN: Sort only the first 3 elements — rest is unspecified order More efficient than
        // full sort when you only need top-N!
        // TR: Sadece ilk 3 elemanı sırala — geri kalan belirsiz sırada Sadece ilk-N gerektiğinde
        // tam sıralamadan daha verimli!
        std::partial_sort(lapTimes.begin(), lapTimes.begin() + 3,
                          lapTimes.end());

        std::cout << "Top 3 fastest laps: ";
        for (int i = 0; i < 3; ++i) std::cout << lapTimes[static_cast<size_t>(i)] << "s ";
        std::cout << "\n\n";
    }

    // ─── 4. std::nth_element — Selection ─────────────────────────────────────────────────────────
    {
        std::cout << "--- 4. std::nth_element (median finder) ---\n";

        std::vector<int> temps = {72, 88, 65, 91, 78, 85, 70};
        size_t mid = temps.size() / 2;

        // EN: After nth_element: temps[mid] is the median. Elements before it are ≤ it, after
        // are ≥ it (unordered). O(N) average — much faster than O(N log N) full sort!
        // TR: nth_element sonrası: temps[mid] medyan. Öncesindekiler ≤, sonrasındakiler ≥
        // (sırasız). Ortalama O(N) — tam sıralamadan çok daha hızlı!
        std::nth_element(temps.begin(), temps.begin() + static_cast<long>(mid),
                         temps.end());
        std::cout << "Median temperature: " << temps[mid] << "°C\n\n";
    }

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [SEARCHING FAMILY / ARAMA AİLESİ]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 5. std::find / std::find_if ─────────────────────────────────────────────────────────────
    {
        std::cout << "--- 5. std::find / std::find_if ---\n";

        std::vector<int> errorCodes = {0, 0, 301, 0, 420, 0, 171};

        // EN: find first non-zero error code
        // TR: ilk sıfır olmayan hata kodunu bul
        auto it = std::find_if(errorCodes.begin(), errorCodes.end(),
                               [](int code) { return code != 0; });
        if (it != errorCodes.end()) {
            std::cout << "First error: P0" << *it
                      << " at index " << std::distance(errorCodes.begin(), it)
                      << "\n\n";
        }
    }

    // ─── 6. Binary Search Family (SORTED ranges only!) ───────────────────────────────────────────
    {
        std::cout << "--- 6. Binary Search (lower/upper_bound, equal_range) ---\n";

        // EN: MUST be sorted for binary search algorithms!
        // TR: Binary search algoritmaları için SIRALANMIŞ olmalı!
        std::vector<int> sortedRPMs = {1000, 1500, 2000, 2500, 3000,
                                        3000, 3000, 3500, 4000, 5000};

        // EN: lower_bound — first element ≥ value (insertion point)
        // TR: lower_bound — değere ≥ ilk eleman (ekleme noktası)
        auto lo = std::lower_bound(sortedRPMs.begin(), sortedRPMs.end(), 3000);
        std::cout << "lower_bound(3000): index "
                  << std::distance(sortedRPMs.begin(), lo) << "\n";

        // EN: upper_bound — first element > value
        // TR: upper_bound — değerden > ilk eleman
        auto up = std::upper_bound(sortedRPMs.begin(), sortedRPMs.end(), 3000);
        std::cout << "upper_bound(3000): index "
                  << std::distance(sortedRPMs.begin(), up) << "\n";

        // EN: equal_range — returns [lower_bound, upper_bound) pair Count of 3000s = upper -
        // lower
        // TR: equal_range — [lower_bound, upper_bound) çifti döndürür
        auto [eqLo, eqUp] = std::equal_range(sortedRPMs.begin(),
                                               sortedRPMs.end(), 3000);
        std::cout << "Count of 3000 RPM readings: "
                  << std::distance(eqLo, eqUp) << "\n";

        // EN: binary_search — just returns true/false (exists or not)
        // TR: binary_search — sadece true/false döndürür (var mı yok mu)
        bool has4500 = std::binary_search(sortedRPMs.begin(),
                                           sortedRPMs.end(), 4500);
        std::cout << "Has 4500 RPM? " << (has4500 ? "yes" : "no") << "\n\n";
    }

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [MODIFYING ALGORITHMS / DEĞİŞTİRİCİ ALGORİTMALAR]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 7. std::transform — Map Operation ───────────────────────────────────────────────────────
    {
        std::cout << "--- 7. std::transform (map operation) ---\n";

        std::vector<double> celsiusReadings = {22.5, 18.0, 35.2, 0.0, -5.0};
        std::vector<double> fahrenheit;

        // EN: Convert Celsius → Fahrenheit using transform
        // TR: Transform ile Celsius → Fahrenheit dönüşümü
        std::transform(celsiusReadings.begin(), celsiusReadings.end(),
                       std::back_inserter(fahrenheit),
                       [](double c) { return c * 9.0 / 5.0 + 32.0; });

        std::cout << "Fahrenheit: ";
        for (double f : fahrenheit) std::cout << f << " ";
        std::cout << "\n\n";
    }

    // ─── 8. std::remove_if + erase (Erase-Remove Idiom) ──────────────────────────────────────────
    {
        std::cout << "--- 8. Erase-Remove Idiom ---\n";

        std::vector<int> readings = {85, -1, 72, -1, 91, 68, -1, 77};

        // EN: remove_if moves "removed" elements to the end, returns new logical end. You MUST
        // call erase() to actually shrink! -1 represents invalid/error readings.
        // TR: remove_if "silinen" elemanları sona taşır, yeni mantıksal sonu döndürür. Gerçekten
        // küçültmek için erase() ÇAĞIRMALISIN!
        auto newEnd = std::remove_if(readings.begin(), readings.end(),
                                      [](int r) { return r < 0; });
        readings.erase(newEnd, readings.end());

        std::cout << "After removing invalid (-1): ";
        for (int r : readings) std::cout << r << " ";
        std::cout << " (size=" << readings.size() << ")\n\n";
    }

    // ─── 9. std::unique — Remove Consecutive Duplicates ──────────────────────────────────────────
    {
        std::cout << "--- 9. std::unique (consecutive duplicates) ---\n";

        std::vector<int> canFrames = {100, 100, 200, 200, 200, 300, 100, 100};

        // EN: unique only removes CONSECUTIVE duplicates — sort first for all!
        // TR: unique sadece ARDIŞIK tekrarları siler — hepsi için önce sırala!
        auto it = std::unique(canFrames.begin(), canFrames.end());
        canFrames.erase(it, canFrames.end());

        std::cout << "After unique: ";
        for (int f : canFrames) std::cout << f << " ";
        std::cout << "\n\n";
    }

    // ─── 10. std::rotate — Circular Shift ────────────────────────────────────────────────────────
    {
        std::cout << "--- 10. std::rotate ---\n";

        std::vector<int> buffer = {1, 2, 3, 4, 5, 6, 7, 8};

        // EN: Rotate left by 3 positions: element at position 3 becomes first
        // TR: 3 pozisyon sola döndür: 3. pozisyondaki eleman ilk olur
        std::rotate(buffer.begin(), buffer.begin() + 3, buffer.end());

        std::cout << "After rotate left 3: ";
        for (int b : buffer) std::cout << b << " ";
        std::cout << "\n\n";
    }

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [PARTITIONING / BÖLÜMLEME]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 11. std::partition / std::stable_partition ──────────────────────────────────────────────
    {
        std::cout << "--- 11. std::partition ---\n";

        std::vector<SensorReading> data = {
            {"OilTemp", 95.0, 2},  {"Coolant", 110.0, 1},
            {"TireFL", 32.0, 3},   {"BrakeFL", 500.0, 1},
            {"Battery", 12.1, 3},  {"FuelLvl", 15.0, 2}};

        // EN: Partition into critical (priority ≤ 2) and non-critical
        // TR: Kritik (öncelik ≤ 2) ve kritik olmayan olarak böl
        auto boundary = std::stable_partition(
            data.begin(), data.end(),
            [](const SensorReading& s) { return s.priority <= 2; });

        std::cout << "Critical alerts: ";
        for (auto it = data.begin(); it != boundary; ++it) {
            std::cout << it->name << " ";
        }
        std::cout << "\nInfo alerts: ";
        for (auto it = boundary; it != data.end(); ++it) {
            std::cout << it->name << " ";
        }
        std::cout << "\n\n";
    }

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [NUMERIC ALGORITHMS / SAYISAL ALGORİTMALAR]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 12. std::accumulate, std::reduce (C++17) ────────────────────────────────────────────────
    {
        std::cout << "--- 12. std::accumulate & std::reduce ---\n";

        std::vector<double> fuelConsumption = {7.2, 8.1, 6.5, 9.0, 7.8};

        // EN: accumulate — sequential fold (guarantees left-to-right)
        // TR: accumulate — sıralı katlama (soldan sağa garanti)
        double totalFuel = std::accumulate(fuelConsumption.begin(),
                                           fuelConsumption.end(), 0.0);
        std::cout << "Total fuel: " << totalFuel << " L\n";
        std::cout << "Average: " << totalFuel / static_cast<double>(fuelConsumption.size())
                  << " L/100km\n";

        // EN: reduce (C++17) — may reorder operations (parallelizable!) For floating point,
        // reduce may give slightly different results due to reordering. For integers, identical
        // to accumulate.
        // TR: reduce (C++17) — işlemleri yeniden sıralayabilir (paralelleştirilebilir!)
        double totalReduce = std::reduce(fuelConsumption.begin(),
                                          fuelConsumption.end(), 0.0);
        std::cout << "reduce total: " << totalReduce << " L\n\n";
    }

    // ─── 13. std::inner_product — Dot Product ────────────────────────────────────────────────────
    {
        std::cout << "--- 13. std::inner_product ---\n";

        // EN: Weighted sensor scores: score = Σ(value × weight)
        // TR: Ağırlıklı sensör skorları: skor = Σ(değer × ağırlık)
        std::vector<double> values = {95.0, 78.0, 88.0, 92.0};
        std::vector<double> weights = {0.3, 0.2, 0.25, 0.25};

        double weightedScore = std::inner_product(
            values.begin(), values.end(), weights.begin(), 0.0);
        std::cout << "Weighted health score: " << weightedScore << "\n\n";
    }

    // ─── 14. std::partial_sum — Running Total ────────────────────────────────────────────────────
    {
        std::cout << "--- 14. std::partial_sum (cumulative) ---\n";

        std::vector<int> dailyKm = {120, 85, 200, 150, 95};
        std::vector<int> cumulative;

        // EN: Cumulative distance: [120, 205, 405, 555, 650]
        // TR: Kümülatif mesafe: [120, 205, 405, 555, 650]
        std::partial_sum(dailyKm.begin(), dailyKm.end(),
                         std::back_inserter(cumulative));

        std::cout << "Daily km: ";
        for (int d : dailyKm) std::cout << d << " ";
        std::cout << "\nCumulative: ";
        for (int c : cumulative) std::cout << c << " ";
        std::cout << "\n\n";
    }

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [PREDICATES & CONDITIONS / KOŞULLAR]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 15. all_of / any_of / none_of ───────────────────────────────────────────────────────────
    {
        std::cout << "--- 15. all_of / any_of / none_of ---\n";

        std::vector<int> tirePressures = {32, 33, 31, 34};

        bool allSafe = std::all_of(tirePressures.begin(), tirePressures.end(),
                                    [](int p) { return p >= 28 && p <= 36; });
        bool anyLow = std::any_of(tirePressures.begin(), tirePressures.end(),
                                   [](int p) { return p < 28; });
        bool noneFlat = std::none_of(tirePressures.begin(), tirePressures.end(),
                                      [](int p) { return p == 0; });

        std::cout << "All tires safe (28-36 PSI)? " << (allSafe ? "yes" : "no") << "\n";
        std::cout << "Any tire low (<28 PSI)? " << (anyLow ? "yes" : "no") << "\n";
        std::cout << "No flat tires? " << (noneFlat ? "yes" : "no") << "\n\n";
    }

    // ─── 16. std::count / std::count_if ──────────────────────────────────────────────────────────
    {
        std::cout << "--- 16. std::count_if ---\n";

        std::vector<int> speedLog = {60, 85, 120, 130, 95, 140, 110, 70};
        int speedLimit = 120;

        auto violations = std::count_if(
            speedLog.begin(), speedLog.end(),
            [speedLimit](int s) { return s > speedLimit; });
        std::cout << "Speed violations (>" << speedLimit << " km/h): "
                  << violations << "\n\n";
    }

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [MIN/MAX / MİN/MAKS]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 17. min_element / max_element / minmax_element / clamp ──────────────────────────────────
    {
        std::cout << "--- 17. min/max/clamp ---\n";

        std::vector<double> voltages = {12.4, 11.8, 13.2, 12.9, 11.5, 13.8};

        auto [minIt, maxIt] = std::minmax_element(voltages.begin(),
                                                    voltages.end());
        std::cout << "Min voltage: " << *minIt << "V\n";
        std::cout << "Max voltage: " << *maxIt << "V\n";

        // EN: clamp (C++17) — restrict a value to [lo, hi] range
        // TR: clamp (C++17) — değeri [lo, hi] aralığına kısıtla
        double rawThrottle = 1.15;
        double clamped = std::clamp(rawThrottle, 0.0, 1.0);
        std::cout << "Throttle " << rawThrottle << " clamped to [0,1]: "
                  << clamped << "\n\n";
    }

    // ─── 18. std::for_each with Side Effects ─────────────────────────────────────────────────────
    {
        std::cout << "--- 18. std::for_each ---\n";

        std::vector<std::string> modules = {"ECM", "TCM", "BCM", "ABS"};

        int index = 0;
        std::for_each(modules.begin(), modules.end(),
                      [&index](const std::string& m) {
                          std::cout << "  [" << index++ << "] " << m << "\n";
                      });
        std::cout << "\n";
    }

    // ─── 19. std::generate — Fill with Computed Values ───────────────────────────────────────────
    {
        std::cout << "--- 19. std::generate ---\n";

        std::vector<int> sensorIDs(8);
        int nextID = 1000;

        // EN: Generate sequential sensor IDs starting from 1000
        // TR: 1000'den başlayarak sıralı sensör ID'leri üret
        std::generate(sensorIDs.begin(), sensorIDs.end(),
                      [&nextID]() { return nextID++; });

        std::cout << "Generated IDs: ";
        for (int id : sensorIDs) std::cout << id << " ";
        std::cout << "\n\n";
    }

    // ─── 20. std::mismatch — Find First Difference ───────────────────────────────────────────────
    {
        std::cout << "--- 20. std::mismatch ---\n";

        std::vector<int> expected = {100, 200, 300, 400, 500};
        std::vector<int> actual   = {100, 200, 350, 400, 500};

        // EN: Find where calibration data diverges from expected values
        // TR: Kalibrasyon verilerinin beklenen değerlerden nerede saptığını bul
        auto [expIt, actIt] = std::mismatch(expected.begin(), expected.end(),
                                             actual.begin());
        if (expIt != expected.end()) {
            std::cout << "First mismatch at index "
                      << std::distance(expected.begin(), expIt)
                      << ": expected=" << *expIt
                      << " actual=" << *actIt << "\n";
        }
    }

    std::cout << "\n=== ALGORITHMS MASTERCLASS COMPLETE ===\n";
    return 0;
}
