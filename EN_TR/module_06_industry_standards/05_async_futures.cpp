/**
 * @file module_06_industry_standards/05_async_futures.cpp
 * @brief Industry Standards — Asynchronous Code: std::async, std::future, std::promise,
 * std::packaged_task & std::shared_future
 *
 * @details
 * =============================================================================
 * [THEORY: Why std::async instead of std::thread? / TEORİ: std::async vs std::thread]
 * =============================================================================
 * EN: When you launch `std::thread`, it runs completely parallel, but it does NOT easily RETURN
 * a value back to the main thread. Also, managing raw threads can be messy. `std::async` is a
 * higher-level tool. It runs a task in the background and returns a "TICKET" called
 * `std::future`. You can continue doing your work on the main thread, and when you finally need
 * the answer, you cash in the ticket (`ticket.get()`).
 *
 * TR: `std::thread` ile boyut açtığımızda harika bir şekilde paralel çalışır (ANCAK), işi
 * bitirdiğinde Ana Thread'e (Main) "Sonucu (Return değeri)" geri yollaması büyük bir eziyettir.
 * C++11 ile `std::async` geldi.
 *
 * `std::async` arka planda bir thread acar, işi o thread'e verir ve sana garantör olarak bir
 * "Fiş / Bilet / Kupon" (`std::future`) uzatır. "Sen git kendi isine bak, işlemler bitince bana
 * bu biletle geleceksin ve ben sana hesapladığım değeri geri vereceğim." der. Eğer sen bileti
 * tahsil etmeye `ticket.get()` ile gidersen ve arka plandaki is HENÜZ BİTMEMİŞSE, Main Thread o
 * noktada işlemi beklemeye (BLOKE OLMAYA) gecer!
 *
 * =============================================================================
 * [THEORY: std::promise — The Manual Push Channel / TEORİ: std::promise — Manuel Değer İtme
 * Kanalı]
 * =============================================================================
 * EN: `std::async` automatically decides which thread runs the work and delivers the result. But
 * sometimes YOU want to control when and what value gets pushed. `std::promise<T>` is a
 * write-end channel: you call `promise.set_value(42)` from any thread, and the paired
 * `std::future<T>` (obtained via `promise.get_future()`) receives it. Think of it as a one-shot
 * mailbox: the sender drops a letter, the receiver picks it up.
 *
 * TR: `std::async` işi otomatik planlar. Ancak bazen HANGİ değerin, HANGİ anda gönderileceğini
 * SİZ kontrol etmek istersiniz. `std::promise<T>` bir "yazma ucu" kanalıdır. Herhangi bir
 * thread'den `promise.set_value(42)` dersiniz, eşleştirilmiş `std::future<T>`
 * (promise.get_future() ile alınan) o değeri alır. Tek kullanımlık posta kutusu gibidir:
 * gönderici mektubu atar, alıcı kutudan çeker.
 *
 * =============================================================================
 * [THEORY: std::packaged_task — Callable Wrapper with Future / TEORİ: std::packaged_task —
 * Future Bağlantılı Çağrılabilir Sarmalayıcı]
 * =============================================================================
 * EN: `std::packaged_task<R(Args...)>` wraps ANY callable (function, lambda, functor) and
 * automatically connects it to a `std::future`. When you invoke the packaged_task (via
 * `operator()`), the result is stored in the future. Unlike `std::async`, YOU decide exactly
 * when and on which thread the task executes — giving you full scheduling control (perfect for
 * thread pools!).
 *
 * TR: `std::packaged_task<R(Args...)>` herhangi bir çağrılabilir nesneyi (fonksiyon, lambda,
 * functor) sarar ve otomatik olarak bir `std::future` ile eşleştirir. packaged_task'ı
 * `operator()` ile çağırdığınızda sonuç future'a yazılır. `std::async`'den farkı: görevi NE
 * ZAMAN ve HANGİ thread'de çalıştıracağınıza SİZ karar verirsiniz — Thread Pool mimarisi için
 * ideal!
 *
 * =============================================================================
 * [THEORY: std::shared_future — Multiple Readers / TEORİ: std::shared_future — Çoklu Okuyucu]
 * =============================================================================
 * EN: `std::future::get()` can only be called ONCE (the ticket is torn). `std::shared_future<T>`
 * allows MULTIPLE threads to call `.get()` and each receives a copy of the result. Created from
 * `future.share()`. Useful when a single computation feeds many consumers (e.g., config loading
 * shared by 10 worker threads).
 *
 * TR: `std::future::get()` yalnızca BİR KEZ çağrılabilir (bilet yırtılır).
 * `std::shared_future<T>` ise BİRDEN FAZLA thread'in `.get()` çağırmasına izin verir ve her biri
 * sonucun bir kopyasını alır. `future.share()` ile oluşturulur. Tek bir hesaplamanın birçok
 * tüketiciye dağıtılması gerektiğinde kullanılır (örneğin, 10 işçi thread'e paylaşılan config
 * yüklemesi).
 *
 * =============================================================================
 * [CPPREF DEPTH: The Silent Block of `std::future` Destructor / CPPREF DERİNLİK: `std::future`
 * Yıkıcısının Sessiz Bloğu]
 * =============================================================================
 * EN: A massive trap! If you use `std::async(std::launch::async, ...)` but don't assign the
 * returned ticket to a variable, the temporary `std::future` dies immediately at the semicolon.
 * Its Destructor WILL BLOCK THE MAIN THREAD until the async task finishes! So it completely
 * defeats the purpose of async!
 *
 * TR: Devasa bir mülakat tuzağı! Eğer `std::async(std::launch::async, ...)` ile asenkron iş
 * başlatır ama dönen o Bileti (future) bir değişkene atamazsanız, isimsiz geçici future nesnesi
 * o satır sonunda anında ölür. CppRef kurallarına göre o Future'ın Destructor'ı, arka plandaki
 * iş bitene kadar ANA THREAD'İ KİLİTLER VE BEKLER! Yani asenkron yaptığınızı sanarken kodu
 * tamamen senkronize bloke etmiş olursunuz!
 *
 * =============================================================================
 * [CPPREF DEPTH: std::promise::set_exception — Error Propagation Across Threads / CPPREF
 * DERİNLİK: std::promise::set_exception — Thread'ler Arası Hata İletimi]
 * =============================================================================
 * EN: If a worker thread encounters an error, it can call
 * `promise.set_exception(std::current_exception())`. The consumer thread calling `future.get()`
 * will then receive the exception as if it was thrown locally. This is the ONLY safe way to
 * propagate exceptions across thread boundaries.
 * cppreference.com/w/cpp/thread/promise/set_exception
 *
 * TR: Eğer işçi thread bir hatayla karşılaşırsa,
 * `promise.set_exception(std::current_exception())` çağırabilir. Tüketici thread `future.get()`
 * çağırdığında, istisna sanki yerel olarak fırlatılmış gibi alınır. Bu, thread sınırları
 * arasında istisna iletmenin TEK güvenli yoludur.
 * cppreference.com/w/cpp/thread/promise/set_exception
 *
 * =============================================================================
 * [CPPREF DEPTH: std::packaged_task is MoveOnly — No Copying! / CPPREF DERİNLİK:
 * std::packaged_task MoveOnly'dir — Kopyalanamaz!]
 * =============================================================================
 * EN: `std::packaged_task` is move-only, just like `std::unique_ptr`. You cannot copy it. To
 * pass it into a `std::thread`, you MUST use `std::move(task)`. Attempting to copy triggers a
 * deleted copy-constructor error at compile time. cppreference.com/w/cpp/thread/packaged_task
 *
 * TR: `std::packaged_task`, tıpkı `std::unique_ptr` gibi yalnızca taşınabilir (move-only).
 * Kopyalanamaz. Bir `std::thread`'e geçirmek için `std::move(task)` kullanmanız ZORUNLUDUR.
 * Kopyalama girişimi derleme zamanında silinmiş kopya yapıcı hatası verir.
 * cppreference.com/w/cpp/thread/packaged_task
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_async_futures.cpp -o 05_async_futures
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <atomic>
#include <chrono> // EN: Time simulations
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * @brief EN: Heavy function simulating a network/database request.
 * TR: Ağdan / veritabanından veri ceker gibi bekleyen agir fonksiyon.
 * @return 200 (OK status) after 3 seconds.
 */
int heavyServerRequest() {
  std::cout << "[WORKER THREAD] Accessing Server... ETA: 3 Seconds (Sunucuya "
               "erisiliyor...)"
            << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(3));

  std::cout << "[WORKER THREAD] Server Responded / Sunucu yanit verdi: [OK - 200]" << std::endl;

  // EN: We are actually RETURNING a value! You can't easily do this with std::thread.
  // TR: Dikkat, değer DONDURUYORUZ! std::thread ile bunu basitce yakalayamazsiniz.
  return 200;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEMO 2: std::promise - Manual Value Push / Elle Değer İtme]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
/**
 * @brief EN: Worker receives a promise and pushes the result manually.
 * TR: İşçi bir promise alır ve sonucu elle gönderir.
 */
void workerWithPromise(std::promise<std::string> prom) {
  std::cout << "[PROMISE WORKER] Computing secret key..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  // EN: Push the value into the channel — the future on the other side wakes.
  // TR: Değeri kanala it — diğer uçtaki future uyanır.
  prom.set_value("SECRET-KEY-XJ9-ALPHA");
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEMO 3: std::packaged_task - Deferred Callable / Ertelenmiş Görev]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
/**
 * @brief EN: A pure function to be wrapped in packaged_task.
 * TR: packaged_task içine sarılacak saf fonksiyon.
 */
int computeChecksum(int a, int b) {
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  return (a * 31) ^ (b * 17);
}

int main() {
  std::cout << "=== MODULE 6: ADVANCED CONCURRENCY (ASYNC / FUTURE / "
               "PROMISE / PACKAGED_TASK / SHARED_FUTURE) ===\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 1: std::async + std::future (Original Demo)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- PART 1: std::async + std::future ---\n" << std::endl;

  auto startTime = std::chrono::high_resolution_clock::now();

  // EN: std::launch::async -> Forces it to run strictly on a new separate thread. "Work in the
  // background and give me a ticket!"
  // TR: "Arka planda çalış, bana bilet (future) fırlat!" std::launch::async -> Kesinlikle yeni
  // bir çekirdekte çalıştir demektir.
  std::future<int> ticket = std::async(std::launch::async, heavyServerRequest);

  // EN: Main Thread continues working!
  // TR: Ana döngü bos durmuyor!
  std::cout << "\n[MAIN] Task sent asynchronously! Meanwhile, UI is rendering "
               "frames..."
            << std::endl;

  for (int i = 1; i <= 3; i++) {
    std::cout << "[MAIN] Drawing Frame (Kare cizimi...) " << i << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // EN: Once `.get()` is called: if the heavy code isn't finished yet, MAIN FREEZES and WAITS!
  // TR: ".get()" cagirildiginda eğer ağır iş henuz isini bitirmemisse, ANA PROGRAM DONAR!
  int result = ticket.get();

  auto endTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> totalTime = endTime - startTime;

  std::cout << "\n--- ASYNC RESULTS / SONUCLAR ---" << std::endl;
  std::cout << "Received Server Code (Gelen Kod): " << result << std::endl;
  std::cout << "Total Elapsed Time: " << totalTime.count() << " seconds.\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 2: std::promise + std::future (Manual Push)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Create a promise-future pair. The promise is the "write end", the future is the "read
  // end". We send the promise to a worker thread.
  // TR: Bir promise-future çifti oluştur. Promise "yazma ucu", future "okuma ucu". Promise'i
  // işçi thread'e gönderiyoruz.
  std::cout << "--- PART 2: std::promise (Manual Channel) ---\n" << std::endl;

  std::promise<std::string> secretPromise;
  std::future<std::string> secretFuture = secretPromise.get_future();

  // EN: We MUST move the promise into the thread (it's move-only).
  // TR: Promise taşınabilir-only; std::move ZORUNLU.
  std::thread promThread(workerWithPromise, std::move(secretPromise));

  std::cout << "[MAIN] Waiting for secret key from worker..." << std::endl;
  std::string key = secretFuture.get(); // Blocks until set_value()
  std::cout << "[MAIN] Received key: " << key << "\n" << std::endl;

  promThread.join();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 3: std::packaged_task (Deferred Execution)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Wrap computeChecksum in a packaged_task. We get the future BEFORE running the task, then
  // move the task into a thread.
  // TR: computeChecksum'u packaged_task ile sar. Görevi çalıştırmadan ÖNCE future'ı al, ardından
  // görevi thread'e taşı.
  std::cout << "--- PART 3: std::packaged_task (Deferred Callable) ---\n"
            << std::endl;

  std::packaged_task<int(int, int)> task(computeChecksum);
  std::future<int> checksumFuture = task.get_future();

  // EN: Move the task into a thread (packaged_task is move-only like unique_ptr).
  // TR: Görevi thread'e taşı (packaged_task unique_ptr gibi move-only).
  std::thread taskThread(std::move(task), 42, 99);

  std::cout << "[MAIN] packaged_task dispatched. Doing other work..." << std::endl;
  int checksum = checksumFuture.get(); // Blocks until task completes
  std::cout << "[MAIN] Checksum result: " << checksum << "\n" << std::endl;

  taskThread.join();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 4: std::shared_future (Multiple Readers)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: One computation, many consumers. shared_future allows multiple .get() calls from
  // different threads.
  // TR: Tek hesaplama, çok tüketici. shared_future birden fazla thread'in .get() çağırmasına
  // izin verir.
  std::cout << "--- PART 4: std::shared_future (Multiple Readers) ---\n"
            << std::endl;

  std::shared_future<int> sharedResult =
      std::async(std::launch::async, []() -> int {
        std::cout << "[SHARED WORKER] Loading config..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // EN: Config value
        // TR: Ayar değeri
        return 9999;
      // EN: .share() converts future -> shared_future
      // TR: .share() future'ı shared_future'a dönüştürür
      }).share();

  // EN: Multiple threads read the same result simultaneously.
  // TR: Birden fazla thread aynı sonucu eşzamanlı okur.
  std::vector<std::thread> readers;
  for (int i = 0; i < 3; i++) {
    readers.emplace_back([sharedResult, i]() {
      int val = sharedResult.get(); // All 3 threads can call get()!
      std::cout << "[READER " << i << "] Config loaded: " << val << std::endl;
    });
  }

  for (auto &r : readers) {
    r.join();
  }

  std::cout << "\n=> std::shared_future allows MULTIPLE .get() calls — unlike "
               "std::future which tears the ticket on first use!"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 5: std::jthread and stop_token (C++20)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: std::jthread = "joining thread". Automatically calls .join() in destructor.
  //     Also supports cooperative cancellation via stop_token/stop_source.
  //     No more forgetting .join() — the #1 cause of std::terminate crashes!
  // TR: std::jthread = "birleşen thread". Yıkıcıda otomatik .join() çağırır.
  //     stop_token/stop_source ile işbirlikçi iptal desteği de sunar.
  std::cout << "\n--- PART 5: std::jthread & stop_token (C++20) ---\n" << std::endl;

  {
    // EN: jthread destructor auto-joins — no need for explicit .join()
    // TR: jthread yıkıcısı otomatik birleşir — açık .join() gerekmez
    std::jthread worker([](std::stop_token stoken) {
      int count = 0;
      while (!stoken.stop_requested()) {
        std::cout << "  [jthread] Working cycle " << count++ << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (count >= 5) break;  // EN: Safety limit for demo
      }
      std::cout << "  [jthread] Stopped after " << count << " cycles" << std::endl;
    });

    // EN: request_stop() signals the thread to finish cooperatively
    // TR: request_stop() thread'e işbirlikçi olarak bitmesini sinyaller
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    worker.request_stop();
    std::cout << "  [main] Stop requested" << std::endl;
    // EN: No .join() needed — jthread destructor handles it!
    // TR: .join() gereksiz — jthread yıkıcısı halleder!
  }

  std::cout << "  [main] jthread auto-joined on scope exit\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 6: stop_callback — Registering Cancellation Handlers
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: stop_callback registers a function that runs when stop is requested.
  //     Useful for cleanup: closing sockets, flushing buffers, etc.
  // TR: stop_callback, durdurma istendiğinde çalışan bir fonksiyon kaydeder.
  std::cout << "--- PART 6: stop_callback (C++20) ---\n" << std::endl;
  {
    std::jthread sensor([](std::stop_token stoken) {
      // EN: Register callback that fires on stop
      // TR: Durdurulduğunda tetiklenen geri çağrı kaydet
      std::stop_callback cb(stoken, []() {
        std::cout << "  [callback] Cleanup: flushing sensor buffer..." << std::endl;
      });

      int readings = 0;
      while (!stoken.stop_requested() && readings < 3) {
        std::cout << "  [sensor] Reading " << readings++ << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sensor.request_stop();
    // EN: callback fires immediately when stop is requested
    // TR: durdurma istendiğinde geri çağrı hemen tetiklenir
  }
  std::cout << "  [main] sensor jthread cleaned up\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // PART 7: Simple Thread Pool Pattern
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: A thread pool reuses a fixed number of threads for many tasks.
  //     Instead of creating/destroying threads per task (expensive), tasks go
  //     into a queue and idle workers pick them up.
  // TR: Thread havuzu, birçok görev için sabit sayıda thread'i yeniden kullanır.
  //     Görev başına thread oluşturmak/yok etmek yerine (pahalı), görevler
  //     kuyruğa girer ve boştaki işçiler alır.
  std::cout << "--- PART 7: Simple Thread Pool Pattern ---\n" << std::endl;
  {
    std::mutex queueMtx;
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
    std::atomic<bool> poolDone{false};
    std::atomic<int> completed{0};

    // EN: Create 4 worker threads
    // TR: 4 işçi thread oluştur
    std::vector<std::jthread> pool;
    for (int i = 0; i < 4; ++i) {
      pool.emplace_back([&, i](std::stop_token) {
        while (true) {
          std::function<void()> job;
          {
            std::unique_lock<std::mutex> lk(queueMtx);
            cv.wait(lk, [&] { return !tasks.empty() || poolDone.load(); });
            if (poolDone.load() && tasks.empty()) break;
            job = std::move(tasks.front());
            tasks.pop();
          }
          job();
          completed++;
        }
      });
    }

    // EN: Submit 12 tasks
    // TR: 12 görev gönder
    {
      std::lock_guard<std::mutex> lk(queueMtx);
      for (int i = 0; i < 12; ++i) {
        tasks.push([i]() {
          std::this_thread::sleep_for(std::chrono::milliseconds(20));
        });
      }
    }
    cv.notify_all();

    // EN: Wait for tasks then shut down pool
    // TR: Görevleri bekle sonra havuzu kapat
    while (completed.load() < 12) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    poolDone = true;
    cv.notify_all();
    pool.clear();  // EN: jthread destructors auto-join

    std::cout << "  Thread pool: 4 workers processed " << completed.load()
              << " tasks" << std::endl;
    std::cout << "  (jthread auto-joins on clear — no manual join needed)\n" << std::endl;
  }

  return 0;
}
