/**
 * @file module_06_industry_standards/03_unit_testing_gtest.cpp
 * @brief Industry Standards: Unit Testing (TDD) with Google Test Simulation
 *
 * @details
 * =============================================================================
 * [THEORY: Why is Unit Testing the Peak of Professionalism?]
 * =============================================================================
 * EN: Writing code is easy. Proving that it works perfectly under every condition is
 * engineering. If you change a function today, how do you know you didn't break something you
 * wrote 3 months ago? "Test-Driven Development (TDD)" and frameworks like "Google Test (GTest)"
 * automate this. The cycle:
 *   1) Write a FAILING test,
 *   2) Write minimal code to pass,
 *   3) Refactor.
 *
 *
 * =============================================================================
 * [THEORY: TEST() and TEST_F() Macros]
 * =============================================================================
 * EN: In real GTest, `TEST(SuiteName, TestName)` defines a standalone test case.
 * `TEST_F(FixtureName, TestName)` defines a test that uses a "Test Fixture" — a class with
 * `SetUp()` and `TearDown()` methods that run before and after every test. Fixtures share setup
 * logic (like creating database connections) across multiple tests without code duplication.
 *
 *
 * =============================================================================
 * [THEORY: EXPECT vs ASSERT]
 * =============================================================================
 * EN: `ASSERT_EQ(a, b)` — if fails, test STOPS immediately. Use for critical preconditions.
 * `EXPECT_EQ(a, b)` — if fails, test CONTINUES running remaining checks (reports all failures at
 * end). In production, `EXPECT_*` is preferred so you see ALL failures in one run, not just the
 * first one.
 *
 *
 * =============================================================================
 * [THEORY: Dependency Injection for Testability]
 * =============================================================================
 * EN: Tightly coupled code (class directly calls database/network) is UNTESTABLE because you
 * can't run the test without an actual server. Solution: inject dependencies through interfaces
 * (abstract classes). In tests, you pass a "Mock" or "Stub" instead of the real implementation.
 * The class under test doesn't know the difference!
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: GTest Assertion Macros Reference]
 * =============================================================================
 * EN: Key GTest macros from the official documentation:
 *     1. `EXPECT_EQ(a, b)` / `ASSERT_EQ(a, b)` — equality.
 *     2. `EXPECT_NE(a, b)` — not equal.
 *     3. `EXPECT_LT(a, b)` / `EXPECT_GT(a, b)` — less/greater.
 *     4. `EXPECT_TRUE(cond)` / `EXPECT_FALSE(cond)` — boolean.
 *     5. `EXPECT_THROW(stmt, ExType)` — expects exception.
 *     6. `EXPECT_NEAR(a, b, eps)` — floating point comparison.
 *     7. `EXPECT_THAT(val, matcher)` — GMock matchers.
 *     google.github.io/googletest/reference/assertions.html
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: GMock — Interface Mocking for Isolated Tests]
 * =============================================================================
 * EN: Google Mock (GMock) allows creating mock classes that override virtual methods with
 * `MOCK_METHOD(ReturnType, Name, (Args), (Qualifiers))`. You can then set expectations:
 * `EXPECT_CALL(mock, method()).Times(1)`. GMock verifies that the expected calls were made. This
 * enables testing code WITHOUT any real external dependencies (network, database, filesystem).
 * google.github.io/googletest/gmock_for_dummies.html
 *
 *
 * @warning EN: Never test your own code by "just running and looking". Tests MUST be automated
 * (CI/CD)!
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_unit_testing_gtest.cpp -o 03_unit_testing_gtest
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [GTest SIMULATION FRAMEWORK]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: We simulate GTest macros (TEST, EXPECT_EQ, EXPECT_TRUE, EXPECT_THROW, EXPECT_NEAR) without
// needing the actual library.

namespace MiniGTest {

struct TestResult {
  std::string suiteName;
  std::string testName;
  bool passed = true;
  std::vector<std::string> failures;
};

std::vector<TestResult> allResults;
TestResult *currentTest = nullptr;

void registerTest(const std::string &suite, const std::string &name,
                  std::function<void()> fn) {
  allResults.push_back({suite, name, true, {}});
  currentTest = &allResults.back();
  try {
    fn();
  } catch (const std::exception &e) {
    currentTest->passed = false;
    currentTest->failures.push_back(std::string("EXCEPTION: ") + e.what());
  }
}

void expectEq(int expected, int actual, const char *file, int line) {
  if (expected != actual) {
    currentTest->passed = false;
    std::ostringstream oss;
    oss << file << ":" << line << " EXPECT_EQ failed: expected=" << expected
        << " actual=" << actual;
    currentTest->failures.push_back(oss.str());
  }
}

void expectTrue(bool cond, const char *file, int line) {
  if (!cond) {
    currentTest->passed = false;
    std::ostringstream oss;
    oss << file << ":" << line << " EXPECT_TRUE failed";
    currentTest->failures.push_back(oss.str());
  }
}

void expectFalse(bool cond, const char *file, int line) {
  if (cond) {
    currentTest->passed = false;
    std::ostringstream oss;
    oss << file << ":" << line << " EXPECT_FALSE failed";
    currentTest->failures.push_back(oss.str());
  }
}

void expectNear(double a, double b, double eps, const char *file, int line) {
  if (std::fabs(a - b) > eps) {
    currentTest->passed = false;
    std::ostringstream oss;
    oss << file << ":" << line << " EXPECT_NEAR failed: |" << a << " - " << b
        << "| > " << eps;
    currentTest->failures.push_back(oss.str());
  }
}

void printSummary() {
  int passed = 0;
  int failed = 0;
  for (const auto &r : allResults) {
    std::cout << (r.passed ? "\033[32m[ PASSED ]\033[0m "
                           : "\033[31m[ FAILED ]\033[0m ")
              << r.suiteName << "." << r.testName << std::endl;
    if (!r.passed) {
      for (const auto &f : r.failures) {
        std::cout << "           " << f << std::endl;
      }
      failed++;
    } else {
      passed++;
    }
  }
  std::cout << "\n========================================" << std::endl;
  std::cout << "[ SUMMARY ] " << passed << " passed, " << failed << " failed, "
            << (passed + failed) << " total." << std::endl;
}

} // namespace MiniGTest

// Macros simulating real GTest interface
#define TEST(suite, name)                                                      \
  void suite##_##name();                                                       \
  struct suite##_##name##_reg {                                                \
    suite##_##name##_reg() {                                                   \
      MiniGTest::registerTest(#suite, #name, suite##_##name);                  \
    }                                                                          \
  } suite##_##name##_instance;                                                 \
  void suite##_##name()

#define EXPECT_EQ(expected, actual)                                            \
  MiniGTest::expectEq(expected, actual, __FILE__, __LINE__)
#define EXPECT_TRUE(cond) MiniGTest::expectTrue(cond, __FILE__, __LINE__)
#define EXPECT_FALSE(cond) MiniGTest::expectFalse(cond, __FILE__, __LINE__)
#define EXPECT_NEAR(a, b, eps)                                                \
  MiniGTest::expectNear(a, b, eps, __FILE__, __LINE__)

#define EXPECT_THROW(stmt, ExType)                                            \
  do {                                                                        \
    bool caught = false;                                                      \
    try {                                                                     \
      stmt;                                                                   \
    } catch (const ExType &) {                                                \
      caught = true;                                                          \
    } catch (...) {                                                           \
    }                                                                         \
    if (!caught) {                                                            \
      MiniGTest::currentTest->passed = false;                                 \
      MiniGTest::currentTest->failures.push_back(                             \
          std::string(__FILE__) + ":" + std::to_string(__LINE__) +            \
          " EXPECT_THROW: no " #ExType " thrown");                            \
    }                                                                         \
  } while (0)

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [PRODUCTION CODE]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// --- Brake Force Calculator (Automotive) ---
int calculateBrakeForce(int pedalPressure, int brakeWear) {
  int force = pedalPressure - brakeWear;
  return (force < 0) ? 0 : force;
}

// --- Temperature Converter ---
double celsiusToFahrenheit(double c) { return c * 9.0 / 5.0 + 32.0; }

// --- Safe Division with Exception ---
double safeDivide(double a, double b) {
  if (b == 0.0) {
    throw std::invalid_argument("Division by zero!");
  }
  return a / b;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEPENDENCY INJECTION]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Interface (abstract class) for a payment gateway. Production code uses RealPaymentGateway;
// tests use MockPaymentGateway.

class IPaymentGateway {
public:
  virtual ~IPaymentGateway() = default;
  virtual bool processPayment(double amount) = 0;
  virtual double getBalance() const = 0;
};

// EN: Mock implementation for testing — no real payment processing!
class MockPaymentGateway : public IPaymentGateway {
  double balance_;
  int callCount_ = 0;

public:
  explicit MockPaymentGateway(double balance) : balance_(balance) {}
  bool processPayment(double amount) override {
    callCount_++;
    if (amount > balance_) return false;
    balance_ -= amount;
    return true;
  }
  double getBalance() const override { return balance_; }
  int getCallCount() const { return callCount_; }
};

// EN: The class under test — takes IPaymentGateway via constructor injection.
class OrderProcessor {
  IPaymentGateway &gateway_;

public:
  explicit OrderProcessor(IPaymentGateway &gw) : gateway_(gw) {}
  bool placeOrder(double price) {
    if (price <= 0) return false;
    return gateway_.processPayment(price);
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [TEST CASES — Simulating Real GTest TEST() Macros]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// --- Suite: BrakeForceTest ---
TEST(BrakeForceTest, NormalBraking) {
  EXPECT_EQ(20, calculateBrakeForce(50, 30));
}

TEST(BrakeForceTest, WornBrakesAbsorbFully) {
  EXPECT_EQ(0, calculateBrakeForce(10, 50));
}

TEST(BrakeForceTest, NewBrakePads) {
  EXPECT_EQ(100, calculateBrakeForce(100, 0));
}

TEST(BrakeForceTest, EqualPressureAndWear) {
  EXPECT_EQ(0, calculateBrakeForce(30, 30));
}

// --- Suite: TemperatureTest ---
TEST(TemperatureTest, BoilingPoint) {
  EXPECT_NEAR(212.0, celsiusToFahrenheit(100.0), 0.001);
}

TEST(TemperatureTest, FreezingPoint) {
  EXPECT_NEAR(32.0, celsiusToFahrenheit(0.0), 0.001);
}

TEST(TemperatureTest, BodyTemperature) {
  EXPECT_NEAR(98.6, celsiusToFahrenheit(37.0), 0.1);
}

// --- Suite: DivisionTest ---
TEST(DivisionTest, NormalDivision) {
  EXPECT_NEAR(5.0, safeDivide(10.0, 2.0), 0.001);
}

TEST(DivisionTest, DivisionByZeroThrows) {
  EXPECT_THROW(safeDivide(10.0, 0.0), std::invalid_argument);
}

// --- Suite: PaymentTest (Dependency Injection + Mock) ---
TEST(PaymentTest, SuccessfulOrder) {
  MockPaymentGateway mock(100.0);
  OrderProcessor processor(mock);
  EXPECT_TRUE(processor.placeOrder(30.0));
  EXPECT_NEAR(70.0, mock.getBalance(), 0.001);
}

TEST(PaymentTest, InsufficientFunds) {
  MockPaymentGateway mock(20.0);
  OrderProcessor processor(mock);
  EXPECT_FALSE(processor.placeOrder(50.0));
  EXPECT_NEAR(20.0, mock.getBalance(), 0.001);
}

TEST(PaymentTest, InvalidPrice) {
  MockPaymentGateway mock(100.0);
  OrderProcessor processor(mock);
  EXPECT_FALSE(processor.placeOrder(-10.0));
  EXPECT_EQ(0, mock.getCallCount());
}

TEST(PaymentTest, MockCallCount) {
  MockPaymentGateway mock(1000.0);
  OrderProcessor processor(mock);
  processor.placeOrder(10.0);
  processor.placeOrder(20.0);
  processor.placeOrder(30.0);
  EXPECT_EQ(3, mock.getCallCount());
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [MAIN — Run All Tests]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
int main() {
  std::cout << "=== MODULE 6: GOOGLE TEST (GTEST) SIMULATION ===\n"
            << std::endl;
  std::cout << "Running " << MiniGTest::allResults.size() << " tests...\n"
            << std::endl;
  MiniGTest::printSummary();
  return 0;
}
