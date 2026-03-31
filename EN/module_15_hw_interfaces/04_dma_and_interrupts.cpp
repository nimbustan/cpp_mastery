/**
 * @file module_15_hw_interfaces/04_dma_and_interrupts.cpp
 * @brief DMA Engines & Interrupt Handling
 *
 * @details
 * =============================================================================
 * [THEORY: Interrupt System (ARM Cortex-M NVIC)]
 * =============================================================================
 *
 * EN: The Nested Vectored Interrupt Controller (NVIC) manages all interrupts:
 *     - Up to 240 external interrupts + 16 system exceptions
 *     - Programmable priority (0-255, lower = higher priority)
 *     - Priority grouping: preemption priority + sub-priority
 *     - Tail-chaining: back-to-back ISR without full context switch
 *     - Late-arriving: higher-priority preempts during stacking
 *
 *     Interrupt Lifecycle:
 *     ┌─────────┐   ┌──────────┐   ┌─────────┐   ┌──────────┐
 *     │ Trigger │ → │ Pending  │ → │ Active  │ → │ Return   │
 *     └─────────┘   └──────────┘   └─────────┘   └──────────┘
 *          HW event    NVIC queues   ISR runs     Context restore
 *
 *     Vector Table (Cortex-M):
 *     ┌─────────┬──────────────────────────────────┐
 *     │ Offset  │ Exception / IRQ                  │
 *     ├─────────┼──────────────────────────────────┤
 *     │ 0x00    │ Initial SP value                 │
 *     │ 0x04    │ Reset handler                    │
 *     │ 0x08    │ NMI handler                      │
 *     │ 0x0C    │ HardFault handler                │
 *     │ 0x10    │ MemManage handler                │
 *     │ 0x14    │ BusFault handler                 │
 *     │ 0x18    │ UsageFault handler               │
 *     │ 0x2C    │ SVCall handler                   │
 *     │ 0x38    │ PendSV handler                   │
 *     │ 0x3C    │ SysTick handler                  │
 *     │ 0x40+   │ External IRQ 0, 1, 2, ...        │
 *     └─────────┴──────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: DMA (Direct Memory Access)]
 * =============================================================================
 *
 * EN: DMA transfers data between memory and peripherals without CPU:
 *
 *     CPU workload comparison:
 *     ┌────────────────────────┬────────────┬────────────┐
 *     │ Transfer Method        │ CPU Usage  │ Latency    │
 *     ├────────────────────────┼────────────┼────────────┤
 *     │ Polling                │ 100%       │ Lowest     │
 *     │ Interrupt per byte     │ ~30-50%    │ Medium     │
 *     │ DMA                    │ ~0-5%      │ Background │
 *     │ DMA + Double buffer    │ ~0%        │ Zero-copy  │
 *     └────────────────────────┴────────────┴────────────┘
 *
 *     DMA Modes:
 *     - Normal: transfer N bytes, then stop
 *     - Circular: automatically restart after N bytes
 *     - Double-buffer: ping-pong between two memory regions
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_dma_and_interrupts.cpp -o 04_dma_int
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <iomanip>
#include <functional>
#include <queue>
#include <algorithm>
#include <cassert>
#include <bitset>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: NVIC Simulator
// ═════════════════════════════════════════════════════════════════════════════

class NVICSimulator {
public:
    static constexpr int MAX_IRQ = 64;

    struct IRQConfig {
        std::string name;
        uint8_t priority = 15;     // 0 = highest, 15 = lowest (4-bit)
        bool enabled = false;
        bool pending = false;
        bool active = false;
        std::function<void()> handler;
    };

private:
    std::array<IRQConfig, MAX_IRQ> irqs_{};
    int active_irq_ = -1;
    int preemption_count_ = 0;
    std::vector<std::string> execution_log_;

public:
    void configure_irq(int irq_num, std::string name, uint8_t priority,
                        std::function<void()> handler) {
        if (irq_num < 0 || irq_num >= MAX_IRQ) return;
        auto& irq = irqs_[static_cast<size_t>(irq_num)];
        irq.name = std::move(name);
        irq.priority = priority;
        irq.handler = std::move(handler);
    }

    void enable_irq(int irq_num) {
        if (irq_num >= 0 && irq_num < MAX_IRQ)
            irqs_[static_cast<size_t>(irq_num)].enabled = true;
    }

    void disable_irq(int irq_num) {
        if (irq_num >= 0 && irq_num < MAX_IRQ)
            irqs_[static_cast<size_t>(irq_num)].enabled = false;
    }

    // EN: Set interrupt pending (hardware trigger)
    void set_pending(int irq_num) {
        if (irq_num >= 0 && irq_num < MAX_IRQ)
            irqs_[static_cast<size_t>(irq_num)].pending = true;
    }

    // EN: Process all pending interrupts by priority
    void process_pending() {
        // EN: Find highest priority pending+enabled interrupt
        int best = -1;
        uint8_t best_pri = 255;

        for (int i = 0; i < MAX_IRQ; ++i) {
            auto& irq = irqs_[static_cast<size_t>(i)];
            if (irq.pending && irq.enabled && !irq.active && irq.priority < best_pri) {
                // EN: Check preemption: can only preempt if higher priority than active
                if (active_irq_ >= 0 &&
                    irq.priority >= irqs_[static_cast<size_t>(active_irq_)].priority) {
                    continue;  // cannot preempt
                }
                best = i;
                best_pri = irq.priority;
            }
        }

        if (best < 0) return;

        auto& irq = irqs_[static_cast<size_t>(best)];
        int prev_active = active_irq_;

        if (active_irq_ >= 0) {
            preemption_count_++;
            execution_log_.push_back("  [PREEMPT] " + irqs_[static_cast<size_t>(active_irq_)].name +
                                      " → " + irq.name);
        }

        irq.pending = false;
        irq.active = true;
        active_irq_ = best;

        execution_log_.push_back("  [ENTER] IRQ " + std::to_string(best) +
                                  " (" + irq.name + ") pri=" +
                                  std::to_string(irq.priority));

        if (irq.handler) irq.handler();

        irq.active = false;
        execution_log_.push_back("  [EXIT]  IRQ " + std::to_string(best) +
                                  " (" + irq.name + ")");
        active_irq_ = prev_active;

        // EN: Tail-chaining: check for more pending
        process_pending();
    }

    void print_config() const {
        std::cout << "  NVIC Configuration:\n";
        std::cout << "  ┌─────┬─────────────────────┬──────────┬─────────┐\n";
        std::cout << "  │ IRQ │ Name                │ Priority │ Enabled │\n";
        std::cout << "  ├─────┼─────────────────────┼──────────┼─────────┤\n";
        for (int i = 0; i < MAX_IRQ; ++i) {
            auto& irq = irqs_[static_cast<size_t>(i)];
            if (!irq.name.empty()) {
                std::cout << "  │ " << std::setw(3) << i
                          << " │ " << std::left << std::setw(19) << irq.name << std::right
                          << " │ " << std::setw(8) << static_cast<int>(irq.priority)
                          << " │ " << std::setw(7) << (irq.enabled ? "YES" : "NO")
                          << " │\n";
            }
        }
        std::cout << "  └─────┴─────────────────────┴──────────┴─────────┘\n";
    }

    void print_log() const {
        std::cout << "  Execution Log:\n";
        for (auto& entry : execution_log_)
            std::cout << entry << "\n";
    }

    int preemption_count() const { return preemption_count_; }

    void clear_log() {
        execution_log_.clear();
        preemption_count_ = 0;
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: DMA Stream Simulator
// ═════════════════════════════════════════════════════════════════════════════

enum class DMADirection : uint8_t {
    PeriphToMemory = 0,
    MemoryToPerph  = 1,
    MemoryToMemory = 2,
};

enum class DMADataWidth : uint8_t {
    Byte     = 1,
    HalfWord = 2,
    Word     = 4,
};

enum class DMAMode : uint8_t {
    Normal   = 0,
    Circular = 1,
};

enum class DMAPriority : uint8_t {
    Low    = 0,
    Medium = 1,
    High   = 2,
    VeryHigh = 3,
};

struct DMAStreamConfig {
    int stream_num = 0;
    int channel = 0;
    DMADirection direction = DMADirection::PeriphToMemory;
    DMADataWidth periph_size = DMADataWidth::Byte;
    DMADataWidth mem_size = DMADataWidth::Byte;
    DMAMode mode = DMAMode::Normal;
    DMAPriority priority = DMAPriority::Medium;
    bool mem_increment = true;
    bool periph_increment = false;
    uint32_t data_count = 0;
};

class DMAStream {
    DMAStreamConfig config_;
    std::vector<uint8_t> source_;
    std::vector<uint8_t> destination_;
    uint32_t transfer_index_ = 0;
    bool enabled_ = false;
    bool complete_ = false;
    bool half_complete_ = false;

    // EN: Callbacks for interrupts
    std::function<void()> on_complete_;
    std::function<void()> on_half_complete_;
    std::function<void()> on_error_;

public:
    void configure(DMAStreamConfig cfg) {
        config_ = std::move(cfg);
        destination_.resize(config_.data_count, 0);
        transfer_index_ = 0;
        complete_ = false;
        half_complete_ = false;
    }

    void set_source(std::vector<uint8_t> data) { source_ = std::move(data); }

    void set_callbacks(std::function<void()> on_complete,
                        std::function<void()> on_half = nullptr,
                        std::function<void()> on_error = nullptr) {
        on_complete_ = std::move(on_complete);
        on_half_complete_ = std::move(on_half);
        on_error_ = std::move(on_error);
    }

    void enable() { enabled_ = true; complete_ = false; transfer_index_ = 0; }
    void disable() { enabled_ = false; }

    // EN: Simulate one DMA transfer beat
    bool transfer_beat() {
        if (!enabled_ || complete_) return false;
        if (transfer_index_ >= config_.data_count) return false;

        if (transfer_index_ < source_.size()) {
            destination_[transfer_index_] = source_[transfer_index_];
        }
        transfer_index_++;

        // EN: Half-transfer interrupt
        if (!half_complete_ && transfer_index_ >= config_.data_count / 2) {
            half_complete_ = true;
            if (on_half_complete_) on_half_complete_();
        }

        // EN: Transfer complete
        if (transfer_index_ >= config_.data_count) {
            if (config_.mode == DMAMode::Circular) {
                transfer_index_ = 0;
                half_complete_ = false;
            } else {
                complete_ = true;
                enabled_ = false;
            }
            if (on_complete_) on_complete_();
        }
        return true;
    }

    // EN: Run entire transfer
    void run_all() {
        while (enabled_ && !complete_) {
            transfer_beat();
        }
    }

    const std::vector<uint8_t>& destination() const { return destination_; }
    bool is_complete() const { return complete_; }
    uint32_t remaining() const { return config_.data_count - transfer_index_; }

    void print_status() const {
        std::string dir_str;
        switch (config_.direction) {
            case DMADirection::PeriphToMemory: dir_str = "P→M"; break;
            case DMADirection::MemoryToPerph:  dir_str = "M→P"; break;
            case DMADirection::MemoryToMemory: dir_str = "M→M"; break;
        }
        std::cout << "    DMA Stream " << config_.stream_num
                  << " [Ch" << config_.channel << "] "
                  << dir_str << " "
                  << transfer_index_ << "/" << config_.data_count
                  << (enabled_ ? " RUNNING" : " STOPPED")
                  << (complete_ ? " COMPLETE" : "")
                  << (config_.mode == DMAMode::Circular ? " CIRCULAR" : "") << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Double-Buffer DMA
// ═════════════════════════════════════════════════════════════════════════════

// EN: Double-buffering allows zero-copy streaming: while DMA fills buffer A,
//     CPU processes buffer B, then they swap.
class DoubleBufferDMA {
    std::vector<uint8_t> buffer_a_;
    std::vector<uint8_t> buffer_b_;
    int active_buffer_ = 0;  // 0=A, 1=B
    uint32_t buffer_size_;
    int swap_count_ = 0;

public:
    explicit DoubleBufferDMA(uint32_t size)
        : buffer_a_(size, 0), buffer_b_(size, 0), buffer_size_(size) {}

    // EN: Get the buffer DMA is currently filling
    std::vector<uint8_t>& dma_buffer() {
        return active_buffer_ == 0 ? buffer_a_ : buffer_b_;
    }

    // EN: Get the buffer CPU can safely process
    const std::vector<uint8_t>& cpu_buffer() const {
        return active_buffer_ == 0 ? buffer_b_ : buffer_a_;
    }

    // EN: Swap buffers (called on DMA TC interrupt)
    void swap() {
        active_buffer_ = 1 - active_buffer_;
        swap_count_++;
    }

    int swap_count() const { return swap_count_; }
    uint32_t size() const { return buffer_size_; }

    void print_status() const {
        std::cout << "    Double-Buffer: DMA→Buf" << (active_buffer_ == 0 ? 'A' : 'B')
                  << " | CPU←Buf" << (active_buffer_ == 0 ? 'B' : 'A')
                  << " | Swaps: " << swap_count_ << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: ISR Best Practices Model
// ═════════════════════════════════════════════════════════════════════════════

// EN: Demonstrates the pattern of keeping ISR short and deferring work
class ISRWorkQueue {
    struct WorkItem {
        std::string description;
        std::function<void()> handler;
    };

    std::queue<WorkItem> deferred_queue_;
    int isr_calls_ = 0;
    int deferred_calls_ = 0;

public:
    // EN: Called from ISR context — MUST be short!
    void isr_handler(const std::string& source, uint8_t data,
                      std::function<void(uint8_t)> process) {
        isr_calls_++;
        // EN: Only critical work in ISR:
        // 1. Clear interrupt flag (simulated)
        // 2. Read hardware data
        // 3. Defer processing

        deferred_queue_.push({
            source + " data=0x" + std::to_string(data),
            [process, data]() { process(data); }
        });
    }

    // EN: Called from main loop — processes deferred work
    void process_deferred() {
        while (!deferred_queue_.empty()) {
            auto item = std::move(deferred_queue_.front());
            deferred_queue_.pop();
            deferred_calls_++;
            item.handler();
        }
    }

    bool has_work() const { return !deferred_queue_.empty(); }
    int isr_count() const { return isr_calls_; }
    int deferred_count() const { return deferred_calls_; }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 15 - DMA & Interrupt Handling\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: NVIC Priority & Execution Order ─────────────────────────
    {
        std::cout << "--- Demo 1: NVIC Priority & Execution Order ---\n\n";

        NVICSimulator nvic;

        nvic.configure_irq(0, "TIM2_IRQHandler", 5,  []{ std::cout << "    → TIM2: 1ms tick\n"; });
        nvic.configure_irq(1, "EXTI0_IRQHandler", 2, []{ std::cout << "    → EXTI0: button press\n"; });
        nvic.configure_irq(2, "DMA1_S0_IRQHandler", 1, []{ std::cout << "    → DMA1: transfer complete\n"; });
        nvic.configure_irq(3, "USART2_IRQHandler", 8, []{ std::cout << "    → USART2: byte received\n"; });
        nvic.configure_irq(4, "CAN1_RX0_IRQHandler", 0, []{ std::cout << "    → CAN1: frame received\n"; });

        for (int i = 0; i < 5; ++i) nvic.enable_irq(i);

        nvic.print_config();
        std::cout << "\n  Triggering all interrupts simultaneously:\n\n";

        // EN: All interrupt at once — NVIC processes by priority
        for (int i = 0; i < 5; ++i) nvic.set_pending(i);
        nvic.process_pending();

        std::cout << "\n";
        nvic.print_log();
        std::cout << "\n";
    }

    // ─── Demo 2: Interrupt Preemption ────────────────────────────────────
    {
        std::cout << "--- Demo 2: Interrupt Preemption ---\n\n";

        NVICSimulator nvic;

        // EN: Low-priority ISR triggers high-priority interrupt
        nvic.configure_irq(10, "SPI1_IRQHandler", 6,
            [&nvic]{
                std::cout << "    → SPI1: processing (pri=6)...\n";
                // EN: While processing SPI, CAN interrupt arrives
                nvic.set_pending(11);
                nvic.process_pending();
                std::cout << "    → SPI1: resumed after preemption\n";
            });
        nvic.configure_irq(11, "CAN1_RX_IRQHandler", 2,
            []{ std::cout << "    → CAN1: URGENT frame (pri=2) 🚨\n"; });

        nvic.enable_irq(10);
        nvic.enable_irq(11);

        std::cout << "  SPI (pri=6) starts, then CAN (pri=2) preempts:\n\n";
        nvic.set_pending(10);
        nvic.process_pending();

        std::cout << "\n";
        nvic.print_log();
        std::cout << "  Preemptions: " << nvic.preemption_count() << "\n\n";
    }

    // ─── Demo 3: DMA Normal Transfer ─────────────────────────────────────
    {
        std::cout << "--- Demo 3: DMA Normal Transfer (UART RX) ---\n\n";

        DMAStream dma;
        DMAStreamConfig cfg;
        cfg.stream_num = 5;
        cfg.channel = 4;
        cfg.direction = DMADirection::PeriphToMemory;
        cfg.mode = DMAMode::Normal;
        cfg.data_count = 12;
        cfg.priority = DMAPriority::High;
        dma.configure(cfg);

        // EN: Simulate UART data arriving
        std::vector<uint8_t> uart_data = {'H','E','L','L','O',' ','W','O','R','L','D','!'};
        dma.set_source(uart_data);

        bool got_half = false, got_complete = false;
        dma.set_callbacks(
            [&got_complete]{ got_complete = true; },
            [&got_half]{ got_half = true; }
        );

        dma.enable();
        dma.print_status();

        dma.run_all();
        dma.print_status();

        std::cout << "    Half-transfer IRQ: " << (got_half ? "YES" : "NO") << "\n";
        std::cout << "    Complete IRQ:      " << (got_complete ? "YES" : "NO") << "\n";

        std::cout << "    Received: \"";
        for (auto b : dma.destination()) std::cout << static_cast<char>(b);
        std::cout << "\"\n\n";
    }

    // ─── Demo 4: DMA Circular Mode (ADC Continuous) ──────────────────────
    {
        std::cout << "--- Demo 4: DMA Circular Mode (ADC Continuous) ---\n\n";

        std::cout << "  Scenario: ADC reads 4 channels continuously via DMA\n";
        std::cout << "  DMA buffer is continuously overwritten (ring buffer)\n\n";

        DMAStream dma;
        DMAStreamConfig cfg;
        cfg.stream_num = 0;
        cfg.channel = 0;
        cfg.direction = DMADirection::PeriphToMemory;
        cfg.mode = DMAMode::Circular;
        cfg.data_count = 4;
        cfg.priority = DMAPriority::High;
        dma.configure(cfg);

        int tc_count = 0;
        dma.set_callbacks([&tc_count]{ tc_count++; });

        // EN: Simulate 3 complete rounds of ADC conversion
        std::vector<std::vector<uint8_t>> rounds = {
            {0x12, 0x34, 0x56, 0x78},  // round 1
            {0xA0, 0xB0, 0xC0, 0xD0},  // round 2
            {0x11, 0x22, 0x33, 0x44},  // round 3
        };

        dma.enable();
        for (int round = 0; round < 3; ++round) {
            dma.set_source(rounds[static_cast<size_t>(round)]);
            for (int i = 0; i < 4; ++i) dma.transfer_beat();

            std::cout << "    Round " << round + 1 << " buffer: ";
            for (auto b : dma.destination()) {
                std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(b) << " ";
            }
            std::cout << std::dec << std::setfill(' ') << "\n";
        }
        std::cout << "    TC interrupts fired: " << tc_count << "\n\n";
    }

    // ─── Demo 5: Double-Buffer DMA ───────────────────────────────────────
    {
        std::cout << "--- Demo 5: Double-Buffer DMA (Zero-Copy Audio) ---\n\n";

        std::cout << "  Scenario: Audio DAC playing from memory via DMA\n";
        std::cout << "  While DMA sends buffer A, CPU fills buffer B\n\n";

        DoubleBufferDMA db(8);

        std::cout << "  ┌────────┬─────────────────┬─────────────────┐\n";
        std::cout << "  │ Cycle  │ DMA sending     │ CPU filling     │\n";
        std::cout << "  ├────────┼─────────────────┼─────────────────┤\n";

        for (int cycle = 0; cycle < 4; ++cycle) {
            // EN: CPU fills its accessible buffer while DMA sends the other
            // cpu_buffer() is safe for CPU, dma_buffer() is for DMA

            std::cout << "  │ " << std::setw(3) << cycle << "    │    Buf "
                      << (db.swap_count() % 2 == 0 ? 'A' : 'B')
                      << "        │    Buf "
                      << (db.swap_count() % 2 == 0 ? 'B' : 'A')
                      << "        │\n";

            // EN: DMA finishes → swap
            db.swap();
        }
        std::cout << "  └────────┴─────────────────┴─────────────────┘\n";
        db.print_status();
        std::cout << "\n";
    }

    // ─── Demo 6: ISR Best Practices ──────────────────────────────────────
    {
        std::cout << "--- Demo 6: ISR Best Practices (Deferred Processing) ---\n\n";

        ISRWorkQueue wq;

        std::cout << "  Pattern: ISR does minimal work, defers to main loop.\n\n";

        // EN: Simulate interrupts arriving
        wq.isr_handler("UART_RX", 0x41, [](uint8_t d){
            std::cout << "    [Main] Process UART byte: '" << static_cast<char>(d) << "'\n";
        });
        wq.isr_handler("CAN_RX", 0x55, [](uint8_t d){
            std::cout << "    [Main] Process CAN frame ID: 0x" << std::hex
                      << static_cast<int>(d) << std::dec << "\n";
        });
        wq.isr_handler("ADC_EOC", 0xAB, [](uint8_t d){
            std::cout << "    [Main] Process ADC value: " << static_cast<int>(d) << "\n";
        });

        std::cout << "  ISR calls: " << wq.isr_count() << " | Pending work: "
                  << wq.has_work() << "\n\n";

        // EN: Main loop processes deferred work
        std::cout << "  Main loop processing:\n";
        wq.process_deferred();

        std::cout << "\n  Deferred calls processed: " << wq.deferred_count() << "\n\n";

        std::cout << "  ISR Rules:\n";
        std::cout << "    ✓ Clear interrupt flag first\n";
        std::cout << "    ✓ Read hardware registers only\n";
        std::cout << "    ✓ Set a flag or enqueue data\n";
        std::cout << "    ✓ Keep execution < 10 μs\n";
        std::cout << "    ✗ No printf/cout in real ISR!\n";
        std::cout << "    ✗ No malloc/new in ISR!\n";
        std::cout << "    ✗ No mutex/semaphore wait in ISR!\n";
        std::cout << "    ✗ No floating-point in ISR (unless FPU context saved)!\n\n";
    }

    // ─── Demo 7: DMA Channel Mapping ─────────────────────────────────────
    {
        std::cout << "--- Demo 7: STM32 DMA Channel Mapping ---\n\n";

        std::cout << "  DMA1 Stream Assignments (STM32F4):\n";
        std::cout << "  ┌────────┬──────┬─────────────────────────────────┐\n";
        std::cout << "  │ Stream │ Ch   │ Peripheral                      │\n";
        std::cout << "  ├────────┼──────┼─────────────────────────────────┤\n";
        std::cout << "  │ S0     │ Ch0  │ SPI3_RX                         │\n";
        std::cout << "  │ S1     │ Ch3  │ TIM2_UP / TIM2_CH3              │\n";
        std::cout << "  │ S2     │ Ch3  │ I2C3_RX                         │\n";
        std::cout << "  │ S3     │ Ch0  │ SPI2_RX                         │\n";
        std::cout << "  │ S4     │ Ch0  │ SPI2_TX                         │\n";
        std::cout << "  │ S5     │ Ch4  │ USART2_RX                       │\n";
        std::cout << "  │ S6     │ Ch4  │ USART2_TX                       │\n";
        std::cout << "  │ S7     │ Ch0  │ SPI3_TX                         │\n";
        std::cout << "  └────────┴──────┴─────────────────────────────────┘\n\n";

        std::cout << "  DMA2 Stream Assignments:\n";
        std::cout << "  ┌────────┬──────┬─────────────────────────────────┐\n";
        std::cout << "  │ Stream │ Ch   │ Peripheral                      │\n";
        std::cout << "  ├────────┼──────┼─────────────────────────────────┤\n";
        std::cout << "  │ S0     │ Ch0  │ ADC1                            │\n";
        std::cout << "  │ S2     │ Ch3  │ SPI1_RX                         │\n";
        std::cout << "  │ S3     │ Ch3  │ SPI1_TX                         │\n";
        std::cout << "  │ S5     │ Ch4  │ USART1_RX                       │\n";
        std::cout << "  │ S7     │ Ch4  │ USART1_TX                       │\n";
        std::cout << "  └────────┴──────┴─────────────────────────────────┘\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of DMA & Interrupt Handling\n";
    std::cout << "============================================\n";

    return 0;
}
