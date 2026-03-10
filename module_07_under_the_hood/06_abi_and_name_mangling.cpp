/**
 * @file 06_abi_and_name_mangling.cpp
 * @brief EN: ABI (Application Binary Interface) and C++ Name Mangling — binary contracts, symbol
 * encoding, extern "C", and plugin APIs
 * TR: ABI (Uygulama İkili Arayüzü) ve C++ İsim Karıştırma — ikili sözleşmeler, sembol kodlama,
 * extern "C" ve eklenti API'leri
 *
 * @details
 * [THEORY]
 *
 * EN: ABI (Application Binary Interface) — The Binary-Level Contract
 * TR: ABI (Uygulama İkili Arayüzü) — İkili Düzey Sözleşme
 *
 * EN: An ABI defines how compiled code interacts at the binary level: calling conventions,
 * register usage, struct layout, vtable ordering, and how function names become symbols. Linked
 * object files must agree on the ABI or linker errors / crashes will follow.
 * TR: ABI, derlenmiş kodun ikili düzeyde nasıl etkileştiğini tanımlar: çağrı kuralları, yazmaç
 * kullanımı, struct düzeni, vtable sıralaması ve fonksiyon adlarının sembollere dönüşümü.
 * Bağlanan nesne dosyaları ABI üzerinde anlaşmalıdır.
 *
 * EN: Name Mangling — Encoding Overloaded Signatures
 * TR: İsim Karıştırma — Aşırı Yüklenmiş İmzaları Kodlama
 *
 * EN: The compiler "mangles" each function into a unique symbol. Under the Itanium ABI: foo(int)
 * → _Z3fooi   |   foo(int,float) → _Z3fooif Each overload gets a distinct mangled name so the
 * linker resolves the correct one.
 * TR: Derleyici her fonksiyonu benzersiz bir sembole "karıştırır". Itanium ABI altında: foo(int)
 * → _Z3fooi   |   foo(int,float) → _Z3fooif Her aşırı yükleme farklı bir karıştırılmış isim
 * alır; bağlayıcı doğru olanı çözümler.
 *
 * EN: extern "C" — Disabling Name Mangling for Interoperability
 * TR: extern "C" — Birlikte Çalışabilirlik İçin İsim Karıştırmayı Devre Dışı Bırakma
 *
 * EN: `extern "C" { ... }` tells the compiler to use C linkage: no mangling. Essential for C
 * interop, shared libraries (.so/.dll), and dlopen/dlsym plugin APIs.
 * TR: `extern "C" { ... }` derleyiciye C bağlantısı kullanmasını söyler: karıştırma yok. C
 * birlikte çalışma, paylaşımlı kütüphaneler ve dlopen/dlsym eklenti API'leri için gerekli.
 *
 * EN: Compiler ABI Incompatibility
 * TR: Derleyici ABI Uyumsuzluğu
 *
 * EN: MSVC, GCC, and Clang each use different mangling schemes. Mixing compilers (or major
 * versions) can cause linker failures. In automotive ECU software, the entire toolchain must be
 * locked to ensure ABI consistency.
 * TR: MSVC, GCC ve Clang farklı karıştırma şemaları kullanır. Derleyici karışımı bağlayıcı
 * hatalarına neden olabilir. Otomotiv ECU yazılımında araç zinciri kilitlenmelidir.
 *
 * EN: Tools: `c++filt` demangles symbols (echo _Z3fooi | c++filt → foo(int)). `nm` lists symbols
 * in object files. Combine: nm mylib.so | c++filt
 * TR: Araçlar: `c++filt` sembolleri çözümler. `nm` sembolleri listeler.
 *
 * EN: Shared Libraries: exposing a C ABI (extern "C") for .so files ensures any compiler version
 * can link your functions — standard for UDS, bootloaders, CAN layers.
 * TR: Paylaşımlı Kütüphaneler: .so dosyaları için C ABI sunmak, herhangi bir derleyici sürümünün
 * fonksiyonlarınızı bağlamasını sağlar — UDS, önyükleyiciler, CAN katmanları.
 *
 * [CPPREF DEPTH: ABI Stability and the Itanium C++ ABI / CPPREF DERİNLİK: ABI Kararlılığı ve
 * Itanium C++ ABI'si]
 * =============================================================================
 * EN: Most Linux/Unix compilers (GCC, Clang) follow the Itanium C++ ABI, which standardises name
 * mangling, vtable layout, exception handling, and RTTI representation. Name mangling encodes:
 * namespace, enclosing class, function name, parameter types (including cv-qualifiers and
 * references), and template arguments. `extern "C"` suppresses mangling so C code can link the
 * symbol directly. ABI breaks happen when: struct/class layout changes (adding or reordering
 * members), virtual function order changes, or the exception model changes. Compiling with
 * `-fvisibility=hidden` and explicitly exporting only public symbols
 * (`__attribute__((visibility("default")))`) produces smaller and faster shared libraries —
 * fewer relocations, faster `dlopen()`. This is the standard practice for plugin architectures.
 *
 * TR: Çoğu Linux/Unix derleyicisi (GCC, Clang) isim bozma, vtable düzeni, istisna işleme ve RTTI
 * temsilini standartlaştıran Itanium C++ ABI'sini takip eder. İsim bozma şunu kodlar: ad alanı,
 * kapsayan sınıf, fonksiyon adı, parametre tipleri (cv-niteleyiciler ve referanslar dahil) ve
 * şablon argümanları. `extern "C"` bozulmayı bastırır, böylece C kodu sembolü doğrudan
 * bağlayabilir. ABI kırılmaları şu durumlarda olur: yapı/sınıf düzeni değiştiğinde, sanal
 * fonksiyon sırası değiştiğinde veya istisna modeli değiştiğinde. `-fvisibility=hidden` ile
 * derleyip yalnızca genel sembolleri açıkça dışa aktarmak daha küçük ve hızlı paylaşımlı
 * kütüphaneler üretir — daha az yer değiştirme, daha hızlı `dlopen()`.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>    // EN: Standard I/O for console output
#include <string>      // EN: std::string for sensor name lookups
#include <functional>  // EN: std::function for modern callback wrappers
#include <typeinfo>    // EN: typeid for RTTI-based type name inspection
#include <cstdio>      // EN: C-style printf for extern "C" callback demos
#include <array>       // EN: std::array for plugin dispatch table

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: C++ Overloaded Functions — Name Mangling in Action
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Three overloads share the name "readSensor" — the compiler mangles each into a unique
// symbol.
// TR: Üç aşırı yükleme "readSensor" adını paylaşır — derleyici her birini benzersiz bir sembole
// karıştırır.

// ─── 1a. Read sensor by numeric ID ───────────────────────────────────────────────────────────────
int readSensor(int id) {
    // EN: Simulates reading raw ADC value from sensor channel
    // TR: Sensör kanalından ham ADC değeri okumayı simüle eder
    int raw_value = 1000 + id * 42;
    std::cout << "  [readSensor(int)] Channel " << id
              << " → raw value: " << raw_value << "\n";
    return raw_value;
}

// ─── 1b. Read sensor by ID with calibration factor ───────────────────────────────────────────────
float readSensor(int id, float calibration) {
    // EN: Applies calibration offset to the sensor reading
    // TR: Sensör okumasına kalibrasyon ofseti uygular
    float calibrated = static_cast<float>(1000 + id * 42) * calibration;
    std::cout << "  [readSensor(int, float)] Channel " << id
              << " with cal=" << calibration
              << " → calibrated: " << calibrated << "\n";
    return calibrated;
}

// ─── 1c. Read sensor by name (string lookup) ─────────────────────────────────────────────────────
int readSensor(const std::string& name) {
    // EN: Simulates named sensor lookup (e.g., from config database)
    // TR: İsimli sensör aramasını simüle eder (ör. yapılandırma veritabanından)
    int resolved_value = static_cast<int>(name.length()) * 100 + 55;
    std::cout << "  [readSensor(string)] Sensor \"" << name
              << "\" → resolved value: " << resolved_value << "\n";
    return resolved_value;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: extern "C" Block — C-Compatible ECU API Stubs
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: extern "C" prevents name mangling — symbols are exported as plain C names.
// TR: extern "C" isim karıştırmayı engeller — semboller düz C adları olarak dışa aktarılır.

extern "C" {

int ecu_init() {
    // EN: Initialize ECU hardware abstraction layer (stub)
    // TR: ECU donanım soyutlama katmanını başlat (saplama)
    std::cout << "  [ecu_init] ECU initialized (firmware v2.4.1)\n";
    return 0;
}

void ecu_shutdown() {
    // EN: Graceful ECU shutdown — flush CAN buffers, store NVM
    // TR: Düzgün ECU kapatma — CAN arabelleklerini boşalt, NVM'yi kaydet
    std::cout << "  [ecu_shutdown] ECU shutdown complete\n";
}

int ecu_read_sensor(int channel) {
    // EN: Read raw sensor value via C-linkage API (no mangling)
    // TR: C bağlantılı API ile ham sensör değerini oku (karıştırma yok)
    int value = 2048 + channel * 17;
    std::cout << "  [ecu_read_sensor] Channel " << channel
              << " → value: " << value << "\n";
    return value;
}

} // extern "C"

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: C-Style Callback Pattern — Diagnostic Service
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Function pointer typedef for diagnostic callbacks (C-compatible signature)
// TR: Tanılama geri çağrıları için fonksiyon işaretçisi typedef'i (C uyumlu imza)
typedef void (*DiagCallback)(int code, const char* msg);

// EN: Global callback storage (simulates registration in a diagnostic manager)
// TR: Global geri çağrı deposu (tanı yöneticisinde kayıt işlemini simüle eder)
static DiagCallback g_diag_callback = nullptr;

void registerCallback(DiagCallback cb) {
    // EN: Register a diagnostic callback function pointer
    // TR: Bir tanılama geri çağrı fonksiyon işaretçisini kaydet
    g_diag_callback = cb;
    std::cout << "  [registerCallback] Callback registered at address: "
              << reinterpret_cast<void*>(cb) << "\n";
}

void triggerDiagnostic(int fault_code, const char* description) {
    // EN: Trigger the registered diagnostic callback if available
    // TR: Varsa kayıtlı tanılama geri çağrısını tetikle
    if (g_diag_callback) {
        g_diag_callback(fault_code, description);
    } else {
        std::cout << "  [triggerDiagnostic] No callback registered!\n";
    }
}

// EN: Sample callback implementations (would be in separate modules in production)
// TR: Örnek geri çağrı uygulamaları (üretimde ayrı modüllerde olurdu)
void onDtcDetected(int code, const char* msg) {
    std::printf("  [DTC Handler] Fault 0x%04X: %s\n", code, msg);
}

void onDtcLogged(int code, const char* msg) {
    std::printf("  [DTC Logger]  Fault 0x%04X stored to NVM: %s\n", code, msg);
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: Simulated Plugin System — Function Pointer Dispatch Table
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Plugin entry point signature — compatible with dlsym/GetProcAddress patterns
// TR: Eklenti giriş noktası imzası — dlsym/GetProcAddress kalıplarıyla uyumlu
using PluginFunc = int (*)(const char* command);

// EN: Simulated plugin implementations (in real systems, loaded from .so files)
// TR: Simüle edilmiş eklenti uygulamaları (gerçek sistemlerde .so dosyalarından yüklenir)
int pluginDiagnostics(const char* command) {
    std::cout << "  [Plugin:Diagnostics] Executing: " << command << "\n";
    return 0;
}

int pluginFlasher(const char* command) {
    std::cout << "  [Plugin:Flasher] Executing: " << command << "\n";
    return 0;
}

int pluginCalibration(const char* command) {
    std::cout << "  [Plugin:Calibration] Executing: " << command << "\n";
    return 0;
}

// EN: Plugin dispatch table — maps slot indices to function pointers
// TR: Eklenti dağıtım tablosu — yuva indekslerini fonksiyon işaretçilerine eşler
struct PluginEntry {
    const char*  name;
    PluginFunc   func;
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstration Scenarios
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "══════════════════════════════════════════════════════\n";
    std::cout << " ABI & Name Mangling — Automotive ECU Examples\n";
    std::cout << "══════════════════════════════════════════════════════\n\n";

    // ─── Demo 1: Overloaded Function Calls ───────────────────────────────────────────────────────
    // EN: Each call resolves to a different mangled symbol at link time
    // TR: Her çağrı bağlama zamanında farklı bir karıştırılmış sembole çözümlenir
    std::cout << "── Demo 1: Overloaded readSensor() calls ──\n";
    readSensor(3);
    readSensor(3, 1.05f);
    readSensor(std::string("throttle_position"));
    std::cout << "\n";

    // ─── Demo 2: extern "C" Function Calls ───────────────────────────────────────────────────────
    // EN: These symbols have no mangling — plain C names in the symbol table
    // TR: Bu sembollerin karıştırması yok — sembol tablosunda düz C adları
    std::cout << "── Demo 2: extern \"C\" ECU API calls ──\n";
    int init_status = ecu_init();
    std::cout << "  Init status: " << init_status << "\n";
    ecu_read_sensor(0);
    ecu_read_sensor(5);
    ecu_shutdown();
    std::cout << "\n";

    // ─── Demo 3: Register and Trigger C-Style Callbacks ──────────────────────────────────────────
    // EN: Function pointer callbacks — standard pattern for C-compatible diagnostic interfaces
    // TR: Fonksiyon işaretçisi geri çağrıları — C uyumlu tanı arayüzleri için standart kalıp
    std::cout << "── Demo 3: Diagnostic callback registration ──\n";
    registerCallback(onDtcDetected);
    triggerDiagnostic(0xC410, "Catalyst temperature above threshold");

    registerCallback(onDtcLogged);
    triggerDiagnostic(0xB112, "CAN bus off — ECU communication lost");
    std::cout << "\n";

    // ─── Demo 4: Function Pointer Table (Simulated Plugin Dispatch) ──────────────────────────────
    // EN: In real systems, these pointers come from dlsym() on loaded .so files.
    // TR: Gerçek sistemlerde bu işaretçiler yüklenen .so dosyalarındaki dlsym()'den gelir.
    std::cout << "── Demo 4: Plugin dispatch table ──\n";
    std::array<PluginEntry, 3> plugins = {{
        {"Diagnostics",  pluginDiagnostics},
        {"Flasher",      pluginFlasher},
        {"Calibration",  pluginCalibration}
    }};

    for (const auto& plugin : plugins) {
        std::cout << "  Dispatching to plugin [" << plugin.name << "]:\n";
        int result = plugin.func("self_test");
        std::cout << "    → Return code: " << result << "\n";
    }
    std::cout << "\n";

    // ─── Demo 5: typeid Names of Different Overloads ─────────────────────────────────────────────
    // EN: typeid reveals the compiler's internal type representation for each overload.
    // TR: typeid her aşırı yükleme için derleyicinin dahili tür temsilini gösterir.
    std::cout << "── Demo 5: typeid of overloaded function pointers ──\n";
    int   (*fp_int)(int)                    = readSensor;
    float (*fp_int_float)(int, float)       = readSensor;
    int   (*fp_string)(const std::string&)  = readSensor;

    std::cout << "  readSensor(int)          → typeid: "
              << typeid(fp_int).name() << "\n";
    std::cout << "  readSensor(int, float)   → typeid: "
              << typeid(fp_int_float).name() << "\n";
    std::cout << "  readSensor(string)       → typeid: "
              << typeid(fp_string).name() << "\n";
    std::cout << "\n  Addresses (all distinct due to mangling):\n";
    std::cout << "    readSensor(int):        " << reinterpret_cast<void*>(fp_int) << "\n";
    std::cout << "    readSensor(int, float): " << reinterpret_cast<void*>(fp_int_float) << "\n";
    std::cout << "    readSensor(string):     " << reinterpret_cast<void*>(fp_string) << "\n";

    std::cout << "\n══════════════════════════════════════════════════════\n";
    std::cout << " Tip: compile and inspect mangled symbols with:\n";
    std::cout << "   nm ./06_abi_and_name_mangling | grep readSensor\n";
    std::cout << "   nm ./06_abi_and_name_mangling | grep ecu_ \n";
    std::cout << "   nm ./06_abi_and_name_mangling | c++filt | grep readSensor\n";
    std::cout << "══════════════════════════════════════════════════════\n";

    return 0;
}
