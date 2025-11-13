# Code Quality Tools

This document provides detailed information about the code quality tools used in the Omusubi project.

## Overview

The Omusubi project uses two primary code quality tools:

1. **clang-format** - Automatic code formatter
2. **clang-tidy** - Static analyzer and linter

Both tools are integrated into the VS Code Dev Container environment and work automatically as you code.

## clang-format (Formatter)

### What it does

clang-format automatically formats your C++ code according to predefined style rules, ensuring consistency across the entire codebase. This project focuses exclusively on C++ (no C code).

### Configuration

The configuration is stored in `.clang-format` at the project root.

**Key settings:**

| Setting | Value | Description |
|---------|-------|-------------|
| `BasedOnStyle` | LLVM | Base style template |
| `IndentWidth` | 4 | Number of spaces for indentation |
| `ColumnLimit` | 100 | Maximum line length |
| `PointerAlignment` | Left | `int* ptr` instead of `int *ptr` |
| `BreakBeforeBraces` | Attach | Opening brace on same line |

### Usage

**Automatic (recommended):**
- Formatting happens automatically when you save a C++ file in VS Code
- Controlled by your global VS Code settings (formatOnSave: true)

**Manual:**
```bash
# Format a single file
clang-format -i src/main.cpp

# Format all C++ files
find include src -name "*.h" -o -name "*.hpp" -o -name "*.cpp" | xargs clang-format -i

# Check formatting without modifying files
clang-format --dry-run --Werror src/main.cpp
```

**VS Code shortcuts:**
- `Shift + Alt + F` (Linux/Windows)
- `Shift + Option + F` (Mac)

### Include ordering

The formatter automatically organizes `#include` directives in this order:

1. Project headers (`"omusubi/..."`)
2. System C headers (if needed, e.g., `<stdint.h>`)
3. C++ standard library (`<cstdint>`, `<string>`, etc.)
4. Other headers

Example:
```cpp
#include "omusubi/system_context.h"
#include "omusubi/string_view.h"

#include <cstdint>

#include <string>
#include <vector>
```

**Note:** Prefer C++ headers (`<cstdint>`, `<cstdio>`) over C headers (`<stdint.h>`, `<stdio.h>`) when possible.

## clang-tidy (Linter)

### What it does

clang-tidy performs static analysis on your code to:
- Detect potential bugs
- Suggest performance improvements
- Enforce coding standards
- Recommend modern C++ idioms
- Check naming conventions

### Configuration

The configuration is stored in `.clang-tidy` at the project root.

**Check categories enabled:**

| Category | Purpose | Example Checks |
|----------|---------|----------------|
| `bugprone-*` | Detect common bugs | Use-after-move, infinite loops |
| `cert-*` | Security guidelines | CERT secure coding standards |
| `cppcoreguidelines-*` | Best practices | C++ Core Guidelines compliance |
| `performance-*` | Optimization | Unnecessary copies, move semantics |
| `readability-*` | Code clarity | Naming, complexity, magic numbers |
| `modernize-*` | C++14 idioms | Use `nullptr`, `auto`, etc. |

### Naming conventions

clang-tidy enforces the Omusubi naming conventions:

```cpp
// Classes and Structs: CamelCase
class SystemContext { };
struct Vector3 { };

// Functions and variables: snake_case
void initialize_device();
int sensor_value = 0;

// Constants and Enums: UPPER_CASE
const int MAX_BUFFER_SIZE = 256;
enum PowerState { ACTIVE, SLEEP };

// Private members: snake_case_ (trailing underscore)
class Device {
private:
    int port_;
    bool enabled_;
};

// Namespaces: snake_case
namespace omusubi {
namespace platform {
namespace m5stack {
}
}
}
```

### Usage

**Automatic (recommended):**
- clang-tidy runs automatically through the clangd language server
- Warnings appear as squiggly lines in VS Code
- Hover over warnings to see details and suggested fixes

**Manual:**
```bash
# Lint a single file
clang-tidy src/main.cpp -- -Iinclude -std=c++14

# Lint all source files
find src -name "*.cpp" -exec clang-tidy {} -- -Iinclude -std=c++14 \;

# Apply automatic fixes (use with caution)
clang-tidy -fix src/main.cpp -- -Iinclude -std=c++14
```

### Suppressing warnings

Sometimes you need to suppress specific warnings. Use these annotations sparingly:

```cpp
// Suppress warning for the next line only
// NOLINTNEXTLINE(check-name)
void* ptr = reinterpret_cast<void*>(address);

// Suppress warning for a single line
int result = legacy_function(); // NOLINT

// Suppress specific check for next line
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
auto* hardware = reinterpret_cast<HardwareRegister*>(0x40000000);
```

**When to suppress warnings:**
- Platform-specific hardware access (e.g., memory-mapped registers)
- Interaction with C libraries
- Performance-critical code where the check is not applicable
- Edge cases where the check produces false positives

**Always include a comment explaining why:**
```cpp
// Hardware register access requires reinterpret_cast
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
auto* reg = reinterpret_cast<volatile uint32_t*>(0x40000000);
```

## Embedded-specific checks

The configuration includes special considerations for embedded development:

### Disabled checks for embedded

Some checks are disabled because they conflict with embedded constraints:

| Check | Why Disabled |
|-------|--------------|
| `cppcoreguidelines-owning-memory` | We use stack allocation, not heap |
| `cppcoreguidelines-pro-bounds-pointer-arithmetic` | Needed for low-level hardware access |
| `cppcoreguidelines-avoid-magic-numbers` | Hardware registers use magic numbers |
| `modernize-avoid-c-arrays` | C arrays are acceptable in embedded |

### Custom thresholds

| Setting | Value | Reason |
|---------|-------|--------|
| Function line count | 100 | Embedded functions tend to be longer |
| Function statement count | 50 | Hardware initialization is complex |

## Integration with Git workflow

### Pre-commit checklist

Before committing code:

1. **Format check:** All files should be formatted
   ```bash
   find include src -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | \
     xargs clang-format --dry-run --Werror
   ```

2. **Lint check:** No critical warnings
   ```bash
   find src -name "*.cpp" -exec clang-tidy {} -- -Iinclude -std=c++14 \;
   ```

3. **Build check:** Code compiles without errors
   ```bash
   make clean && make
   ```

### CI/CD Integration (future)

In the future, these checks will run automatically on:
- Pull requests
- Pre-push hooks
- CI/CD pipelines

## Troubleshooting

### clang-format not working

1. Check that the extension is installed:
   - Open Extensions panel (`Ctrl+Shift+X`)
   - Search for "Clang-Format"
   - Verify "xaver.clang-format" is installed

2. Check VS Code settings:
   - `Ctrl+,` to open settings
   - Search for "format on save"
   - Ensure it's enabled

3. Verify clang-format is in PATH:
   ```bash
   which clang-format
   # Should output: /usr/bin/clang-format
   ```

### clang-tidy not showing warnings

1. Check clangd is running:
   - Look for "clangd" in the status bar
   - If not, reload the window (`Ctrl+Shift+P` → "Reload Window")

2. Check that `compile_commands.json` exists:
   ```bash
   # Generate it if missing
   bear -- make
   ```

3. Check clangd output:
   - `Ctrl+Shift+P` → "Output"
   - Select "Clangd" from dropdown

### False positives

If you consistently get false positives for a specific check:

1. Document the issue
2. Add it to `.clang-tidy` disabled checks
3. Comment why it's disabled

Example:
```yaml
Checks: >
  ...,
  -bugprone-suspicious-check,  # False positives with hardware registers
  ...
```

## References

- [clang-format documentation](https://clang.llvm.org/docs/ClangFormat.html)
- [clang-tidy documentation](https://clang.llvm.org/extra/clang-tidy/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [CERT C++ Coding Standard](https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046682)
