/**
 * @file module_06_industry_standards/01_concurrency_threads.cpp
 * @brief Industry Standards: Threading and Concurrency / Endüstri Standartları: İş Parçacıkları
 * ve Eşzamanlılık
 *
 * @details
 * =============================================================================
 * [THEORY: Why Multithreading? / TEORİ: Neden Çoklu İş Parçacığı?]
 * =============================================================================
 * EN: Single-threaded programs execute one line at a time. If a video is rendering and taking 5
 * minutes, the entire UI freezes. Modern CPUs have 8, 16, or 32 cores. `std::thread` allows C++
 * to split work across these cores simultaneously.
 *
 * TR: Tek iş parçacıklı (single-threaded) programlar aynı anda tek satır okur. Eğer bir video
 * render ediliyorsa ve 5 dakika sürüyorsa, bütün kullanıcı arayüzü (UI) donar. Modern işlemciler
 * 16-32 çekirdeklidir. C++11 ile gelen `std::thread`, donanımın tüm gücünü sömürerek işleri yan
 * yana (aynı anda) fiziksel olarak çalıştırmamızı sağlar.
 *
 * =============================================================================
 * [THEORY: Race Condition and Mutex / TEORİ: Yarış Durumu ve Mutex Kilidi]
 * =============================================================================
 * EN: Two threads trying to write to the SAME variable at the EXACT same nanosecond causes a
 * "Race Condition" (corruption). We must protect shared data. `std::mutex` acts like a bathroom
 * key. Only one thread can hold the key, modify data, and return the key. Others wait at the
 * door.
 *
 * TR: İki ayrı Thread'in saniyenin milyarda biri hızla aynı değişkene (aynı RAM adresine)
 * YAZMAYA çalışmasına 'Race Condition' (Yarış Durumu) denir. Veri anında bozulur. Bunu çözmek
 * için `std::mutex` (Mutual Exclusion / Karşılıklı Dışlama) kullanırız. Mutex bir "Kilitli Kapı
 * Anahtarı" gibidir. Bir Thread anahtarı alır (`lock()`), veriyi değiştirir ve işi bitince
 * anahtarı bırakır (`unlock()`). Diğer Thread'ler kapıda beklemek zorundadır.
 *
 * =============================================================================
 * [THEORY: std::atomic — Lock-Free Thread Safety / TEORİ: std::atomic — Kilitsiz Thread
 * Güvenliği]
 * =============================================================================
 * EN: For simple types (int, bool, pointers), `std::atomic<T>` provides thread-safe reads and
 * writes WITHOUT needing a mutex. The CPU guarantees the operation completes in a single
 * indivisible step. This is dramatically faster than mutex for counters, flags, and simple
 * shared state. Operations like `fetch_add()`, `compare_exchange_strong()`, and `store()/load()`
 * are all atomic.
 *
 * TR: Basit tipler (int, bool, pointer) için `std::atomic<T>`, mutex'e gerek kalmadan
 * thread-güvenli okuma/yazma sağlar. CPU, işlemin TEK bölünmez adımda tamamlanacağını garanti
 * eder. Sayaçlar, bayraklar ve basit paylaşımlı durum için mutex'ten DRAMATIK şekilde hızlıdır.
 * `fetch_add()`, `compare_exchange_strong()`, `store()/load()` gibi işlemler atomiktir.
 *
 * =============================================================================
 * [THEORY: std::condition_variable — Thread Signaling / TEORİ: std::condition_variable — Thread
 * Sinyalleşmesi]
 * =============================================================================
 * EN: Sometimes a thread needs to WAIT until a certain condition is met (e.g., "wait until the
 * queue has data"). Busy-waiting (spin loop) wastes CPU. `std::condition_variable` lets a thread
 * sleep (zero CPU) until another thread calls `notify_one()` or `notify_all()`. This is the
 * foundation of Producer-Consumer patterns.
 *
 * TR: Bazen bir thread belirli bir koşul gerçekleşene kadar BEKLEMELİDİR (örn. "kuyrukta veri
 * olana kadar bekle"). Sürekli döngüde kontrol etmek (spin loop) CPU israfıdır.
 * `std::condition_variable`, bir thread'in sıfır CPU ile uyumasına ve başka bir thread
 * `notify_one()` veya `notify_all()` çağırana kadar beklemesine olanak tanır. Producer-Consumer
 * kalıbının temelidir.
 *
 * =============================================================================
 * [CPPREF DEPTH: Data Race is PURE Undefined Behavior! / CPPREF DERİNLİK: Veri Yarışı SAF
 * Tanımsız Davranıştır!]
 * =============================================================================
 * EN: CppReference explicitly states: A Data Race isn't just "getting the wrong math result". It
 * is strictly Undefined Behavior (UB)! The compiler is allowed to completely optimize away your
 * loop, crash the program, or print garbage. Even READING a variable while another thread is
 * WRITING to it is a Data Race!
 *
 * TR: Data Race (Veri Yarışı) C++'ta sadece "Yanlış matematiksel sonuç çıkması" demek değildir.
 * CppReference'a göre BU KESİN BİR UB'DİR (Tanımsız Davranış)! Derleyici kodun o kısmını tamamen
 * silebilir, çökertebilir. Bir Thread yazarken diğerinin sadece OKUMASI bile UB'dir (Yarı
 * yazılmış bit'leri -Torn Reads- okur).
 *
 * =============================================================================
 * [CPPREF DEPTH: std::atomic Memory Ordering — The Hidden Complexity / CPPREF DERİNLİK:
 * std::atomic Bellek Sıralaması — Gizli Karmaşıklık]
 * =============================================================================
 * EN: By default, `std::atomic` uses `memory_order_seq_cst` (sequentially consistent), the
 * STRONGEST and slowest ordering. For performance-critical code, weaker orderings like
 * `memory_order_relaxed` (no ordering guarantees), `memory_order_acquire/release`
 * (one-directional fence) exist. Using the wrong ordering leads to subtle, platform-specific
 * bugs that only appear on ARM/weak-memory architectures.
 * cppreference.com/w/cpp/atomic/memory_order
 *
 * TR: Varsayılan olarak `std::atomic`, `memory_order_seq_cst` (sıralı tutarlı) kullanır — en
 * GÜÇLÜ ve en yavaş sıralama. Performans-kritik kodlar için `memory_order_relaxed` (sıralama
 * garantisi yok), `memory_order_acquire/ release` (tek yönlü çit) gibi zayıf sıralamalar vardır.
 * Yanlış sıralama, yalnızca ARM/zayıf-bellek mimarilerinde ortaya çıkan ince, platforma özgü
 * hatalara yol açar. cppreference.com/w/cpp/atomic/memory_order
 *
 * =============================================================================
 * [CPPREF DEPTH: Spurious Wakeups in condition_variable / CPPREF DERİNLİK: condition_variable'da
 * Sahte Uyanmalar]
 * =============================================================================
 * EN: `condition_variable::wait()` can wake up WITHOUT anyone calling `notify_*()`. This is
 * called a "Spurious Wakeup" and is allowed by the POSIX and C++ standards. Always use the
 * predicate overload: `cv.wait(lock, [&]{ return !queue.empty(); })` to handle this. The lambda
 * is re-checked every time the thread wakes, spurious or not.
 * cppreference.com/w/cpp/thread/condition_variable/wait
 *
 * TR: `condition_variable::wait()`, kimse `notify_*()` çağırmadan UYANABİLİR. Buna "Sahte Uyanma
 * (Spurious Wakeup)" denir ve POSIX ile C++ standartları tarafından izin verilir. Her zaman
 * predicate (koşul) aşırı yüklemesini kullanın: `cv.wait(lock, [&]{ return !queue.empty(); })`.
 * Lambda, thread her uyandığında (sahte veya değil) yeniden kontrol edilir.
 * cppreference.com/w/cpp/thread/condition_variable/wait
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// EN: Shared Key
// TR: Ortak Kilit
std::mutex mtx;
int sharedBankAccount = 0;

/**
 * @brief EN: A task to be run by multiple threads at the same time.
 * TR: Aynı anda birden fazla iş parçacığı tarafından koşturulacak görev.
 */
void depositMoney(int id, int amount) {
  // EN: Thread ID for identification
  // TR: Thread kimliği
  (void)id;
  for (int i = 0; i < 10000; i++) {
    // [THEORY: std::lock_guard & RAII]
    // EN: lock_guard works like unique_ptr (RAII). It locks the mutex when entering the scope
    // {...} and AUTOMATICALLY unlocks it when the scope ends. No risk of Deadlocks even if an
    // exception is thrown!
    //
    // TR: lock_guard tıpkı unique_ptr gibidir (RAII prensibi). Süslü parantez {...} içine
    // girerken Mutex'i otomatik kilitler (lock). Süslü parantez bitince otomatik olarak kilidi
    // AÇAR (unlock). Exception fırlasa bile sistem kilitlenmez (Deadlock olmaz!)

    std::lock_guard<std::mutex> lock(mtx);

    // EN: CRITICAL SECTION. Only ONE thread is allowed here at any given time.
    // TR: KRİTİK BÖLGE. Burada aynı anda sadece TEK bir thread islem yapabilir.
    sharedBankAccount += amount;

  // EN: lock_guard dies here and returns the key.
  // TR: lock_guard burada yıkılır ve anahtarı devreder.
  }
}

// =================================================================================================
// [DEMO 2: std::atomic - Lock-Free Counter / Kilitsiz Sayaç]
// =================================================================================================
std::atomic<int> atomicCounter{0};

void atomicIncrement(int iterations) {
  for (int i = 0; i < iterations; i++) {
    // EN: fetch_add is ONE atomic CPU instruction — no mutex needed!
    // TR: fetch_add TEK atomik CPU komutu — mutex gerekmez!
    atomicCounter.fetch_add(1, std::memory_order_relaxed);
  }
}

// =================================================================================================
// [DEMO 3: Producer-Consumer with condition_variable]
// =================================================================================================
std::queue<int> taskQueue;
std::mutex queueMtx;
std::condition_variable cv;
bool producerDone = false;

/**
 * @brief EN: Producer pushes tasks into the queue and notifies consumers.
 * TR: Üretici, kuyruğa görev ekler ve tüketicileri bilgilendirir.
 */
void producer(int taskCount) {
  for (int i = 1; i <= taskCount; i++) {
    {
      std::lock_guard<std::mutex> lock(queueMtx);
      taskQueue.push(i);
      std::cout << "[PRODUCER] Pushed task #" << i << std::endl;
    }
    // EN: Wake ONE waiting consumer thread
    // TR: Bekleyen BİR tüketiciyi uyandır
    cv.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  {
    std::lock_guard<std::mutex> lock(queueMtx);
    producerDone = true;
  }
  // EN: Wake ALL remaining consumers to let them exit.
  // TR: Kalan TÜM tüketicileri çıkabilmeleri için uyandır.
  cv.notify_all();
}

/**
 * @brief EN: Consumer waits for tasks and processes them.
 * TR: Tüketici görev bekler ve işler.
 */
void consumer(int id) {
  while (true) {
    std::unique_lock<std::mutex> lock(queueMtx);
    // EN: Wait with predicate to handle SPURIOUS WAKEUPS safely!
    // TR: Sahte uyanmalara karşı predicate ile bekle!
    cv.wait(lock, [] { return !taskQueue.empty() || producerDone; });

    if (taskQueue.empty() && producerDone) {
      // EN: No more work
      // TR: İş kalmadı
      break;
    }

    int task = taskQueue.front();
    taskQueue.pop();
    // EN: Release lock before processing
    // TR: İşlemden önce kilidi bırak
    lock.unlock();

    std::cout << "[CONSUMER " << id << "] Processing task #" << task << std::endl;
  }
}

int main() {
  std::cout << "=== MODULE 6: INDUSTRY STANDARDS - CONCURRENCY ===\n" << std::endl;

  // ===============================================================================================
  // PART 1: Mutex-Protected Bank Account (Original Demo)
  // ===============================================================================================
  std::cout << "--- PART 1: std::mutex + std::lock_guard ---\n" << std::endl;
  std::cout << "Main Thread started (Ana cekirdek acildi).\n" << std::endl;

  std::vector<std::thread> workers;
  for (int i = 0; i < 10; i++) {
    workers.push_back(std::thread(depositMoney, i, 1));
  }

  std::cout << "10 Threads LAUNCHED! Running massively in the background...\n"
               "(10 Thread Firlatildi! Arka planda devasa hizla calisiyorlar...)\n"
            << std::endl;

  for (auto &w : workers) {
    w.join();
  }

  std::cout << "Expected Bank Account: 100000" << std::endl;
  std::cout << "Actual Bank Account:   " << sharedBankAccount << std::endl;

  if (sharedBankAccount == 100000) {
    std::cout << "=> [SUCCESS] std::mutex prevented the Race Condition!\n" << std::endl;
  }

  // ===============================================================================================
  // PART 2: std::atomic — Lock-Free Counter
  // ===============================================================================================
  // EN: 10 threads each increment an atomic counter 10000 times. No mutex needed! The CPU
  // handles atomicity at the hardware level.
  // TR: 10 thread her biri atomik sayacı 10000 kez artırır. Mutex gerekmez! CPU atomikliği
  // donanım seviyesinde halleder.
  std::cout << "--- PART 2: std::atomic (Lock-Free Counter) ---\n" << std::endl;

  std::vector<std::thread> atomicWorkers;
  for (int i = 0; i < 10; i++) {
    atomicWorkers.emplace_back(atomicIncrement, 10000);
  }
  for (auto &w : atomicWorkers) {
    w.join();
  }

  std::cout << "Expected atomic counter: 100000" << std::endl;
  std::cout << "Actual atomic counter:   " << atomicCounter.load() << std::endl;

  if (atomicCounter.load() == 100000) {
    std::cout << "=> [SUCCESS] std::atomic is lock-free and correct!\n" << std::endl;
  }

  // ===============================================================================================
  // PART 3: Producer-Consumer with condition_variable
  // ===============================================================================================
  // EN: Classic multi-threaded pattern: 1 producer pushes tasks, 2 consumers process them.
  // condition_variable coordinates without busy-waiting.
  // TR: Klasik çoklu-thread kalıbı: 1 üretici görev ekler, 2 tüketici işler. condition_variable
  // meşgul-bekleme olmadan koordine eder.
  std::cout << "--- PART 3: Producer-Consumer (condition_variable) ---\n"
            << std::endl;

  std::thread prod(producer, 6);
  std::thread cons1(consumer, 1);
  std::thread cons2(consumer, 2);

  prod.join();
  cons1.join();
  cons2.join();

  std::cout << "\n=> Producer-Consumer pattern completed with zero busy-waiting!"
            << std::endl;

  return 0;
}
