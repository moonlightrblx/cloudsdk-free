# Installation & Build Guide

## Prerequisites

Before building `cloudsdk recode`, ensure you have the following installed:

1.  **Visual Studio 2019/2022**:
    *   Workload: **Desktop development with C++**
    *   Component: **MSVC v143 - VS 2022 C++ x64/x86 build tools**
    *   Component: **Windows 10 SDK** (or Windows 11 SDK)

2.  **Git**: Version control system.

## Clone the Repository

```bash
git clone https://github.com/yourusername/cloudsdk-recode.git
cd cloudsdk-recode
```

## Building with Visual Studio

1.  Navigate to the project directory and open `cloudsdk recode.sln`.
2.  In the top toolbar, set the **Solution Configuration** to `Release` (or `Debug` for development).
3.  Set the **Solution Platform** to `x64` (most UE games are x64).
4.  Right-click on the solution in the **Solution Explorer** and select **Build Solution**, or press `Ctrl + Shift + B`.

## Output

Upon a successful build, the output DLL will be located in:
*   `build\x64\Release\cloudsdk.dll` (or similar path depending on config).

## Troubleshooting

### "Windows SDK version not found"
*   Right-click the project -> **Properties** -> **General** -> **Windows SDK Version**. Select the installed version from the dropdown.

### "Platform Toolset not found"
*   Right-click the project -> **Properties** -> **General** -> **Platform Toolset**. Select the installed toolset (e.g., Visual Studio 2022 (v143)).
