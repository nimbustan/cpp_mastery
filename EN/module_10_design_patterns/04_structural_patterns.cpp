/**
 * @file module_10_design_patterns/04_structural_patterns.cpp
 * @brief Design Patterns: Structural
 *
 * @details
 * =============================================================================
 * [THEORY: Structural Patterns Overview]
 * =============================================================================
 * EN: Structural patterns deal with HOW classes and objects are COMPOSED to form larger
 * structures. They simplify complex relationships, adapt incompatible interfaces, and add new
 * behavior without altering existing code. Think of them as "architectural connectors" that glue
 * components together.
 *
 *
 * =============================================================================
 * [THEORY: Adapter — Make Incompatible Interfaces Work Together
 * =============================================================================
 * EN: The Adapter pattern wraps an existing class with an incompatible interface so it matches
 * the interface the client expects. Like a power adapter that converts EU plugs to US outlets.
 * The adapted class doesn't need ANY modification — the adapter handles the translation.
 *
 *
 * =============================================================================
 * [THEORY: Decorator — Add Behavior Dynamically
 * Ekle]
 * =============================================================================
 * EN: The Decorator pattern wraps an object to ADD new behavior without modifying the original
 * class. Decorators share the same interface as the wrapped object, so they can be stacked
 * (chained). Example: adding encryption, compression, and logging to a data stream — each
 * decorator wraps the previous one.
 *
 *
 * =============================================================================
 * [THEORY: Facade — Simplified Interface to Complex System
 * =============================================================================
 * EN: The Facade pattern provides a SIMPLIFIED interface to a complex subsystem consisting of
 * many classes. Instead of clients managing 10 different objects, they talk to ONE facade that
 * orchestrates everything behind the scenes. Common in automotive middleware, media frameworks,
 * and library wrappers.
 *
 *
 * =============================================================================
 * [THEORY: Composite — Treat Individual and Group Objects Uniformly
 * =============================================================================
 * EN: The Composite pattern lets you compose objects into tree structures and treat individual
 * objects and compositions UNIFORMLY through the same interface. A `File` and a `Folder`
 * (containing files and subfolders) both implement `getSize()`. The client doesn't care if it's
 * a leaf or a branch.
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: Multiple Inheritance for Mixin-Style Decorator Chains
 * =============================================================================
 * EN: C++ supports multiple inheritance, which enables mixin-style decorators where behavior is
 * composed by inheriting from multiple small classes. Each mixin adds ONE capability. Virtual
 * inheritance resolves the diamond problem when mixins share a common base. In practice, prefer
 * composition-based decorators (wrapping via constructor) over inheritance-based ones for better
 * flexibility. cppreference.com/w/cpp/language/derived_class
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: std::shared_ptr for Shared Composite Ownership
 * =============================================================================
 * EN: In Composite pattern implementations, child nodes can be shared among multiple parents
 * (e.g., symbolic links, DAG structures). Use `std::shared_ptr` for shared ownership where
 * multiple owners keep the object alive. Use `std::weak_ptr` to break circular references. For
 * strict tree hierarchies (single parent), `std::unique_ptr` is preferred for zero overhead.
 * cppreference.com/w/cpp/memory/shared_ptr
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_structural_patterns.cpp -o 04_structural_patterns
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [1. ADAPTER PATTERN]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Legacy library we CANNOT modify (third-party).
class LegacyXMLParser {
public:
  std::string parseXML(const std::string &xml) {
    return "[XML Parsed] " + xml;
  }
};

// EN: Interface our system expects.
class IDataParser {
public:
  virtual ~IDataParser() = default;
  virtual std::string parse(const std::string &data) = 0;
};

// EN: Adapter wraps the legacy class to match our interface.
class XMLParserAdapter : public IDataParser {
  LegacyXMLParser legacy_;

public:
  std::string parse(const std::string &data) override {
    return legacy_.parseXML(data);
  }
};

class JSONParser : public IDataParser {
public:
  std::string parse(const std::string &data) override {
    return "[JSON Parsed] " + data;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [2. DECORATOR PATTERN — Stacking Behaviors]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class IMessage {
public:
  virtual ~IMessage() = default;
  virtual std::string getContent() const = 0;
};

class PlainMessage : public IMessage {
  std::string text_;

public:
  explicit PlainMessage(std::string t) : text_(std::move(t)) {}
  std::string getContent() const override { return text_; }
};

// EN: Base decorator — wraps an IMessage.
class MessageDecorator : public IMessage {
protected:
  std::unique_ptr<IMessage> wrapped_;

public:
  explicit MessageDecorator(std::unique_ptr<IMessage> m)
      : wrapped_(std::move(m)) {}
};

class EncryptedMessage : public MessageDecorator {
public:
  using MessageDecorator::MessageDecorator;
  std::string getContent() const override {
    return "[ENCRYPTED]{" + wrapped_->getContent() + "}";
  }
};

class CompressedMessage : public MessageDecorator {
public:
  using MessageDecorator::MessageDecorator;
  std::string getContent() const override {
    return "[COMPRESSED]{" + wrapped_->getContent() + "}";
  }
};

class LoggedMessage : public MessageDecorator {
public:
  using MessageDecorator::MessageDecorator;
  std::string getContent() const override {
    std::cout << "[LOG] Message accessed." << std::endl;
    return wrapped_->getContent();
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [3. FACADE PATTERN — Simplified Interface]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Complex subsystem classes (Instrument Cluster subsystems).
class AudioSubsystem {
public:
  void playChime(const std::string &chime) {
    std::cout << "  [Audio] Playing chime: " << chime << std::endl;
  }
};

class SensorFusionEngine {
public:
  void calibrateSensors(int sensorCount) {
    std::cout << "  [SensorFusion] Calibrating " << sensorCount
              << " sensors" << std::endl;
  }
};

class DisplayRenderer {
public:
  void setResolution(int w, int h) {
    std::cout << "  [Display] Cluster resolution: " << w << "x" << h
              << std::endl;
  }
};

class CANGateway {
public:
  void connectBus(const std::string &busName) {
    std::cout << "  [CAN] Connected to " << busName << " bus" << std::endl;
  }
};

// EN: Facade — ONE method to initialize the entire instrument cluster.
class InstrumentClusterFacade {
  AudioSubsystem audio_;
  SensorFusionEngine sensors_;
  DisplayRenderer display_;
  CANGateway can_;

public:
  void startCluster(const std::string &vehicleId) {
    std::cout << "[Facade] Initializing instrument cluster for "
              << vehicleId << "..." << std::endl;
    display_.setResolution(1920, 720);
    sensors_.calibrateSensors(12);
    audio_.playChime("startup_chime.wav");
    can_.connectBus("Vehicle CAN-HS");
    std::cout << "[Facade] Cluster ready!" << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [4. COMPOSITE PATTERN — File System Tree]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class IFileComponent {
public:
  virtual ~IFileComponent() = default;
  virtual int getSize() const = 0;
  virtual void display(int depth = 0) const = 0;
  virtual std::string getName() const = 0;
};

class File : public IFileComponent {
  std::string name_;
  int size_;

public:
  File(std::string n, int s) : name_(std::move(n)), size_(s) {}
  int getSize() const override { return size_; }
  std::string getName() const override { return name_; }
  void display(int depth) const override {
    for (int i = 0; i < depth; i++) std::cout << "  ";
    std::cout << "📄 " << name_ << " (" << size_ << " KB)" << std::endl;
  }
};

class Folder : public IFileComponent {
  std::string name_;
  std::vector<std::unique_ptr<IFileComponent>> children_;

public:
  explicit Folder(std::string n) : name_(std::move(n)) {}

  void add(std::unique_ptr<IFileComponent> child) {
    children_.push_back(std::move(child));
  }

  int getSize() const override {
    return std::accumulate(children_.begin(), children_.end(), 0,
                           [](int sum, const auto &c) { return sum + c->getSize(); });
  }

  std::string getName() const override { return name_; }

  void display(int depth) const override {
    for (int i = 0; i < depth; i++) std::cout << "  ";
    std::cout << "📁 " << name_ << " (" << getSize() << " KB total)" << std::endl;
    for (const auto &child : children_) {
      child->display(depth + 1);
    }
  }
};

int main() {
  std::cout << "=== MODULE 10: STRUCTURAL DESIGN PATTERNS ===\n" << std::endl;

  // --- 1. Adapter ---
  std::cout << "--- ADAPTER ---\n" << std::endl;
  XMLParserAdapter xmlAdapter;
  JSONParser jsonParser;
  std::cout << xmlAdapter.parse("<user>Ahmet</user>") << std::endl;
  std::cout << jsonParser.parse(R"({"name":"Ahmet"})") << "\n" << std::endl;

  // --- 2. Decorator (Stacking) ---
  std::cout << "--- DECORATOR (Stacking) ---\n" << std::endl;
  // EN: Stack: Plain -> Compressed -> Encrypted -> Logged
  auto msg = std::make_unique<PlainMessage>("Hello, World!");
  auto compressed = std::make_unique<CompressedMessage>(std::move(msg));
  auto encrypted = std::make_unique<EncryptedMessage>(std::move(compressed));
  auto logged = std::make_unique<LoggedMessage>(std::move(encrypted));
  std::cout << "Result: " << logged->getContent() << "\n" << std::endl;

  // --- 3. Facade ---
  std::cout << "--- FACADE ---\n" << std::endl;
  InstrumentClusterFacade cluster;
  cluster.startCluster("VIN_WBAPH5C55BA271234");
  std::cout << std::endl;

  // --- 4. Composite (File System) ---
  std::cout << "--- COMPOSITE (File System Tree) ---\n" << std::endl;
  auto root = std::make_unique<Folder>("project");
  auto src = std::make_unique<Folder>("src");
  src->add(std::make_unique<File>("main.cpp", 12));
  src->add(std::make_unique<File>("utils.cpp", 8));
  auto docs = std::make_unique<Folder>("docs");
  docs->add(std::make_unique<File>("README.md", 3));
  root->add(std::move(src));
  root->add(std::move(docs));
  root->add(std::make_unique<File>("Makefile", 1));
  root->display(0);

  std::cout << "\n=> All structural patterns demonstrated!" << std::endl;

  return 0;
}
