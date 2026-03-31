/**
 * @file module_02_oop/01_classes_objects.cpp
 * @brief Module 02 - Classes & Objects: Fundamental building blocks of C++ OOP
 *
 * @details
 * =============================================================================
 * [THEORY: Class vs Object]
 * =============================================================================
 * EN: A CLASS is a blueprint/template that defines data (member variables) and
 *     behavior (member functions). An OBJECT is a concrete instance of that
 *     class living in memory with its own copy of member variables.
 *     Think: Class = Cookie cutter, Object = Cookie
 *
 * =============================================================================
 * [THEORY: Access Specifiers]
 * =============================================================================
 * EN: C++ has 3 access levels:
 *     • private:   Only accessible inside the class itself (DEFAULT for class)
 *     • protected: Accessible inside class + derived classes (inheritance)
 *     • public:    Accessible from anywhere
 *     Rule of thumb: Data → private, Interface → public
 *
 * =============================================================================
 * [THEORY: Constructor & Destructor]
 * =============================================================================
 * EN: Constructor = special function called automatically when object is created.
 *     - Same name as class, no return type
 *     - Can be overloaded (multiple constructors with different parameters)
 *     - Member Initializer List (MIL) is preferred over body assignment
 *       because MIL directly constructs members, body assigns AFTER default construction
 *     Destructor = called automatically when object goes out of scope.
 *     - Name: ~ClassName(), no parameters, no return type, CANNOT be overloaded
 *     - LIFO order: last constructed = first destroyed
 *
 * =============================================================================
 * [THEORY: struct vs class]
 * =============================================================================
 * EN: In C++, struct and class are almost identical. The ONLY difference:
 *     • struct: members are PUBLIC by default
 *     • class:  members are PRIVATE by default
 *     Convention: struct for POD (Plain Old Data) with no invariants,
 *                 class for objects with encapsulation and behavior.
 *
 * =============================================================================
 * [THEORY: this Pointer]
 * =============================================================================
 * EN: Every non-static member function receives a hidden pointer called `this`
 *     that points to the object on which the function was called.
 *     this->member is implicit; you write it explicitly when:
 *     (1) Parameter name shadows member name
 *     (2) Returning *this for method chaining
 *     (3) Passing the current object to another function
 *
 * =============================================================================
 * [THEORY: const Member Functions]
 * =============================================================================
 * EN: A member function marked `const` promises NOT to modify any member variable.
 *     - The `this` pointer becomes `const ClassName*` inside that function
 *     - const objects can ONLY call const member functions
 *     - Getters should ALWAYS be const
 *     - A const function calling a non-const function = COMPILE ERROR
 *
 * @see https://en.cppreference.com/w/cpp/language/classes
 * @see https://en.cppreference.com/w/cpp/language/constructor
 * @see https://en.cppreference.com/w/cpp/language/destructor
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_classes_objects.cpp -o 01_classes_objects
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// GLOBAL: Automotive-themed class
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class TemperatureSensor
 * @brief EN: Represents an automotive temperature sensor with encapsulation
 *
 * EN: This class demonstrates:
 *     - private data members (encapsulation)
 *     - constructor with Member Initializer List
 *     - destructor with cleanup message
 *     - const getter functions
 *     - setter with input validation
 *     - behavior method that operates on internal state
 *     - `this` pointer usage
 */
class TemperatureSensor {
private:
    // EN: Data is PRIVATE → cannot be accessed from outside the class
    std::string sensorId_;      // EN: Unique sensor identifier
    double currentTempC_;       // EN: Current temperature in Celsius
    double minThreshold_;       // EN: Minimum threshold (alert if below)
    double maxThreshold_;       // EN: Maximum threshold (alert if above)
    int readingCount_;          // EN: How many readings taken

public:
    // ── CONSTRUCTOR ──────────────────────────────────────────────────────

    /**
     * EN: Parameterized constructor with Member Initializer List (MIL)
     *     MIL directly constructs each member → more efficient than body assignment
     *     Body assignment: first default-constructs, THEN assigns (2 steps for non-trivial types)
     */
    TemperatureSensor(const std::string& id, double minTh, double maxTh)
        : sensorId_(id)           // EN: Direct construction (1 step)
        , currentTempC_(0.0)      // EN: Initialize to zero
        , minThreshold_(minTh)    // EN: Set minimum threshold
        , maxThreshold_(maxTh)    // EN: Set maximum threshold
        , readingCount_(0)        // EN: No readings yet
    {
        // EN: Constructor body — runs AFTER MIL completes
        //     Use body for logic that can't go in MIL (validation, logging, etc.)
        std::cout << "[CONSTRUCTOR] Sensor '" << sensorId_ << "' created"
                  << " [range: " << minThreshold_ << "C - " << maxThreshold_ << "C]\n";
    }

    // ── DESTRUCTOR ──────────────────────────────────────────────────────

    /**
     * EN: Destructor — called automatically when object goes out of scope
     *     Perfect place for cleanup: releasing resources, logging, etc.
     *     Cannot be overloaded, cannot take parameters, no return type.
     *     Destruction order is LIFO (Last In, First Out) — reverse of construction.
     */
    ~TemperatureSensor() {
        std::cout << "[DESTRUCTOR] Sensor '" << sensorId_ << "' destroyed"
                  << " (total readings: " << readingCount_ << ")\n";
    }

    // ── CONST GETTERS ───────────────────────────────────────────────────

    /**
     * EN: Getter marked `const` → promises not to modify any member
     *     The `const` after the parameter list makes `this` pointer const:
     *       TemperatureSensor* this  →  const TemperatureSensor* this
     *     Rule: ALL getters should be const. If you forget const,
     *     the function CANNOT be called on a const object.
     */
    std::string getId() const { return sensorId_; }
    double getTemp() const { return currentTempC_; }
    int getReadingCount() const { return readingCount_; }

    // ── SETTER WITH VALIDATION ──────────────────────────────────────────

    /**
     * EN: Setter demonstrates encapsulation: external code can't set temp directly,
     *     must go through this function which validates input.
     *     Without encapsulation: object.temp = -9999; (no protection!)
     *     With encapsulation:    object.updateTemp(-9999); → rejected with message
     */
    void updateTemperature(double newTempC) {
        // EN: Validate — real sensors have physical limits
        if (newTempC < -273.15) {
            // EN: Absolute zero is -273.15°C, nothing can be colder
            std::cout << "  [WARN] Invalid temperature " << newTempC
                      << "C (below absolute zero!) — ignored\n";
            return;  // EN: Reject invalid input
        }
        currentTempC_ = newTempC;
        ++readingCount_;
    }

    // ── BEHAVIOR METHOD ─────────────────────────────────────────────────

    /**
     * EN: This method operates on internal state to produce a result.
     *     It checks thresholds and reports status. This is const because
     *     it only reads member variables, never modifies them.
     */
    void reportStatus() const {
        std::cout << "  Sensor [" << sensorId_ << "]: " << currentTempC_ << "C";

        // EN: Compare against thresholds
        if (currentTempC_ < minThreshold_) {
            std::cout << " ⚠ BELOW minimum (" << minThreshold_ << "C)";
        } else if (currentTempC_ > maxThreshold_) {
            std::cout << " ⚠ ABOVE maximum (" << maxThreshold_ << "C)";
        } else {
            std::cout << " ✓ Normal range";
        }
        std::cout << "  [readings: " << readingCount_ << "]\n";
    }

    // ── THIS POINTER DEMO ───────────────────────────────────────────────

    /**
     * EN: Demonstrates explicit `this` pointer usage.
     *     Case: parameter name 'id' shadows member — `this->` resolves ambiguity.
     *     Without `this->`, 'id' refers to the parameter, not the member.
     */
    void reassignId(const std::string& sensorId_) {
        // EN: Without `this->`, both sides refer to the parameter (self-assignment, USELESS)
        // sensorId_ = sensorId_;  // EN: BUG! Both are the parameter

        // EN: With `this->`, left side is the MEMBER, right side is the PARAMETER
        this->sensorId_ = sensorId_;
        std::cout << "  [INFO] Sensor ID reassigned to: " << this->sensorId_ << "\n";
    }

    /**
     * EN: Returns reference to *this for method chaining pattern.
     *     Usage: sensor.setMin(10).setMax(50);  ← each call returns the same object
     */
    TemperatureSensor& setMinThreshold(double val) {
        minThreshold_ = val;
        return *this;  // EN: Return the current object
    }

    TemperatureSensor& setMaxThreshold(double val) {
        maxThreshold_ = val;
        return *this;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// GLOBAL: struct vs class demonstration
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @struct CANFrame
 * @brief EN: POD struct — all members public by default, no invariants
 *
 * EN: Use struct for simple data containers with no encapsulation needs.
 *     This represents a CAN bus frame — just raw data, no validation logic.
 *     struct members are PUBLIC by default (vs class: PRIVATE by default).
 */
struct CANFrame {
    // EN: No need to write "public:" — struct is public by default
    uint32_t id;          // EN: CAN frame ID
    uint8_t  length;      // EN: Data Length Code (0-8)
    uint8_t  data[8];     // EN: Payload bytes
    bool     isExtended;  // EN: Standard (11-bit) or Extended (29-bit)
};

// ─────────────────────────────────────────────────────────────────────────────
// GLOBAL: Constructor overloading demonstration
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class ECUModule
 * @brief EN: Demonstrates constructor overloading (default + parameterized)
 *
 * EN: A class can have MULTIPLE constructors with different parameter lists.
 *     The compiler selects the correct one based on arguments at the call site.
 *     If you define ANY constructor, the compiler does NOT auto-generate a default one.
 *     To get it back: ECUModule() = default;
 */
class ECUModule {
private:
    std::string name_;
    std::string firmwareVersion_;
    bool isActive_;

public:
    // EN: Default constructor — creates module with placeholder values
    //     This exists so we can create an ECUModule without arguments: ECUModule ecu;
    ECUModule()
        : name_("UNKNOWN")
        , firmwareVersion_("0.0.0")
        , isActive_(false)
    {
        std::cout << "[CONSTRUCTOR] ECU default created: " << name_ << "\n";
    }

    // EN: Parameterized constructor — user specifies name and firmware version
    ECUModule(const std::string& name, const std::string& fw)
        : name_(name)
        , firmwareVersion_(fw)
        , isActive_(true)
    {
        std::cout << "[CONSTRUCTOR] ECU created: " << name_ << " (fw: " << firmwareVersion_ << ")\n";
    }

    // EN: Destructor
    ~ECUModule() {
        std::cout << "[DESTRUCTOR] ECU '" << name_ << "' destroyed\n";
    }

    // EN: Getters — all const
    std::string getName() const { return name_; }
    std::string getFirmware() const { return firmwareVersion_; }
    bool isActive() const { return isActive_; }

    // EN: Display info
    void printInfo() const {
        std::cout << "  ECU [" << name_ << "] fw=" << firmwareVersion_
                  << " active=" << (isActive_ ? "yes" : "no") << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 02 - Classes & Objects\n";
    std::cout << "============================================\n\n";

    // ─── Section 1: Object Creation & Constructor ────────────────────────
    {
        std::cout << "--- Section 1: Object Creation & Constructor ---\n";

        // EN: Creating an object = calling the constructor.
        //     The constructor runs immediately when the object is declared.
        //     Memory: object lives on the STACK (automatic storage duration).
        TemperatureSensor coolant("COOLANT_01", -40.0, 120.0);
        // EN: ↑ This single line does ALL of this:
        //     1. Allocates sizeof(TemperatureSensor) bytes on stack
        //     2. Calls constructor with ("COOLANT_01", -40.0, 120.0)
        //     3. MIL runs → members directly constructed
        //     4. Constructor body runs → prints creation message

        std::cout << "\n";
    }
    // EN: ↑ coolant goes out of scope here → DESTRUCTOR automatically called

    std::cout << "\n";

    // ─── Section 2: Encapsulation — Getter/Setter ────────────────────────
    {
        std::cout << "--- Section 2: Encapsulation — Getter/Setter ---\n";

        TemperatureSensor oil("OIL_TEMP", 0.0, 150.0);

        // EN: We CANNOT access private members directly:
        //     oil.currentTempC_ = 85.5;  ← COMPILE ERROR: 'currentTempC_' is private
        //     This is ENCAPSULATION: data is hidden, access is controlled.

        // EN: Use public setter — validates input before modifying state
        oil.updateTemperature(92.3);
        oil.updateTemperature(105.7);

        // EN: Use const getters to READ private data safely
        std::cout << "  Oil sensor ID: " << oil.getId() << "\n";
        std::cout << "  Oil temp: " << oil.getTemp() << "C\n";
        std::cout << "  Readings so far: " << oil.getReadingCount() << "\n";

        // EN: Try invalid temperature — setter will reject it
        oil.updateTemperature(-300.0);  // EN: Below absolute zero → rejected
        std::cout << "  Temp after invalid attempt: " << oil.getTemp() << "C (unchanged)\n";

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 3: Behavior Methods ─────────────────────────────────────
    {
        std::cout << "--- Section 3: Behavior Methods ---\n";

        TemperatureSensor intake("INTAKE_AIR", -30.0, 60.0);

        // EN: Feed multiple temperature readings and check status
        double readings[] = {25.0, -35.5, 62.0, 40.0};
        for (double temp : readings) {
            intake.updateTemperature(temp);
            intake.reportStatus();
            // EN: ↑ reportStatus() reads internal state and decides:
            //     below min → warning, above max → warning, else → normal
        }

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 4: `this` Pointer ──────────────────────────────────────
    {
        std::cout << "--- Section 4: this Pointer ---\n";

        TemperatureSensor exhaust("EXHAUST_01", 100.0, 900.0);

        // EN: Case 1: Resolving name shadowing with this->
        //     reassignId has a parameter named sensorId_ which shadows the member
        exhaust.reassignId("EXHAUST_LEFT_BANK");

        // EN: Case 2: Method chaining via return *this
        //     Each setter returns the same object, allowing call chaining.
        //     This is the same pattern used by std::cout: cout << a << b << c;
        exhaust.setMinThreshold(150.0).setMaxThreshold(950.0);
        // EN: ↑ Equivalent to:
        //     TemperatureSensor& ref1 = exhaust.setMinThreshold(150.0); // returns exhaust
        //     ref1.setMaxThreshold(950.0);                              // called on same object

        exhaust.updateTemperature(850.0);
        exhaust.reportStatus();

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 5: Multiple Objects — Independence ──────────────────────
    {
        std::cout << "--- Section 5: Multiple Objects — Independence ---\n";

        // EN: Each object has its OWN copy of all member variables.
        //     Modifying one object does NOT affect the other.
        //     They share the same CODE (member functions) but have separate DATA.
        TemperatureSensor cabin("CABIN", 15.0, 30.0);
        TemperatureSensor battery("BATTERY", -20.0, 45.0);

        cabin.updateTemperature(22.0);
        battery.updateTemperature(38.5);

        cabin.reportStatus();    // EN: Shows cabin data only
        battery.reportStatus();  // EN: Shows battery data only

        // EN: Changing cabin temp does NOT touch battery temp
        cabin.updateTemperature(33.0);
        std::cout << "  After cabin update:\n";
        cabin.reportStatus();
        battery.reportStatus();  // EN: Still 38.5 — unchanged

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 6: struct vs class & sizeof ─────────────────────────────
    {
        std::cout << "--- Section 6: struct vs class & sizeof ---\n";

        // EN: struct → POD, all members public by default, aggregate initialization
        //     Aggregate init: no constructor needed, just list values in braces {}
        CANFrame frame = {0x1A3, 4, {0xDE, 0xAD, 0xBE, 0xEF, 0, 0, 0, 0}, false};

        std::cout << "  CAN Frame ID: 0x" << std::hex << frame.id << std::dec << "\n";
        std::cout << "  DLC: " << static_cast<int>(frame.length) << "\n";
        // EN: ↑ static_cast<int> because uint8_t would print as a character
        //     (uint8_t is often typedef'd from unsigned char)

        std::cout << "  Data: ";
        for (int i = 0; i < static_cast<int>(frame.length); ++i) {
            std::cout << "0x" << std::hex << static_cast<int>(frame.data[i]) << " ";
        }
        std::cout << std::dec << "\n";

        // EN: sizeof shows the actual memory footprint of the type/object
        //     Note: sizeof includes PADDING bytes added by the compiler for alignment!
        //     struct layout in memory is NOT always sum of member sizes.
        std::cout << "  sizeof(CANFrame): " << sizeof(CANFrame) << " bytes\n";
        std::cout << "  sizeof(TemperatureSensor): " << sizeof(TemperatureSensor) << " bytes\n";
        std::cout << "  sizeof(ECUModule): " << sizeof(ECUModule) << " bytes\n";

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 7: Constructor Overloading ──────────────────────────────
    {
        std::cout << "--- Section 7: Constructor Overloading ---\n";

        // EN: Default constructor → no arguments needed
        //     The compiler selects ECUModule() because no args are provided
        ECUModule unknown;
        unknown.printInfo();

        // EN: Parameterized constructor → 2 arguments
        //     The compiler selects ECUModule(string, string) based on arg types
        ECUModule brakeCtrl("BrakeController", "2.4.1");
        brakeCtrl.printInfo();

        ECUModule steeringCtrl("SteeringAssist", "1.8.0");
        steeringCtrl.printInfo();

        std::cout << "\n";
    }
    // EN: ↑ End of scope → destructors called in REVERSE (LIFO) order:
    //     steeringCtrl destroyed first, then brakeCtrl, then unknown
    //     (last constructed = first destroyed)

    std::cout << "\n";

    // ─── Section 8: LIFO Destruction Order Demo ──────────────────────────
    {
        std::cout << "--- Section 8: LIFO Destruction Order ---\n";
        std::cout << "  Creating 3 sensors in order: A → B → C\n";

        // EN: Objects are constructed in declaration order: A, B, C
        //     But destroyed in REVERSE order: C, B, A  (LIFO)
        //     This matters when objects depend on each other!
        TemperatureSensor sA("A_first", 0, 100);
        TemperatureSensor sB("B_second", 0, 100);
        TemperatureSensor sC("C_third", 0, 100);

        std::cout << "  Now leaving scope — watch destruction order:\n";
    }
    // EN: ↑ Output will show: C destroyed, B destroyed, A destroyed (LIFO)

    std::cout << "\n";

    // ─── Section 9: const Objects ────────────────────────────────────────
    {
        std::cout << "--- Section 9: const Objects ---\n";

        // EN: A const object cannot be modified after construction.
        //     It can ONLY call const member functions.
        //     Attempting to call a non-const function → COMPILE ERROR.
        const TemperatureSensor frozen("FROZEN_REF", -50.0, 50.0);

        // EN: ✓ These work — getId(), getTemp(), reportStatus() are all const
        std::cout << "  Frozen sensor ID: " << frozen.getId() << "\n";
        std::cout << "  Frozen sensor temp: " << frozen.getTemp() << "C\n";
        frozen.reportStatus();

        // EN: ✗ These would FAIL to compile if uncommented:
        //     frozen.updateTemperature(10.0);  ← ERROR: non-const function on const object
        //     frozen.reassignId("NEW");        ← ERROR: same reason
        //     This is WHY making getters const matters!

        std::cout << "\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of Module 02 - Classes & Objects\n";
    std::cout << "============================================\n";

    return 0;
}
