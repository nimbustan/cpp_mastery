/**
 * @file module_06_industry_standards/11_gtest_advanced.cpp
 * @brief Advanced Google Test Patterns — İleri GTest Desenleri
 *
 * @details
 * =============================================================================
 * [THEORY: What This File Adds / TEORİ: Bu Dosyanın Eklediği]
 * =============================================================================
 *
 * EN: The basic GTest simulation (03_unit_testing_gtest.cpp) covers TEST(),
 *     EXPECT_EQ/TRUE/FALSE/NEAR/THROW, and dependency injection with mocks.
 *     This file adds ADVANCED patterns that real-world projects require:
 *
 *     ┌───────────────────────────────┬───────────────────────────────────────┐
 *     │ Feature                       │ GTest API                             │
 *     ├───────────────────────────────┼───────────────────────────────────────┤
 *     │ Test Fixtures (SetUp/Down)    │ TEST_F(Fixture, Name)                │
 *     │ Parameterized Tests           │ TEST_P + INSTANTIATE_TEST_SUITE_P    │
 *     │ Typed Tests (template)        │ TYPED_TEST / TYPED_TEST_SUITE        │
 *     │ Death Tests                   │ EXPECT_DEATH(stmt, regex)            │
 *     │ GMock EXPECT_CALL             │ EXPECT_CALL(obj, Method()).Times(N)  │
 *     │ GMock Matchers                │ Eq, Gt, Contains, HasSubstr, _       │
 *     │ Code Coverage                 │ gcov + lcov + genhtml                │
 *     └───────────────────────────────┴───────────────────────────────────────┘
 *
 * TR: Temel GTest simülasyonu (03_unit_testing_gtest.cpp) TEST(),
 *     EXPECT_EQ/TRUE/FALSE/NEAR/THROW ve bağımlılık enjeksiyonunu kapsar.
 *     Bu dosya gerçek projelerin GEREKTİRDİĞİ ileri desenleri ekler:
 *
 *     ┌───────────────────────────────┬───────────────────────────────────────┐
 *     │ Özellik                       │ GTest API                             │
 *     ├───────────────────────────────┼───────────────────────────────────────┤
 *     │ Test Fixture (SetUp/Down)     │ TEST_F(Fixture, Name)                │
 *     │ Parametrize Testler           │ TEST_P + INSTANTIATE_TEST_SUITE_P    │
 *     │ Tipli Testler (şablon)        │ TYPED_TEST / TYPED_TEST_SUITE        │
 *     │ Ölüm Testleri                  │ EXPECT_DEATH(stmt, regex)            │
 *     │ GMock EXPECT_CALL             │ EXPECT_CALL(obj, Method()).Times(N)  │
 *     │ GMock Matcher'ı               │ Eq, Gt, Contains, HasSubstr, _       │
 *     │ Kod Kapsama                   │ gcov + lcov + genhtml                │
 *     └───────────────────────────────┴───────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: Test Fixtures / TEORİ: Test Fikstürleri]
 * =============================================================================
 *
 * EN: A TEST FIXTURE is a class that provides shared setup/teardown logic for
 *     multiple tests. In real GTest:
 *
 *         class SensorTest : public ::testing::Test {
 *         protected:
 *             void SetUp() override { sensor_ = new Sensor(42); }
 *             void TearDown() override { delete sensor_; }
 *             Sensor* sensor_;
 *         };
 *         TEST_F(SensorTest, ReadsTemperature) {
 *             EXPECT_GT(sensor_->read(), 0);
 *         }
 *
 *     SetUp() runs before EACH test, TearDown() after EACH test.
 *     The fixture is DESTROYED and RECONSTRUCTED between tests — each test
 *     gets a clean state. This prevents test interdependence.
 *
 * TR: TEST FIXTURE, birden fazla testin paylaştığı kurulum/temizleme mantığı
 *     sağlayan sınıftır. Gerçek GTest'te:
 *
 *         class SensorTest : public ::testing::Test {
 *         protected:
 *             void SetUp() override { sensor_ = new Sensor(42); }
 *             void TearDown() override { delete sensor_; }
 *             Sensor* sensor_;
 *         };
 *         TEST_F(SensorTest, ReadsTemperature) {
 *             EXPECT_GT(sensor_->read(), 0);
 *         }
 *
 *     SetUp() HER testten önce, TearDown() HER testten sonra çalışır.
 *     Fixture testler arasında YIKILIP YENİDEN OLUŞTURULUR — her test
 *     temiz bir durumla başlar. Bu testler arası bağımlılığı önler.
 *
 * =============================================================================
 * [THEORY: Parameterized Tests / TEORİ: Parametrize Testler]
 * =============================================================================
 *
 * EN: Parameterized tests run the SAME test logic with DIFFERENT inputs.
 *     Instead of writing 10 nearly-identical TEST() cases, write ONE TEST_P()
 *     and feed it data:
 *
 *         class FizzBuzzTest : public ::testing::TestWithParam<std::pair<int,string>> {};
 *         TEST_P(FizzBuzzTest, ProducesCorrectOutput) {
 *             auto [input, expected] = GetParam();
 *             EXPECT_EQ(fizzBuzz(input), expected);
 *         }
 *         INSTANTIATE_TEST_SUITE_P(Defaults, FizzBuzzTest, ::testing::Values(
 *             std::make_pair(1, "1"), std::make_pair(3, "Fizz"),
 *             std::make_pair(5, "Buzz"), std::make_pair(15, "FizzBuzz")
 *         ));
 *
 * TR: Parametrize testler AYNI test mantığını FARKLI girdilerle çalıştırır.
 *     10 neredeyse aynı TEST() yazmak yerine, TEK bır TEST_P() yazıp
 *     veri beslersin:
 *
 *         class FizzBuzzTest : public ::testing::TestWithParam<std::pair<int,string>> {};
 *         TEST_P(FizzBuzzTest, ProducesCorrectOutput) {
 *             auto [input, expected] = GetParam();
 *             EXPECT_EQ(fizzBuzz(input), expected);
 *         }
 *         INSTANTIATE_TEST_SUITE_P(Defaults, FizzBuzzTest, ::testing::Values(
 *             std::make_pair(1, "1"), std::make_pair(3, "Fizz"),
 *             std::make_pair(5, "Buzz"), std::make_pair(15, "FizzBuzz")
 *         ));
 *
 * =============================================================================
 * [THEORY: GMock & EXPECT_CALL / TEORİ: GMock ve EXPECT_CALL]
 * =============================================================================
 *
 * EN: GMock goes beyond simple stubs. With EXPECT_CALL you can verify:
 *     - HOW MANY times a method was called (Times(n), AtLeast(1), AtMost(3))
 *     - WHAT arguments were passed (Eq(42), Gt(0), _)
 *     - WHAT to return (Return(true), WillOnce/WillRepeatedly)
 *     - CALL ORDER (InSequence, After)
 *
 *         class MockSensor : public ISensor {
 *         public:
 *             MOCK_METHOD(int, read, (), (override));
 *             MOCK_METHOD(bool, calibrate, (int), (override));
 *         };
 *         TEST(Controller, ReadsAndCalibrates) {
 *             MockSensor mock;
 *             EXPECT_CALL(mock, read()).Times(2).WillRepeatedly(Return(25));
 *             EXPECT_CALL(mock, calibrate(Gt(0))).WillOnce(Return(true));
 *             Controller ctrl(mock);
 *             ctrl.loop();
 *         }
 *
 * TR: GMock basit stub'ların ötesine geçer. EXPECT_CALL ile doğrulama:
 *     - KAÇ KEZ çağrıldı (Times(n), AtLeast(1), AtMost(3))
 *     - HANGİ argümanlarla çağrıldı (Eq(42), Gt(0), _)
 *     - NE döndürdü (Return(true), WillOnce/WillRepeatedly)
 *     - ÇAĞRI SIRASI (InSequence, After)
 *
 *         class MockSensor : public ISensor {
 *         public:
 *             MOCK_METHOD(int, read, (), (override));
 *             MOCK_METHOD(bool, calibrate, (int), (override));
 *         };
 *         TEST(Controller, ReadsAndCalibrates) {
 *             MockSensor mock;
 *             EXPECT_CALL(mock, read()).Times(2).WillRepeatedly(Return(25));
 *             EXPECT_CALL(mock, calibrate(Gt(0))).WillOnce(Return(true));
 *             Controller ctrl(mock);
 *             ctrl.loop();
 *         }
 *
 * =============================================================================
 * [THEORY: Code Coverage / TEORİ: Kod Kapsama]
 * =============================================================================
 *
 * EN: Coverage measures which code lines/branches tests actually execute:
 *
 *     # Compile with coverage instrumentation
 *     g++ --coverage -fprofile-arcs -ftest-coverage -o test test.cpp
 *     ./test
 *     gcov test.cpp              # Generate .gcov files
 *     lcov --capture --directory . --output-file coverage.info
 *     genhtml coverage.info --output-directory coverage_html
 *
 *     Coverage types:
 *     - Line coverage: which lines executed (aim ≥80%)
 *     - Branch coverage: which if/else paths taken (aim ≥70%)
 *     - Function coverage: which functions called (aim 100%)
 *
 *     CMake integration:
 *     target_compile_options(test PRIVATE --coverage)
 *     target_link_options(test PRIVATE --coverage)
 *
 * TR: Kapsama, testlerin hangi kod satırlarını/dallarını çalıştırdığını ölçer:
 *
 *     # Kapsama enstrümantasyonu ile derle
 *     g++ --coverage -fprofile-arcs -ftest-coverage -o test test.cpp
 *     ./test
 *     gcov test.cpp              # .gcov dosyaları üret
 *     lcov --capture --directory . --output-file coverage.info
 *     genhtml coverage.info --output-directory coverage_html
 *
 *     Kapsama türleri:
 *     - Satır kapsama: hangi satırlar çalıştı (hedef ≥%80)
 *     - Dal kapsama: hangi if/else yolları alındı (hedef ≥%70)
 *     - Fonksiyon kapsama: hangi fonksiyonlar çağrıldı (hedef %100)
 *
 *     CMake entegrasyonu:
 *     target_compile_options(test PRIVATE --coverage)
 *     target_link_options(test PRIVATE --coverage)
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 11_gtest_advanced.cpp -o 11_gtest_advanced
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <cmath>
#include <memory>
#include <tuple>
#include <stdexcept>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Extended MiniGTest Framework
// ═════════════════════════════════════════════════════════════════════════════

namespace MiniGTest2 {

struct TestResult {
    std::string suite;
    std::string name;
    bool passed = true;
    std::vector<std::string> failures;
};

std::vector<TestResult> results;
TestResult* current = nullptr;

void addResult(const std::string& suite, const std::string& name) {
    results.push_back({suite, name, true, {}});
    current = &results.back();
}

void fail(const std::string& msg) {
    if (current) {
        current->passed = false;
        current->failures.push_back(msg);
    }
}

void expectEq(int expected, int actual, const char* file, int line) {
    if (expected != actual) {
        std::ostringstream oss;
        oss << file << ":" << line << " EXPECT_EQ: expected=" << expected
            << " actual=" << actual;
        fail(oss.str());
    }
}

void expectStrEq(const std::string& expected, const std::string& actual,
                 const char* file, int line) {
    if (expected != actual) {
        std::ostringstream oss;
        oss << file << ":" << line << " EXPECT_STREQ: expected=\"" << expected
            << "\" actual=\"" << actual << "\"";
        fail(oss.str());
    }
}

void expectTrue(bool cond, const char* file, int line) {
    if (!cond) {
        std::ostringstream oss;
        oss << file << ":" << line << " EXPECT_TRUE failed";
        fail(oss.str());
    }
}

void expectGt(int a, int b, const char* file, int line) {
    if (!(a > b)) {
        std::ostringstream oss;
        oss << file << ":" << line << " EXPECT_GT: " << a << " not > " << b;
        fail(oss.str());
    }
}

void expectNear(double a, double b, double eps, const char* file, int line) {
    if (std::fabs(a - b) > eps) {
        std::ostringstream oss;
        oss << file << ":" << line << " EXPECT_NEAR: |" << a << " - " << b
            << "| > " << eps;
        fail(oss.str());
    }
}

void printSummary() {
    int passed = 0, failed = 0;
    for (const auto& r : results) {
        std::cout << (r.passed ? "  \033[32m[PASSED]\033[0m "
                               : "  \033[31m[FAILED]\033[0m ")
                  << r.suite << "." << r.name << "\n";
        if (!r.passed) {
            for (const auto& f : r.failures)
                std::cout << "           " << f << "\n";
            failed++;
        } else {
            passed++;
        }
    }
    std::cout << "\n  Summary: " << passed << " passed, " << failed
              << " failed, " << (passed + failed) << " total.\n";
}

} // namespace MiniGTest2

// Macros
#define EXPECT_EQ2(e,a)    MiniGTest2::expectEq(e,a,__FILE__,__LINE__)
#define EXPECT_STREQ2(e,a) MiniGTest2::expectStrEq(e,a,__FILE__,__LINE__)
#define EXPECT_TRUE2(c)    MiniGTest2::expectTrue(c,__FILE__,__LINE__)
#define EXPECT_GT2(a,b)    MiniGTest2::expectGt(a,b,__FILE__,__LINE__)
#define EXPECT_NEAR2(a,b,e) MiniGTest2::expectNear(a,b,e,__FILE__,__LINE__)

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Production Code Under Test
// ═════════════════════════════════════════════════════════════════════════════

// EN: Sensor simulator for fixture & parameterized test demos
// TR: Fixture ve parametrize test demoları için sensör simülatörü
class Sensor {
public:
    explicit Sensor(int id) : id_(id), value_(0), calibrated_(false) {}

    void calibrate(int offset) {
        offset_ = offset;
        calibrated_ = true;
    }

    int read() const {
        if (!calibrated_) return -1;
        return value_ + offset_;
    }

    void setValue(int v) { value_ = v; }
    int id() const { return id_; }
    bool isCalibrated() const { return calibrated_; }

private:
    int id_;
    int value_ = 0;
    int offset_ = 0;
    bool calibrated_ = false;
};

// EN: FizzBuzz for parameterized test demo
// TR: Parametrize test demosu için FizzBuzz
std::string fizzBuzz(int n) {
    if (n % 15 == 0) return "FizzBuzz";
    if (n % 3 == 0) return "Fizz";
    if (n % 5 == 0) return "Buzz";
    return std::to_string(n);
}

// EN: Interface for GMock simulation
// TR: GMock simülasyonu için arayüz
class IDataSource {
public:
    virtual ~IDataSource() = default;
    virtual int fetchValue(int key) = 0;
    virtual bool isConnected() const = 0;
};

// EN: Class under test — depends on IDataSource
// TR: Test edilen sınıf — IDataSource'a bağımlı
class DataProcessor {
public:
    explicit DataProcessor(IDataSource& ds) : ds_(ds) {}

    int processKey(int key) {
        if (!ds_.isConnected()) return -1;
        int val = ds_.fetchValue(key);
        return val * 2;
    }

private:
    IDataSource& ds_;
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 06 - Advanced GTest Patterns\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Test Fixture (SetUp / TearDown) ────────────────────────
    {
        std::cout << "--- Demo 1: Test Fixture (SetUp / TearDown) ---\n";

        // EN: Simulate TEST_F — fixture class with SetUp/TearDown
        // TR: TEST_F simülasyonu — SetUp/TearDown ile fixture sınıfı
        struct SensorFixture {
            std::unique_ptr<Sensor> sensor;

            void SetUp() {
                sensor = std::make_unique<Sensor>(42);
                sensor->calibrate(5);
                sensor->setValue(100);
                // EN: Each test starts with calibrated sensor reading 105
                // TR: Her test kalibre edilmiş sensörle (105 okuması) başlar
            }

            void TearDown() {
                sensor.reset();
                // EN: Clean up resources after each test
                // TR: Her testten sonra kaynakları temizle
            }
        };

        // TEST_F(SensorFixture, ReadsCalibratedValue)
        {
            SensorFixture f;
            f.SetUp();
            MiniGTest2::addResult("SensorFixture", "ReadsCalibratedValue");
            EXPECT_EQ2(105, f.sensor->read());
            f.TearDown();
        }

        // TEST_F(SensorFixture, IsCalibrated)
        {
            SensorFixture f;
            f.SetUp();
            MiniGTest2::addResult("SensorFixture", "IsCalibrated");
            EXPECT_TRUE2(f.sensor->isCalibrated());
            f.TearDown();
        }

        // TEST_F(SensorFixture, IdMatches)
        {
            SensorFixture f;
            f.SetUp();
            MiniGTest2::addResult("SensorFixture", "IdMatches");
            EXPECT_EQ2(42, f.sensor->id());
            f.TearDown();
        }

        // TEST_F(SensorFixture, RecalibrateWithNewOffset)
        {
            SensorFixture f;
            f.SetUp();
            MiniGTest2::addResult("SensorFixture", "RecalibrateWithNewOffset");
            f.sensor->calibrate(10);
            EXPECT_EQ2(110, f.sensor->read());
            f.TearDown();
        }

        std::cout << "  (4 fixture tests — SetUp/TearDown per test)\n\n";

        std::cout << "  Real GTest equivalent:\n";
        std::cout << "    class SensorFixture : public ::testing::Test {\n";
        std::cout << "    protected:\n";
        std::cout << "        void SetUp() override { sensor_ = new Sensor(42); }\n";
        std::cout << "        void TearDown() override { delete sensor_; }\n";
        std::cout << "        Sensor* sensor_;\n";
        std::cout << "    };\n";
        std::cout << "    TEST_F(SensorFixture, Reads) { EXPECT_GT(sensor_->read(), 0); }\n\n";
    }

    // ─── Demo 2: Parameterized Tests (TEST_P) ──────────────────────────
    {
        std::cout << "--- Demo 2: Parameterized Tests (TEST_P) ---\n";

        // EN: Test data table — one TEST_P runs with each pair
        // TR: Test veri tablosu — her çift için tek TEST_P çalışır
        struct FizzBuzzParam {
            int input;
            std::string expected;
        };

        std::vector<FizzBuzzParam> params = {
            {1, "1"}, {2, "2"}, {3, "Fizz"}, {4, "4"},
            {5, "Buzz"}, {6, "Fizz"}, {10, "Buzz"},
            {15, "FizzBuzz"}, {30, "FizzBuzz"}, {7, "7"}
        };

        // Simulate TEST_P + INSTANTIATE_TEST_SUITE_P
        for (const auto& p : params) {
            std::string testName = "Input_" + std::to_string(p.input);
            MiniGTest2::addResult("FizzBuzzParam", testName);
            EXPECT_STREQ2(p.expected, fizzBuzz(p.input));
        }

        std::cout << "  (10 parameterized tests from data table)\n\n";

        std::cout << "  Real GTest equivalent:\n";
        std::cout << "    class FizzBuzzTest : public ::testing::TestWithParam<\n";
        std::cout << "        std::pair<int, std::string>> {};\n";
        std::cout << "    TEST_P(FizzBuzzTest, Produces) {\n";
        std::cout << "        auto [in, exp] = GetParam();\n";
        std::cout << "        EXPECT_EQ(fizzBuzz(in), exp);\n";
        std::cout << "    }\n";
        std::cout << "    INSTANTIATE_TEST_SUITE_P(Values, FizzBuzzTest,\n";
        std::cout << "        ::testing::Values(make_pair(3,\"Fizz\"), ...));\n\n";
    }

    // ─── Demo 3: Death Tests ────────────────────────────────────────────
    {
        std::cout << "--- Demo 3: Death Tests (EXPECT_DEATH) ---\n";

        // EN: Death tests verify that code crashes/aborts as expected.
        //     In real GTest: EXPECT_DEATH(func(), "regex matching stderr");
        //     We simulate by catching exceptions (since we can't fork here).
        // TR: Death testler kodun beklendiği gibi crash/abort ettiğini doğrular.

        // Simulate EXPECT_DEATH for assertion-like behavior
        auto expectDeath = [](std::function<void()> fn,
                              const std::string& testName) {
            MiniGTest2::addResult("DeathTest", testName);
            bool died = false;
            try {
                fn();
            } catch (...) {
                died = true;
            }
            if (!died) {
                MiniGTest2::fail("Expected death/exception but survived");
            }
        };

        // EN: Null pointer dereference simulation (throws instead of segfault)
        // TR: Null pointer başvurusu simülasyonu (segfault yerine exception fırlatır)
        expectDeath([]() {
            throw std::runtime_error("null dereference");
        }, "NullDereferenceAborts");

        // EN: Out of range access
        // TR: Aralık dışı erişim
        expectDeath([]() {
            std::vector<int> v = {1, 2, 3};
            (void)v.at(100);
        }, "OutOfRangeAborts");

        // EN: Division by zero throws
        // TR: Sıfıra bölme exception fırlatır
        expectDeath([]() {
            double b = 0.0;
            if (b == 0.0) throw std::logic_error("div by zero");
            volatile double r = 1.0 / b;
            (void)r;
        }, "DivisionByZeroAborts");

        std::cout << "  (3 death tests — all 'died' as expected)\n\n";

        std::cout << "  Real GTest equivalent:\n";
        std::cout << "    TEST(DeathTest, NullDeref) {\n";
        std::cout << "        EXPECT_DEATH(dereference(nullptr), \".*\");\n";
        std::cout << "    }\n";
        std::cout << "    // Runs in forked subprocess on Linux\n";
        std::cout << "    // Use EXPECT_EXIT for exit-code checks\n\n";
    }

    // ─── Demo 4: GMock EXPECT_CALL Simulation ───────────────────────────
    {
        std::cout << "--- Demo 4: GMock EXPECT_CALL Simulation ---\n";

        // EN: Mock that records calls, returns configurable values,
        //     and verifies expectations — simulating GMock behavior.
        // TR: Çağrıları kaydeden, yapılandırılabilir değerler döndüren
        //     ve beklentileri doğrulayan mock — GMock davranışını simüle eder.

        class MockDataSource : public IDataSource {
        public:
            // EN: Configure return values (WillOnce/WillRepeatedly equivalent)
            // TR: Dönüş değerlerini yapılandır (WillOnce/WillRepeatedly karşılığı)
            void whenFetchValue(int returnVal) { fetch_return_ = returnVal; }
            void whenIsConnected(bool val) { connected_ = val; }

            int fetchValue(int key) override {
                fetch_calls_.push_back(key);
                return fetch_return_;
            }

            bool isConnected() const override {
                connect_check_count_++;
                return connected_;
            }

            // EN: Verification methods (EXPECT_CALL equivalent)
            // TR: Doğrulama metotları (EXPECT_CALL karşılığı)
            size_t fetchCallCount() const { return fetch_calls_.size(); }
            const std::vector<int>& fetchCallArgs() const { return fetch_calls_; }
            size_t connectCheckCount() const { return connect_check_count_; }

        private:
            int fetch_return_ = 0;
            bool connected_ = true;
            std::vector<int> fetch_calls_;
            mutable size_t connect_check_count_ = 0;
        };

        // Test: connected, fetches twice
        {
            MockDataSource mock;
            mock.whenIsConnected(true);
            mock.whenFetchValue(42);

            DataProcessor proc(mock);
            MiniGTest2::addResult("GMockSim", "ConnectedProcessesTwice");

            int r1 = proc.processKey(1);
            int r2 = proc.processKey(2);

            EXPECT_EQ2(84, r1);       // 42 * 2
            EXPECT_EQ2(84, r2);       // 42 * 2
            // EXPECT_CALL(mock, fetchValue(_)).Times(2)
            EXPECT_EQ2(2, static_cast<int>(mock.fetchCallCount()));
            // EXPECT_CALL(mock, isConnected()).Times(2)
            EXPECT_EQ2(2, static_cast<int>(mock.connectCheckCount()));
        }

        // Test: disconnected, never fetches
        {
            MockDataSource mock;
            mock.whenIsConnected(false);

            DataProcessor proc(mock);
            MiniGTest2::addResult("GMockSim", "DisconnectedNeverFetches");

            int r = proc.processKey(1);
            EXPECT_EQ2(-1, r);
            // EXPECT_CALL(mock, fetchValue(_)).Times(0)
            EXPECT_EQ2(0, static_cast<int>(mock.fetchCallCount()));
        }

        // Test: verify argument values
        {
            MockDataSource mock;
            mock.whenIsConnected(true);
            mock.whenFetchValue(10);

            DataProcessor proc(mock);
            MiniGTest2::addResult("GMockSim", "VerifiesArgValues");

            proc.processKey(100);
            proc.processKey(200);
            proc.processKey(300);

            const auto& args = mock.fetchCallArgs();
            EXPECT_EQ2(3, static_cast<int>(args.size()));
            EXPECT_EQ2(100, args[0]);
            EXPECT_EQ2(200, args[1]);
            EXPECT_EQ2(300, args[2]);
        }

        std::cout << "  (3 mock tests — call count, connectivity, arg verification)\n\n";

        std::cout << "  Real GMock equivalent:\n";
        std::cout << "    class MockDataSource : public IDataSource {\n";
        std::cout << "    public:\n";
        std::cout << "        MOCK_METHOD(int, fetchValue, (int), (override));\n";
        std::cout << "        MOCK_METHOD(bool, isConnected, (), (const, override));\n";
        std::cout << "    };\n";
        std::cout << "    TEST(Processor, CallsCorrectly) {\n";
        std::cout << "        MockDataSource mock;\n";
        std::cout << "        EXPECT_CALL(mock, isConnected()).WillRepeatedly(Return(true));\n";
        std::cout << "        EXPECT_CALL(mock, fetchValue(100)).WillOnce(Return(42));\n";
        std::cout << "    }\n\n";
    }

    // ─── Demo 5: Code Coverage Reference ────────────────────────────────
    {
        std::cout << "--- Demo 5: Code Coverage & CI Pipeline ---\n";
        std::cout << "  Compile with coverage:\n";
        std::cout << "    g++ --coverage -fprofile-arcs -ftest-coverage \\\n";
        std::cout << "        -o test_binary unit_tests.cpp\n\n";
        std::cout << "  Run and analyze:\n";
        std::cout << "    ./test_binary\n";
        std::cout << "    gcov unit_tests.cpp        # .gcov text report\n";
        std::cout << "    lcov --capture -d . -o cov.info\n";
        std::cout << "    genhtml cov.info -o cov_html/  # HTML report\n\n";
        std::cout << "  CMake integration:\n";
        std::cout << "    option(ENABLE_COVERAGE \"Coverage\" OFF)\n";
        std::cout << "    if(ENABLE_COVERAGE)\n";
        std::cout << "        target_compile_options(tests PRIVATE --coverage)\n";
        std::cout << "        target_link_options(tests PRIVATE --coverage)\n";
        std::cout << "    endif()\n\n";
        std::cout << "  CI/CD (GitHub Actions snippet):\n";
        std::cout << "    - run: cmake -B build -DENABLE_COVERAGE=ON\n";
        std::cout << "    - run: cmake --build build && cd build && ctest\n";
        std::cout << "    - run: lcov --capture -d build -o cov.info\n";
        std::cout << "    - run: genhtml cov.info -o coverage/\n";
        std::cout << "    - uses: codecov/codecov-action@v3\n\n";

        std::cout << "  ┌──────────────────────┬──────────┬──────────────────────────┐\n";
        std::cout << "  │ Coverage Type        │ Target   │ Tool                     │\n";
        std::cout << "  ├──────────────────────┼──────────┼──────────────────────────┤\n";
        std::cout << "  │ Line coverage        │ ≥ 80%    │ gcov, lcov               │\n";
        std::cout << "  │ Branch coverage      │ ≥ 70%    │ lcov --rc branch=1       │\n";
        std::cout << "  │ Function coverage    │ 100%     │ gcov -f                  │\n";
        std::cout << "  │ MC/DC (automotive)   │ 100%     │ BullseyeCoverage, Cantata│\n";
        std::cout << "  └──────────────────────┴──────────┴──────────────────────────┘\n\n";
    }

    // ─── Print Results ──────────────────────────────────────────────────
    std::cout << "============================================\n";
    std::cout << "   Test Results\n";
    std::cout << "============================================\n";
    MiniGTest2::printSummary();

    std::cout << "\n============================================\n";
    std::cout << "   End of Advanced GTest Patterns\n";
    std::cout << "============================================\n";

    return 0;
}
