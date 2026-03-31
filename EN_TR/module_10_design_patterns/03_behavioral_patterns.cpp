/**
 * @file module_10_design_patterns/03_behavioral_patterns.cpp
 * @brief Design Patterns: Behavioral — Observer, Strategy, Command, State — Davranışsal Kalıplar
 *
 * @details
 * =============================================================================
 * [THEORY: Behavioral Patterns Overview / TEORİ: Davranışsal Kalıplar]
 * =============================================================================
 * EN: Behavioral patterns focus on HOW objects communicate and distribute responsibilities. They
 * define the protocols for interaction between objects, making systems more flexible and easier
 * to extend. These patterns are the backbone of event-driven architectures, ECU communication
 * lines, and UI frameworks.
 *
 * TR: Davranışsal kalıplar, nesnelerin NASIL iletişim kurduğuna ve sorumlulukları nasıl
 * dağıttığına odaklanır. Nesneler arasındaki etkileşim protokollerini tanımlar, sistemleri daha
 * esnek ve genişletmesi kolay kılar. Bu kalıplar olay-güdümlü mimarilerin, ECU iletişim
 * hatlarının ve HMI çerçevelerinin omurgasıdır.
 *
 * =============================================================================
 * [THEORY: Observer — Publish/Subscribe Event System / TEORİ: Observer — Yayınla/Abone Ol Olay
 * Sistemi]
 * =============================================================================
 * EN: The Observer pattern defines a one-to-many dependency: when one object (Subject) changes
 * state, ALL registered observers are notified automatically. The subject doesn't know concrete
 * observer types — it only knows the `IObserver` interface. Used in: ECU event buses, HMI
 * notification systems, CAN signal dispatchers, sensor data pipelines.
 *
 * TR: Observer kalıbı bir-çoğa bağımlılık tanımlar: bir nesne (Konu) durum değiştirdiğinde, TÜM
 * kayıtlı gözlemciler otomatik bilgilendirilir. Konu, somut gözlemci tiplerini bilmez — yalnızca
 * `IObserver` arayüzünü bilir. Kullanım: ECU olay veri yolları, HMI bildirim sistemleri, CAN
 * sinyal dağıtıcıları, sensör veri hatları.
 *
 * =============================================================================
 * [THEORY: Strategy — Swappable Algorithms at Runtime / TEORİ: Strategy — Çalışma Zamanında
 * Değiştirilebilir Algoritmalar]
 * =============================================================================
 * EN: The Strategy pattern encapsulates a family of algorithms behind an interface and makes
 * them interchangeable at runtime. Instead of `if/else` chains selecting behavior, you inject
 * the desired strategy object. The context delegates the algorithm call to the strategy. Adding
 * new algorithms requires ZERO modification of existing code (Open/Closed Principle).
 *
 * TR: Strategy kalıbı, bir algoritma ailesini bir arayüz arkasında kapsüller ve çalışma
 * zamanında değiştirilebilir kılar. Davranış seçen `if/else` zincirleri yerine istenen strateji
 * nesnesi enjekte edilir. Bağlam, algoritma çağrısını stratejiye delege eder. Yeni algoritma
 * eklemek mevcut kodda SIFIR değişiklik gerektirir (Açık/Kapalı Prensibi).
 *
 * =============================================================================
 * [THEORY: Command — Encapsulate Request as Object / TEORİ: Command — İsteği Nesne Olarak
 * Kapsülle]
 * =============================================================================
 * EN: The Command pattern turns a request/action into a standalone object with all necessary
 * information. This enables: queuing commands, logging them, implementing undo/redo, and
 * executing them later. Each command implements an `execute()` method. The invoker doesn't know
 * what the command does — it just calls `execute()`.
 *
 * TR: Command kalıbı, bir istek/eylemi gerekli tüm bilgilerle bağımsız bir nesneye dönüştürür.
 * Bu şunları mümkün kılar: komutları kuyruğa alma, loglama, geri alma/yineleme uygulamak ve
 * sonra çalıştırma. Her komut bir `execute()` metodu uygular. Çağırıcı komutun ne yaptığını
 * bilmez — sadece `execute()` çağırır.
 *
 * =============================================================================
 * [THEORY: State — Object Behavior Changes with Internal State / TEORİ: State — Nesne Davranışı
 * İç Durumla Değişir]
 * =============================================================================
 * EN: The State pattern lets an object change its behavior when its internal state changes.
 * Instead of massive `switch/case` blocks, each state is a separate class with its own behavior.
 * State transitions happen by replacing the current state object. The context object delegates
 * all calls to the current state.
 *
 * TR: State kalıbı, bir nesnenin iç durumu değiştiğinde davranışını değiştirmesine olanak tanır.
 * Devasa `switch/case` blokları yerine her durum, kendi davranışına sahip ayrı bir sınıftır.
 * Durum geçişleri mevcut durum nesnesini değiştirerek gerçekleşir. Bağlam nesnesi tüm çağrıları
 * mevcut duruma delege eder.
 *
 * =============================================================================
 * [CPPREF DEPTH: std::function as Strategy Container / CPPREF DERİNLİK: Strateji Kapsayıcı
 * Olarak std::function]
 * =============================================================================
 * EN: In modern C++, you don't always need a full class hierarchy for Strategy.
 * `std::function<ReturnType(Args...)>` can hold any callable (function pointer, lambda, functor,
 * member function via std::bind). This is lighter than virtual dispatch for simple strategies.
 * However, `std::function` has type-erasure overhead (~40 bytes + potential heap allocation for
 * large callables). cppreference.com/w/cpp/utility/functional/function
 *
 * TR: Modern C++'ta Strategy için her zaman tam bir sınıf hiyerarşisine ihtiyacınız yok.
 * `std::function<DönüşTipi(Argümanlar...)>` herhangi bir çağrılabiliryi tutabilir (fonksiyon
 * işaretçisi, lambda, functor, std::bind ile üye fonksiyon). Basit stratejiler için sanal
 * fonksiyon dağıtımından daha hafiftir. Ancak `std::function`'ın tip-silme maliyeti vardır (~40
 * bayt + büyük çağrılabilirler için potansiyel heap tahsisi).
 * cppreference.com/w/cpp/utility/functional/function
 *
 * =============================================================================
 * [CPPREF DEPTH: std::variant as Type-Safe State Machine / CPPREF DERİNLİK: Tip-Güvenli Durum
 * Makinesi Olarak std::variant]
 * =============================================================================
 * EN: C++17's `std::variant` can replace virtual-dispatch State pattern with a zero-overhead,
 * value-based approach. Each state is a struct, the variant holds one at a time. `std::visit`
 * with a visitor dispatches to the correct state handler at compile-time. No heap allocation, no
 * virtual table — pure stack-based state machine. cppreference.com/w/cpp/utility/variant
 *
 * TR: C++17'nin `std::variant`'ı, sanal-dağıtımlı State kalıbını sıfır maliyetli, değer-tabanlı
 * bir yaklaşımla değiştirebilir. Her durum bir struct, variant aynı anda birini tutar. Bir
 * visitor ile `std::visit`, derleme zamanında doğru durum işleyicisine yönlendirir. Heap tahsisi
 * yok, sanal tablo yok — tamamen yığın-tabanlı durum makinesi.
 * cppreference.com/w/cpp/utility/variant
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_behavioral_patterns.cpp -o 03_behavioral_patterns
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [1. OBSERVER PATTERN]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class IObserver {
public:
  virtual ~IObserver() = default;
  virtual void onNotify(const std::string &event) = 0;
};

class EventBus {
  std::vector<IObserver *> observers_;

public:
  void subscribe(IObserver *obs) { observers_.push_back(obs); }

  void notify(const std::string &event) {
    for (auto *obs : observers_) {
      obs->onNotify(event);
    }
  }
};

class FaultLogger : public IObserver {
public:
  void onNotify(const std::string &event) override {
    if (event == "ENGINE_OVERHEAT") {
      std::cout << "[FaultLog] Logging DTC P0217 — Engine Overheat."
                << std::endl;
    } else if (event == "MAINTENANCE_DUE") {
      std::cout << "[FaultLog] Logging scheduled maintenance reminder."
                << std::endl;
    }
  }
};

class ClusterDisplay : public IObserver {
public:
  void onNotify(const std::string &event) override {
    if (event == "ENGINE_OVERHEAT") {
      std::cout << "[Cluster] Showing RED overheat warning on dashboard!"
                << std::endl;
    } else if (event == "MAINTENANCE_DUE") {
      std::cout << "[Cluster] Showing wrench icon — service needed."
                << std::endl;
    }
  }
};

class TelematicsReporter : public IObserver {
public:
  void onNotify(const std::string &event) override {
    if (event == "ENGINE_OVERHEAT") {
      std::cout << "[Telematics] Uploading overheat snapshot to cloud."
                << std::endl;
    }
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [2. STRATEGY PATTERN]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class ISortStrategy {
public:
  virtual ~ISortStrategy() = default;
  virtual void sort(std::vector<int> &data) const = 0;
  virtual std::string name() const = 0;
};

class BubbleSort : public ISortStrategy {
public:
  void sort(std::vector<int> &data) const override {
    for (size_t i = 0; i < data.size(); i++) {
      for (size_t j = 0; j + 1 < data.size() - i; j++) {
        if (data[j] > data[j + 1]) std::swap(data[j], data[j + 1]);
      }
    }
  }
  std::string name() const override { return "BubbleSort"; }
};

class SelectionSort : public ISortStrategy {
public:
  void sort(std::vector<int> &data) const override {
    for (size_t i = 0; i < data.size(); i++) {
      size_t minIdx = i;
      for (size_t j = i + 1; j < data.size(); j++) {
        if (data[j] < data[minIdx]) minIdx = j;
      }
      std::swap(data[i], data[minIdx]);
    }
  }
  std::string name() const override { return "SelectionSort"; }
};

class InsertionSort : public ISortStrategy {
public:
  void sort(std::vector<int> &data) const override {
    for (size_t i = 1; i < data.size(); i++) {
      int key = data[i];
      size_t j = i;
      while (j > 0 && data[j - 1] > key) {
        data[j] = data[j - 1];
        j--;
      }
      data[j] = key;
    }
  }
  std::string name() const override { return "InsertionSort"; }
};

// EN: Context class — delegates sorting to injected strategy.
// TR: Bağlam sınıfı — sıralamayı enjekte edilen stratejiye delege eder.
class Sorter {
  std::unique_ptr<ISortStrategy> strategy_;

public:
  void setStrategy(std::unique_ptr<ISortStrategy> s) {
    strategy_ = std::move(s);
  }
  void doSort(std::vector<int> &data) {
    if (strategy_) {
      std::cout << "[Strategy] Using " << strategy_->name() << ": ";
      strategy_->sort(data);
      for (int v : data) std::cout << v << " ";
      std::cout << std::endl;
    }
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [3. COMMAND PATTERN — Undo/Redo Support]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class ICommand {
public:
  virtual ~ICommand() = default;
  virtual void execute() = 0;
  virtual void undo() = 0;
  virtual std::string description() const = 0;
};

class TextEditor {
  std::string text_;

public:
  void insert(const std::string &s) { text_ += s; }
  void removeLast(size_t n) {
    if (n <= text_.size()) text_.erase(text_.size() - n, n);
  }
  const std::string &getText() const { return text_; }
};

class InsertTextCommand : public ICommand {
  TextEditor &editor_;
  std::string text_;

public:
  InsertTextCommand(TextEditor &e, std::string t)
      : editor_(e), text_(std::move(t)) {}
  void execute() override { editor_.insert(text_); }
  void undo() override { editor_.removeLast(text_.size()); }
  std::string description() const override {
    return "Insert '" + text_ + "'";
  }
};

class CommandHistory {
  std::vector<std::unique_ptr<ICommand>> history_;

public:
  void executeCommand(std::unique_ptr<ICommand> cmd) {
    cmd->execute();
    std::cout << "[Command] Executed: " << cmd->description() << std::endl;
    history_.push_back(std::move(cmd));
  }
  void undoLast() {
    if (!history_.empty()) {
      auto &cmd = history_.back();
      cmd->undo();
      std::cout << "[Command] Undone: " << cmd->description() << std::endl;
      history_.pop_back();
    }
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [4. STATE PATTERN — Traffic Light State Machine]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class TrafficLight; // Forward declaration

class ITrafficState {
public:
  virtual ~ITrafficState() = default;
  virtual void handle(TrafficLight &context) = 0;
  virtual std::string color() const = 0;
};

class TrafficLight {
  std::unique_ptr<ITrafficState> state_;

public:
  explicit TrafficLight(std::unique_ptr<ITrafficState> initial)
      : state_(std::move(initial)) {}

  void setState(std::unique_ptr<ITrafficState> s) { state_ = std::move(s); }

  void next() {
    std::cout << "[State] Current: " << state_->color() << " -> ";
    state_->handle(*this);
  }
};

class GreenState : public ITrafficState {
public:
  void handle(TrafficLight &context) override;
  std::string color() const override { return "GREEN"; }
};

class YellowState : public ITrafficState {
public:
  void handle(TrafficLight &context) override;
  std::string color() const override { return "YELLOW"; }
};

class RedState : public ITrafficState {
public:
  void handle(TrafficLight &context) override {
    std::cout << "switching to GREEN" << std::endl;
    context.setState(std::make_unique<GreenState>());
  }
  std::string color() const override { return "RED"; }
};

void GreenState::handle(TrafficLight &context) {
  std::cout << "switching to YELLOW" << std::endl;
  context.setState(std::make_unique<YellowState>());
}

void YellowState::handle(TrafficLight &context) {
  std::cout << "switching to RED" << std::endl;
  context.setState(std::make_unique<RedState>());
}

int main() {
  std::cout << "=== MODULE 10: BEHAVIORAL DESIGN PATTERNS ===\n" << std::endl;

  // --- 1. Observer ---
  std::cout << "--- OBSERVER (Vehicle Event Bus) ---\n" << std::endl;
  EventBus bus;
  FaultLogger faultLog;
  ClusterDisplay cluster;
  TelematicsReporter telematics;
  bus.subscribe(&faultLog);
  bus.subscribe(&cluster);
  bus.subscribe(&telematics);
  bus.notify("ENGINE_OVERHEAT");
  std::cout << std::endl;
  bus.notify("MAINTENANCE_DUE");
  std::cout << std::endl;

  // --- 2. Strategy ---
  std::cout << "--- STRATEGY (Sorting) ---\n" << std::endl;
  Sorter sorter;
  std::vector<int> data1 = {5, 2, 8, 1, 9};
  std::vector<int> data2 = {7, 3, 6, 4, 0};
  std::vector<int> data3 = {12, 11, 15, 13, 14};

  sorter.setStrategy(std::make_unique<BubbleSort>());
  sorter.doSort(data1);
  sorter.setStrategy(std::make_unique<SelectionSort>());
  sorter.doSort(data2);
  sorter.setStrategy(std::make_unique<InsertionSort>());
  sorter.doSort(data3);
  std::cout << std::endl;

  // --- 3. Command (Undo/Redo) ---
  std::cout << "--- COMMAND (Text Editor with Undo) ---\n" << std::endl;
  TextEditor editor;
  CommandHistory history;

  history.executeCommand(
      std::make_unique<InsertTextCommand>(editor, "Hello"));
  history.executeCommand(
      std::make_unique<InsertTextCommand>(editor, " World"));
  history.executeCommand(
      std::make_unique<InsertTextCommand>(editor, "!"));
  std::cout << "Text: \"" << editor.getText() << "\"" << std::endl;

  history.undoLast(); // Removes "!"
  std::cout << "After undo: \"" << editor.getText() << "\"" << std::endl;
  history.undoLast(); // Removes " World"
  std::cout << "After undo: \"" << editor.getText() << "\"\n" << std::endl;

  // --- 4. State (Traffic Light) ---
  std::cout << "--- STATE (Traffic Light) ---\n" << std::endl;
  TrafficLight light(std::make_unique<GreenState>());
  for (int i = 0; i < 6; i++) {
    light.next(); // GREEN->YELLOW->RED->GREEN->YELLOW->RED
  }

  std::cout << "\n=> All behavioral patterns demonstrated!" << std::endl;

  return 0;
}
