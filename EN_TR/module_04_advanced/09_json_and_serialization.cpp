/**
 * @file module_04_advanced/09_json_and_serialization.cpp
 * @brief JSON & Serialization: Veri Serileştirme ve JSON işleme — Data Serialization & JSON Processing
 *
 * @details
 * =============================================================================
 * [THEORY: What Is Serialization? / TEORİ: Serileştirme Nedir?]
 * =============================================================================
 *
 * EN: Serialization is the process of converting in-memory data structures into a
 *     format that can be stored (file, database) or transmitted (network, IPC).
 *     Deserialization is the reverse: reconstructing objects from that format.
 *
 *     ┌────────────────────────────────────────────────────────────────┐
 *     │ C++ Object  →  serialize()  →  bytes/string  →  file/network   │
 *     │ C++ Object  ←  deserialize() ←  bytes/string  ←  file/network  │
 *     └────────────────────────────────────────────────────────────────┘
 *
 *     Common formats:
 *     ┌──────────────┬────────────┬──────────────┬──────────────────────┐
 *     │ Format       │ Type       │ Human-read?  │ Use Case             │
 *     ├──────────────┼────────────┼──────────────┼──────────────────────┤
 *     │ JSON         │ Text       │ Yes          │ Config, REST APIs    │
 *     │ XML          │ Text       │ Yes          │ Enterprise, SOAP     │
 *     │ YAML         │ Text       │ Yes          │ Config files         │
 *     │ Protobuf     │ Binary     │ No           │ High-perf RPC (gRPC) │
 *     │ MessagePack  │ Binary     │ No           │ Compact JSON-like    │
 *     │ FlatBuffers  │ Binary     │ No           │ Zero-copy access     │
 *     │ Raw binary   │ Binary     │ No           │ Embedded, firmware   │
 *     └──────────────┴────────────┴──────────────┴──────────────────────┘
 *
 * TR: Serileştirme (serialization), bellekteki veri yapılarını saklanabilir
 *     (dosya, veritabanı) veya iletilebilir (ağ, IPC) bir formata dönüştürme
 *     işlemidir. Ters serileştirme (deserialization) ise tersidir.
 *
 *     ┌─────────────────────────────────────────────────────────────┐
 *     │ C++ Nesne  →  serialize()    →  bayt/string  →  dosya/ağ    │
 *     │ C++ Nesne  ←  deserialize()  ←  bayt/string  ←  dosya/ağ    │
 *     └─────────────────────────────────────────────────────────────┘
 *
 *     Yaygın formatlar:
 *     ┌──────────────┬────────────┬───────────────┬───────────────────────┐
 *     │ Format       │ Tür        │ İnsan okunur? │ Kullanım Alanı        │
 *     ├──────────────┼────────────┼───────────────┼───────────────────────┤
 *     │ JSON         │ Metin      │ Evet          │ Config, REST API'lar  │
 *     │ XML          │ Metin      │ Evet          │ Kurumsal, SOAP        │
 *     │ YAML         │ Metin      │ Evet          │ Config dosyaları      │
 *     │ Protobuf     │ İkili      │ Hayır         │ Yüksek perf. RPC(gRPC)│
 *     │ MessagePack  │ İkili      │ Hayır         │ Kompakt JSON-benzeri  │
 *     │ FlatBuffers  │ İkili      │ Hayır         │ Sıfır-kopya erişim    │
 *     │ Ham ikili    │ İkili      │ Hayır         │ Gömülü, firmware      │
 *     └──────────────┴────────────┴───────────────┴───────────────────────┘
 *
 * =============================================================================
 * [THEORY: JSON Format / TEORİ: JSON Formatı]
 * =============================================================================
 *
 * EN: JSON (JavaScript Object Notation) is the most common text serialization format.
 *     It has 6 value types:
 *
 *     - Object:  { "key": value, ... }     → maps to std::map / struct
 *     - Array:   [ value, value, ... ]      → maps to std::vector
 *     - String:  "hello"                    → maps to std::string
 *     - Number:  42, 3.14                   → maps to int, double
 *     - Boolean: true, false                → maps to bool
 *     - Null:    null                       → maps to std::nullopt
 *
 *     JSON is hierarchical — objects and arrays can nest arbitrarily deep.
 *     Whitespace is insignificant (can be minified or pretty-printed).
 *
 * TR: JSON (JavaScript Object Notation) en yaygın metin serileştirme formatıdır.
 *     6 değer tipi vardır:
 *     - Object:  { "anahtar": değer, ... }  → std::map / struct
 *     - Array:   [ değer, değer, ... ]       → std::vector
 *     - String:  "merhaba"                   → std::string
 *     - Number:  42, 3.14                    → int, double
 *     - Boolean: true, false                 → bool
 *     - Null:    null                        → std::nullopt
 *
 * =============================================================================
 * [THEORY: Manual vs Library / TEORİ: Elle vs Kütüphane]
 * =============================================================================
 *
 * EN: In production, use a library like nlohmann/json (header-only, modern C++):
 *       #include <nlohmann/json.hpp>
 *       using json = nlohmann::json;
 *       json j = {{"name", "ECU"}, {"temp", 85.3}};  // create
 *       std::string s = j.dump(2);                     // serialize (pretty)
 *       json parsed = json::parse(s);                  // deserialize
 *
 *     In this file, we implement serialization FROM SCRATCH to teach:
 *     1. How JSON mapping works conceptually
 *     2. How to design to_json / from_json patterns
 *     3. Binary serialization for embedded systems
 *     4. Common pitfalls (endianness, alignment, string escaping)
 *
 * TR: Üretimde nlohmann/json gibi kütüphane kullan (header-only, modern C++):
 *       #include <nlohmann/json.hpp>
 *       using json = nlohmann::json;
 *       json j = {{"name", "ECU"}, {"temp", 85.3}};  // oluştur
 *       std::string s = j.dump(2);                     // serileştir (güzel format)
 *       json parsed = json::parse(s);                  // ters serileştir
 *
 *     Bu dosyada öğretmek için serileştirmeyi SIFIRDAN uyguluoruz:
 *     1. JSON eşleştirmesinin kavramsal olarak nasıl çalıştığı
 *     2. to_json / from_json desenleri nasıl tasarlanır
 *     3. Gömülü sistemler için ikili serileştirme
 *     4. Yaygın tuzaklar (endianness, hizalama, string kaçış)
 *
 * =============================================================================
 * [THEORY: Binary Serialization Pitfalls / TEORİ: İkili Serileştirme Tuzakları]
 * =============================================================================
 *
 * EN: Direct memory dump (memcpy/write struct) seems easy but is DANGEROUS:
 *     1. ENDIANNESS: Big-endian vs little-endian machines interpret bytes differently
 *     2. PADDING: Compilers add padding bytes for alignment — struct size ≠ sum of fields
 *     3. POINTER MEMBERS: Pointers are addresses — meaningless in another process/machine
 *     4. std::string / std::vector: Internal pointers — cannot be memcpy'd
 *     5. VERSIONING: Adding a field breaks all old serialized data
 *
 *     Safe approach: Serialize field-by-field with explicit byte order.
 *
 * TR: Doğrudan bellek dökümü (memcpy/yapı yazma) kolay görünür ama TEHLİKELİDİR:
 *     1. ENDIANNESS: Big/little-endian makineler baytları farklı yorumlar
 *     2. PADDING: Derleyiciler hizalama için dolgu baytları ekler
 *     3. POINTER ÜYELERİ: İşaretçiler adrestir — başka işlem/makinede anlamsız
 *     4. std::string / std::vector: İç işaretçiler — memcpy yapılamaz
 *     5. VERSİYONLAMA: Alan eklemek tüm eski serileştirilmiş veriyi bozar
 *
 * @see https://github.com/nlohmann/json — nlohmann/json (industry standard)
 * @see https://en.cppreference.com/w/cpp/numeric/byteswap — C++23 std::byteswap
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 09_json_and_serialization.cpp -o 09_json_and_serialization
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <cstring>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <fstream>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Simple JSON Builder (Serialization)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A minimal JSON builder that constructs JSON strings from C++ data.
 *     This teaches the PATTERN used by nlohmann/json without the full library.
 *     
 *     Design: Uses std::variant to represent JSON values (type-safe union).
 *     This is similar to how real JSON libraries work internally.
 *
 * TR: C++ verilerinden JSON string'leri oluşturan minimal JSON oluşturucu.
 *     Tam kütüphane olmadan nlohmann/json'ın KALIBINI öğretir.
 *     Tasarım: JSON değerlerini temsil etmek için std::variant kullanır (tip-güvenli birlik).
 */
class JsonValue {
public:
    // EN: The 6 JSON types represented as a std::variant
    //     std::monostate represents JSON null (no value)
    // TR: std::variant olarak temsil edilen 6 JSON tipi
    //     std::monostate JSON null'ı temsil eder (değer yok)
    using Null   = std::monostate;
    using Bool   = bool;
    using Int    = int64_t;
    using Float  = double;
    using String = std::string;
    using Array  = std::vector<JsonValue>;
    using Object = std::map<std::string, JsonValue>;

    using ValueType = std::variant<Null, Bool, Int, Float, String, Array, Object>;

    // EN: Default = null / TR: Varsayılan = null
    JsonValue() : value_(Null{}) {}

    // EN: Constructors for each JSON type
    // TR: Her JSON tipi için constructor'lar
    JsonValue(bool b) : value_(b) {}                         // NOLINT
    JsonValue(int i) : value_(static_cast<int64_t>(i)) {}    // NOLINT
    JsonValue(int64_t i) : value_(i) {}                      // NOLINT
    JsonValue(double d) : value_(d) {}                       // NOLINT
    JsonValue(const char* s) : value_(std::string(s)) {}     // NOLINT
    JsonValue(std::string s) : value_(std::move(s)) {}       // NOLINT
    JsonValue(Array a) : value_(std::move(a)) {}             // NOLINT
    JsonValue(Object o) : value_(std::move(o)) {}            // NOLINT

    // EN: Serialize to JSON string with optional indentation
    //     indent = -1 → compact (no whitespace)
    //     indent >= 0 → pretty-printed with that many spaces per level
    // TR: JSON string'e serileştir, opsiyonel girintileme ile
    //     indent = -1 → kompakt (boşluk yok)
    //     indent >= 0 → o kadar boşlukla güzel yazdırılmış
    std::string dump(int indent = -1) const {
        std::ostringstream oss;
        dump_impl(oss, indent, 0);
        return oss.str();
    }

    // EN: operator[] for object access — creates entries if they don't exist
    // TR: Nesne erişimi için operator[] — yoksa girdi oluşturur
    JsonValue& operator[](const std::string& key) {
        if (!std::holds_alternative<Object>(value_)) {
            value_ = Object{};
        }
        return std::get<Object>(value_)[key];
    }

    // EN: Type checking helpers
    // TR: Tip kontrol yardımcıları
    bool is_null() const { return std::holds_alternative<Null>(value_); }
    bool is_bool() const { return std::holds_alternative<Bool>(value_); }
    bool is_int() const { return std::holds_alternative<Int>(value_); }
    bool is_float() const { return std::holds_alternative<Float>(value_); }
    bool is_string() const { return std::holds_alternative<String>(value_); }
    bool is_array() const { return std::holds_alternative<Array>(value_); }
    bool is_object() const { return std::holds_alternative<Object>(value_); }

    // EN: Value getters (throw if wrong type)
    // TR: Değer alıcılar (yanlış tip ise fırlatır)
    template<typename T>
    const T& get() const { return std::get<T>(value_); }

private:
    ValueType value_;

    // EN: Escape special characters in JSON strings.
    //     JSON requires: \" \\ \/ \b \f \n \r \t and \uXXXX for control chars.
    //     Forgetting to escape = INVALID JSON = parser rejection.
    // TR: JSON string'lerinde özel karakterleri escape et.
    //     JSON şunları gerektirir: \" \\ \/ \b \f \n \r \t ve kontrol karakterleri \uXXXX.
    //     Escape unutmak = GEÇERSİZ JSON = parser reddi.
    static std::string escape_string(const std::string& s) {
        std::string result;
        result.reserve(s.size() + 2);
        result += '"';
        for (char c : s) {
            switch (c) {
                case '"':  result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b";  break;
                case '\f': result += "\\f";  break;
                case '\n': result += "\\n";  break;
                case '\r': result += "\\r";  break;
                case '\t': result += "\\t";  break;
                default:
                    if (static_cast<unsigned char>(c) < 0x20) {
                        // EN: Control character → \u00XX
                        // TR: Kontrol karakteri → \u00XX
                        char buf[8];
                        snprintf(buf, sizeof(buf), "\\u%04x",
                                 static_cast<unsigned int>(static_cast<unsigned char>(c)));
                        result += buf;
                    } else {
                        result += c;
                    }
            }
        }
        result += '"';
        return result;
    }

    void dump_impl(std::ostringstream& oss, int indent, int depth) const {
        std::string pad = (indent >= 0) ? std::string(static_cast<size_t>((depth + 1) * indent), ' ') : "";
        std::string pad_close = (indent >= 0) ? std::string(static_cast<size_t>(depth * indent), ' ') : "";
        std::string nl = (indent >= 0) ? "\n" : "";
        std::string sep = (indent >= 0) ? ", " : ",";

        std::visit([&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, Null>) {
                oss << "null";
            } else if constexpr (std::is_same_v<T, Bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, Int>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, Float>) {
                oss << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, String>) {
                oss << escape_string(val);
            } else if constexpr (std::is_same_v<T, Array>) {
                oss << "[" << nl;
                for (size_t i = 0; i < val.size(); ++i) {
                    oss << pad;
                    val[i].dump_impl(oss, indent, depth + 1);
                    if (i + 1 < val.size()) oss << ",";
                    oss << nl;
                }
                oss << pad_close << "]";
            } else if constexpr (std::is_same_v<T, Object>) {
                oss << "{" << nl;
                size_t i = 0;
                for (const auto& [key, jval] : val) {
                    oss << pad << escape_string(key) << ": ";
                    jval.dump_impl(oss, indent, depth + 1);
                    if (i + 1 < val.size()) oss << ",";
                    oss << nl;
                    ++i;
                }
                oss << pad_close << "}";
            }
        }, value_);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Struct ↔ JSON Mapping (to_json / from_json pattern)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: This is the PATTERN used by nlohmann/json for automatic struct conversion.
 *     You define free functions `to_json` and `from_json` for your types.
 *     The library then automatically converts:
 *       json j = my_struct;        → calls to_json
 *       auto s = j.get<MyStruct>(); → calls from_json
 *
 *     We replicate this pattern manually to teach the concept.
 *
 * TR: nlohmann/json'ın otomatik struct dönüşümü için kullandığı KALIPTIR.
 *     Tipleriniz için serbest `to_json` ve `from_json` fonksiyonları tanımlarsınız.
 */

// EN: Domain struct — represents a sensor reading
// TR: Domain yapısı — sensör okumasını temsil eder
struct SensorReading {
    std::string sensor_id;
    double temperature;
    uint32_t timestamp;
    bool is_critical;
    std::vector<double> history;  // EN: Last N readings / TR: Son N okuma
};

// EN: to_json: Convert SensorReading → JsonValue (serialization)
//     This maps each C++ field to a JSON key-value pair.
// TR: to_json: SensorReading → JsonValue dönüşümü (serileştirme)
//     Her C++ alanını bir JSON anahtar-değer çiftine eşler.
JsonValue to_json(const SensorReading& sr) {
    // EN: Build JSON array from vector
    // TR: Vector'den JSON dizisi oluştur
    JsonValue::Array history_arr;
    history_arr.reserve(sr.history.size());
    for (double h : sr.history) {
        history_arr.emplace_back(h);
    }

    // EN: Build JSON object from struct fields
    // TR: Struct alanlarından JSON nesnesi oluştur
    JsonValue::Object obj;
    obj["sensor_id"] = JsonValue(sr.sensor_id);
    obj["temperature"] = JsonValue(sr.temperature);
    obj["timestamp"] = JsonValue(static_cast<int64_t>(sr.timestamp));
    obj["is_critical"] = JsonValue(sr.is_critical);
    obj["history"] = JsonValue(std::move(history_arr));

    return JsonValue(std::move(obj));
}

// EN: ECU configuration struct — more complex example
// TR: ECU konfigürasyon yapısı — daha karmaşık örnek
struct ECUConfig {
    std::string module_name;
    std::string firmware_version;
    int can_bus_speed;           // EN: kbps / TR: kbps
    bool debug_mode;
    std::vector<std::string> enabled_features;
};

JsonValue to_json(const ECUConfig& cfg) {
    JsonValue::Array features;
    for (const auto& f : cfg.enabled_features) {
        features.emplace_back(f);
    }

    JsonValue::Object obj;
    obj["module_name"] = JsonValue(cfg.module_name);
    obj["firmware_version"] = JsonValue(cfg.firmware_version);
    obj["can_bus_speed_kbps"] = JsonValue(static_cast<int64_t>(cfg.can_bus_speed));
    obj["debug_mode"] = JsonValue(cfg.debug_mode);
    obj["enabled_features"] = JsonValue(std::move(features));

    return JsonValue(std::move(obj));
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Binary Serialization (Portable, Field-by-Field)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: BinaryWriter safely serializes data to a byte buffer.
 *     Unlike JSON (text-based, ~2x size overhead), binary is compact and fast.
 *     Used in embedded systems, game networking, and protocol implementations.
 *
 *     Key principle: ALWAYS write in a fixed byte order (we use little-endian).
 *     Never dump raw structs — serialization must be field-by-field.
 *
 * TR: BinaryWriter veriyi güvenle bir bayt tamponuna serileştirir.
 *     JSON'dan (metin tabanlı, ~2x boyut yükü) farklı olarak ikili kompakt ve hızlıdır.
 *     Gömülü sistemlerde, oyun ağında ve protokol uygulamalarında kullanılır.
 *
 *     Temel prensip: HER ZAMAN sabit bayt sırasında yaz (little-endian kullanıyoruz).
 *     Asla ham yapıları dökme — serileştirme alan-alan olmalı.
 */
class BinaryWriter {
public:
    // EN: Write a single byte
    // TR: Tek bayt yaz
    void write_u8(uint8_t val) {
        buffer_.push_back(val);
    }

    // EN: Write 16-bit in little-endian byte order
    //     Regardless of host CPU endianness, we always write LSB first.
    // TR: 16-bit'i little-endian bayt sırasında yaz
    //     Host CPU endianness'ından bağımsız, her zaman LSB önce yazılır.
    void write_u16(uint16_t val) {
        buffer_.push_back(static_cast<uint8_t>(val & 0xFF));         // EN: LSB / TR: En az anlamlı bayt
        buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));  // EN: MSB / TR: En anlamlı bayt
    }

    // EN: Write 32-bit in little-endian
    // TR: 32-bit'i little-endian'da yaz
    void write_u32(uint32_t val) {
        for (int i = 0; i < 4; ++i) {
            buffer_.push_back(static_cast<uint8_t>((val >> (i * 8)) & 0xFF));
        }
    }

    // EN: Write a double (IEEE 754, 8 bytes)
    //     We memcpy to uint64_t to get the raw bytes, then write in LE order.
    //     This is portable across architectures (unlike casting to char*).
    // TR: double yaz (IEEE 754, 8 bayt)
    //     Ham baytları almak için uint64_t'ye memcpy, sonra LE sırasında yaz.
    void write_f64(double val) {
        uint64_t bits;
        std::memcpy(&bits, &val, sizeof(bits));
        for (int i = 0; i < 8; ++i) {
            buffer_.push_back(static_cast<uint8_t>((bits >> (i * 8)) & 0xFF));
        }
    }

    // EN: Write a string: length prefix (u32) + raw UTF-8 bytes.
    //     Length prefix is crucial — binary format has no delimiter like JSON's quotes.
    //     Without length, the reader doesn't know where the string ends!
    // TR: String yaz: uzunluk öneki (u32) + ham UTF-8 baytları.
    //     Uzunluk öneki kritik — ikili formatın JSON'ın tırnakları gibi sınırlayıcısı yok.
    void write_string(const std::string& s) {
        write_u32(static_cast<uint32_t>(s.size()));
        for (char c : s) {
            buffer_.push_back(static_cast<uint8_t>(c));
        }
    }

    // EN: Write a bool as a single byte (0 = false, 1 = true)
    // TR: bool'u tek bayt olarak yaz (0 = false, 1 = true)
    void write_bool(bool val) {
        buffer_.push_back(val ? 1 : 0);
    }

    const std::vector<uint8_t>& data() const { return buffer_; }
    size_t size() const { return buffer_.size(); }

private:
    std::vector<uint8_t> buffer_;
};

/**
 * EN: BinaryReader reads data back from a byte buffer.
 *     Reads must happen in the EXACT SAME ORDER as writes!
 *     This is why binary protocols need strict documentation.
 *
 * TR: BinaryReader bayt tamponundan veriyi geri okur.
 *     Okumalar yazma ile TAMAMEN AYNI SIRADA yapılmalı!
 *     İkili protokollerin sıkı belgelemeye ihtiyacı bundandır.
 */
class BinaryReader {
public:
    explicit BinaryReader(const std::vector<uint8_t>& data) : data_(data), pos_(0) {}

    uint8_t read_u8() {
        check_bounds(1);
        return data_[pos_++];
    }

    uint16_t read_u16() {
        check_bounds(2);
        uint16_t val = static_cast<uint16_t>(data_[pos_])
                     | (static_cast<uint16_t>(data_[pos_ + 1]) << 8);
        pos_ += 2;
        return val;
    }

    uint32_t read_u32() {
        check_bounds(4);
        uint32_t val = 0;
        for (int i = 0; i < 4; ++i) {
            val |= static_cast<uint32_t>(data_[pos_ + static_cast<size_t>(i)]) << (i * 8);
        }
        pos_ += 4;
        return val;
    }

    double read_f64() {
        check_bounds(8);
        uint64_t bits = 0;
        for (int i = 0; i < 8; ++i) {
            bits |= static_cast<uint64_t>(data_[pos_ + static_cast<size_t>(i)]) << (i * 8);
        }
        pos_ += 8;
        double val;
        std::memcpy(&val, &bits, sizeof(val));
        return val;
    }

    std::string read_string() {
        uint32_t len = read_u32();
        check_bounds(len);
        std::string s(data_.begin() + static_cast<ptrdiff_t>(pos_),
                      data_.begin() + static_cast<ptrdiff_t>(pos_ + len));
        pos_ += len;
        return s;
    }

    bool read_bool() {
        return read_u8() != 0;
    }

    size_t position() const { return pos_; }
    size_t remaining() const { return data_.size() - pos_; }

private:
    const std::vector<uint8_t>& data_;
    size_t pos_;

    void check_bounds(size_t n) const {
        if (pos_ + n > data_.size()) {
            throw std::runtime_error("BinaryReader: buffer underflow at position "
                                     + std::to_string(pos_));
        }
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Binary Serialization for Domain Types
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Serialize SensorReading to binary format.
 *     Protocol: [sensor_id:string][temperature:f64][timestamp:u32][is_critical:bool]
 *               [history_count:u32][history[0]:f64][history[1]:f64]...
 *
 * TR: SensorReading'i ikili formata serileştir.
 *     Protokol: [sensor_id:string][temperature:f64][timestamp:u32][is_critical:bool]
 *               [history_count:u32][history[0]:f64][history[1]:f64]...
 */
void serialize_binary(BinaryWriter& writer, const SensorReading& sr) {
    writer.write_string(sr.sensor_id);
    writer.write_f64(sr.temperature);
    writer.write_u32(sr.timestamp);
    writer.write_bool(sr.is_critical);
    writer.write_u32(static_cast<uint32_t>(sr.history.size()));
    for (double h : sr.history) {
        writer.write_f64(h);
    }
}

SensorReading deserialize_binary(BinaryReader& reader) {
    SensorReading sr;
    sr.sensor_id = reader.read_string();
    sr.temperature = reader.read_f64();
    sr.timestamp = reader.read_u32();
    sr.is_critical = reader.read_bool();
    uint32_t history_count = reader.read_u32();
    sr.history.resize(history_count);
    for (uint32_t i = 0; i < history_count; ++i) {
        sr.history[i] = reader.read_f64();
    }
    return sr;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Simple JSON Parser (Deserialization)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A minimal recursive-descent JSON parser.
 *     Demonstrates how JSON parsing works internally:
 *     1. Tokenize: identify strings, numbers, keywords, delimiters
 *     2. Parse: recursively build a tree structure
 *
 *     Production code should use nlohmann/json or simdjson (SIMD-accelerated).
 *     This parser is educational — it handles the core cases but isn't battle-tested.
 *
 * TR: Minimal özyinelemeli-iniş JSON ayrıştırıcı.
 *     JSON ayrıştırmanın dahili olarak nasıl çalıştığını gösterir.
 *     Üretim kodu nlohmann/json veya simdjson (SIMD hızlandırmalı) kullanmalı.
 */
class JsonParser {
public:
    static JsonValue parse(const std::string& input) {
        size_t pos = 0;
        auto result = parse_value(input, pos);
        return result;
    }

private:
    static void skip_whitespace(const std::string& s, size_t& pos) {
        while (pos < s.size() && (s[pos] == ' ' || s[pos] == '\t' ||
               s[pos] == '\n' || s[pos] == '\r')) {
            ++pos;
        }
    }

    static JsonValue parse_value(const std::string& s, size_t& pos) {
        skip_whitespace(s, pos);
        if (pos >= s.size()) throw std::runtime_error("Unexpected end of input");

        char c = s[pos];
        if (c == '"') return parse_string(s, pos);
        if (c == '{') return parse_object(s, pos);
        if (c == '[') return parse_array(s, pos);
        if (c == 't' || c == 'f') return parse_bool(s, pos);
        if (c == 'n') return parse_null(s, pos);
        if (c == '-' || (c >= '0' && c <= '9')) return parse_number(s, pos);

        throw std::runtime_error(std::string("Unexpected character: ") + c);
    }

    static JsonValue parse_string(const std::string& s, size_t& pos) {
        if (s[pos] != '"') throw std::runtime_error("Expected '\"'");
        ++pos;
        std::string result;
        while (pos < s.size() && s[pos] != '"') {
            if (s[pos] == '\\') {
                ++pos;
                if (pos >= s.size()) throw std::runtime_error("Unexpected end in string escape");
                switch (s[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case 'r': result += '\r'; break;
                    default: result += s[pos]; break;
                }
            } else {
                result += s[pos];
            }
            ++pos;
        }
        if (pos >= s.size()) throw std::runtime_error("Unterminated string");
        ++pos; // skip closing "
        return JsonValue(std::move(result));
    }

    static JsonValue parse_number(const std::string& s, size_t& pos) {
        size_t start = pos;
        bool is_float = false;
        if (s[pos] == '-') ++pos;
        while (pos < s.size() && s[pos] >= '0' && s[pos] <= '9') ++pos;
        if (pos < s.size() && s[pos] == '.') { is_float = true; ++pos; }
        while (pos < s.size() && s[pos] >= '0' && s[pos] <= '9') ++pos;
        if (pos < s.size() && (s[pos] == 'e' || s[pos] == 'E')) {
            is_float = true; ++pos;
            if (pos < s.size() && (s[pos] == '+' || s[pos] == '-')) ++pos;
            while (pos < s.size() && s[pos] >= '0' && s[pos] <= '9') ++pos;
        }
        std::string num_str = s.substr(start, pos - start);
        if (is_float) return JsonValue(std::stod(num_str));
        return JsonValue(static_cast<int64_t>(std::stoll(num_str)));
    }

    static JsonValue parse_bool(const std::string& s, size_t& pos) {
        if (s.compare(pos, 4, "true") == 0) { pos += 4; return JsonValue(true); }
        if (s.compare(pos, 5, "false") == 0) { pos += 5; return JsonValue(false); }
        throw std::runtime_error("Invalid boolean");
    }

    static JsonValue parse_null(const std::string& s, size_t& pos) {
        if (s.compare(pos, 4, "null") == 0) { pos += 4; return JsonValue(); }
        throw std::runtime_error("Invalid null");
    }

    static JsonValue parse_array(const std::string& s, size_t& pos) {
        ++pos; // skip [
        skip_whitespace(s, pos);
        JsonValue::Array arr;
        if (pos < s.size() && s[pos] == ']') { ++pos; return JsonValue(std::move(arr)); }
        while (true) {
            arr.push_back(parse_value(s, pos));
            skip_whitespace(s, pos);
            if (pos < s.size() && s[pos] == ',') { ++pos; skip_whitespace(s, pos); }
            else break;
        }
        if (pos >= s.size() || s[pos] != ']') throw std::runtime_error("Expected ']'");
        ++pos;
        return JsonValue(std::move(arr));
    }

    static JsonValue parse_object(const std::string& s, size_t& pos) {
        ++pos; // skip {
        skip_whitespace(s, pos);
        JsonValue::Object obj;
        if (pos < s.size() && s[pos] == '}') { ++pos; return JsonValue(std::move(obj)); }
        while (true) {
            skip_whitespace(s, pos);
            auto key_val = parse_string(s, pos);
            std::string key = key_val.get<JsonValue::String>();
            skip_whitespace(s, pos);
            if (pos >= s.size() || s[pos] != ':') throw std::runtime_error("Expected ':'");
            ++pos;
            obj[key] = parse_value(s, pos);
            skip_whitespace(s, pos);
            if (pos < s.size() && s[pos] == ',') { ++pos; }
            else break;
        }
        skip_whitespace(s, pos);
        if (pos >= s.size() || s[pos] != '}') throw std::runtime_error("Expected '}'");
        ++pos;
        return JsonValue(std::move(obj));
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   JSON & Serialization\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: JSON Builder — Struct to JSON ───────────────────────────
    {
        std::cout << "--- Demo 1: Struct → JSON (Serialization) ---\n";

        // EN: Create a sensor reading and convert to JSON
        // TR: Sensör okuması oluştur ve JSON'a dönüştür
        SensorReading reading;
        reading.sensor_id = "EXHAUST_01";
        reading.temperature = 742.5;
        reading.timestamp = 1710300000;
        reading.is_critical = true;
        reading.history = {735.0, 738.2, 740.1, 742.5};

        JsonValue json = to_json(reading);

        // EN: dump(2) → pretty-print with 2 spaces indentation
        // TR: dump(2) → 2 boşluk girintileme ile güzel yazdır
        std::cout << json.dump(2) << "\n\n";

        // EN: dump(-1) → compact format (no whitespace, for transmission)
        // TR: dump(-1) → kompakt format (boşluk yok, iletim için)
        std::cout << "  Compact: " << json.dump() << "\n\n";
    }

    // ─── Demo 2: Nested JSON Object ──────────────────────────────────────
    {
        std::cout << "--- Demo 2: Nested JSON Object ---\n";

        // EN: Build a complex JSON structure with nesting
        // TR: İç içe karmaşık JSON yapısı oluştur
        ECUConfig config;
        config.module_name = "BrakeController";
        config.firmware_version = "2.4.1";
        config.can_bus_speed = 500;
        config.debug_mode = false;
        config.enabled_features = {"ABS", "ESC", "TCS", "HillAssist"};

        JsonValue json = to_json(config);
        std::cout << json.dump(2) << "\n\n";
    }

    // ─── Demo 3: JSON Parser (Deserialization) ───────────────────────────
    {
        std::cout << "--- Demo 3: JSON String → Parsed Object ---\n";

        // EN: Parse a JSON string and access its values
        // TR: JSON string'i ayrıştır ve değerlerine eriş
        std::string json_str = R"({
            "vehicle": "Model-X",
            "year": 2026,
            "electric": true,
            "range_km": 450.5,
            "sensors": ["lidar", "radar", "camera"],
            "config": {
                "autopilot": true,
                "level": 3
            }
        })";

        std::cout << "  Input JSON:\n" << json_str << "\n\n";

        JsonValue parsed = JsonParser::parse(json_str);
        std::cout << "  Re-serialized (pretty):\n" << parsed.dump(2) << "\n\n";

        // EN: Access nested values through the parsed structure
        // TR: Ayrıştırılmış yapı üzerinden iç içe değerlere eriş
        if (parsed.is_object()) {
            auto& obj = parsed.get<JsonValue::Object>();
            if (obj.count("vehicle") && obj.at("vehicle").is_string()) {
                std::cout << "  Vehicle: " << obj.at("vehicle").get<JsonValue::String>() << "\n";
            }
            if (obj.count("year") && obj.at("year").is_int()) {
                std::cout << "  Year: " << obj.at("year").get<JsonValue::Int>() << "\n";
            }
            if (obj.count("sensors") && obj.at("sensors").is_array()) {
                auto& sensors = obj.at("sensors").get<JsonValue::Array>();
                std::cout << "  Sensors (" << sensors.size() << "): ";
                for (size_t i = 0; i < sensors.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << sensors[i].get<JsonValue::String>();
                }
                std::cout << "\n";
            }
        }
        std::cout << "\n";
    }

    // ─── Demo 4: Binary Serialization ────────────────────────────────────
    {
        std::cout << "--- Demo 4: Binary Serialization ---\n";

        // EN: Serialize the same sensor reading to binary format
        // TR: Aynı sensör okumasını ikili formata serileştir
        SensorReading original;
        original.sensor_id = "OIL_TEMP";
        original.temperature = 92.3;
        original.timestamp = 1710300500;
        original.is_critical = false;
        original.history = {88.1, 89.5, 91.0, 92.3};

        BinaryWriter writer;
        serialize_binary(writer, original);

        std::cout << "  Binary size: " << writer.size() << " bytes\n";
        std::cout << "  JSON size:   " << to_json(original).dump().size() << " bytes\n";
        // EN: ↑ Binary is typically 2-5x smaller than JSON!
        // TR: ↑ İkili genellikle JSON'dan 2-5x daha küçüktür!

        // EN: Show hex dump of first 32 bytes
        // TR: İlk 32 baytın hex dökümünü göster
        std::cout << "  Hex dump (first 32 bytes): ";
        for (size_t i = 0; i < std::min<size_t>(32, writer.size()); ++i) {
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<int>(writer.data()[i]) << " ";
        }
        std::cout << std::dec << "\n";

        // EN: Deserialize from binary and verify
        // TR: İkiliden ters serileştir ve doğrula
        BinaryReader reader(writer.data());
        SensorReading restored = deserialize_binary(reader);

        std::cout << "\n  Restored from binary:\n";
        std::cout << "    sensor_id:   " << restored.sensor_id << "\n";
        std::cout << "    temperature: " << restored.temperature << "\n";
        std::cout << "    timestamp:   " << restored.timestamp << "\n";
        std::cout << "    is_critical: " << (restored.is_critical ? "true" : "false") << "\n";
        std::cout << "    history:     [";
        for (size_t i = 0; i < restored.history.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << restored.history[i];
        }
        std::cout << "]\n";

        // EN: Verify data integrity
        // TR: Veri bütünlüğünü doğrula
        bool match = (original.sensor_id == restored.sensor_id) &&
                     (original.temperature == restored.temperature) &&
                     (original.timestamp == restored.timestamp) &&
                     (original.is_critical == restored.is_critical) &&
                     (original.history == restored.history);
        std::cout << "    Integrity check: " << (match ? "PASS ✓" : "FAIL ✗") << "\n";
        std::cout << "    Reader consumed: " << reader.position() << " / "
                  << writer.size() << " bytes (remaining: " << reader.remaining() << ")\n\n";
    }

    // ─── Demo 5: String Escaping ─────────────────────────────────────────
    {
        std::cout << "--- Demo 5: JSON String Escaping ---\n";

        // EN: Demonstrate proper JSON string escaping
        //     Without escaping, special characters break JSON validity.
        // TR: JSON string escape'lemesini göster
        //     Escape olmadan özel karakterler JSON geçerliliğini bozar.
        JsonValue::Object obj;
        obj["path"] = JsonValue("C:\\Users\\nimbus\\data.json");
        obj["message"] = JsonValue("He said \"hello\" and left");
        obj["multiline"] = JsonValue("Line 1\nLine 2\tTabbed");

        JsonValue test(std::move(obj));
        std::cout << test.dump(2) << "\n\n";

        // EN: Parse it back to verify round-trip
        // TR: Gidiş-dönüş doğrulamak için geri ayrıştır
        std::string serialized = test.dump();
        JsonValue reparsed = JsonParser::parse(serialized);
        std::cout << "  Round-trip (parsed back):\n" << reparsed.dump(2) << "\n\n";
    }

    // ─── Demo 6: JSON vs Binary Size Comparison ──────────────────────────
    {
        std::cout << "--- Demo 6: Size Comparison (JSON vs Binary) ---\n";

        // EN: Create multiple readings and compare serialization sizes
        // TR: Birden fazla okuma oluşturup serileştirme boyutlarını karşılaştır
        std::vector<SensorReading> readings;
        readings.push_back({"TEMP_01", 85.3, 1710300000, false, {82.1, 83.5, 84.0}});
        readings.push_back({"TEMP_02", 92.1, 1710300001, true, {90.0, 91.2}});
        readings.push_back({"PRESSURE", 2.4, 1710300002, false, {2.3, 2.35, 2.38, 2.4}});

        // EN: JSON size
        // TR: JSON boyutu
        size_t json_total = 0;
        for (const auto& r : readings) {
            json_total += to_json(r).dump().size();
        }

        // EN: Binary size
        // TR: İkili boyut
        BinaryWriter batch_writer;
        for (const auto& r : readings) {
            serialize_binary(batch_writer, r);
        }

        std::cout << "  3 sensor readings:\n";
        std::cout << "    JSON total:   " << json_total << " bytes\n";
        std::cout << "    Binary total: " << batch_writer.size() << " bytes\n";
        double ratio = static_cast<double>(json_total) / static_cast<double>(batch_writer.size());
        std::cout << "    Ratio: JSON is " << std::fixed << std::setprecision(1)
                  << ratio << "x larger than binary\n";
        // EN: Binary is more efficient for embedded systems with limited bandwidth
        //     JSON is better for debugging, configs, and human-readable APIs
        // TR: İkili, sınırlı bant genişliğine sahip gömülü sistemler için daha verimli
        //     JSON, hata ayıklama, konfigürasyon ve insan tarafından okunabilir API'ler için daha iyi
        std::cout << "\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of JSON & Serialization\n";
    std::cout << "============================================\n";

    return 0;
}
