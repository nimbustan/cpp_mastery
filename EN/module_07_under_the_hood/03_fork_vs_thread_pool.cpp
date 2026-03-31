/**
 * @file module_07_under_the_hood/03_fork_vs_thread_pool.cpp
 * @brief Under the Hood: Posix fork() vs std::thread & Thread Pooling
 *
 * @details
 * =============================================================================
 * [THEORY: Process vs Thread]
 * =============================================================================
 * EN: In Linux (POSIX), `fork()` creates a completely new PROCESS. It CLONES the entire memory
 * space of the parent program. Processes do not share memory. If Process A changes a variable,
 * Process B does not see it! (High security, HIGH cost, communication requires "Pipes/IPC").
 *
 * `std::thread` creates a THREAD within the same Process. All threads SHARE the exact same RAM
 * (Heap, Global vars). If Thread A changes a variable, Thread B sees it immediately! (Low cost,
 * HIGH DANGER of Race Conditions -> Requires Mutex).
 *
 * =============================================================================
 * [THEORY: Thread Pooling Architecture]
 * =============================================================================
 * EN: Constantly "Creating a Thread, assigning work, and Destroying the Thread" is slow even at
 * the OS level. Professional automotive middleware (AUTOSAR) or servers (Node.js) create a
 * "Thread Pool" on startup. They keep heavily sleeping workers alive. When there is work, they
 * wake them up, no creation cost overhead!
 *
 * =============================================================================
 * [CPPREF DEPTH: Context Switching Penalty]
 * =============================================================================
 * EN: The OS Scheduler switches between running threads 1000 times a second. This is called a
 * "Context Switch" and it is EXPENSIVE! A thread has to save all its CPU registers to RAM, and
 * load the next thread's registers. If you spawn 1000 threads on an 8-core CPU, the OS spends
 * more time Context Switching than actually executing your code (Thrashing)!
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_fork_vs_thread_pool.cpp -o 03_fork_vs_thread_pool
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
// EN: For waiting on Linux processes
#include <sys/wait.h>
#include <thread>
// EN: POSIX standard (for fork). Does NOT exist on Windows!
#include <unistd.h>

// EN: A variable sitting in the global heap space.
int sharedVariable = 100;

void threadWorkerTask(int id) {
  // EN: All Threads SEE and MODIFY the EXACT SAME variable!
  sharedVariable += 50;
  std::cout << "[THREAD " << id << "] I am a C++ Thread! Shared RAM value is now: " <<
      sharedVariable << std::endl;
}

int main() {
  std::cout << "=== MODULE 7: FORK (PROCESS) VS THREAD ===\n" << std::endl;

  std::cout << "--- 1. C++11 std::thread (Lightweight, Shared Memory) ---"
            << std::endl;
  sharedVariable = 100;

  std::thread t1(threadWorkerTask, 1);
  std::thread t2(threadWorkerTask, 2);

  t1.join();
  t2.join();

  // EN: Output will be 200! (Shared Memory)

  std::cout << "\n----------------------------------------------------\n";

  sharedVariable = 100; // Reset

  // EN: Fork SPLITS the OS execution in two. It returns once as the Parent, once as the Child.
  pid_t processID = fork();

  if (processID < 0) {
    std::cerr << "Fork failed!" << std::endl;
    return 1;
  } else if (processID == 0) {
    // === CHILD PROCESS ZONE (THE PARALLEL UNIVERSE)
    // EN: The child is born with an exact 1:1 photostatic copy of the RAM.
    sharedVariable += 500;
    // EN: Child commits suicide.
    exit(0);

  } else {
    // === PARENT PROCESS ZONE (THE ORIGINAL UNIVERSE)
    // EN: processID > 0 means it's the ID of the son that was just born.

    // EN: MUST WAIT for the child to die, otherwise it becomes a Zombie Process.
    wait(NULL);

    std::cout << "[FORK: PARENT PROCESS] Child finished, I woke up." << std::endl;
    std::cout << "[FORK: PARENT PROCESS] Let's see if the child's +500 "
                 "affected MY UNIVERSE?"
              << std::endl;

    // EN: Output will STILL be 100! The OS blocked the memory via Virtual Memory walls.
  }

  std::cout << "A-) 'fork()' (Process) is ultra safe but HEAVY. Data exchange "
               "(Pipes) is a nightmare."
            << std::endl;
  std::cout << "B-) 'std::thread' runs extremely FAST but causes Race Conditions." << std::endl;
  std::cout << "C-) 'THREAD POOL' is the modern industry secret. Keep "
               "threads alive and waiting!"
            << std::endl;

  return 0;
}
