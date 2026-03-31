/**
 * @file module_04_advanced/08_ranges_and_views.cpp
 * @brief C++20 Ranges & Views — Educational C++17 Implementation — C++20 Aralıklar ve Görünümler
 * — Eğitim Amaçlı C++17 Uygulaması
 *
 * @details
 * ===============================================================================
 * [THEORY: C++20 Ranges & Views — The Pipeline Revolution / TEORİ: C++20 Aralıklar ve
 * Görünümler — Boru Hattı Devrimi]
 * ===============================================================================
 *
 * --- 1. What Are Ranges? ---
 * EN: A Range is any type that exposes begin() and end(). C++20 formalises this with concepts
 * like std::ranges::range and std::ranges::random_access_range. Ranges replace the old
 * iterator-pair idiom with a single object that *is* the sequence.
 *
 * --- 2. Views — Lazy, Non-Owning Wrappers ---
 * A View is a lightweight, non-owning wrapper around a range. Views are cheap to copy and
 * never own the underlying data. Key views:
 * • filter(pred)    — keep elements satisfying predicate
 * • transform(fn)   — apply function to each element
 * • take(n)         — first N elements
 * • drop(n)         — skip first N elements
 * • reverse          — reverse iteration order
 *
 * --- 3. Pipe Syntax ---
 * The pipe operator `|` chains views left-to-right, reading like a processing pipeline on a
 * factory floor: sensorData | views::filter(isValid) | views::transform(calibrate) Each stage
 * lazily yields elements to the next — no intermediate containers are created.
 *
 * --- 4. Why Ranges Matter ---
 * • Eliminate temporary containers between algorithm stages
 * • Improve readability — declarative, not imperative
 * • Lazy evaluation — compute only what is consumed
 * • Composable — small pieces snap together like LEGO bricks
 *
 * --- 5. C++17 Workaround Strategy ---
 * Under C++17 we lack <ranges>, so we emulate the pattern: 1. Template helper functions:
 * filterAndTransform<Container, Pred, Fn> 2. Iterator-based LazyRange<T> that defers computation
 * 3. Pipeline builder class that chains stages via std::function All code below compiles with
 * -std=c++17 -Wall -Wextra -Wpedantic.
 *
 * --- 1. Aralık Nedir? ---
 * TR: Bir Aralık (Range), begin() ve end() sunan herhangi bir tiptir. C++20 bunu
 * std::ranges::range ve std::ranges::random_access_range gibi kavramlarla (concepts)
 * resmîleştirir. Aralıklar, eski yineleyici-çifti deyimini, dizinin *kendisi* olan tek bir
 * nesneyle değiştirir.
 *
 * --- 2. Görünümler — Tembel, Sahiplik Almayan Sarmalayıcılar ---
 * Bir Görünüm (View), aralık etrafındaki hafif, sahiplik almayan bir sarmalayıcıdır.
 * Kopyalaması ucuzdur ve alttaki veriye sahip olmaz:
 * • filter(pred)    — koşulu sağlayan elemanları tut
 * • transform(fn)   — her elemana fonksiyon uygula
 * • take(n)         — ilk N eleman
 * • drop(n)         — ilk N elemanı atla
 * • reverse          — ters sırada yinele
 *
 * --- 3. Boru Sözdizimi ---
 * Boru operatörü `|`, görünümleri soldan sağa zincirler; fabrika bandındaki bir işleme hattı
 * gibi okunur: sensorData | views::filter(isValid) | views::transform(calibrate) Her aşama
 * tembel (lazy) olarak bir sonrakine eleman verir — ara konteyner oluşturulmaz.
 *
 * --- 4. Aralıklar Neden Önemli ---
 * • Algoritma aşamaları arasındaki geçici konteynerleri ortadan kaldırır
 * • Okunabilirliği artırır — bildirimsel, zorunlu değil
 * • Tembel değerlendirme — yalnızca tüketilen hesaplanır
 * • Bileşenleştirilebilir — küçük parçalar LEGO gibi birleşir
 *
 * --- 5. C++17 Geçici Çözüm Stratejisi ---
 * C++17'de <ranges> olmadığından, aşağıdaki stratejiyle öykünüyoruz: 1. Şablon yardımcı
 * fonksiyonlar: filterAndTransform<Container, Pred, Fn> 2. Hesaplamayı erteleyen yineleyici
 * tabanlı LazyRange<T> 3. std::function ile aşamaları zincirleyen boru hattı oluşturucu sınıf
 * Aşağıdaki tüm kod -std=c++17 -Wall -Wextra -Wpedantic ile derlenir.
 *
 * Standart: C++17 (C++20 sections in comments only) Derleme : g++ -std=c++17 -Wall -Wextra
 * -Wpedantic 08_ranges_and_views.cpp
 *
 * [CPPREF DEPTH: Lazy Evaluation and View Composition — Zero-Overhead Abstraction / CPPREF
 * DERİNLİK: Tembel Değerlendirme ve Görünüm Bileşimi — Sıfır Maliyetli Soyutlama]
 * =============================================================================
 * EN: C++20 views are lazy — they produce elements on demand with NO intermediate containers
 * allocated. `views::filter | views::transform` creates a composed view object at COMPILE TIME;
 * the pipeline is a single type encoding the entire chain of operations. Views satisfy O(1)
 * construction and move — they store only the source range reference plus the
 * predicate/transform objects. `views::iota(0)` is an INFINITE range — safe because laziness
 * means elements are generated only when consumed (e.g., via `take(10)`). Borrowed range vs
 * owning range: a borrowed range's iterators remain valid even after the range object is
 * destroyed (e.g., `std::span`, `std::string_view`). Functions like `ranges::find` return
 * `std::ranges::dangling` if given a non-borrowed temporary — compile- time dangling protection!
 * In C++17 (pre-ranges), similar laziness can be achieved with:
 * • Custom iterator pairs wrapping filter/transform logic
 * • Pipeline builder classes chaining `std::function` stages
 * • Coroutine-like generators (Boost.Coroutine2 or custom)
 *
 * TR: C++20 view'ları tembeldir — elemanları talep üzerine üretir, ara konteyner OLUŞTURMAZ.
 * `views::filter | views::transform` DERLEME ZAMANINDA bileşik bir view nesnesi oluşturur; boru
 * hattı tüm işlem zincirini kodlayan tek bir tiptir. View'lar O(1) inşa ve taşıma garantisi
 * sunar — yalnızca kaynak aralık referansı ile yüklem/dönüşüm nesnelerini saklar.
 * `views::iota(0)` SONSUZ bir aralıktır — tembellik sayesinde elemanlar yalnızca tüketildiğinde
 * üretilir (ör. `take(10)` ile). Ödünç alınmış aralık (borrowed range) vs sahipli aralık: ödünç
 * alınmış aralığın iteratörleri, aralık nesnesi yok edildikten sonra bile geçerli kalır (ör.
 * `std::span`, `std::string_view`). `ranges::find` gibi fonksiyonlar, ödünç alınmamış geçici
 * nesne verilirse `std::ranges::dangling` döner — derleme zamanında sarkan iteratör koruması!
 * C++17'de (ranges öncesi) benzer tembellik şu yollarla sağlanabilir:
 * • Filtre/dönüşüm mantığını saran özel iteratör çiftleri
 * • `std::function` aşamalarını zincirleyen boru hattı sınıfları
 * • Coroutine benzeri üreteçler (Boost.Coroutine2 veya özel)
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 08_ranges_and_views.cpp -o 08_ranges_and_views
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>   // EN: STL algorithms (copy_if, transform)
#include <cstdint>     // EN: Fixed-width integers
#include <functional>  // EN: std::function for pipeline stages
#include <iomanip>     // EN: Output formatting
#include <iostream>    // EN: Console I/O
#include <iterator>    // EN: Iterator utilities
#include <numeric>     // EN: Accumulate, iota
#include <sstream>     // EN: String stream for lazy range
#include <string>      // EN: String operations
#include <vector>      // EN: Dynamic array container

// #include <ranges>  // C++20 — not used; all code below is C++17-compatible

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1 — filterAndTransform: C++17 Pipeline Helper
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Generic filter + transform pipeline — C++17 equivalent of:
// TR: Genel filtre + dönüştürme boru hattı — C++17 karşılığı: C++20: container |
// views::filter(pred) | views::transform(fn)
template <typename Container, typename Predicate, typename Transform>
auto filterAndTransform(const Container& input, Predicate pred, Transform fn) {
    using OutputType = decltype(fn(*input.begin()));
    std::vector<OutputType> result;
    result.reserve(input.size());
    for (const auto& elem : input) {
        if (pred(elem)) {
            result.push_back(fn(elem));
        }
    }
    return result;
}

// EN: Take the first N elements from a container
// TR: Bir konteynerden ilk N elemanı al
template <typename Container>
auto takeN(const Container& input, std::size_t n) {
    using ValueType = typename Container::value_type;
    std::vector<ValueType> result;
    std::size_t count = 0;
    for (const auto& elem : input) {
        if (count >= n) break;
        result.push_back(elem);
        ++count;
    }
    return result;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2 — LazyRange: Deferred Computation (C++17 Iterator Approach)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: LazyRange defers filter + transform until iteration — mimics view laziness
// TR: LazyRange, filtre + dönüşümü yinelemeye kadar erteler — görünüm tembelliğini taklit eder
template <typename T>
class LazyRange {
public:
    using FilterFn    = std::function<bool(const T&)>;
    using TransformFn = std::function<T(const T&)>;

    LazyRange(const std::vector<T>& source, FilterFn filter, TransformFn transform)
        : source_(source), filter_(std::move(filter)), transform_(std::move(transform)) {}

    // ─── Inner Iterator ──────────────────────────────────────────────────────────────────────────
    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;

        Iterator(const LazyRange* owner, std::size_t pos)
            : owner_(owner), pos_(pos) { advance(); }

        T operator*() const { return owner_->transform_(owner_->source_[pos_]); }

        Iterator& operator++() { ++pos_; advance(); return *this; }
        Iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }

        bool operator==(const Iterator& o) const { return pos_ == o.pos_; }
        bool operator!=(const Iterator& o) const { return !(*this == o); }

    private:
        void advance() {
            while (pos_ < owner_->source_.size() &&
                   !owner_->filter_(owner_->source_[pos_])) {
                ++pos_;
            }
        }
        const LazyRange* owner_;
        std::size_t pos_;
    };

    Iterator begin() const { return Iterator(this, 0); }
    Iterator end()   const { return Iterator(this, source_.size()); }

    // EN: Materialise lazily-computed results into a vector
    // TR: Tembel hesaplanmış sonuçları bir vektöre topla
    std::vector<T> collect() const {
        return std::vector<T>(begin(), end());
    }

private:
    const std::vector<T>& source_;
    FilterFn              filter_;
    TransformFn           transform_;
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3 — SensorPipeline: Multi-Stage Processing Builder
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Represents a raw automotive sensor reading (e.g., throttle position)
// TR: Ham bir otomotiv sensör okumasını temsil eder (ör. gaz kelebeği konumu)
struct SensorReading {
    int      sensorId;    // EN: Unique sensor identifier / TR: Benzersiz sensör tanımlayıcı
    double   rawValue;    // EN: Raw ADC value / TR: Ham ADC değeri
    bool     valid;       // EN: Validity flag / TR: Geçerlilik bayrağı
};

// EN: SensorPipeline chains: filter invalid → scale → clamp → collect
// TR: SensorPipeline zinciri: geçersiz filtrele → ölçekle → sınırla → topla
class SensorPipeline {
public:
    using Stage = std::function<std::vector<double>(const std::vector<double>&)>;

    // EN: Add a processing stage to the pipeline
    // TR: Boru hattına bir işleme aşaması ekle
    SensorPipeline& addStage(Stage stage) {
        stages_.push_back(std::move(stage));
        return *this;
    }

    // EN: Execute all stages sequentially on the input
    // TR: Tüm aşamaları girdi üzerinde sıralı çalıştır
    [[nodiscard]] std::vector<double> execute(const std::vector<double>& input) const {
        auto data = input;
        for (const auto& stage : stages_) {
            data = stage(data);
        }
        return data;
    }

private:
    std::vector<Stage> stages_;
};

// ─── Pipeline Stage Factories ────────────────────────────────────────────────────────────────────

// EN: Create a filter stage that keeps elements satisfying pred
// TR: Koşulu sağlayan elemanları tutan bir filtre aşaması oluştur
inline auto makeFilterStage(std::function<bool(double)> pred) {
    return [pred = std::move(pred)](const std::vector<double>& in) {
        std::vector<double> out;
        std::copy_if(in.begin(), in.end(), std::back_inserter(out), pred);
        return out;
    };
}

// EN: Create a transform stage that applies fn to each element
// TR: Her elemana fn uygulayan bir dönüşüm aşaması oluştur
inline auto makeTransformStage(std::function<double(double)> fn) {
    return [fn = std::move(fn)](const std::vector<double>& in) {
        std::vector<double> out(in.size());
        std::transform(in.begin(), in.end(), out.begin(), fn);
        return out;
    };
}

// EN: Create a take-N stage that keeps only the first N elements
// TR: Yalnızca ilk N elemanı tutan bir alma aşaması oluştur
inline auto makeTakeStage(std::size_t n) {
    return [n](const std::vector<double>& in) {
        auto count = std::min(n, in.size());
        return std::vector<double>(in.begin(), in.begin() + static_cast<std::ptrdiff_t>(count));
    };
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4 — Classic STL vs Range-Style Comparison
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Classic STL approach — requires intermediate containers
// TR: Klasik STL yaklaşımı — ara konteynerler gerektirir
inline std::vector<double> classicApproach(const std::vector<SensorReading>& readings) {
    // EN: Step 1 — filter valid readings
    // TR: Adım 1 — geçerli okumaları filtrele
    std::vector<SensorReading> validReadings;
    std::copy_if(readings.begin(), readings.end(),
                 std::back_inserter(validReadings),
                 [](const SensorReading& r) { return r.valid; });

    // EN: Step 2 — extract and calibrate raw values
    // TR: Adım 2 — ham değerleri çıkar ve kalibre et
    std::vector<double> calibrated(validReadings.size());
    std::transform(validReadings.begin(), validReadings.end(),
                   calibrated.begin(),
                   [](const SensorReading& r) { return r.rawValue * 0.01; });
    return calibrated;
}

// EN: Range-style approach in C++17 — single pass with filterAndTransform
// TR: C++17'de aralık tarzı yaklaşım — filterAndTransform ile tek geçiş
inline std::vector<double> rangeStyleApproach(const std::vector<SensorReading>& readings) {
    /*  C++20 equivalent / C++20 karşılığı:
     *  auto result = readings
     *      | std::views::filter([](const SensorReading& r) { return r.valid; })
     *      | std::views::transform([](const SensorReading& r) { return r.rawValue * 0.01; });
     */
    return filterAndTransform(
        readings,
        [](const SensorReading& r) { return r.valid; },
        [](const SensorReading& r) { return r.rawValue * 0.01; }
    );
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 5 — Utility Helpers
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Print a vector with a label
// TR: Bir vektörü etiketle yazdır
template <typename T>
void printVec(const std::string& label, const std::vector<T>& v) {
    std::cout << label << ": [";
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << std::fixed << std::setprecision(2) << v[i];
    }
    std::cout << "]\n";
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// main() — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << " C++20 Ranges & Views — C++17 Educational Demos\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // ─── Demo 1: Filter + Transform Pipeline (C++17 Working) ─────────────────────────────────────
    // EN: Filter valid sensor readings, then calibrate
    // TR: Geçerli sensör okumalarını filtrele, ardından kalibre et
    {
        std::cout << "── Demo 1: Filter + Transform Pipeline ──\n";

        std::vector<SensorReading> readings = {
            {1, 512.0, true},  {2, -10.0, false}, {3, 1023.0, true},
            {4, 0.0, true},    {5, 750.0, false},  {6, 200.0, true}
        };

        auto calibrated = filterAndTransform(
            readings,
            [](const SensorReading& r) { return r.valid; },
            [](const SensorReading& r) { return r.rawValue * 0.01; }
        );

        /*  C++20 equivalent / C++20 karşılığı:
         *  auto calibrated = readings
         *      | std::views::filter([](auto& r){ return r.valid; })
         *      | std::views::transform([](auto& r){ return r.rawValue * 0.01; });
         */

        printVec("  Calibrated values", calibrated);
        std::cout << '\n';
    }

    // ─── Demo 2: C++20 Equivalent (Comment Only) ─────────────────────────────────────────────────
    // EN: The following C++20 code is shown for reference only
    // TR: Aşağıdaki C++20 kodu yalnızca referans amaçlıdır
    {
        std::cout << "── Demo 2: C++20 Equivalent (in comments) ──\n";
        std::cout << "  // #include <ranges>\n";
        std::cout << "  // auto result = sensorData\n";
        std::cout << "  //     | std::views::filter(isValid)\n";
        std::cout << "  //     | std::views::transform(calibrate)\n";
        std::cout << "  //     | std::views::take(5);\n";
        std::cout << "  (See source comments for full C++20 syntax)\n\n";

        /*  Full C++20 pipeline / Tam C++20 boru hattı:
         *
         *  #include <ranges>
         *  namespace views = std::views;
         *
         *  auto isValid   = [](const SensorReading& r) { return r.valid; };
         *  auto calibrate = [](const SensorReading& r) { return r.rawValue * 0.01; };
         *
         *  std::vector<SensorReading> sensorData = { ... };
         *
         *  auto result = sensorData
         *      | views::filter(isValid)
         *      | views::transform(calibrate)
         *      | views::take(5);
         *
         *  for (double v : result) std::cout << v << '\n';
         */
    }

    // ─── Demo 3: Multi-Stage Pipeline — filter → transform → take N ──────────────────────────────
    // EN: Build a pipeline: discard negatives → multiply by 2 → take first 3
    // TR: Boru hattı kur: negatifleri at → 2 ile çarp → ilk 3'ü al
    {
        std::cout << "── Demo 3: Multi-Stage Pipeline ──\n";

        std::vector<double> rawSignals = {
            -5.0, 12.0, 3.5, -1.0, 8.8, 22.1, 6.0, -0.5, 15.3
        };

        /*  C++20 equivalent / C++20 karşılığı:
         *  auto result = rawSignals
         *      | views::filter([](double v){ return v >= 0.0; })
         *      | views::transform([](double v){ return v * 2.0; })
         *      | views::take(3);
         */

        SensorPipeline pipeline;
        pipeline
            .addStage(makeFilterStage([](double v) { return v >= 0.0; }))
            .addStage(makeTransformStage([](double v) { return v * 2.0; }))
            .addStage(makeTakeStage(3));

        auto result = pipeline.execute(rawSignals);
        printVec("  Pipeline output (first 3 non-negative × 2)", result);
        std::cout << '\n';
    }

    // ─── Demo 4: Lazy Range vs Eager Evaluation ──────────────────────────────────────────────────
    // EN: Compare lazy (deferred) vs eager (immediate) computation
    // TR: Tembel (ertelenmiş) ile istekli (anlık) hesaplamayı karşılaştır
    {
        std::cout << "── Demo 4: Lazy vs Eager Evaluation ──\n";

        std::vector<double> temperatures = {
            -40.0, 25.5, 105.3, 72.0, -10.0, 88.9, 150.0, 36.6
        };

        // EN: Predicate — keep temperatures in valid ECU operating range [−30, 125]
        // TR: Koşul — geçerli ECU çalışma aralığındaki sıcaklıkları tut [−30, 125]
        auto inRange = [](const double& t) { return t >= -30.0 && t <= 125.0; };

        // EN: Transform — Celsius to Fahrenheit
        // TR: Dönüşüm — Celsius'tan Fahrenheit'a
        auto toFahrenheit = [](const double& t) { return t * 9.0 / 5.0 + 32.0; };

        // --- Eager approach (C++17 filterAndTransform) ---
        // EN: Computes all results immediately into a new vector
        // TR: Tüm sonuçları hemen yeni bir vektöre hesaplar
        auto eagerResult = filterAndTransform(temperatures, inRange, toFahrenheit);
        printVec("  Eager (all at once)", eagerResult);

        // --- Lazy approach (LazyRange) ---
        // EN: Nothing is computed until we iterate or collect
        // TR: Yineleyinceye veya toplayana kadar hiçbir şey hesaplanmaz
        LazyRange<double> lazyView(temperatures, inRange, toFahrenheit);

        std::cout << "  Lazy  (first 3)  : [";
        int count = 0;
        for (auto it = lazyView.begin(); it != lazyView.end() && count < 3; ++it, ++count) {
            if (count > 0) std::cout << ", ";
            std::cout << std::fixed << std::setprecision(2) << *it;
        }
        std::cout << "]\n";

        auto lazyAll = lazyView.collect();
        printVec("  Lazy  (collected)", lazyAll);
        std::cout << '\n';
    }

    // ─── Demo 5: Custom Pipeline Builder Pattern ─────────────────────────────────────────────────
    // EN: Demonstrates the builder pattern for composing processing stages
    // TR: İşleme aşamalarını birleştirmek için inşaatçı kalıbını gösterir
    {
        std::cout << "── Demo 5: Custom Pipeline Builder ──\n";

        // EN: Simulated throttle position sensor data (0–4095 ADC range)
        // TR: Simüle edilmiş gaz kelebeği konum sensörü verisi (0–4095 ADC aralığı)
        std::vector<double> throttleADC = {
            150.0, 4200.0, 2048.0, -50.0, 3500.0, 0.0, 1500.0, 4096.0
        };

        /*  C++20 equivalent / C++20 karşılığı:
         *  auto processed = throttleADC
         *      | views::filter([](double v){ return v >= 0.0 && v <= 4095.0; })
         *      | views::transform([](double v){ return (v / 4095.0) * 100.0; })
         *      | views::transform([](double v){ return std::clamp(v, 0.0, 100.0); });
         */

        SensorPipeline throttlePipeline;
        throttlePipeline
            // EN: Stage 1 — filter out-of-range ADC values
            // TR: Aşama 1 — aralık dışı ADC değerlerini filtrele
            .addStage(makeFilterStage([](double v) {
                return v >= 0.0 && v <= 4095.0;
            }))
            // EN: Stage 2 — normalize to percentage (0–100%)
            // TR: Aşama 2 — yüzdeye normalleştir (0–100%)
            .addStage(makeTransformStage([](double v) {
                return (v / 4095.0) * 100.0;
            }))
            // EN: Stage 3 — clamp to safe throttle range
            // TR: Aşama 3 — güvenli gaz kelebeği aralığına sınırla
            .addStage(makeTransformStage([](double v) {
                return std::max(0.0, std::min(100.0, v));
            }));

        auto throttlePercent = throttlePipeline.execute(throttleADC);
        printVec("  Throttle %", throttlePercent);

        // EN: Compare classic vs range-style on SensorReading data
        // TR: SensorReading verisi üzerinde klasik ile aralık tarzını karşılaştır
        std::vector<SensorReading> mixedReadings = {
            {10, 800.0, true}, {11, -5.0, false}, {12, 400.0, true},
            {13, 950.0, true}, {14, 0.0, false},  {15, 620.0, true}
        };

        auto classicResult = classicApproach(mixedReadings);
        auto rangeResult   = rangeStyleApproach(mixedReadings);

        printVec("  Classic STL result", classicResult);
        printVec("  Range-style result", rangeResult);

        // EN: Verify both approaches produce identical output
        // TR: Her iki yaklaşımın aynı çıktıyı ürettiğini doğrula
        bool match = (classicResult == rangeResult);
        std::cout << "  Results match: " << (match ? "YES" : "NO") << "\n\n";
    }

    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << " All demos completed successfully.\n";
    std::cout << "═══════════════════════════════════════════════════════\n";

    return 0;
}
