# OSL Aroused Skyrim Mod

A high-performance arousal framework for Skyrim Special Edition (SE/AE/VR) with
native (OSL) and SexLab Aroused (SLA) compatibility modes. SKSE64 plugin written
in C++23 with Papyrus script components.

## Build System

The project uses [xmake](https://xmake.io). CommonLibSSE-NG is vendored as the
`lib/CommonLibVR` git submodule — there is no CMake or vcpkg dependency.

### Prerequisites

- xmake 3.0.0+
- Visual Studio 2022 (MSVC) or Clang-CL (C++23 compiler)
- Papyrus Compiler (for compiling .psc scripts)
- Initialize submodules: `git submodule update --init --recursive`

### Build Commands

**Configure and build (Debug):**
```bash
xmake f -m debug
xmake build OSLAroused
```

**Configure and build (Release):**
```bash
xmake f -m releasedbg
xmake build OSLAroused
```

### Run Tests

```bash
# Configure with tests enabled, then build + run
xmake f -m debug --build_tests=y
xmake build OSLArousedTests
xmake run OSLArousedTests

# Unit tests only (exclude integration/e2e by Catch2 tag)
xmake run OSLArousedTests "~[integration]~[e2e]"

# Integration tests only
xmake run OSLArousedTests "[integration]"

# End-to-end tests only (requires running Skyrim engine)
xmake run OSLArousedTests "[e2e]"
```

`build_and_test.bat [debug|release] [clean]` wraps configure + build + test in one step.

### Generate IDE/tooling projects (optional)

```bash
xmake project -k vsxmake             # Visual Studio solution under vsxmakeXXXX/
xmake project -k compile_commands    # compile_commands.json for clangd
```

### Compile Papyrus scripts

- Debug: use `Debug.ppj` with PapyrusCompiler
- Release: use `Release.ppj` with PapyrusCompiler

See `CLAUDE.md` for detailed architecture and build configuration notes.
