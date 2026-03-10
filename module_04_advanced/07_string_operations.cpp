/**
 * @file module_04_advanced/07_string_operations.cpp
 * @brief Templates & STL: String Operations & string_view / Şablonlar ve STL: String İşlemleri
 * ve string_view
 *
 * Konu: std::string deep dive, searching, replacing, splitting, formatting, std::string_view
 * (C++17), small string optimization (SSO) std::string derinlemesine, arama, değiştirme, bölme,
 * biçimlendirme, std::string_view (C++17), küçük string optimizasyonu (SSO)
 *
 * Standart: C++17 Derleme: g++ -std=c++17 -Wall -Wextra -Wpedantic 07_string_operations.cpp
 *
 * @details
 * =====================================================================
 * [THEORY: std::string Internals & string_view / TEORİ: std::string İç Yapısı ve string_view]
 * =====================================================================
 *
 * ───── std::string Memory Model (Bellek Modeli) ─────
 * EN: std::string is a dynamic char array with: - Heap-allocated buffer (for strings > SSO
 * threshold) - size(): number of characters - capacity(): allocated buffer size (can be > size)
 * - SSO (Small String Optimization): strings ≤ ~15-22 chars (implementation-dependent) are
 * stored INSIDE the object itself — no heap allocation! This makes short strings very fast.
 *
 * TR: std::string, şu özelliklere sahip dinamik bir char dizisidir: - Heap tahsisli tampon (SSO
 * eşiğinden büyük stringler için) - size(): karakter sayısı - capacity(): tahsis edilmiş tampon
 * boyutu (size'dan büyük olabilir) - SSO (Küçük String Optimizasyonu): ≤ ~15-22 karakter
 * stringler (uygulamaya bağlı) nesnenin KENDİSİNDE depolanır — heap tahsisi yok! Bu kısa
 * stringleri çok hızlı yapar.
 *
 * ───── std::string_view (C++17) ─────
 * EN: A non-owning, read-only view into a string. Like a "window" that points to someone else's
 * data without copying it. ✓ No allocation, no copy — just pointer + size ✓ Can view
 * std::string, const char*, string literals ✓ Perfect for function parameters that only READ
 * strings ✗ DANGER: The viewed string must outlive the string_view! If the original string is
 * destroyed → dangling view!
 *
 * TR: Stringe sahiplik almayan, salt okunur bir görünüm. Kopyalamadan başkasının verisine işaret
 * eden bir "pencere" gibi. ✓ Tahsis yok, kopya yok — sadece pointer + boyut ✓ std::string, const
 * char*, string sabitleri görüntüleyebilir ✓ Sadece string OKUYAN fonksiyon parametreleri için
 * mükemmel ✗ TEHLİKE: Görüntülenen string, string_view'dan uzun yaşamalı! Orijinal string yok
 * edilirse → sarkan (dangling) görünüm!
 * =====================================================================
 *
 * [CPPREF DEPTH: std::string SSO — Small String Optimization / CPPREF DERİNLİK: std::string SSO
 * — Küçük String Optimizasyonu]
 * =============================================================================
 * EN: Most standard library implementations use SSO (Small String Optimization): strings up to
 * ~15-22 characters (implementation- dependent) are stored IN-PLACE within the std::string
 * object itself — no heap allocation at all. Only when the string exceeds this threshold does a
 * heap allocation occur. Typical layout: `sizeof(std::string)` is 32 bytes (GCC/libstdc++) with
 * SSO buffer of 15 chars + null terminator. `std::string_view` (C++17) is a non-owning view:
 * just a pointer + length. O(1) construction, never allocates, never copies. DANGER: the viewed
 * data MUST outlive the string_view! Returning a string_view to a local std::string → dangling
 * view → UB. This is one of the most common C++17 bugs. `std::string::reserve(n)` pre-allocates
 * n bytes of capacity. `std::string::shrink_to_fit()` is a non-binding request to free excess
 * capacity. C++20 adds `constexpr std::string` — compile-time string operations with transient
 * allocation.
 *
 * TR: Çoğu standart kütüphane SSO (Small String Optimization) kullanır: ~15-22 karaktere kadar
 * olan stringler std::string nesnesinin KENDİSİNDE saklanır — hiç heap tahsisi yapılmaz.
 * Yalnızca bu eşik aşıldığında heap'ten bellek ayrılır. Tipik düzen: `sizeof(std::string)`
 * GCC/libstdc++ ile 32 bayttır, 15 karakter + null sonlandırıcı SSO tamponuyla.
 * `std::string_view` (C++17) sahipsiz bir görünümdür: sadece bir işaretçi + uzunluk. O(1) inşa,
 * asla tahsis etmez, asla kopyalamaz. TEHLİKE: görüntülenen veri, string_view'dan UZUN YAŞAMALI!
 * Yerel bir std::string'e string_view döndürmek → sarkan görünüm → tanımsız davranış. Bu, en
 * yaygın C++17 hatalarından biridir. `reserve(n)` n bayt kapasite önceden ayırır.
 * `shrink_to_fit()` fazla kapasiteyi serbest bırakma isteğidir (bağlayıcı değil). C++20
 * `constexpr std::string` ekler — derleme zamanı string işlemleri.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>    // EN: STL algorithms
#include <cctype>       // EN: std::toupper, std::tolower, std::isdigit
#include <iostream>     // EN: Standard I/O
#include <sstream>      // EN: std::istringstream, std::ostringstream
#include <string>       // EN: std::string
#include <string_view>  // EN: std::string_view (C++17)
#include <vector>       // EN: std::vector

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [PRODUCTION CODE / ÜRETİM KODU]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 1. String Splitting (Tokenization) ──────────────────────────────────────────────────────────

// EN: Split a CAN bus log line by delimiter. No regex needed — just std::string::find() and
// substr(). This is the most common string operation in embedded log parsing.
// TR: CAN bus log satırını ayırıcıya göre böl. Regex gerekmez — sadece std::string::find() ve
// substr(). Gömülü log ayrıştırmada en yaygın string işlemidir.
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = str.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

// ─── 2. String Replace All ───────────────────────────────────────────────────────────────────────

// EN: Replace all occurrences of a substring. Useful for sanitizing diagnostic messages before
// logging.
// TR: Bir alt dizenin tüm tekrarlarını değiştir. Tanılama mesajlarını loglama öncesi temizlemek
// için kullanışlı.
std::string replaceAll(std::string str, const std::string& from,
                       const std::string& to) {
    if (from.empty()) return str;
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

// ─── 3. string_view Utility Functions ────────────────────────────────────────────────────────────

// EN: Extract the DTC category from a code like "P0301". Uses string_view to avoid any
// allocation — just creates a view into the first character.
// TR: "P0301" gibi bir koddan DTC kategorisini çıkar. Herhangi bir tahsitten kaçınmak için
// string_view kullanır — sadece ilk karaktere bir görünüm oluşturur.
std::string_view getDTCCategory(std::string_view code) {
    // EN: P=Powertrain, B=Body, C=Chassis, U=Network
    // TR: P=Motor/Şanzıman, B=Gövde, C=Şasi, U=Ağ
    if (code.empty()) return "Unknown";
    switch (code[0]) {
        case 'P': return "Powertrain";
        case 'B': return "Body";
        case 'C': return "Chassis";
        case 'U': return "Network";
        default:  return "Unknown";
    }
}

// EN: Trim whitespace from both sides (using string_view — zero copy!)
// TR: Her iki taraftan boşlukları kes (string_view ile — sıfır kopya!)
std::string_view trim(std::string_view sv) {
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) {
        sv.remove_prefix(1);
    }
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.back()))) {
        sv.remove_suffix(1);
    }
    return sv;
}

// EN: Check if a VIN (Vehicle Identification Number) is valid format. Uses string_view as
// parameter — accepts string, const char*, literal.
// TR: VIN (Araç Kimlik Numarası) formatının geçerli olup olmadığını kontrol et. Parametre olarak
// string_view kullanır — string, const char*, sabit kabul eder.
bool isValidVIN(std::string_view vin) {
    if (vin.size() != 17) return false;
    // EN: VIN must be alphanumeric and not contain I, O, Q
    // TR: VIN alfanümerik olmalı ve I, O, Q içermemeli
    return std::all_of(vin.begin(), vin.end(), [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) &&
               c != 'I' && c != 'O' && c != 'Q';
    });
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [EXAMPLES / ÖRNEKLER]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== MODULE 4: STRING OPERATIONS & STRING_VIEW ===\n"
              << "=== MODÜL 4: STRİNG İŞLEMLERİ VE STRİNG_VIEW ===\n\n";

    // ─── 1. Basic String Operations ──────────────────────────────────────────────────────────────
    {
        std::cout << "--- 1. Basic String Operations ---\n";

        std::string vin = "1G1YY22G965";

        // EN: Append, concatenate
        // TR: Ekleme, birleştirme
        vin += "123456";
        std::cout << "VIN: " << vin << " (length=" << vin.size() << ")\n";

        // EN: substr — extract a portion
        // TR: substr — bir bölüm çıkar
        std::string wmi = vin.substr(0, 3);     // World Manufacturer ID
        std::string vds = vin.substr(3, 5);     // Vehicle Descriptor Section
        std::cout << "WMI: " << wmi << ", VDS: " << vds << "\n";

        // EN: compare
        // TR: karşılaştır
        std::string other = "1G1YY22G965123456";
        std::cout << "Equal? " << (vin == other ? "yes" : "no") << "\n";
        std::cout << "Compare: " << vin.compare(other) << "\n\n";
    }

    // ─── 2. Search Operations ────────────────────────────────────────────────────────────────────
    {
        std::cout << "--- 2. String Search ---\n";

        std::string logLine = "[2026-03-06 14:22:05] ECM P0301: Cylinder 1 Misfire Detected";

        // EN: find — returns index of first occurrence (or npos)
        // TR: find — ilk bulunma indeksini döndürür (veya npos)
        auto dtcPos = logLine.find("P0");
        if (dtcPos != std::string::npos) {
            std::string dtcCode = logLine.substr(dtcPos, 5);
            std::cout << "Found DTC: " << dtcCode << " at position " << dtcPos << "\n";
        }

        // EN: rfind — search backwards (find LAST occurrence)
        // TR: rfind — geriye doğru ara (SON tekrarı bul)
        auto lastColon = logLine.rfind(':');
        std::cout << "Text after last colon: " << logLine.substr(lastColon + 2) << "\n";

        // EN: find_first_of — find any character from a set
        // TR: find_first_of — bir kümeden herhangi bir karakteri bul
        auto firstDigit = logLine.find_first_of("0123456789");
        std::cout << "First digit at index: " << firstDigit << "\n";

        // EN: find_first_not_of — find first char NOT in set
        // TR: find_first_not_of — kümede OLMAYAN ilk karakteri bul
        std::string padded = "   trimmed text   ";
        auto contentStart = padded.find_first_not_of(' ');
        auto contentEnd = padded.find_last_not_of(' ');
        std::cout << "Trimmed: \"" << padded.substr(contentStart, contentEnd - contentStart + 1)
                  << "\"\n\n";
    }

    // ─── 3. Modification Operations ──────────────────────────────────────────────────────────────
    {
        std::cout << "--- 3. String Modification ---\n";

        std::string msg = "Engine temperature is CRITICAL at 120C";

        // EN: replace a range
        // TR: bir aralığı değiştir
        msg.replace(msg.find("CRITICAL"), 8, "WARNING");
        std::cout << "After replace: " << msg << "\n";

        // EN: erase characters
        // TR: karakterleri sil
        std::string serial = "SN-12345-AB";
        serial.erase(0, 3);  // Remove "SN-"
        std::cout << "Erased prefix: " << serial << "\n";

        // EN: insert at position
        // TR: pozisyona ekle
        serial.insert(0, "VH-");
        std::cout << "Inserted prefix: " << serial << "\n";

        // EN: Transform to uppercase using std::transform
        // TR: std::transform ile büyük harfe dönüştür
        std::string ecuName = "engine_control_module";
        std::transform(ecuName.begin(), ecuName.end(), ecuName.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        std::cout << "Uppercase: " << ecuName << "\n\n";
    }

    // ─── 4. Split / Tokenize ─────────────────────────────────────────────────────────────────────
    {
        std::cout << "--- 4. Split (tokenize) ---\n";

        // EN: Parse a CAN log line: "timestamp|ECU_ID|data_bytes"
        // TR: CAN log satırını ayrıştır: "zaman_damgası|ECU_ID|veri_baytları"
        std::string canLog = "14:22:05.123|0x7E0|03 41 0D 7A";

        auto parts = split(canLog, '|');
        std::cout << "Timestamp: " << parts[0] << "\n";
        std::cout << "ECU ID:    " << parts[1] << "\n";
        std::cout << "Data:      " << parts[2] << "\n";

        // EN: Alternative: split with std::istringstream
        // TR: Alternatif: std::istringstream ile böl
        std::string csv = "85,120,72,91,68";
        std::istringstream iss(csv);
        std::string token;
        std::cout << "CSV values: ";
        while (std::getline(iss, token, ',')) {
            std::cout << token << " ";
        }
        std::cout << "\n\n";
    }

    // ─── 5. Replace All ──────────────────────────────────────────────────────────────────────────
    {
        std::cout << "--- 5. Replace All ---\n";

        std::string template_msg = "Vehicle {VIN} has {DTC_COUNT} active DTCs";
        std::string result = replaceAll(template_msg, "{VIN}", "1G1YY22G965123456");
        result = replaceAll(result, "{DTC_COUNT}", "3");
        std::cout << "Formatted: " << result << "\n\n";
    }

    // ─── 6. std::string_view — Zero-Copy Reading ─────────────────────────────────────────────────
    {
        std::cout << "--- 6. std::string_view (zero-copy) ---\n";

        // EN: string_view doesn't allocate — just points to existing data
        // TR: string_view tahsis etmez — mevcut veriye işaret eder
        std::string fullLog = "[2026-03-06 14:22:05] P0301 Cylinder 1 Misfire";

        // EN: Create views into different parts WITHOUT copying
        // TR: Kopyalamadan farklı bölümlere görünümler oluştur
        std::string_view dateView(fullLog.data() + 1, 10);   // "2026-03-06"
        std::string_view timeView(fullLog.data() + 12, 8);   // "14:22:05"
        std::string_view codeView(fullLog.data() + 22, 5);   // "P0301"

        std::cout << "Date: " << dateView << "\n";
        std::cout << "Time: " << timeView << "\n";
        std::cout << "Code: " << codeView << "\n";
        std::cout << "Category: " << getDTCCategory(codeView) << "\n\n";

        // EN: string_view works with string literals too (no std::string needed!)
        // TR: string_view string sabitleri ile de çalışır (std::string gerekmez!)
        std::cout << "P0420 category: " << getDTCCategory("P0420") << "\n";
        std::cout << "B0100 category: " << getDTCCategory("B0100") << "\n";
        std::cout << "U0100 category: " << getDTCCategory("U0100") << "\n\n";
    }

    // ─── 7. string_view — trim and validate ──────────────────────────────────────────────────────
    {
        std::cout << "--- 7. string_view trim & validate ---\n";

        std::string_view padded = "   Hello World   ";
        std::cout << "Before trim: \"" << padded << "\"\n";
        std::cout << "After trim:  \"" << trim(padded) << "\"\n";

        // EN: VIN validation using string_view (no copies!)
        // TR: string_view ile VIN doğrulama (kopya yok!)
        std::cout << "VIN 'WBA3A5G59DNP12345' valid? "
                  << (isValidVIN("WBA3A5G59DNP12345") ? "yes" : "no") << "\n";
        std::cout << "VIN 'INVALID_SHORT' valid? "
                  << (isValidVIN("INVALID_SHORT") ? "yes" : "no") << "\n";
        std::cout << "VIN 'WBAOI5G59DNP12345' valid? "  // Contains 'O' and 'I'
                  << (isValidVIN("WBAOI5G59DNP12345") ? "yes" : "no") << "\n\n";
    }

    // ─── 8. std::ostringstream — String Building ─────────────────────────────────────────────────
    {
        std::cout << "--- 8. std::ostringstream (string builder) ---\n";

        // EN: Build a formatted diagnostic report without repeated concatenation
        // TR: Tekrarlanan birleştirme olmadan biçimlendirilmiş teşhis raporu oluştur
        std::ostringstream report;
        report << "=== DIAGNOSTIC REPORT ===\n";
        report << "VIN: 1G1YY22G965123456\n";
        report << "DTCs: P0301, P0420\n";
        report << "Battery: " << 12.6 << "V\n";
        report << "Oil Temp: " << 92 << "°C\n";

        std::string finalReport = report.str();
        std::cout << finalReport;
        std::cout << "Report size: " << finalReport.size() << " bytes\n\n";
    }

    // ─── 9. SSO (Small String Optimization) Proof ────────────────────────────────────────────────
    {
        std::cout << "--- 9. SSO (Small String Optimization) ---\n";

        // EN: Likely SSO (no heap)
        // TR: Muhtemelen SSO (heap yok)
        std::string shortStr = "Hi";
        std::string longStr = "This is a much longer string that exceeds SSO threshold";

        std::cout << "Short string:\n";
        std::cout << "  size=" << shortStr.size()
                  << " capacity=" << shortStr.capacity()
                  << " sizeof=" << sizeof(shortStr) << "\n";

        std::cout << "Long string:\n";
        std::cout << "  size=" << longStr.size()
                  << " capacity=" << longStr.capacity()
                  << " sizeof=" << sizeof(longStr) << "\n";

        // EN: sizeof is always the SAME (typically 32 bytes on 64-bit). Short strings fit inside
        // those 32 bytes (SSO). Long strings allocate on the heap — capacity > sizeof.
        // TR: sizeof HER ZAMAN AYNI (genellikle 64-bit'te 32 byte). Kısa stringler bu 32 byte'a
        // sığar (SSO). Uzun stringler heap'te tahsis edilir — capacity > sizeof.
        std::cout << "\n";
    }

    // ─── 10. String Comparison and starts_with (C++20 backport) ──────────────────────────────────
    {
        std::cout << "--- 10. Prefix/Suffix Check ---\n";

        std::string dtc = "P0301";

        // EN: C++17 approach: compare substr
        // TR: C++17 yaklaşımı: substr karşılaştır
        bool isPowertrain = (dtc.compare(0, 1, "P") == 0);
        bool isMisfire = (dtc.compare(dtc.size() - 2, 2, "01") == 0);

        std::cout << dtc << " isPowertrain? " << (isPowertrain ? "yes" : "no") << "\n";
        std::cout << dtc << " isMisfire? " << (isMisfire ? "yes" : "no") << "\n";

        // EN: Using string_view for efficient prefix matching
        // TR: Verimli önek eşleştirme için string_view kullanma
        std::vector<std::string> allDTCs = {"P0301", "P0420", "B0100", "C0035", "U0100"};
        std::cout << "Powertrain codes: ";
        for (const auto& code : allDTCs) {
            std::string_view sv(code);
            if (!sv.empty() && sv[0] == 'P') {
                std::cout << code << " ";
            }
        }
        std::cout << "\n";
    }

    std::cout << "\n=== STRING OPERATIONS COMPLETE ===\n";
    return 0;
}
