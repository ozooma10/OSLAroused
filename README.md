# OSL Aroused Skyrim Mod

A high-performance arousal framework for Skyrim Special Edition (SE/AE/VR) with
native (OSL) and SexLab Aroused (SLA) compatibility modes. It is an SKSE64 plugin
written in C++23 with Papyrus script components.

## Build System

The project uses [xmake](https://xmake.io). CommonLibSSE-NG is vendored as the
`lib/CommonLibVR` git submodule, so CMake and vcpkg are not required.

### Prerequisites

- xmake 3.0.0+
- Visual Studio 2022 with MSVC, or Clang-CL (C++23 compiler)
- Papyrus Compiler from the Skyrim Creation Kit (for `.psc` scripts)
- Initialized submodules: `git submodule update --init --recursive`

### Build Commands

Configure and build Debug:

```bash
xmake f -m debug
xmake build OSLAroused
```

Configure and build Release with debug symbols:

```bash
xmake f -m releasedbg
xmake build OSLAroused
```

### Run Tests

```bash
xmake f -m debug --build_tests=y
xmake build OSLArousedTests
xmake run OSLArousedTests
```

Catch2 tags can be passed directly to the test executable:

```bash
xmake run OSLArousedTests "[LRUCache]"
xmake run OSLArousedTests "~[integration]~[e2e]"
```

`build_and_test.bat [debug|release] [clean]` wraps configure, build, and test.

### Generate IDE/Tooling Projects

```bash
xmake project -k vsxmake
xmake project -k compile_commands
```

### Compile Papyrus Scripts

- Debug: use `Debug.ppj` with PapyrusCompiler
- Release: use `Release.ppj` with PapyrusCompiler

See `CLAUDE.md` for detailed architecture and build configuration notes.
