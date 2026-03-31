/**
 * @file module_04_advanced/04_iterators_and_adaptors.cpp
 * @brief Templates & STL: Iterators, Categories and Adaptors
 *
 *
 *
 * @details
 * =====================================================================
 * [THEORY: Iterator Categories — The 5 Ranks of STL Access]
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
 *
 * =====================================================================
 * [HIERARCHY]
 * =====================================================================
 *
 * EN: Random Access ⊃ Bidirectional ⊃ Forward ⊃ Input ⊃ Output
 * Each higher category includes all capabilities of those below it.
 * std::sort    → Requires Random Access
 * std::find    → Input is sufficient
 * std::reverse → Requires Bidirectional
 *
 *
 * =====================================================================
 *
 * [CPPREF DEPTH: Iterator Categories and Their Guarantees]
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
// [PRODUCTION CODE]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 1. Demonstrating Iterator Categories ────────────────────────────────────────────────────────

// EN: A function that REQUIRES Random Access Iterators (like std::sort).
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
// kaydedicileri).
class SensorRange {
   public:
    // EN: Forward Iterator implementation
    class Iterator {
       public:
        // EN: Required type aliases for std::iterator_traits
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
// [EXAMPLES]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== MODULE 4: ITERATORS & ADAPTORS ===\n\n";

    // ─── 1. Iterator Categories in Action ────────────────────────────────────────────────────────
    {
        std::cout << "--- 1. Iterator Categories ---\n";

        std::vector<int> speeds = {120, 85, 200, 60, 150};

        // EN: vector iterators are Random Access — supports std::sort
        std::cout << "Sorted speeds: ";
        printSorted(speeds.begin(), speeds.end());

        // EN: sumRange works with ANY iterator (Input is enough)
        std::cout << "Sum of speeds: " << sumRange(speeds.begin(), speeds.end()) << "\n";

        // EN: list iterators are Bidirectional — can reverse but NOT random access
        std::list<int> rpms = {3000, 1500, 4500, 2200};
        // EN: list has its own sort (merge sort) — doesn't need Random Access
        rpms.sort();
        std::cout << "Sorted RPMs (list::sort): ";
        for (const auto& r : rpms) std::cout << r << " ";
        std::cout << "\n";

        // EN: forward_list is Forward only — single direction, no size()
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
        SensorRange sensors(0x100, 0x120);

        std::cout << "Sensor addresses: ";
        for (int addr : sensors) {
            std::cout << "0x" << std::hex << addr << " ";
        }
        std::cout << std::dec << "\n";

        // EN: Works with STL algorithms because it satisfies ForwardIterator
        std::cout << "Count of addresses: "
                  << std::distance(sensors.begin(), sensors.end()) << "\n\n";
    }

    // ─── 3. back_inserter — Dynamic Collection ───────────────────────────────────────────────────
    {
        std::cout << "--- 3. back_inserter ---\n";

        std::vector<int> source = {10, 20, 30, 40, 50};
        std::vector<int> dest;

        // EN: back_inserter creates an Output Iterator that calls push_back
        std::copy(source.begin(), source.end(), std::back_inserter(dest));
        std::cout << "Copied via back_inserter: ";
        for (int v : dest) std::cout << v << " ";
        std::cout << "\n";

        // EN: Transform + back_inserter — double each value into a new vector
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
        std::cout << "ECU modules: ";
        std::copy(ecuNames.begin(), ecuNames.end(),
                  std::ostream_iterator<std::string>(std::cout, ", "));
        std::cout << "\n\n";
    }

    // ─── 6. istream_iterator — Parse from String Stream ──────────────────────────────────────────
    {
        std::cout << "--- 6. istream_iterator (from stringstream) ---\n";

        // EN: Simulate reading space-separated sensor values from a data stream
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
        std::cout << "Fault log (newest first): ";
        for (auto it = faultLog.rbegin(); it != faultLog.rend(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // EN: Find last element > 300 using reverse iterators
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
        auto powertrainDTCs = collectActiveDTCs(allDTCs, "P");

        std::cout << "Powertrain DTCs: ";
        std::copy(powertrainDTCs.begin(), powertrainDTCs.end(),
                  std::ostream_iterator<std::string>(std::cout, " "));
        std::cout << "\n";

        // EN: Filter Body codes (start with 'B')
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
        std::iota(sensorIDs.begin(), sensorIDs.end(), 100);

        // EN: Random Access: jump to 5th element directly
        auto it = sensorIDs.begin() + 4;
        std::cout << "5th sensor ID: " << *it << "\n";

        // EN: Iterator arithmetic: distance between two positions
        auto first = std::find(sensorIDs.begin(), sensorIDs.end(), 103);
        auto last = std::find(sensorIDs.begin(), sensorIDs.end(), 108);
        std::cout << "Distance 103→108: "
                  << std::distance(first, last) << " elements\n";

        // EN: advance() moves an iterator by N steps
        auto walker = sensorIDs.begin();
        std::advance(walker, 7);
        std::cout << "After advance(7): " << *walker << "\n";

        // EN: next()/prev() return a NEW iterator without modifying the original
        auto n = std::next(sensorIDs.begin(), 3);
        auto p = std::prev(sensorIDs.end(), 2);
        std::cout << "next(begin,3)=" << *n << "  prev(end,2)=" << *p << "\n";
    }

    std::cout << "\n=== ITERATORS & ADAPTORS COMPLETE ===\n";
    return 0;
}
