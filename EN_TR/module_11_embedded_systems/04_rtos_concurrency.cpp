/**
 * @file module_11_embedded_systems/04_rtos_concurrency.cpp
 * @brief Embedded & Systems: RTOS Patterns, Priority Inversion, Lock-Free Structures — RTOS
 * Kalıpları, Öncelik Tersine Çevrilmesi, Kilitsiz Yapılar
 *
 * @details
 * =============================================================================
 * [THEORY: RTOS Task Model — Cooperative vs Preemptive / TEORİ: RTOS Görev Modeli — İşbirlikli
 * vs Öncelikli]
 * =============================================================================
 * EN: Real-Time Operating Systems (FreeRTOS, AUTOSAR OS, QNX) manage tasks with fixed
 * priorities. Preemptive scheduling: a higher-priority task instantly preempts a lower-priority
 * one. Cooperative: tasks yield voluntarily. In automotive ECUs, critical tasks (brake ABS,
 * airbag) run at highest priority. HMI/Infotainment tasks run at lower priority. Tasks
 * communicate via message queues, semaphores, and shared memory.
 *
 * TR: Gerçek Zamanlı İşletim Sistemleri (FreeRTOS, AUTOSAR OS, QNX) görevleri sabit önceliklerle
 * yönetir. Öncelikli zamanlama: yüksek öncelikli görev anında düşük öncelikli görevi keser.
 * İşbirlikli: görevler gönüllü olarak CPU'yu bırakır. Otomotiv ECU'larda kritik görevler (fren
 * ABS, hava yastığı) en yüksek öncelikte çalışır. HMI/Bilgi-eğlence görevleri daha düşük
 * öncelikte çalışır. Görevler mesaj kuyrukları, semaforlar ve paylaşılan bellek aracılığıyla
 * iletişim kurar.
 *
 * =============================================================================
 * [THEORY: Priority Inversion — The Mars Pathfinder Bug / TEORİ: Öncelik Tersine Çevrilmesi —
 * Mars Pathfinder Hatası]
 * =============================================================================
 * EN: Priority Inversion occurs when a high-priority task waits for a low-priority task holding
 * a shared mutex — while a medium-priority task preempts the low-priority one, BLOCKING the
 * high-priority task indefinitely. This caused the Mars Pathfinder reboot loop in 1997.
 * Solutions:
 *     1. Priority Inheritance: temporarily boost the low-priority task's priority.
 *     2. Priority Ceiling: mutex has a pre-assigned ceiling priority.
 *     3. Lock-free structures: eliminate mutexes entirely.
 *
 * TR: Öncelik Tersine Çevrilmesi, yüksek öncelikli görev paylaşılan bir mutex'i tutan düşük
 * öncelikli görevi beklediğinde oluşur — orta öncelikli görev düşük öncelikliyi keserken,
 * yüksek öncelikli görevi SÜRESİZ BLOKE EDİYOR. Bu, 1997'de Mars Pathfinder yeniden
 * başlatma döngüsüne neden oldu. Çözümler:
 *     1. Öncelik Kalıtımı: düşük öncelikli görevin önceliğini geçici olarak yükselt.
 *     2. Öncelik Tavanı: mutex'in önceden atanmış bir tavan önceliği vardır.
 *     3. Kilitsiz yapılar: mutex'leri tamamen ortadan kaldır.
 *
 * =============================================================================
 * [THEORY: Lock-Free Programming — CAS Operations / TEORİ: Kilitsiz Programlama — CAS İşlemleri]
 * =============================================================================
 * EN: Lock-free data structures use atomic Compare-And-Swap (CAS) instead of mutexes.
 * `std::atomic::compare_exchange_weak/strong` atomically: reads the current value, compares it
 * to "expected," and if equal, swaps to "desired." This eliminates deadlocks, priority
 * inversion, and lock contention. Used extensively in automotive middleware (AUTOSAR COM),
 * sensor data pipelines, and real-time audio processing.
 *
 * TR: Kilitsiz veri yapıları mutex'ler yerine atomik Karşılaştır-Ve-Değiştir (CAS) kullanır.
 * `std::atomic::compare_exchange_weak/strong` atomik olarak: mevcut değeri okur, "beklenen" ile
 * karşılaştırır ve eşitse "istenen" ile değiştirir. Bu, kilitlenmeleri, öncelik tersine
 * çevrilmesini ve kilit çekişmesini ortadan kaldırır. Otomotiv ara yazılımında (AUTOSAR COM),
 * sensör veri hatlarında ve gerçek zamanlı ses işlemede yoğun olarak kullanılır.
 *
 * =============================================================================
 * [CPPREF DEPTH: std::atomic::compare_exchange_weak vs strong / CPPREF DERİNLİK:
 * std::atomic::compare_exchange_weak vs strong]
 * =============================================================================
 * EN: `compare_exchange_weak` may fail spuriously (return false even if current == expected) on
 * some architectures (ARM LL/SC). It MUST be used in a loop. `compare_exchange_strong` never
 * fails spuriously but may be slower on LL/SC platforms. Weak is preferred in CAS loops, strong
 * for single-shot operations. Both take memory_order parameters for fine-grained control.
 * cppreference.com/w/cpp/atomic/atomic/compare_exchange
 *
 * TR: `compare_exchange_weak` bazı mimarilerde (ARM LL/SC) sahte başarısız olabilir (current ==
 * expected olsa bile false döner). Bir döngü içinde kullanılması ZORUNLUDUR.
 * `compare_exchange_strong` asla sahte başarısız olmaz ancak LL/SC platformlarda daha yavaş
 * olabilir. CAS döngülerinde weak, tek seferlik işlemlerde strong tercih edilir.
 * cppreference.com/w/cpp/atomic/atomic/compare_exchange
 *
 * =============================================================================
 * [CPPREF DEPTH: std::memory_order — Acquire/Release Semantics / CPPREF DERİNLİK:
 * std::memory_order — Acquire/Release Semantiği]
 * =============================================================================
 * EN: Memory ordering controls how atomic operations are reordered by CPU/ compiler.
 * `memory_order_relaxed`: no ordering guarantees (fastest, counters). `memory_order_acquire`: no
 * reads/writes can move BEFORE this load. `memory_order_release`: no reads/writes can move AFTER
 * this store. Acquire-Release pair synchronizes: if thread A releases and thread B acquires the
 * same atomic, B sees all writes A made before the release. `memory_order_seq_cst` (default):
 * full sequential consistency — slowest but safest. In embedded, relaxed+acquire/release is
 * heavily used. cppreference.com/w/cpp/atomic/memory_order
 *
 * TR: Bellek sıralaması, atomik işlemlerin CPU/derleyici tarafından nasıl yeniden sıralandığını
 * kontrol eder. `memory_order_relaxed`: sıralama garantisi yok (en hızlı, sayaçlar).
 * `memory_order_acquire`: bu yüklemeden ÖNCE hiçbir okuma/yazma taşınamaz.
 * `memory_order_release`: bu depolamadan SONRA hiçbir okuma/yazma taşınamaz. Acquire-Release
 * çifti senkronize eder. `memory_order_seq_cst` (varsayılan): tam sıralı tutarlılık — en yavaş
 * ama en güvenli. Gömülü sistemlerde relaxed+acquire/release yoğun kullanılır.
 * cppreference.com/w/cpp/atomic/memory_order
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_rtos_concurrency.cpp -o 04_rtos_concurrency
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [1. RTOS TASK SIMULATION — Priority-Based Scheduler]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Simulates RTOS-style tasks with priorities (like FreeRTOS).
// TR: Öncelikli RTOS tarzı görevleri simüle eder (FreeRTOS gibi).
struct RTOSTask {
  std::string name;
  int priority; // Higher = more important
  std::function<void()> taskFunc;
};

class SimpleScheduler {
  std::vector<RTOSTask> tasks_;
  std::mutex mtx_;

public:
  void addTask(const std::string& name, int priority,
               std::function<void()> func) {
    std::lock_guard<std::mutex> lock(mtx_);
    tasks_.push_back({name, priority, std::move(func)});
  }

  // EN: Execute tasks in priority order (highest first).
  // TR: Görevleri öncelik sırasına göre çalıştır (en yüksek önce).
  void runAll() {
    std::lock_guard<std::mutex> lock(mtx_);
    // Sort by priority (descending)
    std::sort(tasks_.begin(), tasks_.end(),
              [](const RTOSTask& a, const RTOSTask& b) {
                return a.priority > b.priority;
              });

    for (const auto& task : tasks_) {
      std::cout << "  [SCHED] Running '" << task.name
                << "' (priority=" << task.priority << ")" << std::endl;
      task.taskFunc();
    }
    tasks_.clear();
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [2. PRIORITY INVERSION DEMONSTRATION]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Shows how priority inversion can occur with shared mutexes.
// TR: Paylaşılan mutex'lerle öncelik tersine çevrilmesinin nasıl oluştuğunu gösterir.

std::mutex sharedResourceMtx;
std::atomic<bool> inversionDemo{true};

void lowPriorityTask() {
  std::cout << "  [LOW] Acquiring shared resource..." << std::endl;
  std::lock_guard<std::mutex> lock(sharedResourceMtx);
  std::cout << "  [LOW] Got resource. Working (simulating slow I/O)..."
            << std::endl;
  // EN: Simulate slow work while holding the lock.
  // TR: Kilidi tutarken yavaş çalışmayı simüle et.
  std::this_thread::sleep_for(std::chrono::milliseconds(80));
  std::cout << "  [LOW] Done. Releasing resource." << std::endl;
}

void highPriorityTask() {
  // EN: Small delay so low-priority grabs the lock first.
  // TR: Düşük öncelikli görev kilidi ilk alsın diye küçük gecikme.
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "  [HIGH] Trying to acquire shared resource..." << std::endl;
  auto start = std::chrono::steady_clock::now();
  std::lock_guard<std::mutex> lock(sharedResourceMtx);
  auto end = std::chrono::steady_clock::now();
  auto waitMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - start).count();
  std::cout << "  [HIGH] Got resource after waiting " << waitMs << "ms!"
            << std::endl;
  std::cout << "  [HIGH] => Priority inversion: HIGH waited for LOW!"
            << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [3. LOCK-FREE SPSC QUEUE (Single Producer, Single Consumer)]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Lock-free SPSC queue using atomic indices — used in automotive sensor pipelines (ADC →
// DSP), CAN message buffers, and audio ring buffers.
// TR: Atomik indeksler kullanan kilitsiz SPSC kuyruğu — otomotiv sensör hatlarında (ADC → DSP),
// CAN mesaj tamponlarında kullanılır.
template <typename T, std::size_t Capacity>
class LockFreeSPSCQueue {
  std::array<T, Capacity> buffer_{};
  std::atomic<std::size_t> readIdx_{0};
  std::atomic<std::size_t> writeIdx_{0};

public:
  bool tryPush(const T& item) {
    std::size_t currWrite = writeIdx_.load(std::memory_order_relaxed);
    std::size_t nextWrite = (currWrite + 1) % Capacity;
    // EN: If next write would hit read, queue is full.
    // TR: Sonraki yazma okumaya çarparsa, kuyruk doludur.
    if (nextWrite == readIdx_.load(std::memory_order_acquire)) {
      return false; // Full
    }
    buffer_[currWrite] = item;
    writeIdx_.store(nextWrite, std::memory_order_release);
    return true;
  }

  bool tryPop(T& item) {
    std::size_t currRead = readIdx_.load(std::memory_order_relaxed);
    if (currRead == writeIdx_.load(std::memory_order_acquire)) {
      return false; // Empty
    }
    item = buffer_[currRead];
    readIdx_.store((currRead + 1) % Capacity,
                   std::memory_order_release);
    return true;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [4. ATOMIC CAS — Lock-Free Counter with Backoff]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Demonstrates compare_exchange_weak in a CAS loop with exponential backoff — common in
// lock-free embedded counters.
// TR: Üstel geri çekilme ile CAS döngüsünde compare_exchange_weak'i gösterir — kilitsiz gömülü
// sayaçlarda yaygındır.

std::atomic<int> lockFreeCounter{0};

void casIncrement(int iterations) {
  for (int i = 0; i < iterations; ++i) {
    int expected = lockFreeCounter.load(std::memory_order_relaxed);
    while (!lockFreeCounter.compare_exchange_weak(
        expected, expected + 1, std::memory_order_release,
        std::memory_order_relaxed)) {
      // EN: CAS failed — expected was updated, retry automatically.
      // TR: CAS başarısız — expected güncellendi, otomatik yeniden dene.
    }
  }
}

int main() {
  std::cout << "=== MODULE 11: RTOS CONCURRENCY PATTERNS ===\n" << std::endl;

  // --- Demo 1: Priority-Based Scheduler ---
  std::cout << "--- DEMO 1: RTOS Priority Scheduler ---\n" << std::endl;
  SimpleScheduler scheduler;
  scheduler.addTask("HMI_Render", 2, []() {
    std::cout << "    -> Rendering instrument cluster gauges" << std::endl;
  });
  scheduler.addTask("ABS_BrakeControl", 10, []() {
    std::cout << "    -> ABS brake actuation — CRITICAL" << std::endl;
  });
  scheduler.addTask("Climate_FanSpeed", 3, []() {
    std::cout << "    -> Adjusting HVAC fan to target temperature" << std::endl;
  });
  scheduler.addTask("Airbag_Deploy", 15, []() {
    std::cout << "    -> Airbag deployment check — HIGHEST PRIORITY"
              << std::endl;
  });
  scheduler.addTask("CAN_Logger", 1, []() {
    std::cout << "    -> Logging CAN bus messages to flash" << std::endl;
  });
  scheduler.runAll();

  // --- Demo 2: Priority Inversion ---
  std::cout << "\n--- DEMO 2: Priority Inversion ---\n" << std::endl;
  std::cout << "Scenario: LOW holds mutex, HIGH must wait:" << std::endl;
  std::thread tLow(lowPriorityTask);
  std::thread tHigh(highPriorityTask);
  tLow.join();
  tHigh.join();

  // --- Demo 3: Lock-Free SPSC Queue ---
  std::cout << "\n--- DEMO 3: Lock-Free SPSC Queue ---\n" << std::endl;
  LockFreeSPSCQueue<int, 8> sensorQueue;

  // EN: Producer: ADC sampling interrupt pushes sensor readings.
  // TR: Üretici: ADC örnekleme kesmesi sensör okumalarını iter.
  std::thread producer([&sensorQueue]() {
    for (int i = 1; i <= 10; ++i) {
      int sensorVal = 1000 + i * 50; // Simulated EGT value in °C
      if (sensorQueue.tryPush(sensorVal)) {
        std::cout << "  [PRODUCER] Pushed EGT reading: " << sensorVal
                  << " C" << std::endl;
      } else {
        std::cout << "  [PRODUCER] Queue full! Dropped: " << sensorVal
                  << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  });

  // EN: Consumer: DSP task processes sensor readings.
  // TR: Tüketici: DSP görevi sensör okumalarını işler.
  std::thread consumer([&sensorQueue]() {
    int val = 0;
    int count = 0;
    while (count < 10) {
      if (sensorQueue.tryPop(val)) {
        std::cout << "  [CONSUMER] Processed EGT: " << val << " C"
                  << (val > 1300 ? " ** WARNING: OVERHEAT! **" : "")
                  << std::endl;
        ++count;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }
  });

  producer.join();
  consumer.join();

  // --- Demo 4: Lock-Free CAS Counter ---
  std::cout << "\n--- DEMO 4: Lock-Free CAS Counter ---\n" << std::endl;
  lockFreeCounter.store(0);
  constexpr int THREADS = 4;
  constexpr int ITERS_PER_THREAD = 10000;
  std::vector<std::thread> workers;
  workers.reserve(THREADS);
  for (int t = 0; t < THREADS; ++t) {
    workers.emplace_back(casIncrement, ITERS_PER_THREAD);
  }
  for (auto& w : workers) w.join();

  std::cout << "  Expected: " << THREADS * ITERS_PER_THREAD << std::endl;
  std::cout << "  Actual:   " << lockFreeCounter.load() << std::endl;
  std::cout << "  Match:    "
            << (lockFreeCounter.load() == THREADS * ITERS_PER_THREAD
                    ? "YES (lock-free is correct!)"
                    : "NO (bug!)")
            << std::endl;

  std::cout << "\n=> RTOS scheduling, priority inversion, lock-free queue, "
               "CAS counter demonstrated!" << std::endl;

  return 0;
}
