/**
 * @file module_09_advanced_mechanics/08_template_metaprogramming.cpp
 * @brief Template Metaprogramming Deep Dive — Şablon Metaprogramlama Derinlemesine
 *
 * @details
 * =============================================================================
 * [THEORY: Template Metaprogramming (TMP) / TEORİ: Şablon Metaprogramlama]
 * =============================================================================
 *
 * EN: Template metaprogramming = computation at COMPILE TIME using templates.
 *     The compiler IS the interpreter. Templates ARE the programming language.
 *
 *     Key concepts:
 *     1. VARIADIC TEMPLATES — Templates with variable number of args (T...)
 *     2. FOLD EXPRESSIONS (C++17) — Reduce parameter packs with operators
 *     3. INDEX SEQUENCES — std::index_sequence for compile-time indexing
 *     4. RECURSIVE EXPANSION — Base case + pack expansion pattern
 *     5. constexpr if (C++17) — Compile-time branching
 *
 *     ┌──────────────────────────┬──────────────────────────────────────────┐
 *     │ Runtime Programming      │ Template Metaprogramming                 │
 *     ├──────────────────────────┼──────────────────────────────────────────┤
 *     │ if/else                  │ if constexpr / SFINAE / concepts         │
 *     │ for loop                 │ Recursive template instantiation         │
 *     │ int x = 42;              │ template<int X> struct Val {};           │
 *     │ function()               │ Template specialization                  │
 *     │ array[i]                 │ std::tuple_element + index_sequence      │
 *     │ sum += x                 │ Fold expression (... + args)             │
 *     └──────────────────────────┴──────────────────────────────────────────┘
 *
 * TR: Şablon metaprogramlama = şablonlar kullanılarak DERLEME ZAMANINDA hesaplama.
 *     Derleyici yorumlayıcıdır. Şablonlar programlama dilidir.
 *
 *     Temel kavramlar:
 *     1. VARIADIC TEMPLATES — Değişken sayıda argümanlı şablonlar (T...)
 *     2. FOLD EXPRESSIONS (C++17) — Parametre paketlerini operatörlerle indirgeme
 *     3. INDEX SEQUENCES — Derleme zamanı indeksleme için std::index_sequence
 *     4. RECURSIVE EXPANSION — Temel durum + paket genişletme deseni
 *     5. constexpr if (C++17) — Derleme zamanı dallanma
 *
 *     ┌──────────────────────────┬──────────────────────────────────────────┐
 *     │ Çalışma Zamanı Prog.     │ Şablon Metaprogramlama                   │
 *     ├──────────────────────────┼──────────────────────────────────────────┤
 *     │ if/else                  │ if constexpr / SFINAE / concepts         │
 *     │ for döngüsü              │ Özyinelemeli şablon örneklemesi          │
 *     │ int x = 42;              │ template<int X> struct Val {};           │
 *     │ function()               │ Şablon uzmanlaşması                      │
 *     │ array[i]                 │ std::tuple_element + index_sequence      │
 *     │ sum += x                 │ Katlama ifadesi (... + args)             │
 *     └──────────────────────────┴──────────────────────────────────────────┘
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 08_template_metaprogramming.cpp -o 08_template_metaprogramming
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <tuple>
#include <array>
#include <type_traits>
#include <utility>
#include <cstdint>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Variadic Templates — Pack Expansion
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Variadic templates accept any number of template arguments.
 *     Parameter pack T... is expanded by the compiler at each instantiation.
 *
 *     template<typename... Args>    // Template parameter pack
 *     void foo(Args... args) {}     // Function parameter pack
 *
 *     Pack expansion:
 *       f(args...)           → f(arg1, arg2, arg3)
 *       f(process(args)...)  → f(process(arg1), process(arg2), ...)
 *       sizeof...(Args)      → Number of types in the pack
 *
 * TR: Variadic şablonlar herhangi sayıda şablon argümanını kabul eder.
 *     Parametre paketi T... derleyici tarafından her örneklemede genişletilir.
 */

// EN: Type-safe printf using variadic templates (recursive unpacking)
// TR: Variadic şablonlar kullanan tip güvenli printf (özyinelemeli açma)

// EN: Base case — no more arguments
// TR: Temel durum — daha fazla argüman yok
void type_safe_print(const char* fmt) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') {
            throw std::runtime_error("Too few arguments for format string");
        }
        std::cout << *fmt++;
    }
}

// EN: Recursive case — process one argument, recurse for rest
// TR: Özyinelemeli durum — bir argümanı işle, geri kalan için tekrarla
template<typename T, typename... Rest>
void type_safe_print(const char* fmt, const T& first, const Rest&... rest) {
    while (*fmt) {
        if (*fmt == '{' && *(fmt + 1) == '}') {
            std::cout << first;
            type_safe_print(fmt + 2, rest...);  // EN: Recurse with remaining args
            return;
        }
        std::cout << *fmt++;
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Fold Expressions (C++17)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Fold expressions reduce a parameter pack with a binary operator.
 *     Four forms:
 *
 *     (args op ...)      → Unary right fold: (a1 op (a2 op (a3 op a4)))
 *     (... op args)      → Unary left fold:  (((a1 op a2) op a3) op a4)
 *     (args op ... op init) → Binary right fold
 *     (init op ... op args) → Binary left fold
 *
 *     Supported operators: + - * / % ^ & | ~ = < > << >>
 *                          += -= *= /= %= ^= &= |= <<= >>=
 *                          == != <= >= && || , .* ->*
 *
 * TR: Fold ifadeleri parametre paketini ikili operatörle indirger.
 *     Dört form: tekli sağ/sol, ikili sağ/sol katlama.
 */

// EN: Sum all values (unary left fold)
// TR: Tüm değerleri topla (tekli sol katlama)
template<typename... Args>
auto fold_sum(Args... args) {
    return (... + args);  // ((a1 + a2) + a3) + ...
}

// EN: All true? (unary left fold with &&)
// TR: Hepsi doğru mu? (tekli sol katlama, &&)
template<typename... Args>
bool fold_all(Args... args) {
    return (... && args);
}

// EN: Any true? (unary left fold with ||)
// TR: Herhangi biri doğru mu? (tekli sol katlama, ||)
template<typename... Args>
bool fold_any(Args... args) {
    return (... || args);
}

// EN: Print with separator using comma fold expression
// TR: Virgül katlama ifadesi ile ayırıcılı yazdırma
template<typename... Args>
void print_all(const Args&... args) {
    ((std::cout << args << " "), ...);  // Comma fold: execute for each arg
    std::cout << "\n";
}

// EN: Concatenate strings using fold
// TR: Katlama kullanarak stringleri birleştir
template<typename... Args>
std::string fold_concat(const Args&... args) {
    return (std::string{} + ... + std::string(args));
}

// EN: Check if ALL types satisfy a condition (compile-time fold)
// TR: TÜM tiplerin bir koşulu sağlayıp sağlamadığını kontrol et
template<typename... Ts>
constexpr bool all_arithmetic = (... && std::is_arithmetic_v<Ts>);

template<typename... Ts>
constexpr bool any_floating = (... || std::is_floating_point_v<Ts>);

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: std::index_sequence / std::integer_sequence
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: std::index_sequence<0, 1, 2, ...> is a compile-time sequence of indices.
 *     Used to "iterate" at compile time, especially with tuples and arrays.
 *
 *     std::make_index_sequence<N> → std::index_sequence<0, 1, ..., N-1>
 *     std::index_sequence_for<Types...> → index_sequence for parameter pack
 *
 * TR: std::index_sequence<0, 1, 2, ...> derleme zamanında indeks dizisidir.
 *     Özellikle tuple'lar ve dizilerle "iterasyon" yapmak için kullanılır.
 */

// EN: Print all elements of a tuple using index_sequence
// TR: index_sequence kullanarak tuple'ın tüm elemanlarını yazdır
template<typename Tuple, size_t... Is>
void print_tuple_impl(const Tuple& t, std::index_sequence<Is...>) {
    ((std::cout << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

template<typename... Ts>
void print_tuple(const std::tuple<Ts...>& t) {
    std::cout << "(";
    print_tuple_impl(t, std::index_sequence_for<Ts...>{});
    std::cout << ")";
}

// EN: Transform each tuple element through a function
// TR: Her tuple elemanını bir fonksiyonla dönüştür
template<typename F, typename Tuple, size_t... Is>
auto transform_tuple_impl(F&& f, const Tuple& t, std::index_sequence<Is...>) {
    return std::make_tuple(f(std::get<Is>(t))...);
}

template<typename F, typename... Ts>
auto transform_tuple(F&& f, const std::tuple<Ts...>& t) {
    return transform_tuple_impl(std::forward<F>(f), t,
                                std::index_sequence_for<Ts...>{});
}

// EN: Compile-time array creation using index_sequence
// TR: index_sequence kullanarak derleme zamanında dizi oluşturma
template<size_t... Is>
constexpr auto make_squares_impl(std::index_sequence<Is...>) {
    return std::array<size_t, sizeof...(Is)>{{(Is * Is)...}};
}

template<size_t N>
constexpr auto make_squares() {
    return make_squares_impl(std::make_index_sequence<N>{});
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: constexpr if (C++17) — Compile-Time Branching
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: if constexpr evaluates at COMPILE TIME. The false branch is discarded
 *     (not even type-checked). This replaces many SFINAE patterns.
 *
 *     Before (SFINAE):
 *       template<typename T, enable_if_t<is_integral_v<T>, int> = 0>
 *       T process(T x) { return x * 2; }
 *
 *     After (if constexpr):
 *       template<typename T>
 *       auto process(T x) {
 *           if constexpr (is_integral_v<T>) return x * 2;
 *           else return x + 0.5;
 *       }
 *
 * TR: if constexpr DERLEME ZAMANINDA değerlendirilir.
 *     Yanlış dal atılır (tip kontrolü bile yapılmaz).
 *     Birçok SFINAE kalıbının yerini alır.
 */

// EN: Type-dependent processing using if constexpr
// TR: if constexpr kullanarak tipe bağımlı işleme
template<typename T>
std::string describe_type(const T& value) {
    if constexpr (std::is_integral_v<T>) {
        return "integer: " + std::to_string(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        return "float: " + std::to_string(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "string: \"" + value + "\"";
    } else {
        return "unknown type";
    }
}

// EN: Compile-time factorial using if constexpr (cleaner than specialization)
// TR: if constexpr kullanan derleme zamanı faktöriyel
template<int N>
constexpr int factorial() {
    if constexpr (N <= 1) return 1;
    else return N * factorial<N - 1>();
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Practical TMP Patterns
// ═════════════════════════════════════════════════════════════════════════════

// EN: Type list — a list of types as a single type
// TR: Tip listesi — tek bir tip olarak tiplerin listesi
template<typename... Ts>
struct TypeList {
    static constexpr size_t size = sizeof...(Ts);
};

// EN: Get Nth type from a TypeList
// TR: TypeList'ten N'inci tipi al
template<size_t N, typename List>
struct TypeAt;

template<size_t N, typename Head, typename... Tail>
struct TypeAt<N, TypeList<Head, Tail...>> : TypeAt<N - 1, TypeList<Tail...>> {};

template<typename Head, typename... Tail>
struct TypeAt<0, TypeList<Head, Tail...>> {
    using type = Head;
};

template<size_t N, typename List>
using TypeAt_t = typename TypeAt<N, List>::type;

// EN: Check if a type is in a TypeList
// TR: Bir tipin TypeList'te olup olmadığını kontrol et
template<typename T, typename List>
struct Contains;

template<typename T, typename... Ts>
struct Contains<T, TypeList<Ts...>> {
    static constexpr bool value = (... || std::is_same_v<T, Ts>);
};

// EN: Compile-time string hash (FNV-1a)
// TR: Derleme zamanı string hash (FNV-1a)
constexpr uint32_t fnv1a_hash(const char* s, size_t len) {
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint32_t>(static_cast<unsigned char>(s[i]));
        hash *= 16777619u;
    }
    return hash;
}

constexpr uint32_t operator""_hash(const char* s, size_t len) {
    return fnv1a_hash(s, len);
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 09 - Template Metaprogramming\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Variadic Templates ─────────────────────────────────────
    {
        std::cout << "--- Demo 1: Variadic Templates ---\n";
        std::cout << "    type_safe_print: ";
        type_safe_print("Sensor {}: temp={}C, status={}\n", 42, 25.3, "OK");

        std::cout << "    sizeof...(pack): ";
        auto count_types = []<typename... Ts>(Ts...) { return sizeof...(Ts); };
        std::cout << count_types(1, 2.0, "three", 'f') << " types in pack\n\n";
    }

    // ─── Demo 2: Fold Expressions ───────────────────────────────────────
    {
        std::cout << "--- Demo 2: Fold Expressions (C++17) ---\n";
        std::cout << "    fold_sum(1,2,3,4,5):  " << fold_sum(1, 2, 3, 4, 5) << "\n";
        std::cout << "    fold_sum(1.5, 2.5):   " << fold_sum(1.5, 2.5) << "\n";
        std::cout << "    fold_all(T,T,T):      " << std::boolalpha
                  << fold_all(true, true, true) << "\n";
        std::cout << "    fold_all(T,F,T):      " << fold_all(true, false, true) << "\n";
        std::cout << "    fold_any(F,F,T):      " << fold_any(false, false, true) << "\n";
        std::cout << "    print_all: ";
        print_all(42, 3.14, "hello", 'X');
        std::cout << "    fold_concat: " << fold_concat("Hello", " ", "World", "!") << "\n";

        // EN: Compile-time type checks with fold
        // TR: Katlama ile derleme zamanlı tip kontrolleri
        std::cout << "    all_arithmetic<int,double,float>: "
                  << all_arithmetic<int, double, float> << "\n";
        std::cout << "    all_arithmetic<int,string>:       "
                  << all_arithmetic<int, std::string> << "\n";
        std::cout << "    any_floating<int,double>:         "
                  << any_floating<int, double> << "\n\n";
    }

    // ─── Demo 3: index_sequence ─────────────────────────────────────────
    {
        std::cout << "--- Demo 3: std::index_sequence ---\n";

        auto sensor_data = std::make_tuple(42, "Engine", 95.5, true);
        std::cout << "    Tuple: ";
        print_tuple(sensor_data);
        std::cout << "\n";

        // EN: Transform tuple: double all numeric values
        // TR: Tuple dönüştür: tüm sayısal değerleri ikiye katla
        auto doubled = transform_tuple([](auto x) {
            if constexpr (std::is_arithmetic_v<std::decay_t<decltype(x)>>)
                return x * 2;
            else
                return x;  // non-numeric: pass through
        }, std::make_tuple(10, 20.5, 30));
        std::cout << "    Doubled tuple: ";
        print_tuple(doubled);
        std::cout << "\n";

        // EN: Compile-time array of squares
        // TR: Derleme zamanlı kareler dizisi
        constexpr auto squares = make_squares<8>();
        std::cout << "    Squares[0..7]: ";
        for (auto s : squares) std::cout << s << " ";
        std::cout << "\n\n";
    }

    // ─── Demo 4: if constexpr ───────────────────────────────────────────
    {
        std::cout << "--- Demo 4: if constexpr ---\n";
        std::cout << "    describe(42):     " << describe_type(42) << "\n";
        std::cout << "    describe(3.14):   " << describe_type(3.14) << "\n";
        std::cout << "    describe(string): " << describe_type(std::string("hello")) << "\n";

        std::cout << "    factorial<5>: " << factorial<5>() << "\n";
        std::cout << "    factorial<10>: " << factorial<10>() << "\n";
        static_assert(factorial<5>() == 120, "5! = 120");
        static_assert(factorial<10>() == 3628800, "10! = 3628800");
        std::cout << "    (static_assert passed at compile time!)\n\n";
    }

    // ─── Demo 5: TypeList & Compile-Time Hash ───────────────────────────
    {
        std::cout << "--- Demo 5: TypeList & Compile-Time Hash ---\n";

        using SensorTypes = TypeList<int, float, double, std::string>;
        std::cout << "    SensorTypes::size: " << SensorTypes::size << "\n";

        // EN: TypeAt — get type at index
        // TR: TypeAt — indeksteki tipi al
        static_assert(std::is_same_v<TypeAt_t<0, SensorTypes>, int>);
        static_assert(std::is_same_v<TypeAt_t<2, SensorTypes>, double>);
        std::cout << "    TypeAt<0>: int (verified at compile time)\n";
        std::cout << "    TypeAt<2>: double (verified at compile time)\n";

        // EN: Contains — check if type is in list
        // TR: Contains — tipin listede olup olmadığını kontrol et
        std::cout << "    Contains<float>:  " << std::boolalpha
                  << Contains<float, SensorTypes>::value << "\n";
        std::cout << "    Contains<char>:   "
                  << Contains<char, SensorTypes>::value << "\n";

        // EN: Compile-time hash (usable in switch statements!)
        // TR: Derleme zamanlı hash (switch ifadelerinde kullanılabilir!)
        constexpr auto h1 = "temperature"_hash;
        constexpr auto h2 = "pressure"_hash;
        std::cout << "    \"temperature\" hash: " << h1 << "\n";
        std::cout << "    \"pressure\" hash:    " << h2 << "\n";

        // EN: Can use in switch!
        // TR: switch'te kullanılabilir!
        auto process = [](const char* sensor, size_t len) {
            switch (fnv1a_hash(sensor, len)) {
                case "temperature"_hash: return "temp handler";
                case "pressure"_hash:    return "press handler";
                default:                 return "unknown";
            }
        };
        std::cout << "    switch(hash(\"temperature\")): "
                  << process("temperature", 11) << "\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Template Metaprogramming\n";
    std::cout << "============================================\n";

    return 0;
}
