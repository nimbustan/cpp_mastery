/**
 * @file module_01_basics/10_filesystem_and_binary_io.cpp
 * @brief std::filesystem & Binary I/O
 *
 * @details
 * =============================================================================
 * [THEORY: std::filesystem (C++17)]
 * =============================================================================
 *
 * EN: C++17 introduced <filesystem> — a complete, portable, cross-platform API
 *     for file and directory operations. No more platform-specific code!
 *
 *     Key types:
 *     ┌─────────────────────────┬────────────────────────────────────────────┐
 *     │ Type                    │ Purpose                                    │
 *     ├─────────────────────────┼────────────────────────────────────────────┤
 *     │ std::filesystem::path   │ File/dir path object (operator/, concat)   │
 *     │ directory_entry         │ A single directory item (file, dir, link)  │
 *     │ directory_iterator      │ Iterate flat directory                     │
 *     │ recursive_dir_iterator  │ Iterate all subdirectories recursively     │
 *     │ file_status             │ Type (regular, directory, symlink, ...)    │
 *     │ space_info              │ Disk capacity, free, available             │
 *     └─────────────────────────┴────────────────────────────────────────────┘
 *
 *     Key functions:
 *     - exists(), is_regular_file(), is_directory(), is_symlink()
 *     - create_directory(), create_directories()  (mkdir -p)
 *     - copy(), copy_file(), rename(), remove(), remove_all()
 *     - file_size(), last_write_time(), permissions()
 *     - current_path(), temp_directory_path(), absolute()
 *     - path::filename(), stem(), extension(), parent_path()
 *     - path::replace_extension(), path operator/
 *
 * =============================================================================
 * [THEORY: Binary I/O]
 * =============================================================================
 *
 * EN: Binary I/O writes raw bytes (no text formatting):
 *
 *     Text I/O vs Binary I/O:
 *     ┌──────────────┬───────────────────────┬──────────────────────────────┐
 *     │ Aspect       │ Text I/O              │ Binary I/O                   │
 *     ├──────────────┼───────────────────────┼──────────────────────────────┤
 *     │ Mode         │ ios::in/out           │ ios::binary                  │
 *     │ Functions    │ operator<< / >>       │ write() / read()             │
 *     │ Data         │ Human-readable text   │ Raw bytes (like in memory)   │
 *     │ Newlines     │ Platform-converted    │ Preserved exactly            │
 *     │ Size         │ Variable              │ Fixed (sizeof struct)        │
 *     │ Speed        │ Slower (formatting)   │ Faster (direct copy)         │
 *     │ Use case     │ Config, logs, CSV     │ Images, databases, firmware  │
 *     └──────────────┴───────────────────────┴──────────────────────────────┘
 *
 *     Binary I/O pattern:
 *     ofstream out("data.bin", ios::binary);
 *     out.write(reinterpret_cast<const char*>(&data), sizeof(data));
 *
 *     ifstream in("data.bin", ios::binary);
 *     in.read(reinterpret_cast<char*>(&data), sizeof(data));
 *
 *     WARNING: Binary I/O of structs is NOT portable:
 *     - Different machines may have different endianness
 *     - Struct padding may differ between compilers
 *     - Use __attribute__((packed)) or #pragma pack for cross-platform
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 10_filesystem_and_binary_io.cpp -o 10_filesystem_and_binary_io
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <iomanip>
#include <cstring>
#include <cstdint>

namespace fs = std::filesystem;

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: std::filesystem::path Operations
// ═════════════════════════════════════════════════════════════════════════════

void demo_path_operations() {
    std::cout << "--- Demo 1: std::filesystem::path ---\n";

    // EN: Constructing paths with operator/
    fs::path base = "/home/ecu";
    fs::path config = base / "config" / "sensors.json";

    std::cout << "    Full path:    " << config << "\n";
    std::cout << "    Parent:       " << config.parent_path() << "\n";
    std::cout << "    Filename:     " << config.filename() << "\n";
    std::cout << "    Stem:         " << config.stem() << "\n";
    std::cout << "    Extension:    " << config.extension() << "\n";

    // EN: Replace extension
    fs::path backup = config;
    backup.replace_extension(".bak");
    std::cout << "    Backup:       " << backup << "\n";

    // EN: Relative path construction
    fs::path rel = fs::path("src") / "module_01" / "main.cpp";
    std::cout << "    Relative:     " << rel << "\n";
    std::cout << "    Is absolute:  " << std::boolalpha << rel.is_absolute() << "\n\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Directory Operations
// ═════════════════════════════════════════════════════════════════════════════

void demo_directory_operations() {
    std::cout << "--- Demo 2: Directory Operations ---\n";

    // EN: Create a temporary test directory structure
    fs::path test_dir = fs::temp_directory_path() / "cpp_mastery_test";

    // EN: Clean up any previous run
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }

    // EN: create_directories = mkdir -p (creates parent dirs too)
    fs::create_directories(test_dir / "sensors" / "temperature");
    fs::create_directories(test_dir / "sensors" / "pressure");
    fs::create_directories(test_dir / "logs");

    // EN: Create test files
    std::ofstream(test_dir / "readme.txt") << "ECU Sensor Data\n";
    std::ofstream(test_dir / "sensors" / "temp_cal.dat") << "25.0 1.0 0.5\n";
    std::ofstream(test_dir / "logs" / "dtc.log") << "P0100: MAF sensor\n";

    // EN: Check existence and type
    std::cout << "    test_dir exists: " << fs::exists(test_dir) << "\n";
    std::cout << "    is_directory:    " << fs::is_directory(test_dir) << "\n";
    std::cout << "    readme exists:   " << fs::is_regular_file(test_dir / "readme.txt") << "\n";

    // EN: File size
    auto readme_size = fs::file_size(test_dir / "readme.txt");
    std::cout << "    readme.txt size: " << readme_size << " bytes\n";

    // EN: Directory iteration (flat)
    std::cout << "    Contents of test_dir:\n";
    for (const auto& entry : fs::directory_iterator(test_dir)) {
        std::cout << "      " << (entry.is_directory() ? "[DIR] " : "[FILE]")
                  << " " << entry.path().filename().string() << "\n";
    }

    // EN: Recursive iteration
    std::cout << "    Recursive listing:\n";
    for (const auto& entry : fs::recursive_directory_iterator(test_dir)) {
        auto rel = fs::relative(entry.path(), test_dir);
        std::cout << "      " << (entry.is_directory() ? "[DIR] " : "      ")
                  << rel.string() << "\n";
    }

    // EN: Copy file
    fs::copy_file(test_dir / "readme.txt", test_dir / "readme_backup.txt");
    std::cout << "    Copied readme.txt → readme_backup.txt\n";

    // EN: Rename (move)
    fs::rename(test_dir / "readme_backup.txt", test_dir / "readme_v2.txt");
    std::cout << "    Renamed readme_backup.txt → readme_v2.txt\n";

    // EN: Disk space
    auto space = fs::space(test_dir);
    std::cout << "    Disk: " << (space.capacity / (1024*1024*1024)) << " GB total, "
              << (space.available / (1024*1024*1024)) << " GB free\n";

    // EN: Cleanup
    fs::remove_all(test_dir);
    std::cout << "    Cleaned up test directory\n\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Binary I/O
// ═════════════════════════════════════════════════════════════════════════════

// EN: Packed struct for binary serialization
#pragma pack(push, 1)
struct SensorRecord {
    uint32_t sensor_id;
    float temperature;
    float pressure;
    uint64_t timestamp_ms;
};
#pragma pack(pop)

void demo_binary_io() {
    std::cout << "--- Demo 3: Binary I/O ---\n";

    fs::path bin_file = fs::temp_directory_path() / "sensors.bin";

    // EN: Write binary records
    std::vector<SensorRecord> records = {
        {1, 25.3f, 1013.25f, 1000},
        {2, 30.1f, 1015.50f, 2000},
        {3, 22.8f, 1010.00f, 3000},
        {4, 28.5f, 1012.75f, 4000},
    };

    {
        std::ofstream out(bin_file, std::ios::binary);
        // EN: Write count first, then records
        uint32_t count = static_cast<uint32_t>(records.size());
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        out.write(reinterpret_cast<const char*>(records.data()),
                  static_cast<std::streamsize>(records.size() * sizeof(SensorRecord)));
    }

    auto file_size = fs::file_size(bin_file);
    std::cout << "    Wrote " << records.size() << " records to binary file\n";
    std::cout << "    File size: " << file_size << " bytes"
              << " (header=" << sizeof(uint32_t)
              << " + " << records.size() << " × " << sizeof(SensorRecord)
              << " = " << sizeof(uint32_t) + records.size() * sizeof(SensorRecord)
              << ")\n";
    std::cout << "    SensorRecord size: " << sizeof(SensorRecord) << " bytes (packed)\n";

    // EN: Read binary records back
    std::vector<SensorRecord> loaded;
    {
        std::ifstream in(bin_file, std::ios::binary);
        uint32_t count = 0;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        loaded.resize(count);
        in.read(reinterpret_cast<char*>(loaded.data()),
                static_cast<std::streamsize>(count * sizeof(SensorRecord)));
    }

    std::cout << "    Read back " << loaded.size() << " records:\n";
    for (const auto& r : loaded) {
        std::cout << "      Sensor " << r.sensor_id
                  << ": T=" << r.temperature << "C"
                  << ", P=" << r.pressure << "hPa"
                  << ", ts=" << r.timestamp_ms << "ms\n";
    }

    // EN: Verify data matches
    bool match = (records.size() == loaded.size());
    for (size_t i = 0; match && i < records.size(); ++i) {
        match = (records[i].sensor_id == loaded[i].sensor_id) &&
                (records[i].temperature == loaded[i].temperature);
    }
    std::cout << "    Data integrity check: " << (match ? "PASS" : "FAIL") << "\n";

    fs::remove(bin_file);
    std::cout << "\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Error Handling with filesystem
// ═════════════════════════════════════════════════════════════════════════════

void demo_filesystem_error_handling() {
    std::cout << "--- Demo 4: Filesystem Error Handling ---\n";

    // EN: Method 1: exceptions (default)
    try {
        auto sz = fs::file_size("/nonexistent/path/file.txt");
        std::cout << "    Size: " << sz << "\n";  // never reached
    } catch (const fs::filesystem_error& e) {
        std::cout << "    Exception: " << e.what() << "\n";
        std::cout << "    Path1: " << e.path1() << "\n";
        std::cout << "    Code: " << e.code().message() << "\n";
    }

    // EN: Method 2: error_code (no exceptions — preferred for embedded)
    std::error_code ec;
    auto sz = fs::file_size("/nonexistent/path/file.txt", ec);
    if (ec) {
        std::cout << "    error_code: " << ec.message() << "\n";
    } else {
        std::cout << "    Size: " << sz << "\n";
    }

    bool exists = fs::exists("/tmp", ec);
    std::cout << "    /tmp exists: " << std::boolalpha << exists
              << " (no throw)\n\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Practical Patterns
// ═════════════════════════════════════════════════════════════════════════════

void demo_practical_patterns() {
    std::cout << "--- Demo 5: Practical Patterns ---\n";

    // EN: Pattern 1: Find all files with extension
    std::cout << "    .cpp files in module_01:\n";
    fs::path module_dir = "module_01_basics";
    if (fs::exists(module_dir)) {
        for (const auto& entry : fs::directory_iterator(module_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".cpp") {
                std::cout << "      " << entry.path().filename().string()
                          << " (" << entry.file_size() << " bytes)\n";
            }
        }
    }

    // EN: Pattern 2: Ensure output directory exists before writing
    fs::path output = fs::temp_directory_path() / "ecu_output" / "diagnostics";
    fs::create_directories(output);  // Safe to call even if exists
    std::ofstream(output / "report.txt") << "DTC Report: No faults\n";
    std::cout << "    Created output dir + report: " << output.string() << "\n";
    fs::remove_all(output.parent_path());

    // EN: Pattern 3: Temporary file with scope guard
    auto temp = fs::temp_directory_path() / "ecu_temp_XXXXXX.dat";
    {
        std::ofstream f(temp, std::ios::binary);
        uint32_t magic = 0xDEADBEEF;
        f.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    }
    std::cout << "    Temp file: " << temp.filename().string()
              << " (" << fs::file_size(temp) << " bytes)\n";
    fs::remove(temp);
    std::cout << "    Temp file cleaned up\n\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 01 - Filesystem & Binary I/O\n";
    std::cout << "============================================\n\n";

    demo_path_operations();
    demo_directory_operations();
    demo_binary_io();
    demo_filesystem_error_handling();
    demo_practical_patterns();

    // ─── Reference Table ─────────────────────────────────────────────────
    std::cout << "--- Reference: Key std::filesystem Functions ---\n";
    std::cout << "  ┌─────────────────────────────┬──────────────────────────────────┐\n";
    std::cout << "  │ Function                    │ Purpose                          │\n";
    std::cout << "  ├─────────────────────────────┼──────────────────────────────────┤\n";
    std::cout << "  │ exists(p)                   │ Check if path exists             │\n";
    std::cout << "  │ is_regular_file(p)          │ Is it a regular file?            │\n";
    std::cout << "  │ is_directory(p)             │ Is it a directory?               │\n";
    std::cout << "  │ file_size(p)                │ Size in bytes                    │\n";
    std::cout << "  │ create_directories(p)       │ mkdir -p equivalent              │\n";
    std::cout << "  │ copy_file(src, dst)         │ Copy a file                      │\n";
    std::cout << "  │ rename(old, new)            │ Move/rename file or dir          │\n";
    std::cout << "  │ remove_all(p)               │ rm -rf equivalent                │\n";
    std::cout << "  │ directory_iterator(p)       │ Iterate directory (flat)         │\n";
    std::cout << "  │ recursive_directory_iter(p) │ Iterate all subdirs              │\n";
    std::cout << "  │ relative(p, base)           │ Compute relative path            │\n";
    std::cout << "  │ current_path()              │ Get/set working directory        │\n";
    std::cout << "  │ temp_directory_path()       │ Platform temp directory          │\n";
    std::cout << "  │ space(p)                    │ Disk space info                  │\n";
    std::cout << "  └─────────────────────────────┴──────────────────────────────────┘\n\n";

    std::cout << "============================================\n";
    std::cout << "   End of Filesystem & Binary I/O\n";
    std::cout << "============================================\n";

    return 0;
}
