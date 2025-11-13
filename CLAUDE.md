# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## CRITICAL: Japanese Communication Required

**All communication with the user MUST be in Japanese.**

- All responses must be in Japanese
- Comments and explanations must be in Japanese
- Code comments must be in Japanese
- Technical terms can remain in English, but explanations must be in Japanese

**English responses to the user are not acceptable. This is a strict requirement.**

## Project Overview

Omusubi is a lightweight, type-safe C++14 framework for embedded devices (primarily M5Stack). It provides a clean architecture centered around `SystemContext` for unified hardware access, with complete interface/implementation separation for hardware-independent code.

**Key Characteristics:**
- Zero-overhead abstractions (no dynamic allocation, no exceptions)
- C++14 with clang++
- Header-only implementations use `.hpp`, declarations use `.h`
- Japanese comments and documentation are standard

## Development Environment

### Dev Container

This project is developed in a Dev Container environment.

**Environment Specifications:**
- **Base Image:** `silkeh/clang:latest`
- **Compiler:** Clang (C++14 support)
- **Build Tools:** Make, CMake, Ninja
- **Debug Tools:** GDB, Valgrind
- **Locale:** ja_JP.UTF-8 (Japanese environment)
- **Timezone:** Asia/Tokyo

**VS Code Extensions:**
- GitLens
- Claude Code (`ghcr.io/anthropics/devcontainer-features/claude-code:1.0.5`)
- Docker outside of Docker

**Getting Started:**
```bash
# Open in VS Code
code .

# Select "Reopen in Container"
# → Dev Container environment will be automatically built
```

## Build Commands

```bash
# Build the project
make

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild

# Build and run
make run
```

The Makefile uses recursive file discovery with `$(shell find)` to automatically detect all `.cpp` files in `src/` and maintain directory structure in `obj/`.

## Architecture: Method Chain Design

### Core Design Philosophy

**SystemContext is a gateway to organized device access through method chaining.**

All hardware access follows this pattern:
```cpp
get_system_context()->get_[category]_context()->get_[device]_context()->method()
```

Example:
```cpp
// Bluetooth connection
ctx->get_connectable_context()->get_bluetooth_context()->connect()

// Serial reading
ctx->get_readable_context()->get_serial0_context()->read_line()

// Accelerometer sensor
ctx->get_sensor_context()->get_accelerometer_context()->get_values()
```

### Layer Structure

### 1. Interface Layer (`include/omusubi/interface/`)
**`*able` interfaces define single-method contracts (following Android pattern):**
- `Readable` - Single pure virtual function for reading
- `Writable` - Single pure virtual function for writing
- `Connectable` - Single pure virtual function for connecting
- `Scannable` - Single pure virtual function for scanning
- `Pressable` - Single pure virtual function for button state
- `Measurable` / `Measurable3D` - Single pure virtual function for measurements
- `Displayable` - Single pure virtual function for display output

**Rule:** Each `*able` interface MUST have exactly one pure virtual function.

### 2. Context Layer (`include/omusubi/context/`)
**Middle-tier contexts group devices by category and serve as DI containers:**

The Context layer is not just for grouping - it functions as a **Dependency Injection (DI) container**:

**DI Container Responsibilities:**
- Device creation and ownership management
- Mock substitution for testing
- Platform switching abstraction
- Dependency management between devices

**Contexts by Category:**

- `ConnectableContext` - All connection-capable devices
  - `SerialContext`, `BluetoothContext`, `WiFiContext`, `BLEContext`
- `ReadableContext` - All readable devices
  - `SerialContext`, `BluetoothContext`
- `WritableContext` - All writable devices
  - `SerialContext`, `BluetoothContext`, `DisplayContext`
- `ScannableContext` - All scannable devices
  - `WiFiContext`, `BluetoothContext`, `BLEContext`
- `SensorContext` - All sensor devices
  - Accelerometer, Gyroscope, Temperature, etc.
- `InputContext` - All input devices
  - Buttons, Touch, etc.
- `OutputContext` - All output devices
  - Display, LED, Speaker, etc.
- `SystemInfoContext` - System information (read-only)
  - Device name, firmware version, chip ID, uptime, free memory
- `PowerContext` - Power management
  - Power state, battery level

### 3. Device Layer (`include/omusubi/device/`)
**Concrete device context classes implementing `*able` interfaces:**
- `SerialContext` (Readable + Writable + Connectable)
- `BluetoothContext` (Readable + Writable + Connectable + Scannable)
- `WiFiContext` (Connectable + Scannable)
- `BLEContext` (Connectable + Scannable)

### 4. Platform Layer (`include/omusubi/platform/`, `src/platform/`)
**Platform-specific implementations:**
- `M5StackSystemContext` - SystemContext implementation for M5Stack
- `M5StackConnectableContext` - ConnectableContext implementation
- `M5StackSensorContext` - SensorContext implementation
- `M5StackSerialContext` - SerialContext implementation (inherits from device layer)
- etc.

**Implementation pattern (Pattern A - Context owns devices):**
```cpp
class M5StackConnectableContext : public ConnectableContext {
private:
    M5StackSerialContext serial0_{0};
    M5StackSerialContext serial1_{1};
    M5StackBluetoothContext bluetooth_;
    M5StackWiFiContext wifi_;
    M5StackBLEContext ble_;

public:
    SerialContext* get_serial0_context() override { return &serial0_; }
    SerialContext* get_serial1_context() override { return &serial1_; }
    BluetoothContext* get_bluetooth_context() override { return &bluetooth_; }
    WiFiContext* get_wifi_context() override { return &wifi_; }
    BLEContext* get_ble_context() override { return &ble_; }
};
```

**SystemContext Implementation Pattern:**

Platform implementation classes follow this pattern:

```cpp
// Platform implementation (users do not directly use this)
class M5StackSystemContext : public SystemContext {
private:
    // Constructor can be private or public (either works)
    // ⚠️ No need for get_instance() or other static methods
    M5StackSystemContext();

public:
    // SystemContext interface implementation
    void begin() override;
    void update() override;
    // ...
};

// Platform-specific implementation file (src/platform/m5stack/system_context.cpp)
namespace omusubi {
    // Created as static local variable in free function (Meyers Singleton)
    SystemContext& get_system_context() {
        static platform::m5stack::M5StackSystemContext inst;
        return inst;
    }
}
```

**Constructor Access Specifier:**
- `private`: More strict (prevents direct instantiation from elsewhere)
- `public`: Simpler (assumes no instantiation outside `get_system_context()`)
- Either works, but `public` is simpler

**Implement the same pattern for other platforms:**

**IMPORTANT:** Always implement `get_system_context()` using this pattern:

```cpp
SystemContext& get_system_context() {
    static [DeviceName]SystemContext inst;
    return inst;
}
```

Implementation examples for each platform:

```cpp
// M5Stack (src/platform/m5stack/system_context.cpp)
namespace omusubi {
    SystemContext& get_system_context() {
        static platform::m5stack::M5StackSystemContext inst;
        return inst;
    }
}

// Arduino (src/platform/arduino/system_context.cpp)
namespace omusubi {
    SystemContext& get_system_context() {
        static platform::arduino::ArduinoSystemContext inst;
        return inst;
    }
}

// Raspberry Pi Pico (src/platform/pico/system_context.cpp)
namespace omusubi {
    SystemContext& get_system_context() {
        static platform::pico::PicoSystemContext inst;
        return inst;
    }
}
```

**Benefits of this pattern:**
- Simple and consistent
- Leverages C++11 thread-safe static initialization (Meyers Singleton)
- No need for `get_instance()` method in platform implementation classes
- Singleton implementation is centralized in `get_system_context()`

**Build system switching:**
- Only the target platform's `system_context.cpp` is linked at build time
- User code remains unchanged (always just calls `get_system_context()`)

**Key principles:**
- User code only uses the `get_system_context()` free function
- `XXXSystemContext::get_instance()` is an internal implementation detail
- User code remains unchanged when switching platforms

### Critical Design Rules

**1. No Heap Allocation**
- All objects MUST be stack-allocated or use placement new with static buffers
- Never use `new`, `delete`, `malloc`, `free`, `std::unique_ptr`, `std::shared_ptr`
- Use static buffers with placement new when dynamic construction is needed

**2. Context Getters: Two Access Patterns**

The Context layer functions as a DI container and supports two access patterns:

**Pattern A: Individual Methods (Primary)**
```cpp
SerialContext* serial = ctx.get_connectable_context()->get_serial0_context();
```
- ✅ Explicit and clear
- ✅ Available for all devices

**Pattern B: Template Parameters (C++14)**
```cpp
SerialContext* serial = ctx.get_connectable_context()->get_serial_context<0>();
```
- ✅ Specify port number at compile time
- ✅ Supports template metaprogramming
- ⚠️ Runtime values not allowed (compile-time constants only)

**Runtime parameters are prohibited:**
- ❌ Bad: `get_serial_context(port)` - Runtime arguments not allowed

**3. Single Primary Access Path**
- Devices with multiple interfaces have ONE primary category
- Communication devices → via `ConnectableContext`
- Sensors → via `SensorContext`
- Input devices → via `InputContext`
- Output devices → via `OutputContext`

**4. Naming Conventions**
- Interface classes: `*able` (Readable, Writable, Connectable)
- Context classes: `*Context` (SerialContext, SystemContext)
- Implementation classes: `M5Stack*Context` (M5StackSerialContext)
- Constants: `UPPER_CASE_WITH_UNDERSCORES`
- Functions/variables: `snake_case`
- Classes: `PascalCase`

**5. SystemContext Access: Free Function Pattern**

**IMPORTANT:** Always access SystemContext using the **free function** `get_system_context()`.

```cpp
// Free function (global namespace)
SystemContext& get_system_context();
```

**Usage:**
```cpp
// ✅ Correct: Use free function
SystemContext& ctx = get_system_context();

// ❌ Wrong: Not a class static method
SystemContext& ctx = SystemContext::get_instance();  // This does not exist
```

**Implementation details (users don't need to worry about this):**
- Platform implementations (M5StackSystemContext, etc.) may use Singleton pattern internally
- The `get_system_context()` free function calls platform-specific implementation internally
- This ensures user code remains unchanged when switching platforms

**6. SystemContext Core Responsibilities**
```cpp
class SystemContext {
public:
    // System control (stays in SystemContext)
    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void delay(uint32_t ms) = 0;
    virtual void reset() = 0;

    // Category context access
    virtual ConnectableContext* get_connectable_context() = 0;
    virtual ReadableContext* get_readable_context() = 0;
    virtual WritableContext* get_writable_context() = 0;
    virtual ScannableContext* get_scannable_context() = 0;
    virtual SensorContext* get_sensor_context() = 0;
    virtual InputContext* get_input_context() = 0;
    virtual OutputContext* get_output_context() = 0;
    virtual SystemInfoContext* get_system_info_context() = 0;
    virtual PowerContext* get_power_context() = 0;
};
```

**Violation Warning:**
If code does not follow these design rules, it violates the framework's architecture philosophy and must be refactored.

## Usage Pattern

### Performance-Critical: Cache Context Pointers

Users retrieve device pointers once in `setup()` and reuse them to avoid method call overhead:

```cpp
// Cache in global variables (performance-oriented)
SystemContext& ctx = get_system_context();
ConnectableContext* connectable = nullptr;
BluetoothContext* bt = nullptr;

void setup() {
    ctx.begin();

    // Retrieve once and cache
    connectable = ctx.get_connectable_context();
    bt = connectable->get_bluetooth_context();
}

void loop() {
    ctx.update();

    // Use cached pointer directly (no overhead)
    bt->connect();
}
```

### RAII Pattern for Classes

When using contexts in classes, follow RAII principles:

```cpp
class MyDevice {
private:
    BluetoothContext* bt_;

public:
    MyDevice(SystemContext& ctx)
        : bt_(ctx.get_connectable_context()->get_bluetooth_context()) {
        // Retrieve and hold Context in constructor
    }

    void connect() {
        bt_->connect();
    }
};
```

**Important:**
- Never call `ctx.get_*()->get_*()` chains repeatedly in `loop()` - cache the final pointer
- Context pointers remain valid for the lifetime of SystemContext

## Coding Conventions

- **Naming:**
  - Files: `snake_case.h`, `snake_case.hpp`, `snake_case.cpp`
  - Functions/variables: `snake_case`
  - Classes: `PascalCase`
  - Namespaces: `snake_case`
  - Constants: `UPPER_CASE_WITH_UNDERSCORES`
- **Macros:** Prohibited - Use `constexpr` functions and variables instead
- **String literals:** Use `_sv` suffix (requires `using namespace omusubi::literals`)
- **Header guards:** Use `#pragma once`
- **Memory:** No heap allocation - stack or placement new with static buffers only

## Core Types

- `StringView` - Non-owning string reference (UTF-8 aware)
- `FixedString<N>` - Stack-allocated string with fixed capacity
- `FixedBuffer<N>` - Stack-allocated byte buffer
- `Vector3` - 3D vector for sensor data
- `PowerState` - Enum for power/battery state

## Writing Examples

Examples in `examples/platform/m5stack/` must follow this pattern:

```cpp
// [Japanese comment explaining what the example does]

#include <omusubi/omusubi.h>

using namespace omusubi;
using namespace omusubi::literals;

// Global variables: retrieve once in setup(), reuse in loop()
SystemContext& ctx = get_system_context();
SerialContext* serial = nullptr;

void setup() {
    // Initialize system
    ctx.begin();

    // Retrieve device (once only)
    // New design: access via method chain
    serial = ctx.get_connectable_context()->get_serial0_context();

    // [Setup logic with comments]
}

void loop() {
    // Update system
    ctx.update();

    // [Loop logic with comments]

    ctx.delay(10);
}
```

**Example guidelines:**
- Include clear Japanese comments explaining purpose and key steps
- Use method chain to access devices: `ctx.get_[category]_context()->get_[device]_context()`
- Retrieve device pointers once in `setup()`, store globally for performance
- Keep examples focused on single functionality
- Use serial output only (avoid display unless specifically demonstrating display)

## Interface-Based Development

When writing hardware-agnostic code, depend only on interfaces:

```cpp
// ✅ Good: Works with any Writable device
void log_message(Writable& output, StringView message) {
    output.write("[LOG] "_sv);
    output.write_line(message);
}

// Can be used with serial, display, Bluetooth, etc.
log_message(*serial, "Started"_sv);
log_message(*display, "Started"_sv);
```

This enables:
- Code reuse across different hardware
- Testing with mock implementations
- Platform portability

## Header File Distinction

- `.h` files: Declarations only (require corresponding `.cpp`)
- `.hpp` files: Header-only with inline implementations (no `.cpp` needed)

Examples:
- `string_view.h` → needs implementation
- `fixed_string.hpp` → complete implementation in header
- `system_context.h` → interface declaration
- `m5stack_system_context.hpp` → platform-specific implementation

## Platform Support

Currently implemented:
- M5Stack (Basic, Core2, etc.) via `M5StackSystemContext`

Planned:
- Arduino
- Raspberry Pi Pico

## Language

- Code comments: Japanese preferred
- Variable names: English
- Documentation: Japanese in README, mixed in code comments

# Embedded Development Design Principles

## Addressing Static Writable Area Constraints

In embedded systems, it is often impossible to link static writable areas into executables.
(Examples: BREW, Android Application Context, etc.)

In environments with such constraints, designs that depend on static or global variables are not viable.

### Solution: Context Pattern Design

**Design Principles**
- Access data through interfaces
- Divide interfaces by category and expose only necessary functionality
- Obtain required interfaces from a parent instance (Context)
- Hide interface implementations from callers

**Design Benefits**
- Interfaces themselves have code efficiency nearly equivalent to direct access in executables
- Forward declarations minimize impact on unrelated code
- Implementation is independent of data placement (static area, dynamic area, external memory, etc.)
- Interfaces can be nested to build hierarchical structures as needed
- Interfaces hide whether data is a single block or has overlapping parts

**Important Notes**
- Singleton is an implementation choice for the implementation side
- The caller should not expect the same instance to always be returned
- The implementation side decides to adopt it for benefits like reduced creation costs

**Reference Designs**
- Android Application Context
- Design Pattern: Abstract Factory Pattern

This design pattern is a guideline for balancing flexibility and efficiency in memory-constrained embedded environments.
