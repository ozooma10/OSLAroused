# OSL Aroused Skyrim Mod

Build system uses https://gitlab.com/colorglass/commonlibsse-sample-plugin



### Build Commands

**Configure and build (Debug):**
```bash
cmake --preset build-debug-msvc
cmake --build build/debug-msvc --preset debug-msvc
```

**Configure and build (Release):**
```bash
cmake --preset build-release-msvc
cmake --build build/release-msvc --preset release-msvc
```

**Run tests:**
```bash
# All tests
ctest --preset tests-all

# Unit tests only (no Skyrim runtime required)
ctest --preset tests-unit

# Integration tests (Skyrim module at rest)
ctest --preset tests-integration

# End-to-end tests (requires running Skyrim engine)
ctest --preset tests-e2e
```
