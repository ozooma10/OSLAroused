# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Important Instructions for Claude

**DO NOT automatically attempt to build this project.** This is a Skyrim SKSE plugin that requires specific Windows development environment setup including:
- Specific versions of Visual Studio and xmake
- Skyrim Special Edition installation with SKSE64
- Papyrus Compiler from the Skyrim Creation Kit
- Properly configured environment variables

The user will manually build the project when needed. Focus on code analysis, suggestions, and modifications without triggering builds.

## Branch Naming Convention

When creating a git branch, use a `type/short-description` format with a kebab-case description. Use these type prefixes:
- `feat/` — new features
- `fix/` — bug fixes
- `chore/` — tooling, build, and maintenance
- `docs/` — documentation changes
- `refactor/` — code restructuring with no behavior change

Examples: `feat/sla-exposure-decay`, `fix/ticker-main-thread-marshalling`, `chore/bump-commonlibsse`.

## Project Overview

OSLAroused is a high-performance arousal framework for Skyrim Special Edition that provides both native (OSL) and SexLab Aroused (SLA) compatibility modes. It's a SKSE64 plugin written in C++23 with Papyrus script components for in-game integration.

## Build System

### Prerequisites
- xmake 3.0.0+
- Visual Studio 2022 (MSVC) or Clang-CL with C++23 support
- Papyrus Compiler (for compiling .psc scripts)
- Initialized git submodules (`git submodule update --init --recursive`)

CommonLibSSE-NG is vendored in the `lib/CommonLibVR` git submodule. The project
does not use CMake or vcpkg.

**Runtime support: SE + AE + VR.** The `commonlibsse-ng.plugin` xmake rule builds
a single DLL that loads on Skyrim SE (1.5.97), AE (1.6.x), and VR (1.4.15) via
runtime detection and Address Library.
VR users additionally need the VR Address Library installed at runtime.

### Build Commands

**Configure and build (Debug):**
```bash
xmake f -m debug
xmake build OSLAroused
```

**Configure and build (Release, optimized with debug symbols):**
```bash
xmake f -m releasedbg
xmake build OSLAroused
```

**Run tests:**
```bash
# Configure with tests enabled, then build and run
xmake f -m debug --build_tests=y
xmake build OSLArousedTests
xmake run OSLArousedTests

# Filter by Catch2 tag, e.g. unit tests only (exclude integration/e2e)
xmake run OSLArousedTests "~[integration]~[e2e]"
```

`build_and_test.bat [debug|release] [clean]` wraps configure, build, and test.

**Compile Papyrus scripts:**
- Debug: Use `Debug.ppj` with PapyrusCompiler
- Release: Use `Release.ppj` with PapyrusCompiler

### Build Configuration

- Default build output: `build/windows/x64/<mode>/`
- The `oslaroused.deploy` rule copies the built DLL/PDB to `contrib/Distribution/PluginDebug/` or `contrib/Distribution/PluginRelease/`
- Automatic deployment to Mod Organizer 2 directories via the `SkyrimPluginTargets` environment variable (semicolon-separated list of mod folders) is preserved
- The `commonlibsse-ng.plugin` rule generates `SKSEPlugin_Version` as an `SKSE::PluginDeclaration`; runtime code reads it through `SKSE::PluginDeclaration::GetSingleton()`. Keep the version in sync between `xmake.lua`, the Papyrus interface, and FOMOD metadata.
- Papyrus script output configured in `.ppj` files

## Code Architecture

### Core Components

#### 1. Arousal System (Dual-Mode Architecture)
The plugin implements a strategy pattern with two interchangeable arousal calculation backends:

- **IArousalSystem** (`src/Managers/ArousalSystem/IArousalSystem.h`): Abstract interface defining arousal system contract
- **ArousalSystemOSL** (`src/Managers/ArousalSystem/ArousalSystemOSL.cpp`): Native OSL arousal calculation system
- **ArousalSystemSLA** (`src/Managers/ArousalSystem/ArousalSystemSLA.cpp`): SexLab Aroused compatibility mode

The active system can be switched at runtime and is persisted in save games. Key difference:
- OSL mode: Direct arousal values with baseline + multiplier system
- SLA mode: Maps OSL concepts to SLA's exposure/time rate system

#### 2. ArousalManager (`src/Managers/ArousalManager.h`)
Singleton manager that owns the active arousal system implementation. Provides:
- `SetArousalSystem()`: Switches between OSL and SLA modes
- `GetArousalSystem()`: Returns reference to active implementation
- Exports C-style DLL functions (`GetArousalExt`, `SetArousalExt`, etc.) for external plugin integration

#### 3. Persisted Data System (`src/PersistedData.h`)
Template-based serialization system for SKSE cosave data:

- **BaseData<T>**: Thread-safe base class template for persisted data types
- **Specialized classes**: ArousalData, BaseLibidoData, ArousalMultiplierData, LastCheckTimeData, LastOrgasmTimeData, ArmorKeywordData, etc.
- **Serialization**: Each data type has unique 4-character key (e.g., 'OSLA', 'OSLB') for cosave storage
- **SettingsData**: Stores which arousal mode (OSL/SLA) was active when game was saved

#### 4. Manager Layer
- **ActorStateManager** (`src/Managers/ActorStateManager.cpp`): Tracks actor states relevant to arousal calculation
- **SceneManager** (`src/Managers/SceneManager.cpp`): Handles sexual scene events and integration
- **WorldChecks/Ticker**: Periodic arousal updates via `ArousalUpdateTicker` (started on kDataLoaded)

#### 5. Papyrus Integration (`src/Papyrus/`)
Native functions exposed to Papyrus scripts:
- **PapyrusInterface**: Main arousal API functions
- **PapyrusActor**: Actor-specific functions
- **PapyrusConfig**: Configuration and settings
- **Papyrus**: General utility functions

#### 6. Integrations (`src/Integrations/`)
- **DevicesIntegration**: Integration with Devious Devices framework for arousal effects from worn restraints
- **ANDIntegration**: Integration with Advanced Nudity Detection mod for granular nudity states

#### 7. Utilities (`src/Utilities/`)
- **LRUCache.h**: Template-based Least Recently Used cache implementation
- **Ticker.h**: Periodic task execution system
- **Utils**: General utility functions (Factions, Keywords, etc.)

### Initialization Flow (src/Main.cpp)

1. **InitializeLogging**: Sets up spdlog with MSVC debugger or file output
2. **InitializeSerialization**: Registers cosave callbacks with SKSE
3. **InitializePapyrus**: Registers native Papyrus functions
4. **InitializeMessaging**: Registers for SKSE messaging events:
   - `kDataLoaded`: Start arousal ticker, load INI config, initialize factions
   - `kNewGame`: Reset to OSL mode
   - `kPostLoadGame`: Distribute persisted keywords, restore saved arousal mode

### Key Data Flow

1. Arousal values stored per-actor using FormID in PersistedData singletons
2. Arousal updates triggered by:
   - Periodic ticker (WorldChecks::ArousalUpdateTicker)
   - Equipment events (RuntimeEvents::OnEquipEvent)
   - Scene events (via SceneManager)
   - Direct Papyrus/C++ API calls
3. Mode switching preserves data by mapping between OSL/SLA concepts or resetting if requested
4. All data thread-safe via recursive mutex locks in BaseData<T>

### Distribution Structure

- `contrib/Distribution/Assets/`: ESM/ESP files, translations, config files
- `contrib/Distribution/PapyrusSources/`: Source Papyrus scripts (.psc)
- `contrib/Distribution/Patches/`: Compatibility patches for other mods (e.g., Devious Devices)
- `contrib/Distribution/fomod/`: FOMOD installer configuration

### Testing

Tests are located in `test/`. Example: `test_LRUCache.cpp` tests the LRU cache utility. Tests use Catch2 and are enabled with the xmake `--build_tests=y` option.

### Advanced Nudity Detection (A.N.D.) Integration

The plugin integrates with the Advanced Nudity Detection mod to provide more granular nudity tracking:

#### Architecture
- **ANDIntegration** (`src/Integrations/ANDIntegration.h/cpp`): Singleton class managing A.N.D. communication
- **Faction-Based System**: Uses 8 A.N.D. factions to track different nudity states:
  - Nude (index 0): Full nudity - baseline 50
  - Topless (index 1): Upper body exposed - baseline 20
  - Bottomless (index 2): Lower body exposed - baseline 30
  - ShowingChest (index 3): Partial chest exposure - baseline 12
  - ShowingAss (index 4): Partial rear exposure - baseline 8
  - ShowingGenitals (index 5): Partial front exposure - baseline 15
  - ShowingBra (index 6): Wearing only bra - baseline 8
  - ShowingUnderwear (index 7): Wearing only underwear - baseline 8

#### Key Features
- **Priority System**: Higher-priority states override lower ones (e.g., Topless overrides ShowingChest)
- **Synergy Bonus**: Topless + Bottomless (without full Nude) gives special score of 37
- **Configurable Baselines**: All faction baselines configurable via INI and MCM
- **Graceful Fallback**: Reverts to simple nudity detection if A.N.D. not installed
- **Cache Invalidation**: Uses ModEvent "OSLA_ANDUpdate" to invalidate cache when nudity changes

#### Configuration
- INI Section: `[ANDIntegration]` in OSLAroused.ini
- Enable/disable via `UseANDIntegration` setting
- Individual faction baselines configurable (0-100 range with validation)
- Constants defined in `src/Integrations/ANDFactionIndices.h`

#### API Functions
- `GetANDNudityScore(Actor)`: Returns nudity score (0-50)
- `GetANDFactionContributions(Actor)`: Returns 8-element array of faction contributions
- `SetANDFactionBaseline(index, value)`: Configure faction baseline with validation
- `IsANDIntegrationEnabled()`: Check if A.N.D. is available and enabled

## Important Notes

- The plugin supports running without sexlab.esm (see commit e5d0414)
- Arousal mode is loaded from persisted data on game load (not from INI)
- FormID resolution during load uses SKSE's ResolveFormID for cross-save compatibility
- The project uses the vendored CommonLibSSE-NG `lib/CommonLibVR` submodule for SKSE plugin development, building one DLL for SE/AE/VR
- Logging uses `SKSE::log::{trace,debug,info,warn,error,critical}` (NG's source-location-capturing loggers), not the libxse fork's `REX::` macros
- PCH (Precompiled Header) is in `src/PCH.h`
- A.N.D. integration requires "Advanced Nudity Detection.esp" in load order
