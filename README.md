# C++ Mastery

**Professional C++ Learning Curriculum — From Fundamentals to Automotive & Embedded Systems**

[![License: Apache 2.0](https://img.shields.io/badge/Code-Apache%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)
[![License: CC BY-NC-SA 4.0](https://img.shields.io/badge/Book-CC%20BY--NC--SA%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by-nc-sa/4.0/)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)]()
[![C++20](https://img.shields.io/badge/C%2B%2B-20-green.svg)]()
[![C++23](https://img.shields.io/badge/C%2B%2B-23-orange.svg)]()

> **Primary Reference:** [cppreference.com](https://en.cppreference.com/) — Every source file includes relevant cppreference links in its `[CPPREF DEPTH]` section.

---

## About

C++ Mastery is a comprehensive, open-source C++ learning curriculum designed for professional developers. It covers the entire spectrum from language fundamentals to automotive embedded systems, with real-world examples, in-depth theory blocks, and compilable code.

The project is available in **two editions**:

| Edition | Language | Description | PDF |
|---------|----------|-------------|-----|
| **[EN](EN/)** | English | Fully English edition | [Download](https://github.com/nimbustan/cpp_mastery/raw/master/C%2B%2B_Mastery_Book_EN.pdf) |
| **[EN_TR](EN_TR/)** | English + Turkish | Bilingual edition with Turkish theory explanations | [Download](https://github.com/nimbustan/cpp_mastery/raw/master/C%2B%2B_Mastery_Book_EN_TR.pdf) |

**Website:** [nimbustan.github.io/cpp_mastery](https://nimbustan.github.io/cpp_mastery/)

---

## Project Stats

| | |
|---|---|
| **Modules** | 18 |
| **Source Files** | 118 per edition (114 `.cpp` + 2 `.txt` + 2 `.md`) |
| **Total Lines** | ~46,000+ per edition |
| **Standards** | C++17 / C++20 / C++23 |
| **Book Pages** | EN: 706 pages · EN_TR: 849 pages |

---

## Module Overview

| # | Module | Directory | Std | Files |
|---|--------|-----------|-----|-------|
| 1 | Basics — Variables & Memory | `module_01_basics` | C++17 | 10 |
| 2 | OOP — Classes & Polymorphism | `module_02_oop` | C++17 | 8 |
| 3 | Modern C++ — Safety & Performance | `module_03_modern_cpp` | C++17/C++23 | 6 |
| 4 | Templates & STL — Generic Programming | `module_04_advanced` | C++17 | 9 |
| 5 | Certification Prep — Exam Topics | `module_05_certification` | C++17 | 7 |
| 6 | Industry Standards — Professional Tools | `module_06_industry_standards` | C++17 | 11 |
| 7 | Under the Hood — Compiler & OS Internals | `module_07_under_the_hood` | C++17/C++20 | 11 |
| 8 | Core Guidelines — Best Practices | `module_08_core_guidelines` | C++17 | 9 |
| 9 | Advanced Mechanics — Low-Level Techniques | `module_09_advanced_mechanics` | C++20 | 8 |
| 10 | Software Architecture & Design Patterns | `module_10_design_patterns` | C++20 | 7 |
| 11 | Embedded Systems — Bare Metal & RTOS | `module_11_embedded_systems` | C++20 | 5 |
| 12 | Coroutines — Async Programming | `module_12_coroutines` | C++20 | 2 |
| 13 | Networking — Sockets & I/O | `module_13_networking` | C++20 | 2 |
| 14 | Automotive Networks — CAN, LIN, FlexRay | `module_14_automotive_networks` | C++20 | 6 |
| 15 | Hardware Interfaces — GPIO, ADC, DMA | `module_15_hw_interfaces` | C++20 | 5 |
| 16 | AUTOSAR — Classic & Adaptive Platform | `module_16_autosar` | C++20 | 5 |
| 17 | SDV — Software-Defined Vehicles | `module_17_sdv` | C++20 | 3 |
| 18 | Driver Development — HAL & BSP | `module_18_driver_development` | C++20 | 4 |

---

## Repository Structure

```
cpp_mastery/
├── EN/                              # English-only edition
│   ├── module_01_basics/            # 18 module directories
│   ├── ...
│   ├── module_18_driver_development/
│   ├── main.cpp                     # Entry point
│   ├── compile_commands.json        # Build configuration
│   ├── cpp_learning_plan.md         # Learning roadmap
│   ├── curriculum_details.md        # Detailed curriculum
│   ├── README.md                    # EN edition details
│   └── LICENSE
│
├── EN_TR/                           # Bilingual EN+TR edition
│   ├── module_01_basics/
│   ├── ...
│   ├── module_18_driver_development/
│   ├── main.cpp
│   ├── compile_commands.json
│   ├── cpp_learning_plan.md
│   ├── curriculum_details.md
│   ├── Doxyfile                     # Doxygen configuration
│   ├── README.md                    # EN_TR edition details
│   └── LICENSE
│
├── docs/                            # GitHub Pages website
│   ├── index.html                   # Language selector
│   ├── en/                          # English site
│   └── tr/                          # Turkish site
│
├── C++_Mastery_Book_EN.pdf          # English PDF (706 pages)
├── C++_Mastery_Book_EN_TR.pdf       # Bilingual PDF (849 pages)
├── LICENSE
└── README.md                        # ← You are here
```

---

## Quick Start

```bash
# Clone the repository
git clone https://github.com/nimbustan/cpp_mastery.git
cd cpp_mastery

# Choose your edition
cd EN        # English only
# or
cd EN_TR     # Bilingual EN+TR

# Compile any file
g++ -std=c++20 -Wall -Wextra module_01_basics/01_variables_and_memory.cpp -o demo
./demo

# Compile with main.cpp entry point
g++ -std=c++20 -Wall -Wextra main.cpp -o main
./main
```

---

## Source File Structure

Every `.cpp` file follows a consistent pattern:

```
📄 Doxygen header (@file, @brief, @details)
📖 [THEORY] blocks — Detailed English explanations (+ Turkish in EN_TR)
🔗 [CPPREF DEPTH] — cppreference.com links for further reading
💻 Compilable code examples with output
✅ main() — Every file is independently compilable and runnable
```

---

## License

| Component | License |
|-----------|---------|
| **Source Code** (`.cpp`, `.h`, build files) | [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0) |
| **Book Content** (PDF, theory blocks, documentation) | [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) |

---

## Author

**Murat Mecit KAHRAMANLI**

- GitHub: [@nimbustan](https://github.com/nimbustan)
- Project: [nimbustan.github.io/cpp_mastery](https://nimbustan.github.io/cpp_mastery/)
