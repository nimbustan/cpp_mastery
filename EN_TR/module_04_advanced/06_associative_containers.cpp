/**
 * @file module_04_advanced/06_associative_containers.cpp
 * @brief Templates & STL: Associative & Unordered Containers — Şablonlar ve STL: İlişkisel ve
 * Sırasız Konteynerler
 *
 * Konu: map, multimap, set, multiset, unordered variants, custom comparators, custom hash
 * functions, container adaptors (stack, queue, priority_queue) map, multimap, set, multiset,
 * sırasız çeşitler, özel karşılaştırıcılar, özel hash fonksiyonları, konteyner adaptörleri
 * (stack, queue, priority_queue)
 *
 * Standart: C++17 Derleme: g++ -std=c++17 -Wall -Wextra -Wpedantic 06_associative_containers.cpp
 *
 * @details
 * =====================================================================
 * [THEORY: Ordered Containers — Red-Black Tree / TEORİ: Sıralı Konteynerler — Kırmızı-Siyah
 * Ağaç]
 * =====================================================================
 *
 * EN: std::set, std::map, std::multiset, std::multimap Backed by Red-Black Tree (self-balancing
 * BST).
 * ✓ Always sorted by key
 * ✓ O(log N) insert, find, erase
 * ✓ Supports ordered traversal, lower_bound, upper_bound
 * ✗ Slower than hash for pure lookup
 *
 * TR: std::set, std::map, std::multiset, std::multimap Kırmızı-Siyah Ağaç (kendi kendini
 * dengeleyen BST) ile desteklenir.
 * ✓ Her zaman anahtara göre sıralı
 * ✓ O(log N) ekleme, bulma, silme
 * ✓ Sıralı gezinme, lower_bound, upper_bound destekler
 * ✗ Saf arama için hash'ten yavaş
 *
 * =====================================================================
 * [THEORY: Unordered Containers — Hash Table / TEORİ: Sırasız Konteynerler — Hash Tablosu]
 * =====================================================================
 *
 * EN: std::unordered_set, std::unordered_map, std::unordered_multiset/map Backed by Hash Table
 * (bucket array + linked lists).
 * ✓ O(1) average insert, find, erase
 * ✓ Fastest for pure key lookup
 * ✗ No ordering guarantee
 * ✗ Worst case O(N) if many hash collisions
 * ✗ Requires hash function + equality operator
 *
 * TR: std::unordered_set, std::unordered_map, std::unordered_multiset/map Hash Tablosu (kova
 * dizisi + bağlı listeler) ile desteklenir.
 * ✓ Ortalama O(1) ekleme, bulma, silme
 * ✓ Saf anahtar araması için en hızlı
 * ✗ Sıralama garantisi yok
 * ✗ Çok çarpışma varsa en kötü O(N)
 * ✗ Hash fonksiyonu + eşitlik operatörü gerektirir
 *
 * =====================================================================
 * [THEORY: Container Adaptors / TEORİ: Konteyner Adaptörleri]
 * =====================================================================
 *
 * EN: std::stack (LIFO), std::queue (FIFO), std::priority_queue (max-heap). These are NOT real
 * containers — they WRAP an underlying container (default: deque for stack/queue, vector for
 * priority_queue) and restrict its interface.
 *
 * TR: std::stack (LIFO), std::queue (FIFO), std::priority_queue (max-heap). Bunlar gerçek
 * konteyner DEĞİL — alttaki bir konteyneri SARARLAR (varsayılan: stack/queue için deque,
 * priority_queue için vector) ve arayüzünü kısıtlarlar.
 * =====================================================================
 *
 * [CPPREF DEPTH: Red-Black Trees vs Hash Tables — When Order Matters / CPPREF DERİNLİK:
 * Kırmızı-Siyah Ağaç vs Hash Tablosu — Sıra Ne Zaman Önemli]
 * =============================================================================
 * EN: `std::map` / `std::set` are red-black trees (self-balancing BST):
 * • All operations O(log n) — insert, find, erase.
 * • Iteration yields elements in SORTED order by key.
 * • Iterators are stable: insert/erase never invalidates other iterators.
 * `std::unordered_map` / `std::unordered_set` use hash tables:
 * • Average O(1) for insert/find/erase, worst-case O(n) (all collide).
 * • No ordering guarantee — iteration order is implementation-defined.
 * • Load factor = size / bucket_count. When load_factor > max_load_factor (default 1.0), a
 *   REHASH occurs: new bucket array allocated, all elements re-hashed — O(n) and ALL
 *   iterators are invalidated!
 * Custom hash: specialize `std::hash<T>` or pass as template argument:
 * `std::unordered_map<K, V, MyHash, MyEqual>` A good hash distributes uniformly. Combine
 * fields with: `seed ^= std::hash<T>{}(val) + 0x9e3779b9 + (seed<<6) + (seed>>2);` C++17
 * `try_emplace()` avoids constructing the value if key exists — more efficient than
 * `emplace()` for expensive-to-construct values.
 *
 * TR: `std::map` / `std::set` kırmızı-siyah ağaçtır (dengeli BST):
 * • Tüm işlemler O(log n) — ekleme, arama, silme.
 * • İterasyon elemanları anahtara göre SIRALI verir.
 * • İteratörler kararlıdır: ekleme/silme diğer iteratörleri bozmaz.
 * `std::unordered_map` / `std::unordered_set` hash tablosu kullanır:
 * • Ortalama O(1), en kötü O(n) (tüm anahtarlar çakışırsa).
 * • Sıralama garantisi YOKTUR — gezinme sırası uygulama tanımlıdır.
 * • Yük faktörü = size / bucket_count. max_load_factor (varsayılan 1.0) aşılınca REHASH olur:
 *   yeni kova dizisi, tüm elemanlar yeniden hash'lenir — O(n) ve TÜM iteratörler geçersiz
 *   olur!
 * Özel hash: `std::hash<T>` özelleştirmesi veya şablon argümanı: `std::unordered_map<K, V,
 * MyHash, MyEqual>` İyi bir hash fonksiyonu düzgün dağılım sağlar. Alanları birleştirmek
 * için: `seed ^= hash(val) + 0x9e3779b9 + (seed<<6) + (seed>>2);` C++17 `try_emplace()`
 * anahtar varsa değer oluşturmaktan kaçınır — pahalı nesnelerde `emplace()`'ten daha
 * verimlidir.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_associative_containers.cpp -o 06_associative_containers
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>       // EN: STL algorithms
#include <functional>      // EN: std::greater
#include <iostream>        // EN: Standard I/O
#include <map>             // EN: std::map, std::multimap
#include <queue>           // EN: std::queue, std::priority_queue
#include <set>             // EN: std::set, std::multiset
#include <stack>           // EN: std::stack
#include <string>          // EN: std::string
#include <unordered_map>   // EN: std::unordered_map
#include <unordered_set>   // EN: std::unordered_set
#include <vector>          // EN: std::vector

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [PRODUCTION CODE / ÜRETİM KODU]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 1. Custom Comparator for std::set ───────────────────────────────────────────────────────────

// EN: Diagnostic Trouble Code (DTC) with severity and timestamp.
// TR: Arıza Kodu (DTC) — şiddet derecesi ve zaman damgası ile.
struct DTC {
    std::string code;     // EN: e.g. "P0301"
    int severity;         // EN: 1=critical..5=info
    int timestampMs;      // EN: milliseconds since boot

    friend std::ostream& operator<<(std::ostream& os, const DTC& d) {
        return os << d.code << "(sev=" << d.severity
                  << ",t=" << d.timestampMs << "ms)";
    }
};

// EN: Custom comparator: sort DTCs by severity first, then by time. This is a FUNCTOR — std::set
// stores it internally.
// TR: Özel karşılaştırıcı: DTC'leri önce şiddete, sonra zamana göre sırala. Bu bir FUNCTOR —
// std::set bunu dahili olarak saklar.
struct DTCSeverityOrder {
    bool operator()(const DTC& a, const DTC& b) const {
        if (a.severity != b.severity) return a.severity < b.severity;
        return a.timestampMs < b.timestampMs;
    }
};

// ─── 2. Custom Hash for unordered_map ────────────────────────────────────────────────────────────

// EN: Sensor key: ECU ID + Sensor Index. We need a custom hash because std::pair doesn't have a
// default hash in C++.
// TR: Sensör anahtarı: ECU ID + Sensör İndeksi. std::pair'in C++'ta varsayılan hash'i olmadığı
// için özel hash gerekir.
struct SensorKey {
    int ecuId;
    int sensorIndex;

    bool operator==(const SensorKey& other) const {
        return ecuId == other.ecuId && sensorIndex == other.sensorIndex;
    }
};

struct SensorKeyHash {
    size_t operator()(const SensorKey& k) const {
        // EN: Combine two hashes using XOR + bit shift (a common technique)
        // TR: İki hash'i XOR + bit kaydırma ile birleştir (yaygın teknik)
        size_t h1 = std::hash<int>{}(k.ecuId);
        size_t h2 = std::hash<int>{}(k.sensorIndex);
        return h1 ^ (h2 << 1);
    }
};

// ─── 3. Priority Queue for Alert Processing ──────────────────────────────────────────────────────

// EN: Vehicle alert with priority level.
// TR: Öncelik seviyeli araç uyarısı.
struct VehicleAlert {
    std::string message;
    int priority;  // EN: lower = more urgent

    // EN: priority_queue is max-heap by default, so we invert: higher priority number = LESS
    // urgent → goes to bottom
    // TR: priority_queue varsayılan max-heap, bu yüzden ters çeviriyoruz: yüksek öncelik
    // numarası = DAHA AZ acil → alta gider
    bool operator<(const VehicleAlert& other) const {
        return priority > other.priority;  // min-heap behavior
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [EXAMPLES / ÖRNEKLER]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== MODULE 4: ASSOCIATIVE & UNORDERED CONTAINERS ===\n"
              << "=== MODÜL 4: İLİŞKİSEL VE SIRASIZ KONTEYNERLER ===\n\n";

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [ORDERED CONTAINERS / SIRALI KONTEYNERLER]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 1. std::map — Sorted Key-Value ──────────────────────────────────────────────────────────
    {
        std::cout << "--- 1. std::map (sorted key-value) ---\n";

        std::map<std::string, double> sensorData;

        // EN: Insert via [] operator (creates if not exists)
        // TR: [] operatörü ile ekle (yoksa oluşturur)
        sensorData["OilTemp"] = 92.5;
        sensorData["Coolant"] = 88.0;
        sensorData["Battery"] = 12.6;

        // EN: Insert via insert() — does NOT overwrite existing!
        // TR: insert() ile ekle — mevcut olanın üzerine YAZMAZ!
        auto [it1, inserted1] = sensorData.insert({"OilTemp", 999.0});
        std::cout << "OilTemp insert: " << (inserted1 ? "new" : "exists")
                  << " val=" << it1->second << "\n";

        // EN: insert_or_assign() (C++17) — inserts or overwrites
        // TR: insert_or_assign() (C++17) — ekler veya üzerine yazar
        sensorData.insert_or_assign("OilTemp", 95.0);

        // EN: Traversal is ALWAYS sorted by key (alphabetical for string)
        // TR: Gezinme HER ZAMAN anahtara göre sıralı (string için alfabetik)
        std::cout << "All sensors (sorted):\n";
        for (const auto& [key, val] : sensorData) {
            std::cout << "  " << key << " = " << val << "\n";
        }

        // EN: find() returns iterator — O(log N)
        // TR: find() iterator döndürür — O(log N)
        auto found = sensorData.find("Coolant");
        if (found != sensorData.end()) {
            std::cout << "Found: " << found->first << " = " << found->second << "\n";
        }

        // EN: count() — 0 or 1 for map (useful for existence check)
        // TR: count() — map için 0 veya 1 (varlık kontrolü için kullanışlı)
        std::cout << "Has 'Turbo'? " << sensorData.count("Turbo") << "\n\n";
    }

    // ─── 2. std::multimap — Multiple Values Per Key ──────────────────────────────────────────────
    {
        std::cout << "--- 2. std::multimap (multiple values per key) ---\n";

        std::multimap<int, std::string> errorsByHour;

        // EN: Same key (hour) can have multiple error entries
        // TR: Aynı anahtar (saat) birden fazla hata girişine sahip olabilir
        errorsByHour.insert({9, "P0301 — Cylinder misfire"});
        errorsByHour.insert({9, "P0420 — Catalyst efficiency"});
        errorsByHour.insert({10, "P0171 — Lean mixture"});
        errorsByHour.insert({9, "C0035 — ABS sensor"});

        // EN: equal_range returns [first, last) for a given key
        // TR: equal_range verilen anahtar için [ilk, son) döndürür
        auto [lo, hi] = errorsByHour.equal_range(9);
        std::cout << "Errors at hour 9:\n";
        for (auto it = lo; it != hi; ++it) {
            std::cout << "  " << it->second << "\n";
        }
        std::cout << "Total at hour 9: " << errorsByHour.count(9) << "\n\n";
    }

    // ─── 3. std::set with Custom Comparator ──────────────────────────────────────────────────────
    {
        std::cout << "--- 3. std::set<DTC, DTCSeverityOrder> ---\n";

        std::set<DTC, DTCSeverityOrder> dtcQueue;

        dtcQueue.insert({"P0301", 3, 1500});
        dtcQueue.insert({"P0420", 2, 800});
        dtcQueue.insert({"C0035", 1, 200});
        dtcQueue.insert({"B0100", 4, 3000});
        dtcQueue.insert({"P0171", 1, 1200});

        // EN: Iteration is in severity order (1=critical first)
        // TR: Gezinme şiddet sırasında (1=kritik ilk)
        std::cout << "DTCs by severity:\n";
        for (const auto& dtc : dtcQueue) {
            std::cout << "  " << dtc << "\n";
        }
        std::cout << "\n";
    }

    // ─── 4. std::set — Set Operations ────────────────────────────────────────────────────────────
    {
        std::cout << "--- 4. Set Operations (union, intersection, diff) ---\n";

        std::set<std::string> mondayDTCs = {"P0301", "P0420", "C0035", "P0171"};
        std::set<std::string> tuesdayDTCs = {"P0420", "B0100", "P0171", "U0100"};

        // EN: set_intersection — DTCs that appeared BOTH days
        // TR: set_intersection — HER İKİ gün görülen DTC'ler
        std::vector<std::string> persistent;
        std::set_intersection(mondayDTCs.begin(), mondayDTCs.end(),
                              tuesdayDTCs.begin(), tuesdayDTCs.end(),
                              std::back_inserter(persistent));
        std::cout << "Persistent DTCs: ";
        for (const auto& d : persistent) std::cout << d << " ";

        // EN: set_difference — Monday only (cleared by Tuesday)
        // TR: set_difference — Sadece Pazartesi (Salı'ya kadar temizlendi)
        std::vector<std::string> clearedDTCs;
        std::set_difference(mondayDTCs.begin(), mondayDTCs.end(),
                            tuesdayDTCs.begin(), tuesdayDTCs.end(),
                            std::back_inserter(clearedDTCs));
        std::cout << "\nCleared DTCs: ";
        for (const auto& d : clearedDTCs) std::cout << d << " ";

        // EN: set_union — all unique DTCs across both days
        // TR: set_union — iki gündeki tüm benzersiz DTC'ler
        std::set<std::string> allDTCs;
        std::set_union(mondayDTCs.begin(), mondayDTCs.end(),
                       tuesdayDTCs.begin(), tuesdayDTCs.end(),
                       std::inserter(allDTCs, allDTCs.begin()));
        std::cout << "\nAll unique DTCs: ";
        for (const auto& d : allDTCs) std::cout << d << " ";
        std::cout << "\n\n";
    }

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [UNORDERED CONTAINERS / SIRASIZ KONTEYNERLER]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 5. std::unordered_map — O(1) Lookup ─────────────────────────────────────────────────────
    {
        std::cout << "--- 5. std::unordered_map (hash table) ---\n";

        std::unordered_map<std::string, int> ecuModules;
        // EN: Engine Control Module address
        // TR: Motor Kontrol Modülü adresi
        ecuModules["ECM"] = 0x7E0;
        // EN: Transmission Control Module
        // TR: Şanzıman Kontrol Modülü
        ecuModules["TCM"] = 0x7E1;
        // EN: Anti-lock Braking System
        // TR: Kilitlenme Önleyici Fren Sistemi
        ecuModules["ABS"] = 0x7B0;
        // EN: Body Control Module
        // TR: Gövde Kontrol Modülü
        ecuModules["BCM"] = 0x740;

        // EN: O(1) average lookup — much faster than map for large datasets
        // TR: Ortalama O(1) arama — büyük veri setleri için map'ten çok daha hızlı
        std::cout << "ECM address: 0x" << std::hex << ecuModules["ECM"]
                  << std::dec << "\n";

        // EN: Bucket information — shows hash table internals
        // TR: Kova bilgisi — hash tablosu iç yapısını gösterir
        std::cout << "Buckets: " << ecuModules.bucket_count()
                  << ", Load factor: " << ecuModules.load_factor()
                  << "\n\n";
    }

    // ─── 6. unordered_map with Custom Hash ───────────────────────────────────────────────────────
    {
        std::cout << "--- 6. unordered_map with Custom Hash ---\n";

        std::unordered_map<SensorKey, double, SensorKeyHash> sensorMap;

        // EN: Key = {ECU_ID, SensorIndex}, Value = reading
        // TR: Anahtar = {ECU_ID, SensörIndeks}, Değer = okuma
        sensorMap[{0x7E0, 0}] = 92.5;   // ECM, OilTemp
        sensorMap[{0x7E0, 1}] = 88.0;   // ECM, Coolant
        sensorMap[{0x7B0, 0}] = 32.0;   // ABS, WheelSpeedFL
        sensorMap[{0x7B0, 1}] = 33.0;   // ABS, WheelSpeedFR

        SensorKey query{0x7B0, 0};
        auto it = sensorMap.find(query);
        if (it != sensorMap.end()) {
            std::cout << "ABS WheelSpeedFL: " << it->second << "\n\n";
        }
    }

    // ─── 7. std::unordered_set — Fast Membership Test ────────────────────────────────────────────
    {
        std::cout << "--- 7. std::unordered_set (fast contains) ---\n";

        // EN: Blacklisted VIN prefixes (known recall vehicles)
        // TR: Kara listedeki VIN ön ekleri (bilinen geri çağırma araçları)
        std::unordered_set<std::string> recallVINs = {
            "1G1YY", "2T1BU", "3FA6P", "5YJ3E", "JN1TB"};

        std::string testVIN = "3FA6P";
        // EN: O(1) average — contains() is C++20, use count() for C++17
        // TR: Ortalama O(1) — contains() C++20, C++17 için count() kullan
        if (recallVINs.count(testVIN) > 0) {
            std::cout << "⚠ VIN " << testVIN << " is in recall list!\n\n";
        }
    }

    // ═════════════════════════════════════════════════════════════════════════════════════════════
    // [CONTAINER ADAPTORS / KONTEYNER ADAPTÖRLERİ]
    // ═════════════════════════════════════════════════════════════════════════════════════════════

    // ─── 8. std::stack — LIFO (Undo History) ─────────────────────────────────────────────────────
    {
        std::cout << "--- 8. std::stack (LIFO — undo history) ---\n";

        std::stack<std::string> undoHistory;

        // EN: Each user action is pushed onto the stack
        // TR: Her kullanıcı aksiyonu yığına itilir
        undoHistory.push("Set RPM limit to 6000");
        undoHistory.push("Enable traction control");
        undoHistory.push("Set drive mode to Sport");

        std::cout << "Undo sequence:\n";
        while (!undoHistory.empty()) {
            std::cout << "  Undo: " << undoHistory.top() << "\n";
            undoHistory.pop();
        }
        std::cout << "\n";
    }

    // ─── 9. std::queue — FIFO (Message Queue) ────────────────────────────────────────────────────
    {
        std::cout << "--- 9. std::queue (FIFO — message processing) ---\n";

        std::queue<std::string> canBusQueue;

        // EN: CAN bus messages arrive in order and must be processed FIFO
        // TR: CAN bus mesajları sırayla gelir ve FIFO işlenmelidir
        canBusQueue.push("0x7E0: Engine RPM = 3200");
        canBusQueue.push("0x7B0: Wheel Speed = 85");
        canBusQueue.push("0x740: Door Status = Closed");

        std::cout << "Processing CAN messages:\n";
        while (!canBusQueue.empty()) {
            std::cout << "  → " << canBusQueue.front() << "\n";
            canBusQueue.pop();
        }
        std::cout << "\n";
    }

    // ─── 10. std::priority_queue — Alert Dispatcher ──────────────────────────────────────────────
    {
        std::cout << "--- 10. std::priority_queue (alert dispatcher) ---\n";

        std::priority_queue<VehicleAlert> alertQueue;

        // EN: Push alerts with different priorities
        // TR: Farklı önceliklerle uyarılar ekle
        alertQueue.push({"Low washer fluid", 5});
        alertQueue.push({"ENGINE OVERHEATING!", 1});
        alertQueue.push({"Tire pressure low", 3});
        alertQueue.push({"BRAKE FAILURE!", 1});
        alertQueue.push({"Service due in 500 km", 4});

        // EN: Dequeue processes the most urgent (lowest number) first!
        // TR: Kuyruktan çıkarma en acil olanı (en düşük numara) ilk işler!
        std::cout << "Alert processing order:\n";
        while (!alertQueue.empty()) {
            const auto& a = alertQueue.top();
            std::cout << "  [P" << a.priority << "] " << a.message << "\n";
            alertQueue.pop();
        }
        std::cout << "\n";
    }

    // ─── 11. std::map — Structured Bindings & try_emplace ────────────────────────────────────────
    {
        std::cout << "--- 11. C++17: try_emplace & extract ---\n";

        std::map<int, std::string> ecuRegistry;
        ecuRegistry[0x7E0] = "ECM";
        ecuRegistry[0x7B0] = "ABS";

        // EN: try_emplace (C++17) — only constructs value if key doesn't exist More efficient
        // than insert for expensive-to-construct values
        // TR: try_emplace (C++17) — anahtar yoksa değeri oluşturur Pahalı oluşturma maliyetli
        // değerler için insert'ten verimli
        auto [it, ok] = ecuRegistry.try_emplace(0x7E0, "DUPLICATE");
        std::cout << "try_emplace 0x7E0: " << (ok ? "inserted" : "exists")
                  << " -> " << it->second << "\n";

        // EN: extract (C++17) — remove a node without destroying it Can be re-inserted with a
        // different key!
        // TR: extract (C++17) — düğümü yok etmeden çıkar Farklı bir anahtarla tekrar
        // eklenebilir!
        auto node = ecuRegistry.extract(0x7B0);
        if (!node.empty()) {
            // EN: Change key!
            // TR: Anahtarı değiştir!
            node.key() = 0x7B1;
            ecuRegistry.insert(std::move(node));
        }

        std::cout << "After extract+reinsert:\n";
        for (const auto& [addr, name] : ecuRegistry) {
            std::cout << "  0x" << std::hex << addr << std::dec
                      << " → " << name << "\n";
        }
    }

    std::cout << "\n=== ASSOCIATIVE CONTAINERS COMPLETE ===\n";
    return 0;
}
