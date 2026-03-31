/**
 * @file module_06_industry_standards/06_debugging_techniques.cpp
 * @brief Industry Standards: Debugging Techniques & Defensive Programming — Endüstri
 * Standartları: Hata Ayıklama Teknikleri ve Savunmacı Programlama
 *
 * @details
 * ===============================================================================
 * [THEORY: Debugging, Assertions & Defensive Programming / TEORİ: Hata Ayıklama, Onaylamalar ve
 * Savunmacı Programlama]
 * ===============================================================================
 *
 * --- 1. GDB Basics ---
 * EN: GDB is the GNU Debugger — the primary debugging tool on Linux. Compile with `-g` to
 * include debug symbols: g++ -g -std=c++17 -o prog 06_debugging_techniques.cpp
 * Key commands:
 * — break main    — set breakpoint at main()
 * — run           — start execution
 * — next (n)      — step over to next line
 * — step (s)      — step into function call
 * — print var     — inspect variable value
 * — backtrace (bt)— show call stack
 * — watch var     — break when variable changes (watchpoint)
 *
 * --- 2. Core Dumps ---
 * Enable core dumps: `ulimit -c unlimited` When a program crashes (SIGSEGV, SIGABRT), the OS
 * writes a core file. Analyze: `gdb ./program core` — see exact crash location and variables.
 *
 * --- 3. Static Analysis & Sanitizers ---
 * Compiler warnings (-Wall -Wextra -Wpedantic) are the first line of defense.
 * Address/Thread/UB sanitizers catch bugs at runtime:
 * — -fsanitize=address   — buffer overflow, use-after-free
 * — -fsanitize=undefined — signed overflow, null deref, alignment
 * — -fsanitize=thread    — data races in multithreaded code
 *
 * --- 4. assert() vs static_assert() ---
 * assert(expr) — runtime check; aborts if expr is false. Disabled by defining NDEBUG.
 * static_assert(expr, msg) — compile-time check; zero runtime cost. Use for type traits, sizes,
 * alignment guarantees.
 *
 * --- 5. Defensive Programming ---
 * — Preconditions — validate inputs before use.
 * — Postconditions — verify outputs before returning.
 * — Invariants — maintain class state consistency across all methods.
 *     [[nodiscard]] — force callers to check return values.
 *
 * --- 6. Logging Levels ---
 * — TRACE — finest granularity, every function entry/exit
 * — DEBUG — diagnostic information for developers
 * — INFO  — general operational messages
 * — WARN  — potentially harmful situations
 * — ERROR — error events, program can continue
 * — FATAL — severe error, program must abort
 *
 * --- 1. GDB Temelleri ---
 * TR: GDB, GNU Hata Ayıklayıcısıdır — Linux'ta temel hata ayıklama aracıdır. Hata ayıklama
 * sembollerini eklemek için `-g` ile derleyin: g++ -g -std=c++17 -o prog
 * 06_debugging_techniques.cpp
 * Temel komutlar:
 * — break main    — main()'de kırılma noktası koy
 * — run           — çalıştırmayı başlat
 * — next (n)      — sonraki satıra geç
 * — step (s)      — fonksiyon çağrısına gir
 * — print var     — değişken değerini incele
 * — backtrace (bt)— çağrı yığınını göster
 * — watch var     — değişken değişince dur (izleme noktası)
 *
 * --- 2. Çekirdek Dökümleri ---
 * Çekirdek dökümünü etkinleştir: `ulimit -c unlimited` Program çöktüğünde (SIGSEGV,
 * SIGABRT), işletim sistemi bir core dosyası yazar. Analiz: `gdb ./program core` — tam çökme
 * noktasını ve değişkenleri gör.
 *
 * --- 3. Statik Analiz ve Temizleyiciler ---
 * Derleyici uyarıları (-Wall -Wextra -Wpedantic) ilk savunma hattıdır. Adres/İş parçacığı/TB
 * temizleyicileri çalışma zamanında hata yakalar:
 * — -fsanitize=address   — tampon taşması, serbest bırakılmış kullanımı
 * — -fsanitize=undefined — işaretli taşma, null ref, hizalama
 * — -fsanitize=thread    — çok iş parçacıklı kodda veri yarışları
 *
 * --- 4. assert() vs static_assert() ---
 * assert(ifade) — çalışma zamanı kontrolü; ifade false ise iptal eder. NDEBUG tanımlanarak
 * devre dışı bırakılır. static_assert(ifade, msg) — derleme zamanı kontrolü; çalışma zamanı
 * maliyeti sıfır. Tip özellikleri, boyutlar, hizalama garantileri için kullanılır.
 *
 * --- 5. Savunmacı Programlama ---
 * — Ön koşullar — kullanmadan önce girdileri doğrula.
 * — Son koşullar — dönmeden önce çıktıları doğrula.
 * — Değişmezler — tüm yöntemlerde sınıf durum tutarlılığını koru.
 *     [[nodiscard]] — çağıranları dönüş değerini kontrol etmeye zorla.
 *
 * --- 6. Günlük Seviyeleri ---
 * — TRACE — en ince ayrıntı, her fonksiyon giriş/çıkış
 * — DEBUG — geliştiriciler için tanılama bilgisi
 * — INFO  — genel operasyonel mesajlar
 * — WARN  — potansiyel zararlı durumlar
 * — ERROR — hata olayları, program devam edebilir
 * — FATAL — ağır hata, program iptal etmeli
 *
 * Standart: C++17 Derleme : g++ -std=c++17 -Wall -Wextra -Wpedantic 06_debugging_techniques.cpp
 *
 * [CPPREF DEPTH: GDB Advanced — Conditional Breakpoints, Watchpoints, and Reverse Debugging /
 * CPPREF DERİNLİK: İleri GDB — Koşullu Kırılma Noktaları, İzleme Noktaları ve Geri Hata
 * Ayıklama]
 * =============================================================================
 * EN: `break func if x > 100` sets a conditional breakpoint — execution stops only when the
 * condition is true, invaluable for debugging iteration N of a million-iteration loop. `watch
 * variable` sets a hardware watchpoint; the CPU halts when the watched memory location changes
 * value. `rwatch` triggers on read access, `awatch` on any access. GDB reverse debugging:
 * `record` starts recording, then `reverse-next`, `reverse-step`, and `reverse-continue` replay
 * execution backwards — you can step BACK to find where a variable was corrupted. `gdb -tui`
 * launches a terminal UI with source code pane. Core dump analysis: `thread apply all bt` prints
 * all thread backtraces — essential for multithreaded crash investigation. Python scripting
 * inside GDB lets you write custom pretty-printers for complex types (e.g., printing a red-black
 * tree as a sorted list). `set pagination off` and `set logging on` for scripted batch
 * debugging.
 *
 * TR: `break func if x > 100` koşullu kırılma noktası koyar — yürütme yalnızca koşul doğru
 * olduğunda durur; milyon iterasyonlu bir döngüde N. iterasyonu hata ayıklamak için paha
 * biçilmezdir. `watch variable` donanım izleme noktası koyar; izlenen bellek konumunun değeri
 * değiştiğinde CPU durur. `rwatch` okuma erişiminde, `awatch` herhangi bir erişimde tetiklenir.
 * GDB geri hata ayıklama: `record` kaydı başlatır, ardından `reverse-next`, `reverse-step` ve
 * `reverse-continue` yürütmeyi GERİYE doğru oynatır — bir değişkenin nerede bozulduğunu bulmak
 * için GERİ adım atabilirsiniz. `gdb -tui` kaynak kodlu terminal arayüzü başlatır. Çekirdek
 * dökümü analizi: `thread apply all bt` tüm iş parçacığı geri izlerini yazdırır — çok iş
 * parçacıklı çökme araştırması için zorunludur. GDB içinde Python betikleme, karmaşık tipler
 * için özel pretty-printer yazmanızı sağlar.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_debugging_techniques.cpp -o 06_debugging_techniques
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cassert>     // EN: Runtime assertions
#include <chrono>      // EN: Timestamp generation
#include <cstdint>     // EN: Fixed-width integers
#include <functional>  // EN: std::function for invariant checks
#include <iomanip>     // EN: Output formatting
#include <iostream>    // EN: Console I/O
#include <sstream>     // EN: String stream for log formatting
#include <string>      // EN: String operations
#include <type_traits> // EN: Type traits for static_assert

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1 — ECULogger: Singleton Logger with Levels
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Log severity levels for automotive ECU diagnostics
// TR: Otomotiv ECU tanılaması için günlük önem seviyeleri
enum class LogLevel : std::uint8_t {
    TRACE = 0,
    DEBUG = 1,
    INFO  = 2,
    WARN  = 3,
    ERROR = 4,
    FATAL = 5
};

// EN: Convert LogLevel enum to string representation
// TR: LogLevel enum'ını metin temsiline dönüştür
inline const char* logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
    }
    return "?????";
}

// EN: Singleton logger — centralized logging for ECU software
// TR: Tekil günlükçü — ECU yazılımı için merkezi günlükleme
class ECULogger {
public:
    // EN: Get the singleton instance
    // TR: Tekil örneği al
    static ECULogger& instance() {
        static ECULogger logger;
        return logger;
    }

    // EN: Set the minimum log level — messages below this are suppressed
    // TR: Minimum günlük seviyesini ayarla — altındaki mesajlar bastırılır
    void setLevel(LogLevel level) { minLevel_ = level; }

    // EN: Log a message with level, file and line information
    // TR: Seviye, dosya ve satır bilgisiyle bir mesaj günlükle
    void log(LogLevel level, const char* file, int line, const std::string& msg) {
        if (level < minLevel_) return;

        auto now = std::chrono::system_clock::now();
        auto epoch = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();

        std::cout << "[" << std::setw(13) << millis << "ms] "
                  << "[" << logLevelToString(level) << "] "
                  << file << ":" << line << " — " << msg << "\n";
    }

    // EN: Deleted copy/move — singleton pattern
    // TR: Silinen kopya/taşıma — tekil kalıp
    ECULogger(const ECULogger&)            = delete;
    ECULogger& operator=(const ECULogger&) = delete;
    ECULogger(ECULogger&&)                 = delete;
    ECULogger& operator=(ECULogger&&)      = delete;

private:
    ECULogger() = default;
    LogLevel minLevel_ = LogLevel::TRACE;
};

// ─── Convenience Macros ──────────────────────────────────────────────────────────────────────────

// EN: LOG macro captures __FILE__ and __LINE__ automatically
// TR: LOG makrosu __FILE__ ve __LINE__ bilgisini otomatik yakalar
#define ECU_LOG(level, msg) \
    ECULogger::instance().log(level, __FILE__, __LINE__, msg)

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2 — ECU_ASSERT: Enhanced Assertion with Logging
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Enhanced assert macro — logs the failure before aborting
// TR: Geliştirilmiş onaylama makrosu — iptal etmeden önce hatayı günlükler
#define ECU_ASSERT(cond, msg)                                                  \
    do {                                                                       \
        if (!(cond)) {                                                         \
            ECU_LOG(LogLevel::FATAL,                                           \
                    std::string("ASSERTION FAILED: ") + #cond + " — " + msg); \
            assert((cond) && (msg));                                           \
        }                                                                      \
    } while (false)

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3 — static_assert Examples: Compile-Time Guarantees
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Automotive CAN frame — strict layout required for hardware DMA
// TR: Otomotiv CAN çerçevesi — donanım DMA için katı yerleşim gerekli
struct CANFrame {
    std::uint32_t id;         // EN: CAN identifier (11 or 29 bit)
    std::uint8_t  dlc;        // EN: Data length code (0–8)
    std::uint8_t  data[8];    // EN: Payload bytes
    std::uint8_t  padding[3]; // EN: Alignment padding
};

// EN: Compile-time checks — zero runtime cost
// TR: Derleme zamanı kontrolleri — sıfır çalışma zamanı maliyeti
static_assert(sizeof(CANFrame) == 16, "CANFrame must be exactly 16 bytes for DMA");
static_assert(std::is_trivially_copyable_v<CANFrame>,
              "CANFrame must be trivially copyable for memcpy/DMA");
static_assert(alignof(CANFrame) <= 4, "CANFrame alignment must be at most 4 bytes");

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4 — Precondition / Postcondition Pattern
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Compute throttle percentage from raw ADC value with pre/post checks
// TR: Ham ADC değerinden gaz kelebeği yüzdesini ön/son kontrollerle hesapla
[[nodiscard]] inline double computeThrottlePercent(int rawADC) {
    // EN: Precondition — raw ADC must be in valid 12-bit range
    // TR: Ön koşul — ham ADC geçerli 12-bit aralığında olmalı
    ECU_ASSERT(rawADC >= 0 && rawADC <= 4095,
               "rawADC out of 12-bit range [0, 4095]");

    double percent = (static_cast<double>(rawADC) / 4095.0) * 100.0;

    // EN: Postcondition — result must be in [0, 100]
    // TR: Son koşul — sonuç [0, 100] aralığında olmalı
    ECU_ASSERT(percent >= 0.0 && percent <= 100.0,
               "Throttle percent out of [0, 100] range");

    return percent;
}

// EN: Validate engine RPM — returns clamped value with [[nodiscard]]
// TR: Motor devrini doğrula — [[nodiscard]] ile sınırlanmış değer döndürür
[[nodiscard]] inline int clampRPM(int rpm) {
    // EN: Precondition — RPM should be non-negative from sensor
    // TR: Ön koşul — sensörden gelen RPM negatif olmamalı
    ECU_ASSERT(rpm >= 0, "RPM cannot be negative");

    constexpr int kMaxRPM = 8000;
    if (rpm > kMaxRPM) {
        ECU_LOG(LogLevel::WARN,
                "RPM " + std::to_string(rpm) + " exceeds max, clamping to " +
                std::to_string(kMaxRPM));
        rpm = kMaxRPM;
    }
    return rpm;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 5 — InvariantChecker: RAII Guard for Class Invariants
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: RAII guard that checks a class invariant when the scope exits
// TR: Kapsam çıkışında sınıf değişmezini kontrol eden RAII koruyucusu
class InvariantChecker {
public:
    using CheckFn = std::function<bool()>;

    // EN: Construct with an invariant check function and description
    // TR: Bir değişmez kontrol fonksiyonu ve açıklama ile oluştur
    InvariantChecker(CheckFn check, std::string description)
        : check_(std::move(check)), desc_(std::move(description)) {}

    // EN: On destruction, verify the invariant holds
    // TR: Yıkımda, değişmezin geçerli olduğunu doğrula
    ~InvariantChecker() {
        if (!check_()) {
            ECU_LOG(LogLevel::ERROR,
                    "INVARIANT VIOLATED: " + desc_);
        }
    }

    InvariantChecker(const InvariantChecker&)            = delete;
    InvariantChecker& operator=(const InvariantChecker&) = delete;

private:
    CheckFn     check_;
    std::string desc_;
};

// EN: Simple battery monitor class that must maintain voltage invariant
// TR: Voltaj değişmezini koruması gereken basit akü izleme sınıfı
class BatteryMonitor {
public:
    explicit BatteryMonitor(double voltage) : voltage_(voltage) {}

    // EN: Update voltage with automatic invariant checking on scope exit
    // TR: Kapsam çıkışında otomatik değişmez kontrolüyle voltajı güncelle
    void updateVoltage(double newVoltage) {
        InvariantChecker guard(
            [this]() { return voltage_ >= 0.0 && voltage_ <= 16.0; },
            "Battery voltage must be in [0.0, 16.0]V"
        );
        voltage_ = newVoltage;
    }

    [[nodiscard]] double voltage() const { return voltage_; }

private:
    double voltage_;
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// main() — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << " Debugging Techniques & Defensive Programming Demos\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // ─── Demo 1: ECULogger — Log at Different Levels ─────────────────────────────────────────────
    // EN: Demonstrate logging at various severity levels
    // TR: Çeşitli önem seviyelerinde günlüklemeyi göster
    {
        std::cout << "── Demo 1: ECULogger — Log Levels ──\n";

        auto& logger = ECULogger::instance();
        logger.setLevel(LogLevel::TRACE);

        ECU_LOG(LogLevel::TRACE, "Engine control loop iteration 42");
        ECU_LOG(LogLevel::DEBUG, "Throttle ADC raw = 2048");
        ECU_LOG(LogLevel::INFO,  "ECU initialised — firmware v3.2.1");
        ECU_LOG(LogLevel::WARN,  "Coolant temperature approaching limit: 98°C");
        ECU_LOG(LogLevel::ERROR, "CAN bus timeout on node 0x1A — retrying");

        // EN: Raise minimum level to suppress TRACE and DEBUG
        // TR: TRACE ve DEBUG'ı bastırmak için minimum seviyeyi yükselt
        logger.setLevel(LogLevel::INFO);
        ECU_LOG(LogLevel::TRACE, "This TRACE message will be suppressed");
        ECU_LOG(LogLevel::DEBUG, "This DEBUG message will be suppressed");
        ECU_LOG(LogLevel::INFO,  "Only INFO and above are visible now");
        std::cout << '\n';
    }

    // ─── Demo 2: ECU_ASSERT — Precondition Checking ──────────────────────────────────────────────
    // EN: Validate inputs using ECU_ASSERT macro
    // TR: Girdileri ECU_ASSERT makrosuyla doğrula
    {
        std::cout << "── Demo 2: ECU_ASSERT — Precondition Checking ──\n";

        // EN: Valid input — should pass assertions
        // TR: Geçerli girdi — onaylamaları geçmeli
        double throttle = computeThrottlePercent(2048);
        std::cout << "  Throttle at ADC 2048: "
                  << std::fixed << std::setprecision(1)
                  << throttle << "%\n";

        throttle = computeThrottlePercent(0);
        std::cout << "  Throttle at ADC 0:    " << throttle << "%\n";

        throttle = computeThrottlePercent(4095);
        std::cout << "  Throttle at ADC 4095: " << throttle << "%\n";

        // EN: Would trigger assertion (uncomment to test):
        // TR: Onaylamayı tetikler (test etmek için yorum işaretini kaldırın): double bad =
        // computeThrottlePercent(-100);  // ASSERTION FAILED
        std::cout << '\n';
    }

    // ─── Demo 3: static_assert — Compile-Time Guarantees ─────────────────────────────────────────
    // EN: These checks happen at compile time; if they fail, build stops
    // TR: Bu kontroller derleme zamanında olur; başarısız olursa derleme durur
    {
        std::cout << "── Demo 3: static_assert — Compile-Time Checks ──\n";

        std::cout << "  sizeof(CANFrame)  = " << sizeof(CANFrame)  << " bytes\n";
        std::cout << "  alignof(CANFrame) = " << alignof(CANFrame) << " bytes\n";
        std::cout << "  is_trivially_copyable: "
                  << std::boolalpha
                  << std::is_trivially_copyable_v<CANFrame> << "\n";

        // EN: Additional compile-time checks
        // TR: Ek derleme zamanı kontrolleri
        static_assert(sizeof(int) >= 4, "int must be at least 32 bits");
        static_assert(std::is_same_v<decltype(CANFrame::id), std::uint32_t>,
                      "CAN ID must be uint32_t");

        std::cout << "  All static_assert checks passed at compile time.\n\n";
    }

    // ─── Demo 4: InvariantChecker — Automatic Postcondition Verify ───────────────────────────────
    // EN: RAII guard automatically checks invariant when scope exits
    // TR: RAII koruyucusu kapsam çıkışında otomatik olarak değişmezi kontrol eder
    {
        std::cout << "── Demo 4: InvariantChecker — RAII Guard ──\n";

        BatteryMonitor battery(12.6);
        std::cout << "  Initial voltage: " << battery.voltage() << "V\n";

        // EN: Normal update — invariant holds
        // TR: Normal güncelleme — değişmez geçerli
        battery.updateVoltage(14.2);
        std::cout << "  After charge:    " << battery.voltage() << "V\n";

        // EN: Edge-case update — invariant still holds
        // TR: Sınır değer güncelleme — değişmez hâlâ geçerli
        battery.updateVoltage(0.0);
        std::cout << "  Deep discharge:  " << battery.voltage() << "V\n";

        // EN: Violation — InvariantChecker logs error on scope exit
        // TR: İhlal — InvariantChecker kapsam çıkışında hata günlükler
        battery.updateVoltage(18.5);
        std::cout << "  Over-voltage:    " << battery.voltage()
                  << "V (invariant violated!)\n\n";
    }

    // ─── Demo 5: Defensive Coding Patterns ───────────────────────────────────────────────────────
    // EN: Demonstrate [[nodiscard]], clamp, and safe parameter handling
    // TR: [[nodiscard]], sınırlama ve güvenli parametre işlemeyi göster
    {
        std::cout << "── Demo 5: Defensive Coding Patterns ──\n";

        // EN: [[nodiscard]] ensures the caller uses the return value
        // TR: [[nodiscard]] çağıranın dönüş değerini kullanmasını sağlar
        int safeRPM = clampRPM(6500);
        std::cout << "  clampRPM(6500)  = " << safeRPM << "\n";

        safeRPM = clampRPM(9500);
        std::cout << "  clampRPM(9500)  = " << safeRPM << " (clamped)\n";

        // EN: Trying to ignore [[nodiscard]] result causes compiler warning:
        // TR: [[nodiscard]] sonucunu yok saymak derleyici uyarısına neden olur: clampRPM(5000); 
        // // WARNING: ignoring return value with 'nodiscard'

        // EN: Defensive null/range checking pattern
        // TR: Savunmacı null/aralık kontrol kalıbı
        auto safeDivide = [](double a, double b) -> double {
            ECU_ASSERT(b != 0.0, "Division by zero in safeDivide");
            return a / b;
        };

        double ratio = safeDivide(100.0, 3.0);
        std::cout << "  safeDivide(100, 3) = "
                  << std::fixed << std::setprecision(4)
                  << ratio << "\n";

        std::cout << '\n';
    }

    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << " All demos completed successfully.\n";
    std::cout << "═══════════════════════════════════════════════════════\n";

    return 0;
}
