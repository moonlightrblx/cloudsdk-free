# cloudsdk recode

## Introduction
`cloudsdk recode` is a C++ project designed for game manipulation and analysis, specifically targeting Unreal Engine games. It provides a set of utilities and hooks to interact with game internals, such as string searching and function spoofing.

## Features
- **String Reference Scanner**: Efficiently finds string references in memory to locate objects like `StaticFindObject`.
- **Return Address Spoofing**: `SPOOF_FUNC` macro to hide the origin of function calls.
- **Console Allocation**: Automatically allocates a console for standard input/output debugging.
- **Modular Design**: Structured with separate directories for memory, protection, and utilities.

## Getting Started

### Prerequisites
- Windows OS
- Visual Studio 2019 or later (with C++ Desktop Development workload)
- Git

### Building
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/cloudsdk-recode.git
   ```
2. Open `cloudsdk recode.sln` in Visual Studio.
3. Select the desired configuration (Debug/Release) and platform (x64 recommended).
4. Build the solution (Ctrl+Shift+B).

## Usage
After building, the output DLL can be injected into the target process using your preferred injector. Ensure the architecture matches the target game (usually x64).

> **Note**: This software is for educational purposes only.

## Disclaimer
This software is provided "as is", without warranty of any kind. Use it at your own risk. The authors are not responsible for any bans or damages caused by the use of this software.