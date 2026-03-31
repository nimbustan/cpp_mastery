/**
 * @file module_05_certification/02_object_slicing_virtual.cpp
 * @brief CPA/CPP Prep: Object Slicing and Virtual Tables
 *
 * @details
 * =============================================================================
 * [THEORY: What is Object Slicing?]
 * =============================================================================
 * EN: Polymorphism (Dynamic Binding) ONLY works over Pointers (*) or References (&). If you
 * directly assign a Derived object to a Base object (Pass by Value), the compiler literally
 * "slices off" all the new variables and overridden functions that belong to the Derived class.
 * The object becomes a pure Base object!
 *
 * =============================================================================
 * [THEORY: Why does it slice? (Memory Architecture)]
 * =============================================================================
 * EN: In C++, objects are contiguous blocks of memory.
 * - Base Class: 4 Bytes
 * - Derived Class: 8 Bytes
 * If you say `Base b = Derived();`, the compiler cannot fit 8 Bytes into a 4 Byte box. It
 * HAS to SLICE the overflowing parts off.
 *
 * =============================================================================
 * [CPPREF DEPTH: Slicing Destroys RTTI]
 * =============================================================================
 * EN: Slicing doesn't just cut off memory; it permanently DESTROYS the object's dynamic type
 * identity (typeid / RTTI pointer). The sliced object's hidden `vptr` is forcibly rewritten to
 * point to the Base class's `vtable`. It is completely unrecoverable!
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_object_slicing_virtual.cpp -o 02_object_slicing_virtual
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

class Document {
public:
  std::string name;
  Document(std::string docName) : name(docName) {}

  // EN: Virtual function is the key to polymorphism.
  virtual void print() const {
    std::cout << "[Base] Printing generic Document: " << name << std::endl;
  }
};

class PDFDocument : public Document {
public:
  // EN: Specific to subclass.
  int securityProtocol;

  PDFDocument(std::string docName, int proto)
      : Document(docName), securityProtocol(proto) {}

  // EN: Overriding the base function.
  void print() const override {
    std::cout << "[Derived] Printing PDF: " << name << " | Security Protocol: "
        << securityProtocol << std::endl;
  }
};

// EN: 1. UNSAFE: Pass by Value -> SLICED!
void sloppyPrint(Document doc) {
  // EN: This object is no longer a PDF. It is just a regular Document.
  doc.print();
}

// EN: 2. SAFE: Pass by Reference -> NO SLICING!
void securePrint(const Document &doc) {
  // EN: The V-Table is consulted. The true owner (PDF) method is called.
  doc.print();
}

int main() {
  std::cout << "=== MODULE 5: OBJECT SLICING ===\n" << std::endl;

  PDFDocument topSecretFile("NuclearCodes.pdf", 256);

  // --- TEST 1: THE SLICING DISASTER ---
  std::cout << "1. Pass by Value (Sliced Object):" << std::endl;
  std::cout << "-> INTERVIEW TRAP: Compiler gives NO ERROR, but calls the "
               "wrong function!"
            << std::endl;
  sloppyPrint(topSecretFile);

  std::cout << "\n-------------------------------------------------\n" << std::endl;

  // --- TEST 2: CORRECT & SAFE POLYMORPHISM ---
  std::cout << "2. Pass by Reference (Real Polymorphism):" << std::endl;
  std::cout << "-> INTACT OBJECT: The V-Table recognized the true owner!" << std::endl;
  securePrint(topSecretFile);

  std::cout << "\n-------------------------------------------------\n" << std::endl;

  // EN: A classic CppQuiz Question:
  std::cout << "3. Direct Assignment (Slicing):" << std::endl;
  Document standardDoc = topSecretFile;
  standardDoc.print();
  // std::cout << standardDoc.securityProtocol; // ERROR! Won't compile because it was sliced
  // off!

  return 0;
}
