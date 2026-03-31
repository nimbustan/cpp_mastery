# Clean Code Reference for C++

© 2026 Murat Mecit KAHRAMANLI. All rights reserved.
Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0

---

## 1. Naming Conventions

### EN: Common C++ Naming Styles

| Element            | Style                | Example                          |
|--------------------|----------------------|----------------------------------|
| Class/Struct       | PascalCase           | `SensorManager`                  |
| Function/Method    | camelCase or snake   | `readSensor()` / `read_sensor()` |
| Variable           | camelCase or snake   | `sensorValue` / `sensor_value`   |
| Constant           | kPascal or UPPER     | `kMaxRetries` / `MAX_RETRIES`    |
| Enum               | PascalCase           | `enum class SensorState`         |
| Enum value         | PascalCase           | `SensorState::Active`            |
| Namespace          | lowercase            | `namespace sensors`              |
| Template param     | PascalCase           | `template<typename ValueType>`   |
| Private member     | trailing underscore  | `int count_;`                    |
| Header guard       | UPPER_SNAKE          | `SENSOR_MANAGER_HPP`             |
| File name          | snake_case           | `sensor_manager.cpp`             |

### Important Rules
1. **Names should reveal intent**
   - BAD:  `int d;  // elapsed time in days`
   - GOOD: `int elapsedDays;`
2. **Avoid abbreviations**
   - BAD:  `int tpc;  // temperature processing count`
   - GOOD: `int temperatureProcessingCount;`
3. **One letter names only for loops**
   - OK: `for (int i = 0; i < n; ++i)`
4. **Bool names should be questions**
   - `bool isActive;`, `bool hasPermission;`, `bool shouldRetry;`

---

## 2. Function Design

### EN: Function Length & Complexity

| Metric                   | Target     | Max Acceptable |
|--------------------------|------------|----------------|
| Lines per function       | ≤ 20       | 40             |
| Parameters               | ≤ 3        | 5              |
| Cyclomatic complexity    | ≤ 5        | 10             |
| Nesting depth            | ≤ 2        | 4              |

### Rules

1. **Do ONE thing**
   ```cpp
   // BAD: Does validation AND processing AND logging
   void handleSensorData(int raw) { ... }
   
   // GOOD: Split into focused functions
   bool validateReading(int raw);
   double processReading(int raw);
   void logReading(double processed);
   ```

2. **Use early return to reduce nesting**
   ```cpp
   // BAD: Deep nesting
   void process(Sensor* s) {
       if (s != nullptr) {
           if (s->isActive()) {
               if (s->hasData()) {
                   // actual logic buried deep
               }
           }
       }
   }
   
   // GOOD: Guard clauses
   void process(Sensor* s) {
       if (!s) return;
       if (!s->isActive()) return;
       if (!s->hasData()) return;
       // actual logic at top level
   }
   ```

3. **Parameter passing rules**
   ```cpp
   void observe(const Widget& w);       // Input: const ref
   void mutate(Widget& w);              // Input-output: ref
   void sink(Widget w);                 // Consuming: by value
   Widget create();                     // Output: return by value
   void optional(Widget* w);            // Optional: pointer (nullable)
   ```

---

## 3. Code Smells to Avoid

### EN: Common C++ Code Smells

| Smell                    | Fix                                                |
|--------------------------|----------------------------------------------------|
| Raw `new`/`delete`       | Use `make_unique`/`make_shared`                    |
| C-style casts `(int)x`   | Use `static_cast<int>(x)`                          |
| `#define` constants      | Use `constexpr` or `const`                         |
| `#define` functions      | Use `inline` functions or templates                |
| Magic numbers            | Use named constants: `constexpr int kMaxRetry = 3;`|
| God class (1000+ lines)  | Split into focused classes with SRP                |
| Long parameter lists     | Use structs/builder pattern                        |
| Comments explaining what | Make code self-documenting (better names)          |
| `using namespace std;`   | Never in headers! OK sparingly in .cpp             |
| Implicit conversions     | Use `explicit` on constructors                     |
| Output parameters        | Return by value (thanks to RVO/NRVO)               |
| `void*`                  | Use templates, `std::variant`, or `std::any`       |
| `errno`-style errors     | Use exceptions or `std::expected`                  |

---

## 4. Header File Best Practices

```cpp
#pragma once  // or include guards

// 1. Standard library headers
#include <string>
#include <vector>

// 2. Third-party library headers
#include <fmt/core.h>

// 3. Project headers
#include "sensors/temperature.hpp"
#include "utils/math_helpers.hpp"

namespace ecu::sensors {

// Forward declarations (instead of #include)
class PressureSensor;

// Class declaration (minimal — implementation in .cpp)
class TemperatureSensor {
public:
    explicit TemperatureSensor(int channel);
    double read() const;

private:
    struct Impl;                    // PIMPL for heavy private members
    std::unique_ptr<Impl> impl_;
};

}  // namespace ecu::sensors
```

### Rules
1. **#pragma once** at the top of every header
2. **Include order**: standard → third-party → project
3. **Forward declare** in headers, include in .cpp
4. **No `using namespace`** in headers — EVER
5. **Minimize** what headers expose (PIMPL, forward decl)

---

## 5. clang-format Configuration

### Recommended `.clang-format` file

```yaml
# .clang-format
---
Language: Cpp
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
AccessModifierOffset: -4
AlignAfterOpenBracket: Align
AllowShortFunctionsOnASingleLine: Inline
AllowShortIfStatementsOnASingleLine: Never
AllowShortLoopsOnASingleLine: false
BreakBeforeBraces: Attach
NamespaceIndentation: None
PointerAlignment: Left
SortIncludes: CaseInsensitive
SpaceBeforeParens: ControlStatements
IncludeBlocks: Regroup
IncludeCategories:
  - Regex: '^<[a-z_]+>'
    Priority: 1
  - Regex: '^<'
    Priority: 2
  - Regex: '^"'
    Priority: 3
...
```

### Usage
```bash
# Format a file
clang-format -i src/main.cpp

# Check formatting (CI)
clang-format --dry-run --Werror src/*.cpp

# Format all C++ files in project
find src/ -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

---

## 6. C++ Core Guidelines Quick Reference

### Most Important Rules

| Rule   | Description                                           |
|--------|-------------------------------------------------------|
| F.1    | Package meaningful operations as functions            |
| F.3    | Keep functions short and simple                       |
| F.4    | If function may need compile-time eval, use constexpr |
| F.16   | Input: `const T&` for cheap-to-move, `T` for sink     |
| C.2    | Use class if has invariant; struct if members vary    |
| C.9    | Minimize member exposure                              |
| C.35   | Base class needs virtual dtor OR protected non-virtual|
| C.48   | Prefer `{}` init for constants, `=` for others        |
| R.1    | Manage resources with RAII                            |
| R.3    | Raw pointer = non-owning                              |
| R.11   | Avoid calling `new`/`delete` explicitly               |
| ES.1   | Prefer standard library over hand-written code        |
| ES.5   | Keep scopes small                                     |
| ES.23  | Prefer `{}` init to avoid narrowing                   |
| ES.49  | Don't use casts; if must, use named casts             |
| P.1    | Express ideas directly in code                        |
| P.3    | Express intent                                        |
| I.11   | Never transfer ownership by raw pointer               |

---

## 7. Modern C++ Idioms Checklist

### Before Committing

- [ ] **RAII everywhere** — No resource leaks (files, locks, memory)
- [ ] **const correct** — `const` on methods, `const&` on parameters
- [ ] **explicit constructors** — Single-arg and implicit conversion ctors
- [ ] **noexcept on move** — Move ctor/assignment are noexcept
- [ ] **[[nodiscard]]** — On functions where ignoring return = likely bug
- [ ] **auto wisely** — Use auto for complex types, not for int/double
- [ ] **Range-for with const auto&** — `for (const auto& item : container)`
- [ ] **Smart pointers** — No raw `new`/`delete`
- [ ] **No narrowing** — Use `static_cast` or `gsl::narrow`
- [ ] **Rule of 0/3/5** — Default, or all three/five
- [ ] **Scoped enums** — `enum class` not plain `enum`
- [ ] **nullptr** — Not `NULL` or `0` for pointers
- [ ] **constexpr** — For compile-time computable values
- [ ] **Structured bindings** — `auto [key, value] = pair;`
- [ ] **std::string_view** — For non-owning string references
