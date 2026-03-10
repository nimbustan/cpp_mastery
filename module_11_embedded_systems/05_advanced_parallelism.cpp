/**
 * @file module_11_embedded_systems/05_advanced_parallelism.cpp
 * @brief Embedded & Systems: Thread Pools, C++17 Parallel Algorithms, Work Stealing / İş
 * Parçacığı Havuzları, C++17 Paralel Algoritmalar, İş Çalma
 *
 * @details
 * =============================================================================
 * [THEORY: Thread Pools — Why Not Spawn Per Task / TEORİ: İş Parçacığı Havuzları — Neden Görev
 * Başına Oluşturma Yapılmaz]
 * =============================================================================
 * EN: Creating a new `std::thread` for each task is expensive: OS must allocate stack memory
 * (~1-8MB), register the thread with the scheduler, and tear it down when done. In
 * embedded/automotive systems with limited resources, this is unacceptable. A thread pool
 * pre-creates N worker threads that continuously pull tasks from a shared queue. This gives: (1)
 * bounded resource usage, (2) amortized creation cost, (3) predictable latency. AUTOSAR Adaptive
 * Platform uses thread pools for service-oriented RPCs.
 *
 * TR: Her görev için yeni bir `std::thread` oluşturmak pahalıdır: OS yığıt belleği (~1-8MB)
 * ayırmalı, iş parçacığını zamanlayıcıya kaydetmeli ve bittiğinde yıkmalıdır. Sınırlı kaynaklara
 * sahip gömülü/otomotiv sistemlerde bu kabul edilemez. Bir iş parçacığı havuzu, sürekli olarak
 * paylaşılan bir kuyruktan görev çeken N işçi iş parçacığı önceden oluşturur. Bu şunları verir:
 * (1) sınırlı kaynak kullanımı, (2) amortize oluşturma maliyeti, (3) öngörülebilir gecikme.
 *
 * =============================================================================
 * [THEORY: C++17 Parallel Algorithms — std::execution Policies / TEORİ: C++17 Paralel
 * Algoritmalar — std::execution Politikaları]
 * =============================================================================
 * EN: C++17 added execution policies to STL algorithms: `std::execution::seq` (sequential),
 * `std::execution::par` (parallel), `std::execution::par_unseq` (parallel + vectorized). This
 * means existing `std::sort`, `std::transform`, `std::reduce` can run in parallel with just one
 * extra argument! In automotive: parallel map data processing, multiple sensor fusion, batch
 * diagnostics. Note: requires linking `-ltbb` on GCC (Intel TBB backend).
 *
 * TR: C++17, STL algoritmalarına yürütme politikaları ekledi: `std::execution::seq` (sıralı),
 * `std::execution::par` (paralel), `std::execution::par_unseq` (paralel + vektörize). Bu, mevcut
 * `std::sort`, `std::transform`, `std::reduce`'un tek bir ek argümanla paralel çalışabileceği
 * anlamına gelir! Otomotivde: paralel harita veri işleme, çoklu sensör füzyonu, toplu tanılama.
 * Not: GCC'de `-ltbb` bağlantısı gerektirir (Intel TBB arka ucu).
 *
 * =============================================================================
 * [THEORY: Work Stealing — Load-Balanced Parallelism / TEORİ: İş Çalma — Yük-Dengeli Paralellik]
 * =============================================================================
 * EN: In a basic thread pool, if one task is much longer than others, its worker stays busy
 * while others idle. Work stealing solves this: each worker has its own deque. When a worker's
 * deque is empty, it "steals" a task from another worker's deque (from the BACK, to minimize
 * contention). Intel TBB, Java ForkJoinPool, and Tokio (Rust) all use work stealing. In
 * automotive: useful for parallel sensor data pipelines with varying latencies.
 *
 * TR: Temel bir iş parçacığı havuzunda, bir görev diğerlerinden çok uzunsa, işçisi meşgul
 * kalırken diğerleri boşta kalır. İş çalma bunu çözer: her işçinin kendi deque'ü vardır. Bir
 * işçinin deque'ü boşaldığında, başka bir işçinin deque'ünden (çekişmeyi en aza indirmek için
 * ARKADAN) bir görev "çalar". Intel TBB, Java ForkJoinPool ve Tokio (Rust) hepsi iş çalma
 * kullanır.
 *
 * =============================================================================
 * [CPPREF DEPTH: std::execution Policy Tag Types / CPPREF DERİNLİK: std::execution Politika
 * Etiket Tipleri]
 * =============================================================================
 * EN: `std::execution::seq` — no parallelism, same as before. Elements accessed in order on
 * calling thread. `std::execution::par` — may execute in parallel on unspecified threads but
 * elements are NOT interleaved within a thread. `std::execution::par_unseq` — may execute in
 * parallel AND vectorized (SIMD). Element access functions must NOT use mutexes or blocking
 * synchronization! `std::execution::unseq` (C++20) — vectorized on the calling thread only.
 * cppreference.com/w/cpp/algorithm/execution_policy_tag
 *
 * TR: `std::execution::seq` — paralellik yok, öncekiyle aynı. `std::execution::par` —
 * belirtilmemiş iş parçacıklarında paralel yürütülebilir ancak elemanlar bir iş parçacığı içinde
 * serpiştirilmez. `std::execution::par_unseq` — paralel VE vektörize (SIMD) yürütülebilir.
 * Eleman erişim fonksiyonları mutex veya blokleyici senkronizasyon KULLANMAMALIDIR!
 * `std::execution::unseq` (C++20) — yalnızca çağıran iş parçacığında vektörize.
 * cppreference.com/w/cpp/algorithm/execution_policy_tag
 *
 * =============================================================================
 * [CPPREF DEPTH: std::reduce vs std::accumulate — Parallel Reduction / CPPREF DERİNLİK:
 * std::reduce vs std::accumulate — Paralel İndirgeme]
 * =============================================================================
 * EN: `std::accumulate` (C++98) is strictly left-fold: `((init op a) op b) op c`. Cannot be
 * parallelized because order is fixed. `std::reduce` (C++17) is unordered: the operation can be
 * applied in any order, enabling parallel execution. The operation MUST be associative and
 * commutative (e.g., `+`, `*`, `max`). For floating-point, `reduce` may give slightly different
 * results due to different summation order (affects rounding). `std::transform_reduce` is the
 * parallel map-reduce primitive. cppreference.com/w/cpp/algorithm/reduce
 *
 * TR: `std::accumulate` (C++98) kesinlikle sol-katlama: sıra sabittir. Paralelleştirilemez.
 * `std::reduce` (C++17) sırasızdır: işlem herhangi bir sırada uygulanabilir, paralel yürütmeyi
 * mümkün kılar. İşlem ilişkisel ve değişmeli OLMALIDIR (ör. `+`, `*`, `max`). Kayan nokta için
 * `reduce` farklı toplama sırası nedeniyle biraz farklı sonuçlar verebilir.
 * `std::transform_reduce` paralel map-reduce ilkesidir. cppreference.com/w/cpp/algorithm/reduce
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// =================================================================================================
// [1. THREAD POOL IMPLEMENTATION]
// =================================================================================================

// EN: A production-quality thread pool for embedded task dispatching.
// TR: Gömülü görev dağıtımı için üretim kalitesinde iş parçacığı havuzu.
class ThreadPool {
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> taskQueue_;
  std::mutex mtx_;
  std::condition_variable cv_;
  std::atomic<bool> shutdown_{false};

public:
  explicit ThreadPool(std::size_t numThreads) {
    for (std::size_t i = 0; i < numThreads; ++i) {
      workers_.emplace_back([this]() { workerLoop(); });
    }
  }

  ~ThreadPool() {
    shutdown_.store(true, std::memory_order_release);
    cv_.notify_all();
    for (auto& w : workers_) {
      if (w.joinable()) w.join();
    }
  }

  // EN: Submit a task to the pool.
  // TR: Havuza bir görev gönder.
  void submit(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(mtx_);
      taskQueue_.push(std::move(task));
    }
    cv_.notify_one();
  }

  // EN: Wait until all queued tasks are completed.
  // TR: Kuyruktaki tüm görevler tamamlanana kadar bekle.
  void waitAll() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this]() {
      return taskQueue_.empty();
    });
    // EN: Small grace period for in-flight tasks to complete.
    // TR: Devam eden görevlerin tamamlanması için küçük bekleme süresi.
    lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

private:
  void workerLoop() {
    while (true) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() {
          return shutdown_.load(std::memory_order_acquire) ||
                 !taskQueue_.empty();
        });
        if (shutdown_.load(std::memory_order_acquire) &&
            taskQueue_.empty()) {
          return;
        }
        task = std::move(taskQueue_.front());
        taskQueue_.pop();
      }
      task();
      cv_.notify_all(); // Wake waitAll()
    }
  }
};

// =================================================================================================
// [2. SIMULATED SENSOR FUSION — Parallel Processing]
// =================================================================================================

// EN: Simulate processing multiple automotive sensor streams in parallel.
// TR: Birden fazla otomotiv sensör akışını paralel olarak işlemeyi simüle et.
struct SensorReading {
  std::string sensorName;
  double value;
  std::string unit;
};

SensorReading processSensor(const std::string& name, double rawValue,
                            double calibrationOffset) {
  // EN: Simulate some computation (filtering, calibration).
  // TR: Bazı hesaplamaları simüle et (filtreleme, kalibrasyon).
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  double calibrated = rawValue + calibrationOffset;
  std::string unit = (name.find("EGT") != std::string::npos)   ? "C"
                     : (name.find("RPM") != std::string::npos)  ? "rpm"
                     : (name.find("MAP") != std::string::npos)  ? "kPa"
                     : (name.find("AFR") != std::string::npos)  ? "lambda"
                                                                : "raw";
  return {name, calibrated, unit};
}

// =================================================================================================
// [3. MANUAL std::reduce EQUIVALENT — Portable Sum]
// =================================================================================================

// EN: Since parallel std::execution requires -ltbb and may not be available, we demonstrate the
// concept with manual parallel reduction.
// TR: Paralel std::execution -ltbb gerektirdiğinden ve mevcut olmayabileceğinden kavramı manuel
// paralel indirgeme ile gösteriyoruz.

double parallelReduce(const std::vector<double>& data, std::size_t numThreads) {
  std::vector<double> partialSums(numThreads, 0.0);
  std::vector<std::thread> threads;
  std::size_t chunkSize = data.size() / numThreads;

  for (std::size_t t = 0; t < numThreads; ++t) {
    std::size_t start = t * chunkSize;
    std::size_t end = (t == numThreads - 1) ? data.size() : start + chunkSize;
    threads.emplace_back(
        [&data, &partialSums, t, start, end]() {
          double sum = 0.0;
          for (std::size_t i = start; i < end; ++i) {
            sum += data[i];
          }
          partialSums[t] = sum;
        });
  }
  for (auto& th : threads) th.join();

  // EN: Final sequential merge of partial sums.
  // TR: Kısmi toplamların son sıralı birleştirmesi.
  double total = 0.0;
  for (double ps : partialSums) total += ps;
  return total;
}

int main() {
  std::cout << "=== MODULE 11: ADVANCED PARALLELISM ===\n" << std::endl;

  // --- Demo 1: Thread Pool ---
  std::cout << "--- DEMO 1: Thread Pool (4 workers) ---\n" << std::endl;
  {
    ThreadPool pool(4);
    std::mutex printMtx;

    // EN: Submit automotive ECU tasks to the pool.
    // TR: Otomotiv ECU görevlerini havuza gönder.
    std::vector<std::string> ecuTasks = {
        "BCM: Check door lock status",
        "Cluster: Update speedometer needle",
        "HVAC: Read cabin temperature sensor",
        "Powertrain: Calculate fuel injection timing",
        "HMI: Render navigation overlay",
        "Chassis: Process wheel speed ABS data",
        "Telematics: Upload OBD-II snapshot",
        "BCM: Toggle headlight relay"};

    for (const auto& taskDesc : ecuTasks) {
      pool.submit([&printMtx, taskDesc]() {
        std::lock_guard<std::mutex> lock(printMtx);
        std::cout << "  [Worker " << std::this_thread::get_id()
                  << "] " << taskDesc << std::endl;
      });
    }
    pool.waitAll();
  }
  std::cout << "  Thread pool destroyed — all workers joined.\n" << std::endl;

  // --- Demo 2: Parallel Sensor Fusion ---
  std::cout << "--- DEMO 2: Parallel Sensor Fusion ---\n" << std::endl;
  {
    struct SensorJob {
      std::string name;
      double rawVal;
      double offset;
    };

    std::vector<SensorJob> jobs = {
        {"EGT_Cyl1", 720.0, +2.5},  {"EGT_Cyl2", 735.0, -1.2},
        {"EGT_Cyl3", 710.0, +0.8},  {"EGT_Cyl4", 745.0, -0.5},
        {"RPM_Crank", 3200.0, 0.0},  {"MAP_Intake", 85.0, +1.1},
        {"AFR_Wideband", 0.98, +0.02}};

    std::vector<SensorReading> results(jobs.size());
    std::vector<std::thread> threads;

    auto startTime = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < jobs.size(); ++i) {
      threads.emplace_back([&jobs, &results, i]() {
        results[i] = processSensor(jobs[i].name, jobs[i].rawVal,
                                   jobs[i].offset);
      });
    }
    for (auto& t : threads) t.join();
    auto endTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                       endTime - startTime).count();

    for (const auto& r : results) {
      std::cout << "  " << r.sensorName << " = " << r.value << " "
                << r.unit << std::endl;
    }
    std::cout << "  Parallel time: " << elapsed << "ms"
              << " (sequential would be ~" << jobs.size() * 20 << "ms)\n"
              << std::endl;
  }

  // --- Demo 3: Parallel Reduction ---
  std::cout << "--- DEMO 3: Parallel Reduction ---\n" << std::endl;
  {
    constexpr std::size_t DATA_SIZE = 1000000;
    std::vector<double> sensorData(DATA_SIZE);
    for (std::size_t i = 0; i < DATA_SIZE; ++i) {
      sensorData[i] = static_cast<double>(i % 100) * 0.01;
    }

    // EN: Sequential sum using std::accumulate.
    // TR: std::accumulate kullanarak sıralı toplam.
    auto t1 = std::chrono::steady_clock::now();
    double seqSum = std::accumulate(sensorData.begin(), sensorData.end(), 0.0);
    auto t2 = std::chrono::steady_clock::now();
    auto seqMs = std::chrono::duration_cast<std::chrono::microseconds>(
                     t2 - t1).count();

    // EN: Parallel sum using manual thread-based reduction.
    // TR: Manuel iş parçacığı tabanlı indirgeme kullanarak paralel toplam.
    auto t3 = std::chrono::steady_clock::now();
    double parSum = parallelReduce(sensorData, 4);
    auto t4 = std::chrono::steady_clock::now();
    auto parMs = std::chrono::duration_cast<std::chrono::microseconds>(
                     t4 - t3).count();

    std::cout << "  Sequential sum: " << seqSum << " (" << seqMs << " us)"
              << std::endl;
    std::cout << "  Parallel sum:   " << parSum << " (" << parMs << " us)"
              << std::endl;
    std::cout << "  Match: "
              << (std::abs(seqSum - parSum) < 0.001 ? "YES" : "DRIFT")
              << std::endl;
  }

  std::cout << "\n--- DEMO 4: std::execution Policy Reference ---\n"
            << std::endl;
  std::cout << "  // With -ltbb, these would work:" << std::endl;
  std::cout << "  // std::sort(std::execution::par, v.begin(), v.end());"
            << std::endl;
  std::cout << "  // std::reduce(std::execution::par, v.begin(), v.end());"
            << std::endl;
  std::cout << "  // std::transform_reduce(std::execution::par_unseq, ...);"
            << std::endl;
  std::cout << "  // See cppreference.com/w/cpp/algorithm/execution_policy_tag"
            << std::endl;

  std::cout << "\n=> Thread pool, sensor fusion, parallel reduction "
               "demonstrated!" << std::endl;

  return 0;
}
