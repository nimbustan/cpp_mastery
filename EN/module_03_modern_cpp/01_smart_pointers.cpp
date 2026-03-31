/**
 * @file module_03_modern_cpp/01_smart_pointers.cpp
 * @brief Modern C++: Smart Pointers and RAII
 *
 * @details
 * =============================================================================
 * [THEORY: Why Do We Need Smart Pointers?]
 * =============================================================================
 * EN: In old C++ (before C++11), developers managed memory dynamically using `new` and `delete`.
 * This was a nightmare. If a function threw an exception or returned early, `delete` was skipped
 * -> Memory Leak! If we deleted a pointer twice -> Double Free Crash!
 *
 * =============================================================================
 * [THEORY: What is RAII?]
 * =============================================================================
 * EN: The backbone principle of C++. "When a resource (memory, file, socket) is Acquired, bind
 * it to a local/stack Object. When that Object goes out of scope ({...}), its Destructor
 * automatically runs and safely frees the resource." Smart Pointers are pure RAII. They silently
 * call `delete` for you.
 * 1. std::unique_ptr: UNIQUE Ownership. Cannot be copied (only moved).
 *    "I am the sole owner of this memory. If I die, it dies."
 * 2. std::shared_ptr: SHARED Ownership. Counts how many people hold the memory
 *    (Reference Counting). When the count hits 0, the memory vanishes.
 * 3. std::weak_ptr: A "watcher" for shared_ptr that doesn't increment
 *    the count, preventing circular reference loops!
 *
 * =============================================================================
 * [CPPREF DEPTH: `std::make_shared` vs `new` & The Control Block]
 * =============================================================================
 * EN: CppReference dictates: Always use `std::make_shared` instead of `std::shared_ptr<T>(new
 * T())`. Why? Because `shared_ptr` needs TWO things:
 *   1. The actual object.
 *   2. A "Control Block" (holds the counter).
 * Using `new` allocates these separately (2 RAM jumps). `make_shared` allocates them in ONE
 * single contiguous memory block (Faster Cache Hits, No memory leaps).
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_smart_pointers.cpp -o 01_smart_pointers
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdio>
#include <string>
// EN: Required for smart pointers
#include <memory>

/**
 * @class HeavyResource
 * @brief Represents a memory-heavy object like a 4K texture or a gigabyte DB.
 */
class HeavyResource {
public:
  HeavyResource() {
    std::cout << "[HeavyResource] Acquired from OS." << std::endl;
  }
  ~HeavyResource() {
    std::cout << "[HeavyResource] Destroyed safely! -> NO LEAK" << std::endl;
  }
  void render() {
    std::cout << "Rendering 4K Textures..." << std::endl;
  }
};

int main() {
  std::cout << "=== MODERN C++: SMART POINTERS & RAII ===\n" << std::endl;

  std::cout << "--- 1. Raw Pointer (Legacy System - Bad Practice) ---" << std::endl;
  HeavyResource *rawPointer = new HeavyResource();

  // EN: If an exception happens here, 'delete' is skipped -> Memory Leak!
  delete rawPointer;

  std::cout << "\n--- 2. std::unique_ptr (C++14 Way - Best Practice) ---" << std::endl;
  {
    // EN: Scope begins! `std::make_unique()` is faster and SAFER than calling `new`.
    std::unique_ptr<HeavyResource> uniquePtr = std::make_unique<HeavyResource>();
    uniquePtr->render();

    // auto uniquePtr2 = uniquePtr; // ERROR! Compiler forbids it. Ownership is unique. / HATA!
    // KOPYALANAMAZ.

    // EN: Allowed! Ownership transferred via Move Semantics.
    std::unique_ptr<HeavyResource> movedPtr = std::move(uniquePtr);

  } // EN: Scope ends. movedPtr dies. Destructor AUTO-RUNS!

  std::cout << "\n--- 3. std::shared_ptr (Shared Ownership) ---" << std::endl;
  {
    // Count = 1 (Saya)
    std::shared_ptr<HeavyResource> shared1 = std::make_shared<HeavyResource>();
    std::cout << "Owners :" << shared1.use_count() << std::endl;

    {
      // EN: Copied! Count = 2
      std::shared_ptr<HeavyResource> shared2 = shared1;
      std::cout << "Owners in lower scope :" << shared1.use_count() << std::endl;
    } // EN: shared2 dies. Count drops to 1. Object still lives.

    std::cout << "Owners after lower scope :" << shared1.use_count() << std::endl;
  } // EN: shared1 dies. Count becomes 0. OBJECT DESTROYED!

  // ═══════════════════════════════════════════════════════════════════════════
  // PART 4: Custom Deleter
  // ═══════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 4. Custom Deleter (unique_ptr with FILE*) ---" << std::endl;
  {
    // EN: unique_ptr can manage non-new resources with a custom deleter.
    //     Classic example: C FILE* must be closed with fclose(), not delete.
    auto fileDeleter = [](FILE* fp) {
      if (fp) {
        std::fclose(fp);
        std::cout << " Custom deleter: FILE* closed" << std::endl;
      }
    };

    {
      std::unique_ptr<FILE, decltype(fileDeleter)> filePtr(
          std::fopen("/tmp/smart_ptr_test.txt", "w"), fileDeleter);
      if (filePtr) {
        std::fputs("Hello from unique_ptr with custom deleter!\n", filePtr.get());
        std::cout << "  Written to /tmp/smart_ptr_test.txt" << std::endl;
      }
    } // EN: fileDeleter auto-called here — fclose(), not delete!

    // EN: Another pattern: array deleter (C++11 style, C++14 has make_unique<T[]>)
    auto arrayDeleter = [](int* p) {
      std::cout << " Custom deleter: array deleted" << std::endl;
      delete[] p;
    };
    {
      std::unique_ptr<int, decltype(arrayDeleter)> arr(new int[5]{10, 20, 30, 40, 50},
                                                       arrayDeleter);
      std::cout << "  Array[2] = " << arr.get()[2] << std::endl;
    } // EN: arrayDeleter auto-called — delete[], not delete!
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // PART 5: enable_shared_from_this
  // ═══════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 5. enable_shared_from_this ---" << std::endl;
  {
    // EN: Problem: Inside a method, you need a shared_ptr to `this`.
    //     BAD:  shared_ptr<Foo>(this)  — creates SECOND control block → double free!
    //     GOOD: inherit enable_shared_from_this<Foo>, call shared_from_this().

    struct EventEmitter : public std::enable_shared_from_this<EventEmitter> {
      std::string name;

      explicit EventEmitter(const std::string& n) : name(n) {}

      std::shared_ptr<EventEmitter> getShared() {
        return shared_from_this();
      }

      void describe() const {
        std::cout << "  EventEmitter: " << name << std::endl;
      }
    };

    auto emitter = std::make_shared<EventEmitter>("SensorHub");
    std::cout << "  use_count before: " << emitter.use_count() << std::endl;

    {
      // EN: Safe! Same control block, count incremented.
      auto alias = emitter->getShared();
      std::cout << "  use_count with alias: " << emitter.use_count() << std::endl;
      alias->describe();
    }

    std::cout << "  use_count after alias dies: " << emitter.use_count() << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // PART 6: Aliasing Constructor
  // ═══════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 6. Aliasing Constructor (shared_ptr to member) ---" << std::endl;
  {
    // EN: The aliasing constructor creates a shared_ptr that shares ownership
    //     with another shared_ptr, but points to a DIFFERENT object (e.g., a member).
    //     Signature: shared_ptr<T>(shared_ptr<U> owner, T* ptr)
    //     The owner keeps the parent alive; ptr is what you dereference.

    struct Engine {
      int horsepower = 450;
    };

    struct Car {
      std::string model = "M3 GTR";
      Engine engine;
    };

    auto car = std::make_shared<Car>();
    std::cout << "  car use_count: " << car.use_count() << std::endl;

    // EN: Aliasing: enginePtr shares ownership with car, but points to car->engine.
    //     As long as enginePtr is alive, the entire Car stays alive.
    std::shared_ptr<Engine> enginePtr(car, &car->engine);
    std::cout << "  car use_count after alias: " << car.use_count() << std::endl;
    std::cout << "  enginePtr->horsepower: " << enginePtr->horsepower << std::endl;

    car.reset(); // EN: We release car, but enginePtr keeps it alive
    std::cout << "  After car.reset(), enginePtr->horsepower: "
              << enginePtr->horsepower << std::endl;
    std::cout << "  (Car still alive because enginePtr holds ownership)"
              << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // PART 7: weak_ptr — Breaking Circular References
  // ═══════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- 7. weak_ptr (Circular Reference Breaker) ---" << std::endl;
  {
    // EN: Without weak_ptr, two objects holding shared_ptr to each other
    //     create a circular reference — neither is ever freed (memory leak).

    struct Node {
      std::string name;
      std::weak_ptr<Node> partner;  // EN: weak, not shared!

      explicit Node(const std::string& n) : name(n) {
        std::cout << "  Node " << name << " created" << std::endl;
      }
      ~Node() {
        std::cout << "  Node " << name << " destroyed" << std::endl;
      }
    };

    {
      auto nodeA = std::make_shared<Node>("A");
      auto nodeB = std::make_shared<Node>("B");

      nodeA->partner = nodeB;  // EN: weak reference, count NOT incremented
      nodeB->partner = nodeA;

      std::cout << "  A use_count: " << nodeA.use_count()
                << ", B use_count: " << nodeB.use_count() << std::endl;

      // EN: Access partner through weak_ptr — must lock() first
      if (auto locked = nodeA->partner.lock()) {
        std::cout << "  A's partner: " << locked->name << std::endl;
      }

      // EN: Check if partner is still alive
      std::cout << "  B's partner expired? " << std::boolalpha
                << nodeA->partner.expired() << std::endl;
    } // EN: Both destroyed! No circular leak because weak_ptr doesn't hold ownership.
  }

  return 0;
}
