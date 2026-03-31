/**
 * @file module_15_hw_interfaces/02_adc_dac.cpp
 * @brief ADC & DAC
 *
 * @details
 * =============================================================================
 * [THEORY: ADC (Analog-to-Digital Converter)]
 * =============================================================================
 *
 * EN: ADC converts continuous analog voltages to discrete digital values.
 *     Key parameters:
 *     - Resolution: number of bits (8, 10, 12, 16, 24-bit)
 *     - Sampling Rate: conversions per second (Samples/s)
 *     - Reference Voltage (Vref): defines full-scale range
 *     - Input Range: 0V to Vref (single-ended) or ±Vref (differential)
 *
 *     ADC Types:
 *     ┌──────────────────┬──────────┬───────────────┬────────────────────────┐
 *     │ Architecture     │ Speed    │ Resolution    │ Use Case               │
 *     ├──────────────────┼──────────┼───────────────┼────────────────────────┤
 *     │ SAR              │ 1-5 MSPS │ 8-18 bit      │ MCU general purpose    │
 *     │ Sigma-Delta (ΣΔ) │ <100 SPS │ 16-24 bit     │ Precision measurement  │
 *     │ Pipeline         │ >100 MSPS│ 8-16 bit      │ Video, radar           │
 *     │ Flash            │ >1 GSPS  │ 4-8 bit       │ Oscilloscope, telecom  │
 *     └──────────────────┴──────────┴───────────────┴────────────────────────┘
 *
 *     Conversion Formula:
 *       Digital_Value = (V_in / V_ref) × (2^n - 1)
 *       V_in = Digital_Value × V_ref / (2^n - 1)
 *
 * =============================================================================
 * [THEORY: DAC (Digital-to-Analog Converter)]
 * =============================================================================
 *
 * EN: DAC converts digital values back to analog voltages.
 *     V_out = Digital_Value × V_ref / (2^n - 1)
 *
 *     Common uses: audio output, waveform generation, control loops
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_adc_dac.cpp -o 02_adc_dac
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <cmath>
#include <array>
#include <vector>
#include <string>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <functional>
#include <random>
#include <cassert>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: ADC Simulator
// ═════════════════════════════════════════════════════════════════════════════

// EN: Simulates a SAR ADC with configurable resolution and Vref

enum class ADCResolution : uint8_t {
    Bit8  = 8,
    Bit10 = 10,
    Bit12 = 12,
    Bit16 = 16,
};

struct ADCConfig {
    ADCResolution resolution = ADCResolution::Bit12;
    double vref = 3.3;              // Reference voltage
    uint32_t sample_rate_hz = 1000000;  // 1 MSPS
    bool differential = false;       // single-ended vs differential

    uint32_t max_value() const {
        return (1U << static_cast<uint32_t>(resolution)) - 1;
    }

    double lsb_voltage() const {
        return vref / static_cast<double>(max_value());
    }
};

class ADCSimulator {
    ADCConfig config_;
    std::mt19937 rng_{42};  // fixed seed for reproducibility
    std::normal_distribution<double> noise_{0.0, 0.5};  // ±0.5 LSB noise

    // EN: Simulated analog input voltages (up to 16 channels)
    std::array<double, 16> channel_voltages_{};

public:
    explicit ADCSimulator(ADCConfig cfg = {}) : config_(std::move(cfg)) {}

    void set_channel_voltage(int ch, double voltage) {
        if (ch >= 0 && ch < 16) {
            channel_voltages_[static_cast<size_t>(ch)] = voltage;
        }
    }

    // EN: Convert analog voltage to digital value
    uint32_t convert(int channel) {
        double v_in = channel_voltages_[static_cast<size_t>(channel)];
        // Clamp to valid range
        v_in = std::clamp(v_in, 0.0, config_.vref);

        double ideal = (v_in / config_.vref) * static_cast<double>(config_.max_value());
        // Add quantization noise
        double noisy = ideal + noise_(rng_);
        noisy = std::clamp(noisy, 0.0, static_cast<double>(config_.max_value()));

        return static_cast<uint32_t>(std::round(noisy));
    }

    // EN: Convert digital value back to voltage
    double to_voltage(uint32_t digital) const {
        return static_cast<double>(digital) * config_.vref
               / static_cast<double>(config_.max_value());
    }

    // EN: Multi-sample averaging (oversampling for noise reduction)
    uint32_t convert_averaged(int channel, int samples) {
        uint64_t sum = 0;
        for (int i = 0; i < samples; ++i) {
            sum += convert(channel);
        }
        return static_cast<uint32_t>(sum / static_cast<uint64_t>(samples));
    }

    const ADCConfig& config() const { return config_; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: DAC Simulator
// ═════════════════════════════════════════════════════════════════════════════

struct DACConfig {
    uint8_t resolution_bits = 12;
    double vref = 3.3;

    uint32_t max_value() const {
        return (1U << resolution_bits) - 1;
    }
};

class DACSimulator {
    DACConfig config_;
    uint32_t current_value_ = 0;

public:
    explicit DACSimulator(DACConfig cfg = {}) : config_(std::move(cfg)) {}

    void write(uint32_t value) {
        current_value_ = std::min(value, config_.max_value());
    }

    double output_voltage() const {
        return static_cast<double>(current_value_) * config_.vref
               / static_cast<double>(config_.max_value());
    }

    uint32_t current_value() const { return current_value_; }
    const DACConfig& config() const { return config_; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Automotive Sensor Models
// ═════════════════════════════════════════════════════════════════════════════

// EN: Common automotive sensor transfer functions

struct SensorCalibration {
    std::string name;
    std::string unit;
    double v_min;       // voltage at physical minimum
    double v_max;       // voltage at physical maximum
    double phys_min;    // physical minimum
    double phys_max;    // physical maximum

    // EN: Linear transfer function: V → Physical
    double voltage_to_physical(double voltage) const {
        if (std::abs(v_max - v_min) < 1e-9) return phys_min;
        return phys_min + (voltage - v_min) / (v_max - v_min) * (phys_max - phys_min);
    }

    // EN: Physical → Voltage
    double physical_to_voltage(double physical) const {
        if (std::abs(phys_max - phys_min) < 1e-9) return v_min;
        return v_min + (physical - phys_min) / (phys_max - phys_min) * (v_max - v_min);
    }

    // EN: Check if voltage is in valid range (open/short circuit detection)
    bool is_valid(double voltage) const {
        constexpr double margin = 0.1;  // 100 mV margin
        return voltage >= (v_min - margin) && voltage <= (v_max + margin);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Waveform Generator (DAC Application)
// ═════════════════════════════════════════════════════════════════════════════

class WaveformGenerator {
    DACSimulator& dac_;

public:
    explicit WaveformGenerator(DACSimulator& dac) : dac_(dac) {}

    // EN: Generate one period of waveform, return voltage samples
    std::vector<double> generate_sine(int samples_per_period, double amplitude_v,
                                       double offset_v) {
        std::vector<double> result;
        result.reserve(static_cast<size_t>(samples_per_period));
        for (int i = 0; i < samples_per_period; ++i) {
            double t = static_cast<double>(i) / static_cast<double>(samples_per_period);
            double v = offset_v + amplitude_v * std::sin(2.0 * M_PI * t);
            v = std::clamp(v, 0.0, dac_.config().vref);
            uint32_t digital = static_cast<uint32_t>(
                std::round(v / dac_.config().vref * static_cast<double>(dac_.config().max_value())));
            dac_.write(digital);
            result.push_back(dac_.output_voltage());
        }
        return result;
    }

    std::vector<double> generate_sawtooth(int samples_per_period, double max_v) {
        std::vector<double> result;
        result.reserve(static_cast<size_t>(samples_per_period));
        for (int i = 0; i < samples_per_period; ++i) {
            double v = max_v * static_cast<double>(i) / static_cast<double>(samples_per_period);
            uint32_t digital = static_cast<uint32_t>(
                std::round(v / dac_.config().vref * static_cast<double>(dac_.config().max_value())));
            dac_.write(digital);
            result.push_back(dac_.output_voltage());
        }
        return result;
    }

    std::vector<double> generate_triangle(int samples_per_period, double max_v) {
        std::vector<double> result;
        result.reserve(static_cast<size_t>(samples_per_period));
        int half = samples_per_period / 2;
        for (int i = 0; i < samples_per_period; ++i) {
            double v;
            if (i < half)
                v = max_v * static_cast<double>(i) / static_cast<double>(half);
            else
                v = max_v * static_cast<double>(samples_per_period - i) / static_cast<double>(half);
            uint32_t digital = static_cast<uint32_t>(
                std::round(v / dac_.config().vref * static_cast<double>(dac_.config().max_value())));
            dac_.write(digital);
            result.push_back(dac_.output_voltage());
        }
        return result;
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Oversampling & Averaging Filter
// ═════════════════════════════════════════════════════════════════════════════

// EN: Oversampling effectively increases ADC resolution.
//     Extra bits = log2(oversampling_ratio) / 2
//     4x oversampling = +1 bit, 16x = +2 bits, 64x = +3 bits, 256x = +4 bits

struct OversamplingResult {
    uint32_t raw_avg;
    double effective_resolution_bits;
    double voltage;
    int num_samples;
};

OversamplingResult oversample(ADCSimulator& adc, int channel, int ratio) {
    uint64_t sum = 0;
    for (int i = 0; i < ratio; ++i) {
        sum += adc.convert(channel);
    }
    uint32_t avg = static_cast<uint32_t>(sum / static_cast<uint64_t>(ratio));
    double extra_bits = std::log2(static_cast<double>(ratio)) / 2.0;
    double eff_res = static_cast<double>(static_cast<uint32_t>(adc.config().resolution)) + extra_bits;

    return {avg, eff_res, adc.to_voltage(avg), ratio};
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 15 - ADC & DAC Programming\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: ADC Resolution Comparison ──────────────────────────────
    {
        std::cout << "--- Demo 1: ADC Resolution Comparison ---\n\n";
        std::cout << "  Input voltage: 1.65V (Vref=3.3V, mid-scale)\n\n";

        std::cout << "  ┌────────────┬────────────┬────────────────┬──────────────┐\n";
        std::cout << "  │ Resolution │ Max Value  │ Digital Output │ LSB Voltage  │\n";
        std::cout << "  ├────────────┼────────────┼────────────────┼──────────────┤\n";

        for (auto res : {ADCResolution::Bit8, ADCResolution::Bit10,
                         ADCResolution::Bit12, ADCResolution::Bit16}) {
            ADCConfig cfg{res, 3.3, 1000000, false};
            ADCSimulator adc(cfg);
            adc.set_channel_voltage(0, 1.65);
            uint32_t raw = adc.convert(0);
            double lsb = cfg.lsb_voltage();

            std::cout << "  │ " << std::setw(6) << static_cast<int>(res) << "-bit"
                      << " │ " << std::setw(10) << cfg.max_value()
                      << " │ " << std::setw(14) << raw
                      << " │ " << std::fixed << std::setprecision(6)
                      << std::setw(9) << (lsb * 1000.0) << " mV │\n";
        }
        std::cout << "  └────────────┴────────────┴────────────────┴──────────────┘\n\n";
    }

    // ─── Demo 2: Automotive Sensor Reading ──────────────────────────────
    {
        std::cout << "--- Demo 2: Automotive Sensor Reading ---\n\n";

        ADCSimulator adc(ADCConfig{ADCResolution::Bit12, 3.3});

        // EN: Define automotive sensors
        std::vector<SensorCalibration> sensors = {
            {"Coolant Temp (NTC)",  "°C",   0.5, 2.8,  -40.0, 150.0},
            {"Throttle Position",   "%",    0.5, 4.5,    0.0, 100.0},
            {"MAP Sensor",          "kPa",  0.4, 4.65,  20.0, 250.0},
            {"O2 Sensor (Lambda)",  "V",    0.1, 0.9,    0.1,   0.9},
            {"Battery Voltage",     "V",    0.0, 3.3,    0.0,  16.5},
            {"Fuel Level",          "%",    0.3, 3.0,    0.0, 100.0},
        };

        // EN: Set simulated sensor voltages
        double test_voltages[] = {1.8, 2.5, 2.0, 0.45, 2.8, 1.5};

        std::cout << "  ┌───────────────────────┬─────────┬─────────┬───────────┬────────┐\n";
        std::cout << "  │ Sensor                │ V_in    │ Raw ADC │ V_read    │ Phys   │\n";
        std::cout << "  ├───────────────────────┼─────────┼─────────┼───────────┼────────┤\n";

        for (size_t i = 0; i < sensors.size(); ++i) {
            int ch = static_cast<int>(i);
            adc.set_channel_voltage(ch, test_voltages[i]);
            uint32_t raw = adc.convert_averaged(ch, 16);
            double v_read = adc.to_voltage(raw);
            double phys = sensors[i].voltage_to_physical(v_read);
            bool valid = sensors[i].is_valid(v_read);

            std::cout << "  │ " << std::left << std::setw(21) << sensors[i].name
                      << std::right
                      << " │ " << std::fixed << std::setprecision(3)
                      << std::setw(5) << test_voltages[i] << " V"
                      << " │ " << std::setw(7) << raw
                      << " │ " << std::setprecision(3) << std::setw(7) << v_read << " V"
                      << " │ " << std::setprecision(1) << std::setw(5) << phys
                      << (valid ? " " : "!") << " │\n";
        }
        std::cout << "  └───────────────────────┴─────────┴─────────┴───────────┴────────┘\n";
        std::cout << "  (! = out-of-range / sensor fault)\n\n";
    }

    // ─── Demo 3: Oversampling for Resolution ────────────────────────────
    {
        std::cout << "--- Demo 3: Oversampling for Higher Resolution ---\n\n";

        ADCSimulator adc(ADCConfig{ADCResolution::Bit12, 3.3});
        adc.set_channel_voltage(0, 1.234);  // precise voltage

        std::cout << "  Target voltage: 1.234V (12-bit ADC, Vref=3.3V)\n\n";
        std::cout << "  ┌──────────────────┬───────────────┬────────────┬───────────┐\n";
        std::cout << "  │ Oversampling     │ Eff. Res.     │ Raw Avg    │ Voltage   │\n";
        std::cout << "  ├──────────────────┼───────────────┼────────────┼───────────┤\n";

        for (int ratio : {1, 4, 16, 64, 256}) {
            auto result = oversample(adc, 0, ratio);
            std::cout << "  │ " << std::setw(7) << ratio << "× samples"
                      << " │ " << std::fixed << std::setprecision(1)
                      << std::setw(8) << result.effective_resolution_bits << " bits"
                      << " │ " << std::setw(10) << result.raw_avg
                      << " │ " << std::setprecision(4)
                      << std::setw(7) << result.voltage << " V │\n";
        }
        std::cout << "  └──────────────────┴───────────────┴────────────┴───────────┘\n\n";
    }

    // ─── Demo 4: DAC Basic Operation ────────────────────────────────────
    {
        std::cout << "--- Demo 4: DAC Basic Operation ---\n\n";

        DACSimulator dac(DACConfig{12, 3.3});

        std::cout << "  12-bit DAC, Vref=3.3V:\n\n";

        struct TestCase { uint32_t value; std::string label; };
        std::vector<TestCase> tests = {
            {0,    "Zero"},
            {2048, "Mid-scale"},
            {4095, "Full-scale"},
            {1000, "~0.806V"},
            {3000, "~2.418V"},
        };

        std::cout << "  ┌──────────────┬─────────────┬──────────────┐\n";
        std::cout << "  │ Digital      │ Voltage     │ Label        │\n";
        std::cout << "  ├──────────────┼─────────────┼──────────────┤\n";
        for (auto& t : tests) {
            dac.write(t.value);
            std::cout << "  │ " << std::setw(5) << t.value << " / 4095"
                      << " │ " << std::fixed << std::setprecision(3)
                      << std::setw(7) << dac.output_voltage() << " V"
                      << "   │ " << std::left << std::setw(12) << t.label
                      << std::right << " │\n";
        }
        std::cout << "  └──────────────┴─────────────┴──────────────┘\n\n";
    }

    // ─── Demo 5: DAC Waveform Generation ────────────────────────────────
    {
        std::cout << "--- Demo 5: DAC Waveform Generation ---\n\n";

        DACSimulator dac(DACConfig{12, 3.3});
        WaveformGenerator wg(dac);

        // EN: Generate sine wave (1 period, 20 samples)
        auto sine = wg.generate_sine(20, 1.5, 1.65);

        std::cout << "  Sine wave (Amp=1.5V, Offset=1.65V):\n  ";
        // EN: ASCII art waveform
        for (size_t i = 0; i < sine.size(); ++i) {
            int col = static_cast<int>(sine[i] / 3.3 * 30.0);
            for (int j = 0; j < col; ++j) std::cout << ' ';
            std::cout << '*' << "\n  ";
        }
        std::cout << "\n";

        // EN: Triangle wave
        auto tri = wg.generate_triangle(20, 3.0);
        std::cout << "  Triangle wave (max=3.0V):\n  ";
        for (size_t i = 0; i < tri.size(); ++i) {
            int col = static_cast<int>(tri[i] / 3.3 * 30.0);
            for (int j = 0; j < col; ++j) std::cout << ' ';
            std::cout << '*' << "\n  ";
        }
        std::cout << "\n";
    }

    // ─── Demo 6: Sensor Fault Detection ─────────────────────────────────
    {
        std::cout << "--- Demo 6: Sensor Fault Detection (Open/Short Circuit) ---\n\n";

        SensorCalibration throttle{"Throttle Position", "%", 0.5, 4.5, 0.0, 100.0};

        struct FaultTest {
            double voltage;
            std::string scenario;
        };

        std::vector<FaultTest> tests = {
            {2.5,  "Normal (50% throttle)"},
            {0.0,  "SHORT to GND"},
            {3.3,  "SHORT to Vcc (3.3V)"},
            {0.3,  "Below valid range (wire damage?)"},
            {0.5,  "At min boundary"},
        };

        std::cout << "  ┌─────────┬───────────────────────────────────┬────────────┬────────┐\n";
        std::cout << "  │ V_in    │ Scenario                          │ Physical   │ Status │\n";
        std::cout << "  ├─────────┼───────────────────────────────────┼────────────┼────────┤\n";
        for (auto& t : tests) {
            double phys = throttle.voltage_to_physical(t.voltage);
            bool valid = throttle.is_valid(t.voltage);
            std::cout << "  │ " << std::fixed << std::setprecision(1)
                      << std::setw(5) << t.voltage << " V"
                      << " │ " << std::left << std::setw(33) << t.scenario << std::right
                      << " │ " << std::setprecision(1) << std::setw(7) << phys << " %"
                      << "  │ " << std::setw(6) << (valid ? "OK" : "FAULT") << " │\n";
        }
        std::cout << "  └─────────┴───────────────────────────────────┴────────────┴────────┘\n\n";
    }

    // ─── Demo 7: ADC+DAC Loopback ───────────────────────────────────────
    {
        std::cout << "--- Demo 7: ADC+DAC Loopback (Closed-Loop Control) ---\n\n";

        std::cout << "  Scenario: PID-like control using ADC to read sensor,\n";
        std::cout << "            DAC to drive actuator.\n\n";

        ADCSimulator adc(ADCConfig{ADCResolution::Bit12, 3.3});
        DACSimulator dac(DACConfig{12, 3.3});

        double target_temp = 80.0;  // °C target
        SensorCalibration temp_sensor{"Coolant Temp", "°C", 0.5, 2.8, -40.0, 150.0};

        // EN: Simulate simple proportional control
        double current_temp = 60.0;  // starting temperature
        double kp = 0.05;            // proportional gain

        std::cout << "  Target: " << target_temp << " °C\n\n";
        std::cout << "  ┌──────┬──────────┬─────────┬──────────┬───────────┬──────────┐\n";
        std::cout << "  │ Step │ Temp(°C) │ ADC Raw │ Error    │ DAC Out   │ Actuator │\n";
        std::cout << "  ├──────┼──────────┼─────────┼──────────┼───────────┼──────────┤\n";

        for (int step = 0; step < 8; ++step) {
            // Sensor voltage from current temperature
            double v_sensor = temp_sensor.physical_to_voltage(current_temp);
            adc.set_channel_voltage(0, v_sensor);
            uint32_t raw = adc.convert_averaged(0, 4);
            double v_read = adc.to_voltage(raw);
            double measured_temp = temp_sensor.voltage_to_physical(v_read);

            double error = target_temp - measured_temp;
            // DAC output = proportional to error (0V=off, 3.3V=full power)
            double dac_v = std::clamp(error * kp, 0.0, 3.3);
            uint32_t dac_val = static_cast<uint32_t>(
                std::round(dac_v / 3.3 * 4095.0));
            dac.write(dac_val);

            std::cout << "  │ " << std::setw(4) << step
                      << " │ " << std::fixed << std::setprecision(1)
                      << std::setw(6) << current_temp << " °"
                      << " │ " << std::setw(7) << raw
                      << " │ " << std::setprecision(1)
                      << std::setw(6) << error << " °"
                      << " │ " << std::setprecision(3)
                      << std::setw(7) << dac.output_voltage() << " V"
                      << " │ " << std::setprecision(0)
                      << std::setw(5) << (dac.output_voltage()/3.3*100.0) << " %"
                      << "  │\n";

            // EN: Simulate temperature rising toward target
            current_temp += error * 0.3;
        }
        std::cout << "  └──────┴──────────┴─────────┴──────────┴───────────┴──────────┘\n\n";
    }

    // ─── Demo 8: ADC Architecture Comparison ────────────────────────────
    {
        std::cout << "--- Demo 8: ADC Architecture Comparison ---\n\n";

        std::cout << "  ┌──────────────────┬──────────┬──────────┬──────────┬───────────────────────┐\n";
        std::cout << "  │ Architecture     │ Speed    │ Res.     │ Power    │ Typical Application   │\n";
        std::cout << "  ├──────────────────┼──────────┼──────────┼──────────┼───────────────────────┤\n";
        std::cout << "  │ SAR              │ 1M SPS   │ 12-18b   │ Low      │ MCU (STM32, NXP)      │\n";
        std::cout << "  │ Sigma-Delta (ΣΔ) │ 10 SPS   │ 24b      │ Low      │ Weigh scales, temp    │\n";
        std::cout << "  │ Pipeline         │ 100M SPS │ 12-16b   │ High     │ Radar, video, SDR     │\n";
        std::cout << "  │ Flash            │ 1G SPS   │ 4-8b     │ V.High   │ Oscilloscope, telecom │\n";
        std::cout << "  │ Dual-slope       │ <10 SPS  │ 14-20b   │ V.Low    │ DMM, precision meters │\n";
        std::cout << "  └──────────────────┴──────────┴──────────┴──────────┴───────────────────────┘\n\n";

        std::cout << "  Automotive ADC Selection:\n";
        std::cout << "    - Engine control (MAP, TPS): 12-bit SAR, ~1 MSPS\n";
        std::cout << "    - Battery management (cell V): 16-bit ΣΔ, precise\n";
        std::cout << "    - Radar front-end: 12-14 bit pipeline, 50+ MSPS\n";
        std::cout << "    - Knock sensor: 10-bit SAR, fast sampling for vibration\n";
        std::cout << "    - Interior temp/humidity: 10-bit SAR, low-power\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of ADC & DAC Programming\n";
    std::cout << "============================================\n";

    return 0;
}
