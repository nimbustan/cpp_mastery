/**
 * @file module_04_advanced/02_functors_callbacks.cpp
 * @brief Advanced STL: Functors and Callbacks
 *
 * @details
 * =============================================================================
 * [THEORY: Why Are Standard Functions Insufficient? (What is a Functor?)]
 * =============================================================================
 * EN: A regular function holds NO state (no memory of past calls). A "Functor" (Function Object)
 * is a Class that acts like a function by overloading `operator()`. It can have internal
 * variables (state), a Constructor, and can be passed to STL algorithms like `std::sort` or
 * `std::for_each`.
 *
 *
 * =============================================================================
 * [THEORY: Callbacks and std::function]
 * =============================================================================
 * EN: In HMI (Head-Up Display) or Networking, when a button is pressed, you don't know exactly
 * which logic should run at compile time. You "donate" a function to that button. This is called
 * a "Callback". Using `#include <functional>`, `std::function` acts as a universal vessel
 * capable of storing ANY callable (Lambdas, Functors, raw functions) like a variable!
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: `std::function` CPU Penalty & Type Erasure (Tip Silinmesi)]
 * =============================================================================
 * EN: `std::function` achieves this magic using "Type Erasure". However, if the lambda/functor
 * you store inside it is too large (usually > 16-32 bytes), it FAILS the "Small Object
 * Optimization (SOO)" and silently calls `new` to allocate heap memory! In a rendering loop (60
 * FPS), this silent allocation can murder system performance!
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_functors_callbacks.cpp -o 02_functors_callbacks
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>
#include <functional> // EN: std::function
#include <iostream>
#include <vector>

/**
 * @class DeductorFunctor
 * @brief EN: A Stateful Functor evaluating ongoing calculations.
 */
class DeductorFunctor {
private:
  // EN: STATE (Memory of past calls)
  int balance;
public:
  DeductorFunctor(int initialBalance) : balance(initialBalance) {}

  // EN: Overloading the parenthesis () operator! Now this CLASS can be called like a FUNCTION!
  void operator()(int amount) {
    balance -= amount;
    std::cout << amount << " spent. Remaining Balance: " << balance << std::endl;
  }
};

/**
 * @brief EN: GUI or Networking Simulation. "Give me a function to execute later!"
 * @param callback
 */
void executeEvent(std::function<void(int)> callback) {
  std::cout << "\n[SYSTEM] Event triggered! Starting Callback..."
            << std::endl;
  // EN: Running the injected function internally.
  callback(50);
}

int main() {
  std::cout << "=== MODULE 4: FUNCTORS & CALLBACKS (std::function) ===\n" << std::endl;

  std::cout << "--- 1. FUNCTORS (Stateful Functions) ---" << std::endl;
  DeductorFunctor bankSystem(100);

  std::cout << ">> Used exactly like a function, but it's an Object!"
            << std::endl;
  // EN: 1st Call: Balance is 80. (It remembers!)
  bankSystem( 20);
  // EN: 2nd Call: Balance is 65.
  bankSystem(15);

  // 2. Callbacks Test
  std::cout << "\n--- 2. CALLBACKS (std::function) ---" << std::endl;

  // EN: Lambda Carrier
  std::function<void(int)> myLambda = [](int x) {
    std::cout << "Lambda executed! Given value: " << x << std::endl;
  };

  executeEvent(myLambda);

  // EN: Callback via Functor
  executeEvent(bankSystem);

  return 0;
}
