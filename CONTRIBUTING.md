# contributing.md

## cloudsdk engineering & contribution guidelines

**february 2026 edition**

---

# 1. purpose

This document defines the mandatory engineering, style, and architectural standards for contributing to **cloudsdk**.

These rules ensure:

* consistency across modules
* predictable formatting
* hardened production builds
* security-first design
* long-term maintainability

All contributors are expected to follow these guidelines without exception unless explicitly approved by a core maintainer.

---

# 2. general principles

cloudsdk code must be:

* consistent
* deterministic
* minimal
* secure by default
* performance-aware
* production-ready

If unsure about style or structure:

> open an adjacent file in cloudsdk and mirror its decisions exactly.

consistency is more important than personal preference.

---

# 3. comment policy

### 3.1 lowercase enforcement

all comment lines must be written entirely in lowercase.

no uppercase letters are permitted anywhere inside comments.

this includes:

* function names
* class names
* acronyms
* macro names
* external libraries

if referencing an identifier, it must be lowercased within the comment.

✅ valid:

```cpp
// validates process integrity and checks base address
```

❌ invalid:

```cpp
// Validates Process Integrity using WinAPI
```

---

# 4. naming conventions

## 4.1 universal rule

strict snake_case everywhere.

do not introduce:

* PascalCase
* camelCase
* hungarian notation

unless deliberately mirroring legacy cloudsdk code.

---

## 4.2 identifiers

| category                        | rule                               | example                               |
| ------------------------------- | ---------------------------------- | ------------------------------------- |
| functions                       | snake_case                         | `start_process_monitor()`             |
| variables                       | snake_case                         | `process_handle`                      |
| members                         | snake_case                         | `last_heartbeat_time_ms`              |
| classes/structs                 | snake_case                         | `kernel_interface`                    |
| type aliases                    | must end in `_t`                   | `memory_region_t`                     |
| prefixed implementation classes | prefix `c_`                        | `c_driver_session`                    |
| enums                           | snake_case enum + lowercase values | `status_code { ok, failed, timeout }` |
| macros (discouraged)            | ALL_CAPS                           | `#define MAX_BUFFER_SIZE 4096`        |

---

# 5. formatting standards

formatting must match existing cloudsdk files.

### 5.1 indentation & braces

* mirror indentation width exactly.
* mirror brace placement style exactly.
* do not introduce alternative brace patterns.

### 5.2 pointer & reference formatting

match existing style precisely:

```cpp
type* ptr;
type& ref;
```

do not vary spacing conventions.

### 5.3 line length

follow the dominant line width visible in the surrounding module.

---

# 6. language standards

minimum: **c++17**
preferred: **c++20**, where toolchain allows.

encouraged features:

* constexpr
* scoped enums
* strong typing
* std::span
* std::string_view
* structured bindings
* concepts (if used in the module)
* ranges (when clarity improves)

avoid feature use purely for novelty.

---

# 7. memory management policy

## 7.1 allocation discipline

dynamic allocation should:

* occur primarily during initialization
* be avoided in tight runtime loops
* be isolated to clearly defined subsystems

prefer:

* pre-reserved containers
* stack allocation
* raii wrappers

---

## 7.2 unsafe c-style functions

the following are prohibited across trust boundaries:

* strcpy
* sprintf
* unbounded memcpy
* raw buffer arithmetic without bounds validation

prefer:

* std::string_view
* std::span
* bounded algorithms
* safe abstractions

---

# 8. error handling

follow the established cloudsdk pattern:

* `result<t>`-style return types
* expected-like error propagation
* status enums where appropriate

do not introduce inconsistent exception-heavy patterns unless already used in that module.

error handling must be:

* explicit
* readable
* deterministic

---

# 9. container and utility usage

prefer container and utility choices already used in the module.

if cloudsdk consistently uses:

* `std::vector` → use it
* `std::array` → use it
* custom allocators → reuse them
* logging macros → reuse them

do not mix paradigms within the same subsystem.

---

# 10. project structure

new modules must align with existing directory conventions.

common structure examples:

```
/core
/platform
/crypto
/network
/runtime
/util
```

do not invent new top-level categories without review.

internal organization must feel native to cloudsdk.

---

# 11. third-party code policy

all third-party code must:

* be vendored as source
* include exact commit hash or version tag
* be reviewed before inclusion

no live git submodules in production branches.

no runtime dependency fetching.

---

# 12. security & hardening

cloudsdk is security-oriented. contributions must not weaken:

* integrity verification logic
* anti-tamper checks
* cryptographic hygiene
* validation boundaries

critical paths may be subject to:

* control-flow hardening
* obfuscation (release builds)
* integrity checks

do not bypass protection layers for convenience.

---

# 13. logging policy

debug builds may include:

* file logging
* console logging
* verbose diagnostics

production builds must:

* strip debug logging
* remove unnecessary output
* avoid leaking sensitive state

logging must never expose:

* cryptographic material
* raw identifiers
* internal detection heuristics

---

# 14. networking standards

all network communication must:

* use encryption
* avoid plaintext transport
* avoid static keys
* support per-session entropy

server authority must be respected for state decisions.

---

# 15. release branch rules

these rules fully apply to:

* main
* release
* production branches

development branches may temporarily relax:

* logging verbosity
* experimental allocations
* debugging instrumentation

all deviations must be removed before merging.

---

# 16. pull request requirements

every pull request must:

* match surrounding style exactly
* avoid unnecessary refactors
* include clear reasoning
* avoid unrelated formatting changes
* be scoped to a single concern

large structural changes require prior discussion.

---

# 17. philosophy

cloudsdk prioritizes:

* clarity over cleverness
* consistency over individuality
* safety over convenience
* determinism over dynamic behavior

if you are unsure how something should look:

> find the closest existing implementation in cloudsdk and mirror it.

consistency is mandatory.
