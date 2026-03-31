/**
 * @file module_11_embedded_systems/03_embedded_constraints.cpp
 * @brief Embedded & Systems: No-Heap Allocation, Static Buffers, constexpr, Placement New /
 * Yığınsız Tahsis, Statik Tamponlar, constexpr, Placement New
 *
 * @details
 * =============================================================================
 * [THEORY: No-Heap Programming — Why malloc/new Are Banned / TEORİ: Yığınsız Programlama —
 * malloc/new Neden Yasaklıdır]
 * =============================================================================
 * EN: In safety-critical embedded systems (automotive, medical, aerospace), dynamic memory
 *     allocation (malloc/new) is BANNED because:
 *     1. Heap fragmentation can exhaust memory unpredictably.
 *     2. Allocation time is non-deterministic — bad for real-time systems.
 *     3. Heap metadata overhead wastes scarce RAM.
 *     4. Memory leaks in long-running firmware are catastrophic.
 *     Instead, all memory is statically allocated at compile time using fixed-size
 *     arrays, stack variables, and memory pools (placement new).
 *
 * TR: Güvenlik-kritik gömülü sistemlerde (otomotiv, tıbbi, havacılık) dinamik bellek
 *     tahsisi (malloc/new) YASAKLIDIR çünkü:
 *     1. Yığın parçalanması belleği öngörülemez şekilde tüketebilir.
 *     2. Tahsis süresi belirleyici değildir — gerçek zamanlı sistemler icin kötü.
 *     3. Yığın meta veri ek yükü kıt RAM'i israf eder.
 *     4. Uzun süre çalışan donanım yazılımında bellek sızıntıları felakettir.
 *     Bunun yerine, tüm bellek derleme zamanında sabit boyutlu diziler, yığıt
 *     değişkenleri ve bellek havuzları (placement new) kullanılarak statik olarak
 *     tahsis edilir.
 *
 * =============================================================================
 * [THEORY: constexpr — Compile-Time Computation / TEORİ: constexpr — Derleme Zamanı Hesaplama]
 * =============================================================================
 * EN: `constexpr` forces computation to happen at compile time, producing zero
 *     runtime overhead. In embedded, this means:
 *     1. CRC/checksum lookup tables computed at compile time.
 *     2. Baud rate divisor calculations resolved before flashing.
 *     3. Trigonometric lookup tables baked into firmware ROM.
 *     C++17 allows `constexpr if` for compile-time branching, and C++20 allows
 *     `consteval` to GUARANTEE compile-time-only execution.
 *
 * TR: `constexpr` hesaplamayı derleme zamanında gerçekleşmeye zorlar, sıfır çalışma
 *     zamanı ek yükü üretir. Gömülü sistemlerde bu şu anlama gelir:
 *     1. CRC/sağlama toplamı arama tabloları derleme zamanında hesaplanır.
 *     2. Baud hızı bölücü hesaplamaları flaşlamadan önce çözümlenir.
 *     3. Trigonometrik arama tabloları donanım yazılımı ROM'una gömülür.
 *     C++17 derleme zamanı dallanma için `constexpr if`'e izin verir ve C++20
 *     yalnızca derleme zamanı yürütmeyi GARANTİ etmek için `consteval`'e izin verir.
 *
 * =============================================================================
 * [THEORY: Placement New — Object Construction in Pre-Allocated Memory / TEORİ: Placement New —
 * Önceden Tahsis Edilmiş Bellekte Nesne Oluşturma]
 * =============================================================================
 * EN: `placement new` constructs an object at a specific memory address without
 *     allocating. Syntax: `new (address) Type(args);`
 *     This is essential for:
 *     1. Memory pools with fixed-size blocks.
 *     2. DMA buffer management.
 *     3. Constructing objects on stack-allocated aligned buffers.
 *     You must call the destructor manually: `obj->~Type();`
 *
 * TR: `placement new` tahsis yapmadan belirli bir bellek adresinde nesne oluşturur.
 *     Sözdizimi: `new (adres) Tip(argümanlar);`
 *     Bu, şunlar için gereklidir:
 *     1. Sabit boyutlu bloklara sahip bellek havuzları.
 *     2. DMA tampon yönetimi.
 *     3. Yığıt-tahsisli hizalanmış tamponlarda nesne oluşturma.
 *     Yıkıcıyı manuel olarak çağırmanız gerekir: `obj->~Type();`
 *
 * =============================================================================
 * [CPPREF DEPTH: constexpr Specifier Rules and Limits / CPPREF DERİNLİK: constexpr Belirleyici
 * Kuralları ve Sınırları]
 * =============================================================================
 * EN: A `constexpr` function must:
 *     1. Have a literal return type.
 *     2. Not contain goto, asm, or non-literal variables (pre-C++20).
 *     C++14 relaxed most restrictions (loops, multiple statements).
 *     C++20 allows virtual constexpr, try-catch, dynamic_cast.
 *     `consteval` (C++20) guarantees the function is ONLY called at compile
 *     time — if forced at runtime, it's a compilation error.
 *     `constinit` ensures static storage duration variables are initialized
 *     at compile time.
 *     ⚠  cppreference.com/w/cpp/language/constexpr
 *
 * TR: Bir `constexpr` fonksiyon:
 *     1. Literal dönüş tipine sahip olmalı.
 *     2. goto, asm veya literal olmayan değişkenler içermemeli (C++20 öncesi).
 *     C++14 çoğu kısıtlamayı gevşetti (döngüler, çoklu ifadeler).
 *     C++20 sanal constexpr, try-catch, dynamic_cast'e izin verir.
 *     `consteval` (C++20) fonksiyonun YALNIZCA derleme zamanında
 *     çağrıldığını garanti eder.
 *     `constinit` statik depolama süresine sahip değişkenlerin derleme zamanında
 *     başlatılmasını sağlar.
 *     ⚠  cppreference.com/w/cpp/language/constexpr
 *
 * =============================================================================
 * [CPPREF DEPTH: Placement New and Object Lifetime / CPPREF DERİNLİK: Placement New ve Nesne
 * Yaşam Süresi]
 * =============================================================================
 * EN: Placement new does NOT allocate memory — it only calls the constructor
 *     at a given address. The programmer is responsible for:
 *     1. Ensuring proper alignment via `alignas` or `std::aligned_storage`.
 *     2. Manually calling the destructor before reusing memory.
 *     3. NOT calling `delete` on placement-new objects.
 *     `std::launder` (C++17) is needed after placement new to access the object
 *     through old pointers when the type has const/reference members.
 *     ⚠  cppreference.com/w/cpp/language/new#Placement_new
 *
 * TR: Placement new bellek TAHSİS ETMEZ — yalnızca verilen adreste yapıcıyı
 *     çağırır. Programcı şunlardan sorumludur:
 *     1. `alignas` veya `std::aligned_storage` ile uygun hizalamayı sağlama.
 *     2. Belleği yeniden kullanmadan önce yıkıcıyı manuel çağırma.
 *     3. Placement-new nesnelerinde `delete` ÇAĞIRMAMAK.
 *     `std::launder` (C++17) const/referans üyeleri olan türlerde eski işaretçiler
 *     aracılığıyla nesneye erişmek için placement new sonrası gereklidir.
 *     ⚠  cppreference.com/w/cpp/language/new#Placement_new
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_embedded_constraints.cpp -o 03_embedded_constraints
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstdint>
#include <cstring>
#include <iostream>
#include <new>      // placement new
#include <array>
#include <type_traits>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [1. STATIC RING BUFFER — No-Heap Circular Buffer]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: A fixed-size ring buffer using no dynamic allocation.
// TR: Dinamik tahsis kullanmayan sabit boyutlu halka tampon.
template <typename T, std::size_t N>
class StaticRingBuffer {
  std::array<T, N> buffer_{};
  std::size_t head_ = 0;
  std::size_t tail_ = 0;
  std::size_t count_ = 0;

public:
  bool push(const T& item) {
    if (count_ == N) return false; // Full
    buffer_[tail_] = item;
    tail_ = (tail_ + 1) % N;
    ++count_;
    return true;
  }

  bool pop(T& item) {
    if (count_ == 0) return false; // Empty
    item = buffer_[head_];
    head_ = (head_ + 1) % N;
    --count_;
    return true;
  }

  [[nodiscard]] std::size_t size() const { return count_; }
  [[nodiscard]] bool empty() const { return count_ == 0; }
  [[nodiscard]] bool full() const { return count_ == N; }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [2. CONSTEXPR — Compile-Time CRC-8 Table]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Generate a CRC-8 lookup table entirely at compile time.
// TR: CRC-8 arama tablosunu tamamen derleme zamanında oluştur.
constexpr uint8_t CRC8_POLY = 0x07;

constexpr std::array<uint8_t, 256> generateCRC8Table() {
  std::array<uint8_t, 256> table{};
  for (int i = 0; i < 256; ++i) {
    uint8_t crc = static_cast<uint8_t>(i);
    for (int bit = 0; bit < 8; ++bit) {
      if (crc & 0x80U) {
        crc = static_cast<uint8_t>((crc << 1) ^ CRC8_POLY);
      } else {
        crc = static_cast<uint8_t>(crc << 1);
      }
    }
    table[static_cast<std::size_t>(i)] = crc;
  }
  return table;
}

// EN: Table is fully resolved at compile time — zero runtime cost.
// TR: Tablo tamamen derleme zamanında çözümlenir — sıfır çalışma zamanı maliyeti.
constexpr auto CRC8_TABLE = generateCRC8Table();

constexpr uint8_t computeCRC8(const uint8_t* data, std::size_t len) {
  uint8_t crc = 0x00;
  for (std::size_t i = 0; i < len; ++i) {
    crc = CRC8_TABLE[static_cast<std::size_t>(crc ^ data[i])];
  }
  return crc;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [3. CONSTEXPR — Baud Rate Divisor]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Compute UART baud rate divisor at compile time.
// TR: UART baud hızı bölücüsünü derleme zamanında hesapla.
constexpr uint32_t computeBaudDivisor(uint32_t clockHz, uint32_t baudRate) {
  return (clockHz + baudRate / 2) / baudRate; // Rounded division
}

// EN: Resolved at compile time — no runtime division needed.
// TR: Derleme zamanında çözümlenir — çalışma zamanı bölmeye gerek yok.
constexpr uint32_t UART_DIVISOR_115200 = computeBaudDivisor(16000000, 115200);
constexpr uint32_t UART_DIVISOR_9600   = computeBaudDivisor(16000000, 9600);

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [4. PLACEMENT NEW — Memory Pool]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: A simple fixed-size memory pool using placement new.
// TR: Placement new kullanan basit sabit boyutlu bellek havuzu.
struct Sensor {
  uint16_t id;
  float value;
  bool active;

  Sensor(uint16_t sensorId, float val)
      : id(sensorId), value(val), active(true) {}

  void print() const {
    std::cout << "  Sensor[" << id << "] value=" << value
              << " active=" << active << std::endl;
  }

  ~Sensor() {
    std::cout << "  ~Sensor[" << id << "] destroyed" << std::endl;
  }
};

// EN: Pool with compile-time capacity, no heap allocation.
// TR: Derleme zamanı kapasiteli havuz, yığın tahsisi yok.
template <typename T, std::size_t MaxObjects>
class StaticPool {
  // EN: Aligned raw storage for objects.
  // TR: Nesneler için hizalanmış ham depolama.
  alignas(T) uint8_t storage_[MaxObjects * sizeof(T)]{};
  bool occupied_[MaxObjects]{};

public:
  template <typename... Args>
  T* allocate(Args&&... args) {
    for (std::size_t i = 0; i < MaxObjects; ++i) {
      if (!occupied_[i]) {
        occupied_[i] = true;
        T* ptr = reinterpret_cast<T*>(&storage_[i * sizeof(T)]);
        new (ptr) T(std::forward<Args>(args)...); // placement new
        return ptr;
      }
    }
    return nullptr; // Pool exhausted
  }

  void deallocate(T* ptr) {
    auto addr = reinterpret_cast<uint8_t*>(ptr);
    auto base = &storage_[0];
    if (addr < base || addr >= base + MaxObjects * sizeof(T)) return;

    std::size_t idx = static_cast<std::size_t>(addr - base) / sizeof(T);
    if (occupied_[idx]) {
      ptr->~T(); // Manual destructor call
      occupied_[idx] = false;
    }
  }

  [[nodiscard]] std::size_t usedCount() const {
    std::size_t count = 0;
    for (std::size_t i = 0; i < MaxObjects; ++i) {
      if (occupied_[i]) ++count;
    }
    return count;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [5. STATIC LOOKUP TABLE — Compile-Time Sine Approximation]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

constexpr double PI = 3.14159265358979323846;

constexpr double constexprSin(double x) {
  // EN: Taylor series sin(x) = x - x^3/3! + x^5/5! - x^7/7! + ...
  // TR: Taylor serisi sin(x) = x - x^3/3! + x^5/5! - x^7/7! + ...
  double result = 0.0;
  double term = x;
  for (int n = 1; n <= 15; n += 2) {
    result += term;
    term *= -x * x / static_cast<double>((n + 1) * (n + 2));
  }
  return result;
}

// EN: 16-entry sine table, built at compile time.
// TR: Derleme zamanında oluşturulan 16 girişli sinüs tablosu.
constexpr std::array<double, 16> generateSineTable() {
  std::array<double, 16> table{};
  for (int i = 0; i < 16; ++i) {
    table[static_cast<std::size_t>(i)] =
        constexprSin(2.0 * PI * static_cast<double>(i) / 16.0);
  }
  return table;
}

constexpr auto SINE_TABLE = generateSineTable();

int main() {
  std::cout << "=== MODULE 11: EMBEDDED CONSTRAINTS ===\n" << std::endl;

  // --- Demo 1: Static Ring Buffer ---
  std::cout << "--- DEMO 1: Static Ring Buffer (No Heap) ---\n" << std::endl;
  StaticRingBuffer<int, 4> rb;
  rb.push(10);
  rb.push(20);
  rb.push(30);
  rb.push(40);
  std::cout << "Buffer full? " << (rb.full() ? "YES" : "NO") << std::endl;
  std::cout << "Push(50) success? " << (rb.push(50) ? "YES" : "NO") << std::endl;

  int val = 0;
  rb.pop(val);
  std::cout << "Popped: " << val << " | Size: " << rb.size() << "\n" << std::endl;

  // --- Demo 2: Compile-Time CRC-8 ---
  std::cout << "--- DEMO 2: Constexpr CRC-8 Table ---\n" << std::endl;
  const uint8_t testData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  uint8_t crc = computeCRC8(testData, 5);
  std::cout << "CRC-8 of {01,02,03,04,05} = 0x" << std::hex
            << static_cast<int>(crc) << std::dec << std::endl;
  std::cout << "CRC table[0x00]=" << static_cast<int>(CRC8_TABLE[0])
            << " table[0xFF]=" << static_cast<int>(CRC8_TABLE[255])
            << "\n" << std::endl;

  // --- Demo 3: Baud Rate Divisor ---
  std::cout << "--- DEMO 3: Compile-Time Baud Divisor ---\n" << std::endl;
  std::cout << "16MHz / 115200 baud => divisor=" << UART_DIVISOR_115200
            << std::endl;
  std::cout << "16MHz / 9600 baud   => divisor=" << UART_DIVISOR_9600
            << "\n" << std::endl;

  // --- Demo 4: Placement New Memory Pool ---
  std::cout << "--- DEMO 4: Static Memory Pool (Placement New) ---\n"
            << std::endl;
  StaticPool<Sensor, 4> pool;
  Sensor* s1 = pool.allocate(static_cast<uint16_t>(1), 23.5f);
  Sensor* s2 = pool.allocate(static_cast<uint16_t>(2), 45.1f);
  Sensor* s3 = pool.allocate(static_cast<uint16_t>(3), 67.8f);
  std::cout << "Pool used: " << pool.usedCount() << "/4" << std::endl;
  if (s1) s1->print();
  if (s2) s2->print();
  if (s3) s3->print();

  std::cout << "\nDeallocating sensor 2..." << std::endl;
  pool.deallocate(s2);
  std::cout << "Pool used: " << pool.usedCount() << "/4" << std::endl;

  Sensor* s4 = pool.allocate(static_cast<uint16_t>(4), 99.9f);
  if (s4) s4->print();
  std::cout << "Pool used: " << pool.usedCount() << "/4" << std::endl;

  // EN: Clean up remaining objects.
  // TR: Kalan nesneleri temizle.
  pool.deallocate(s1);
  pool.deallocate(s3);
  pool.deallocate(s4);

  // --- Demo 5: Compile-Time Sine Table ---
  std::cout << "\n--- DEMO 5: Constexpr Sine Lookup Table ---\n" << std::endl;
  std::cout << "16-point sine table (computed at compile time):" << std::endl;
  for (std::size_t i = 0; i < SINE_TABLE.size(); ++i) {
    std::cout << "  sin(" << i << "*22.5deg) = ";
    if (SINE_TABLE[i] >= 0) std::cout << " ";
    std::cout << SINE_TABLE[i] << std::endl;
  }

  std::cout << "\n=> No-heap, constexpr, memory pool, placement new "
               "demonstrated!" << std::endl;

  return 0;
}
