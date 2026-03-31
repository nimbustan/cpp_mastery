/**
 * @file module_01_basics/08_file_io_and_streams.cpp
 * @brief Basics: File I/O and Stream Operations
 *
 * @details
 * =============================================================================
 * [THEORY: C++ I/O Streams]
 * =============================================================================
 * EN: C++ I/O is built on a stream hierarchy:
 *     1. `std::cin`  → Standard Input  (keyboard).
 *     2. `std::cout` → Standard Output (console).
 *     3. `std::cerr` → Standard Error  (unbuffered, for errors).
 *     4. `std::clog` → Standard Log    (buffered).
 *
 *     For File I/O, `<fstream>` provides:
 *     1. `std::ofstream` → Write to file (Output File Stream).
 *     2. `std::ifstream` → Read from file (Input File Stream).
 *     3. `std::fstream`  → Read AND Write.
 *
 * =============================================================================
 * [CPPREF DEPTH: RAII and File Handles]
 * =============================================================================
 * EN: `std::ofstream` and `std::ifstream` use RAII: the file is automatically closed when the
 * object goes out of scope (destructor runs). You do NOT need to call `.close()` explicitly if
 * the object is about to be destroyed. Calling `.close()` is only needed if you want to reopen
 * or reuse the stream.
 *
 * The `is_open()` check is critical — opening a file can FAIL silently (bad path, permissions),
 * and writing to a failed stream is Undefined Behavior.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/io/basic_ofstream
 * ⚠  Ref: https://en.cppreference.com/w/cpp/io/basic_ifstream
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 08_file_io_and_streams.cpp -o 08_file_io_and_streams
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::cout << "=== MODULE 1: FILE I/O & STREAMS ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. CONSOLE OUTPUT FORMATTING
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. Console Output ---" << std::endl;

  // EN: BUFFERING — std::cout collects characters in an internal buffer and writes
  //     them to the screen in chunks (efficient). `std::endl` does TWO things:
  //         1. Inserts a newline character '\n'.
  //         2. FLUSHES the buffer — forces all pending data to the screen immediately.
  //     Flushing on every line is SLOW (each flush = a system call to the OS).
  //     Plain '\n' only inserts the newline WITHOUT flushing — the buffer is written
  //     when it's full or when the program ends. In performance-critical or embedded
  //     code, prefer '\n' and flush only when you truly need immediate output.
  std::cout << "Line 1 with endl" << std::endl; // Newline + flush (slow)
  std::cout << "Line 2 with \\n\n";              // Newline only, no flush (fast)

  // EN: std::cerr is UNBUFFERED — every character is written immediately, even
  //     if the program crashes right after. This is why errors go to cerr:
  //     if the program segfaults, buffered cout data might be LOST, but cerr
  //     output is guaranteed to have been written. cerr also writes to stderr
  //     (file descriptor 2), which can be redirected separately from stdout.
  //     Example: `./program > output.txt 2> errors.txt`
  std::cerr << "[ERROR] Simulated ECU fault (stderr)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. CONSOLE INPUT (std::cin)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. Console Input (std::cin) ---" << std::endl;

  // EN: std::cin reads from stdin (keyboard). The >> operator reads ONE TOKEN
  //     at a time, separated by whitespace (space, tab, newline).
  //     TRAP: If you type "John Doe", `cin >> name` reads only "John"!
  //     The " Doe" part stays in the buffer and pollutes the NEXT read.
  //
  //     Pattern for single value:
  //         int userRpm;
  //         std::cout << "Enter RPM: ";
  //         std::cin >> userRpm;
  //
  //     Pattern for full line (including spaces):
  //         std::string fullLine;
  //         std::getline(std::cin, fullLine);
  //
  //     COMMON BUG: Mixing >> and getline. After `cin >> x`, the '\n' remains
  //     in the buffer. The next `getline()` reads an EMPTY string!
  //     Fix: call `std::cin.ignore()` between >> and getline to discard '\n'.
  //
  std::cout << "[SKIPPED] Interactive cin disabled for automated build.\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. WRITE TO FILE (std::ofstream)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Write to File (std::ofstream) ---" << std::endl;

  // EN: std::ofstream (Output File Stream) opens a file for WRITING.
  //     DEFAULT BEHAVIOR: If the file exists, it is ERASED and overwritten.
  //     If the file doesn't exist, it is CREATED. This is equivalent to
  //     opening with `std::ios::out | std::ios::trunc` (truncate).
  //
  //     The `{ }` braces create a SCOPE BLOCK. We put the ofstream inside it
  //     so that when we reach the closing `}`, the ofstream destructor runs
  //     and AUTOMATICALLY closes the file. This is RAII (Resource Acquisition
  //     Is Initialization) — the most important C++ idiom for resource safety.
  //     No need to call `.close()` manually — the destructor handles it.
  //
  const std::string filename = "/tmp/ecu_dtc_log.txt";
  {
    std::ofstream outFile(filename);

    // EN: ALWAYS check is_open() after creating a file stream! Opening can
    //     FAIL SILENTLY for many reasons: bad path, no write permission,
    //     disk full, directory doesn't exist. If you write to a failed stream,
    //     the data simply VANISHES — no crash, no error, just silent data loss.
    if (!outFile.is_open()) {
      std::cerr << "[ERROR] Cannot open file for writing: " << filename
                << std::endl;
      return 1;
    }

    // EN: The << operator works exactly like std::cout — because both inherit
    //     from std::ostream. Anything you can print to console, you can write
    //     to a file with the same syntax.
    outFile << "=== ECU DTC LOG ===" << std::endl;
    outFile << "DTC: P0300 — Random/Multiple Cylinder Misfire Detected"
            << std::endl;
    outFile << "DTC: P0171 — System Too Lean (Bank 1)" << std::endl;
    outFile << "DTC: P0420 — Catalyst Efficiency Below Threshold" << std::endl;
    outFile << "DTC: P0442 — EVAP System Leak Detected (Small)" << std::endl;
    outFile << "Total DTCs: 4" << std::endl;

    std::cout << "Written 4 DTCs to: " << filename << std::endl;

    // EN: When this closing brace `}` is reached, `outFile` goes out of scope.
    //     Its destructor runs → calls close() → flushes buffer → releases file handle.
    //     This is RAII in action: you CANNOT forget to close the file.
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. READ FROM FILE (std::ifstream)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 4. Read from File (std::ifstream) ---" << std::endl;

  {
    // EN: std::ifstream (Input File Stream) opens a file for READING.
    //     If the file doesn't exist, is_open() returns false — no exception thrown.
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
      std::cerr << "[ERROR] Cannot open file for reading: " << filename
                << std::endl;
      return 1;
    }

    // EN: std::getline(stream, string) reads characters until it finds '\n'.
    //     It stores everything EXCEPT the '\n' into `line`, then discards '\n'.
    //     When the stream reaches EOF (End Of File), getline returns false
    //     and the while loop stops.
    //
    //     WHY getline INSTEAD OF >>?
    //     `inFile >> word` reads ONE WORD (stops at whitespace).
    //     `getline(inFile, line)` reads the ENTIRE LINE (including spaces).
    //     For log files, config files, CSV — you almost always want getline.
    //
    std::string line;
    int lineNum = 1;
    while (std::getline(inFile, line)) {
      std::cout << "  [L" << lineNum << "] " << line << std::endl;
      ++lineNum;
    }

    std::cout << "Read " << (lineNum - 1) << " lines from: " << filename
              << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. APPEND TO FILE
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 5. Append to File ---" << std::endl;

  {
    // EN: The second parameter controls the OPEN MODE. Important modes:
    //     std::ios::out   → Write (default for ofstream). ERASES existing content.
    //     std::ios::app   → Append. Writes are always added to the END of the file.
    //                       Existing content is PRESERVED. File is created if absent.
    //     std::ios::ate   → Open and seek to End, but you CAN seek back (unlike app).
    //     std::ios::in    → Read (default for ifstream).
    //     std::ios::binary→ Binary mode (no text translation of \n ↔ \r\n).
    //     Modes can be combined with `|`: `std::ios::out | std::ios::binary`
    //
    //     KEY DIFFERENCE — app vs ate:
    //     app: Every write goes to EOF. Even if you seekp() backward, the write
    //          still appends. This is SAFE for concurrent logging.
    //     ate: Opens at EOF, but you CAN seekp() to any position and overwrite.
    //
    std::ofstream appendFile(filename, std::ios::app);

    if (!appendFile.is_open()) {
      std::cerr << "[ERROR] Cannot open file for appending." << std::endl;
      return 1;
    }

    appendFile << "DTC: P0128 — Coolant Temperature Below Thermostat Range"
               << std::endl;
    appendFile << "Total DTCs: 5 (updated)" << std::endl;

    std::cout << "Appended 1 more DTC to: " << filename << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. RE-READ TO VERIFY APPEND
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 6. Verify Append ---" << std::endl;

  // EN: Re-read the entire file to confirm that Section 5's append worked.
  //     The file should now contain the original 4 DTCs PLUS the 1 we appended.
  //     This pattern (write → close → re-read) is common in testing/validation.
  {
    std::ifstream verifyFile(filename);
    if (verifyFile.is_open()) {
      std::string line;
      while (std::getline(verifyFile, line)) {
        std::cout << "  " << line << std::endl;
      }
    }
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 7. FILE STATUS CHECKS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 7. File Status Checks ---" << std::endl;

  // EN: Every stream has a set of STATE FLAGS you can query:
  //     good()    → True if NO errors at all. The stream is ready for I/O.
  //     fail()    → True if a logical error occurred (wrong type read, open failed).
  //                 Also true when badbit is set.
  //     bad()     → True if an IRRECOVERABLE I/O error occurred (disk failure).
  //     eof()     → True if the stream reached End Of File.
  //     is_open() → True if the underlying file was successfully associated.
  //
  //     A stream used in a boolean context (e.g., `if (inFile)`) checks !fail().
  //     This is why `while (std::getline(inFile, line))` stops at EOF — getline
  //     sets eofbit on the stream, which makes fail() return true.
  //

  // EN: Test with a file that does NOT exist:
  std::ifstream badFile("/tmp/non_existent_file_xyz.txt");
  std::cout << "Bad file is_open(): " << badFile.is_open() << std::endl;
  std::cout << "Bad file good()   : " << badFile.good() << std::endl;
  std::cout << "Bad file fail()   : " << badFile.fail() << std::endl;

  // EN: Test with a file that DOES exist (we created it in Section 3):
  std::ifstream goodFile(filename);
  std::cout << "Good file is_open(): " << goodFile.is_open() << std::endl;
  std::cout << "Good file good()   : " << goodFile.good() << std::endl;

  return 0;
}
