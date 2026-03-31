/**
 * @file module_07_under_the_hood/03_fork_vs_thread_pool.cpp
 * @brief Under the Hood: Posix fork() vs std::thread & Thread Pooling — İşletim Sistemi: Process
 * vs Thread
 *
 * @details
 * =============================================================================
 * [THEORY: Process vs Thread / TEORİ: İşlem (Process) vs İş Parçacığı (Thread)]
 * =============================================================================
 * EN: In Linux (POSIX), `fork()` creates a completely new PROCESS. It CLONES the entire memory
 * space of the parent program. Processes do not share memory. If Process A changes a variable,
 * Process B does not see it! (High security, HIGH cost, communication requires "Pipes/IPC").
 *
 * `std::thread` creates a THREAD within the same Process. All threads SHARE the exact same RAM
 * (Heap, Global vars). If Thread A changes a variable, Thread B sees it immediately! (Low cost,
 * HIGH DANGER of Race Conditions -> Requires Mutex).
 *
 * TR: İşletim Sistemi Mimarisinde `fork()` (Linux'a özeldir) kocaman bir ağaçtır. Sen `fork`
 * çağırdığında programın RAM'deki her şeyinin (3 GB bile olsa) İşletim Sistemi tarafından İKİNCİ
 * BİR KOPYASI yaratılır! İki paralel evren oluşur. Birbirlerinin değişkenlerini GÖREMEZLER!
 * (Güvenlik devasa, Bedel devasa).
 *
 * `std::thread` (C++11) ise aynı evin içinde yeni bir odadır. Tüm Thread'ler AYNI SALONU (RAM /
 * Heap Bellek) paylaşırlar. Bir Thread masadaki (RAM) değeri değiştirirse, diğeri anında görür!
 * (Bedel yok denecek kadar azdır ama Race Condition (Yarış Durumu) çıkmaması için `std::mutex`
 * Kilidi mecburidir).
 *
 * =============================================================================
 * [THEORY: Thread Pooling Architecture / TEORİ: İş Parçacığı Havuzu]
 * =============================================================================
 * EN: Constantly "Creating a Thread, assigning work, and Destroying the Thread" is slow even at
 * the OS level. Professional automotive middleware (AUTOSAR) or servers (Node.js) create a
 * "Thread Pool" on startup. They keep heavily sleeping workers alive. When there is work, they
 * wake them up, no creation cost overhead!
 *
 * TR: Sürekli "Thread Yarat, iş ver, Thread'i Yık" döngüsü işletim sistemi seviyesinde bile
 * yavaştır. Profesyonel otomotiv ara katmanları (AUTOSAR) veya sunucular (Node.js) uygulama
 * açılışında bir "Thread Pool" (İş Parçacığı Havuzu) açar. 8 boş çekirdek için 8 tane "aç uyuyan
 * işçi" Thread yaratılır. İş oldukça onlara yollanır, bitince uyumaya geri dönerler. ASLA baştan
 * oluşturulmaz (Zero overhead).
 *
 * =============================================================================
 * [CPPREF DEPTH: Context Switching Penalty / CPPREF DERİNLİK: Bağlam Değişimi Cezası]
 * =============================================================================
 * EN: The OS Scheduler switches between running threads 1000 times a second. This is called a
 * "Context Switch" and it is EXPENSIVE! A thread has to save all its CPU registers to RAM, and
 * load the next thread's registers. If you spawn 1000 threads on an 8-core CPU, the OS spends
 * more time Context Switching than actually executing your code (Thrashing)!
 *
 * TR: İşletim Sistemi Zamanlayıcısı (OS Scheduler) saniyede 1000 kere koşan Thread'ler arasında
 * geçiş yapar. Buna "Context Switch (Bağlam Değiştirme)" denir ve ÇOK PAHALIDIR! Bir Thread tüm
 * işlemci yazmaçlarını RAM'e kaydeder, diğeri de RAM'den işlemciye yüklenir. 8 Core bir CPU'da
 * 1000 tane Thread açarsanız, donanım kodunuzu çalıştırmaktan çok sadece Thread'ler arası geçiş
 * yaparak alev alır ve sistem kitlenir (Thrashing/Donma)! Thread Pool bu yüzden şarttır.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_fork_vs_thread_pool.cpp -o 03_fork_vs_thread_pool
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
// EN: For waiting on Linux processes
// TR: Linux süreçlerini beklemek için
#include <sys/wait.h>
#include <thread>
// EN: POSIX standard (for fork). Does NOT exist on Windows!
// TR: POSIX standardı (fork için). Windows'ta YOKTUR!
#include <unistd.h>

// EN: A variable sitting in the global heap space.
// TR: Global uzayda oturan ortak bir değişken.
int sharedVariable = 100;

void threadWorkerTask(int id) {
  // EN: All Threads SEE and MODIFY the EXACT SAME variable!
  // TR: Tüm İplikler (Threads) Aynı "sharedVariable"ı GÖRÜR VE DEĞİŞTİRİR!
  sharedVariable += 50;
  std::cout << "[THREAD " << id << "] I am a C++ Thread! Shared RAM value is now: " <<
      sharedVariable << std::endl;
}

int main() {
  std::cout << "=== MODULE 7: FORK (PROCESS) VS THREAD (IS PARCACIGI) ===\n" << std::endl;

  std::cout << "--- 1. C++11 std::thread (Lightweight, Shared Memory / Hafif "
               "ve Ortak) ---"
            << std::endl;
  sharedVariable = 100;

  std::thread t1(threadWorkerTask, 1);
  std::thread t2(threadWorkerTask, 2);

  t1.join();
  t2.join();

  std::cout << "[MAIN] Threads finished. Did the shared variable TRULY change "
               "globally?\n"
            << "[MAIN] Degisken sistemde GERCEKTEN DEĞİŞTİ Mİ?: "
            << sharedVariable << std::endl;
  // EN: Output will be 200! (Shared Memory)
  // TR: Çıktı 200 olacaktır! (Ortak Bellek)

  std::cout << "\n----------------------------------------------------\n";

  std::cout << "--- 2. Linux POSIX fork() (Heavy, Cloned Universes / Ağır, "
               "Paralel Evrenler) ---"
            << std::endl;
  sharedVariable = 100; // Reset / Sıfırlıyoruz.

  // EN: Fork SPLITS the OS execution in two. It returns once as the Parent, once as the Child.
  // TR: Fork işletim sistemini 2'ye yarar. İşletim sistemi aynı satırda ikiz kod döndürür.
  pid_t processID = fork();

  if (processID < 0) {
    std::cerr << "Fork failed! (Fork fiyaskosu!)" << std::endl;
    return 1;
  } else if (processID == 0) {
    // === CHILD PROCESS ZONE (THE PARALLEL UNIVERSE) / ÇOCUK BÖLGESİ ===
    // EN: The child is born with an exact 1:1 photostatic copy of the RAM.
    // TR: Çocuk, babasının anılarının (RAM'in) 1'e 1 fotokopisine sahip başlar!
    sharedVariable += 500;
    std::cout << "[FORK: CHILD PROCESS / ÇOCUK] My pID: " << getpid() <<
        ". In my parallel universe, sharedVariable is (Benim "
                 "evrenimde değer): "
              << sharedVariable << std::endl;
    // EN: Child commits suicide.
    // TR: Çocuk işini zorla bitirip "exit" yapar.
    exit(0);

  } else {
    // === PARENT PROCESS ZONE (THE ORIGINAL UNIVERSE) / BABA (ASIL) BÖLGE ===
    // EN: processID > 0 means it's the ID of the son that was just born.
    // TR: processID > 0 demek, Oğlumun doğduğu pID numarasıdır.

    // EN: MUST WAIT for the child to die, otherwise it becomes a Zombie Process.
    // TR: Çocuğun paralel evrendeki işini bitirmesini Bekliyoruz (Zombi Proces olmasın diye).
    wait(NULL);

    std::cout << "[FORK: PARENT PROCESS / BABA] Child finished, I woke up." << std::endl;
    std::cout << "[FORK: PARENT PROCESS / BABA] Let's see if the child's +500 "
                 "affected MY UNIVERSE?"
              << std::endl;
    std::cout << "[RESULT / SONUÇ] Parent's sharedVariable is (Babanın "
                 "evrenindeki değer): "
              << sharedVariable << " !!!\n"
              << std::endl;

    // EN: Output will STILL be 100! The OS blocked the memory via Virtual Memory walls.
    // TR: Cıktı Hala 100 kalıcaktır! İşletim Sistemi aralarına Duvar (Virtual Memory
    // Segregation) cektì!
  }

  std::cout << "\n[ARCHITECTURAL CONCLUSION / MİMARİ SONUÇ]:" << std::endl;
  std::cout << "A-) 'fork()' (Process) is ultra safe but HEAVY. Data exchange "
               "(Pipes) is a nightmare."
            << std::endl;
  std::cout << "B-) 'std::thread' runs extremely FAST but causes Race Conditions." << std::endl;
  std::cout << "C-) 'THREAD POOL' (Havuz) is the modern industry secret. Keep "
               "threads alive and waiting!"
            << std::endl;

  return 0;
}
