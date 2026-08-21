# OSL Aroused - Arousal Reborn

OSL Aroused is a framework-independent, SKSE-based arousal framework for Skyrim.
It is a drop-in replacement for both SexLab Aroused and OAroused, and is fully backwards compatible with all mods using those frameworks.

This is not a direct port of either mod, but created from the ground up with an SKSE core. The goal is to minimize Papyrus script load while maintaining full compatibility with mods using either SL Aroused or OAroused.

You can use OSL Aroused with just SexLab, just OStim, both frameworks, or neither. Integration with sex frameworks is automatically detected and enabled if you have them installed.

## Primary Features

**Performance**
- Core system written in native SKSE DLL to reduce script load
- No script polling or cloaks - all perimeter checks run in native code
- All data managed in OSLAroused SKSE cosave, not PapyrusUtil
- Better state management with ability to clear data from MCM
- Significantly reduces cosave bloat that SL Aroused traditionally causes

**New Arousal Mechanics**
Complete redesign of how arousal updates over time:
- **Baseline Arousal** - Tracks what an actor's arousal wants to move towards based on their current state (being naked, viewing nudity, wearing devices, participating in scenes, etc.)
- **Current Arousal** - The actor's overall arousal value, gradually moves towards baseline
- **Libido** - Represents minimum baseline arousal, very slowly moves towards current arousal
- Dynamic and reactive system that responds to actor state in real-time

**Advanced Nudity Detection (A.N.D.) Integration** *(New in 2.8.0)*
- Full integration with Advanced Nudity Detection mod for scaled arousal responses
- NPCs react appropriately to partial nudity (topless, bottomless, underwear, etc.) instead of binary naked/clothed
- Fully configurable baselines for each nudity state in MCM
- Gracefully falls back to legacy nudity detection if A.N.D. is not installed

**Keyword Configuration**
- Built-in support for adding keywords to armor at runtime
- Currently supports Baka's "EroticArmor" and SLS "Bikini" keywords
- Easy to add additional keywords or user-defined keywords

**Backwards Compatible**
- Works with majority of mods using SL Aroused or OAroused without patches
- Includes stubbed versions of SexLabAroused.esm, OAroused.esp, and required scripts
- Scripts redirect mod requests to OSL Aroused native DLL
- Maintains faction/global state that SexLab Aroused requires

**Minimal Dependencies**
- No unnecessary dependencies
- Neither SexLab nor OStim required
- Framework adapters loaded at runtime only if frameworks are installed

**Cross Framework Arousal**
- Acts as a bridge between OAroused and SL Aroused
- Both frameworks share the same arousal state

## Requirements
- SKSE matching your Skyrim runtime
- Address Library matching your Skyrim runtime
- SkyUI

## Installation
1. Install via mod manager using the included FOMOD
2. Ensure the SKSE and Address Library versions match your Skyrim runtime; OSL Aroused uses one DLL for SE, AE, and VR
3. Place towards bottom of load order - **MUST OVERWRITE ALL OTHER MODS** (like SLSO)
4. **DO NOT INSTALL SexLab Aroused OR OAroused** - this is a replacement
5. If you don't use mods requiring OAroused, you can disable OAroused.esp

## Uninstallation
Simply remove from your load order - no additional steps required.

## Configuration
- Extensive MCM menu with many options
- Use the Show Arousal key (RCtrl default) while aiming at NPC to edit their stats in MCM
- Press while not aiming at NPC to edit player stats

## Arousal Mode Selection
Switch between arousal systems in MCM:
- **SLA Mode** - Original SexLab Aroused mechanics (TimeRate/Exposure)
- **OSL Mode** - New mechanics described below (default)

## Arousal Mechanics (OSL Mode)

OSL Aroused uses a robust arousal management system supporting both "state" and "event" based modification.

**Core Attributes:**

**Current Arousal**
- Represents an actor's current arousal level
- Modified over time based on Baseline Arousal
- Manipulated by events (vibration, orgasm, etc.)

**Baseline Arousal**
- Target value that Current Arousal moves towards
- Determined by actor's current state:
  - Nudity level (full nude, topless, bottomless, partial - scales with A.N.D. if installed)
  - Spectating nude actors or sex scenes
  - Wearing lewd clothes (EroticArmor keyword)
  - Wearing devices (Devious Devices and TOYS support)
  - Participating in or viewing sex scenes

**Libido**
- Minimum Baseline Arousal value
- Very slowly moves towards Current Arousal
- Long-term tracker of arousal state
- Staying at high arousal increases libido, making it harder to lower arousal

**Raising Current Arousal:**
- Triggered by other mods via SLAroused API
- Automatically triggered when sex scenes begin/advance
- Maintaining Baseline above Current gradually raises arousal

**Lowering Current Arousal:**
- Triggered by other mods via SLAroused API
- Automatically triggered on actor orgasm
- Maintaining Baseline below Current gradually lowers arousal

All rates and values are configurable via MCM.

## Credits
This mod is built on the shoulders of giants:
- Anyone who worked on SLA or variants (redneck2x, fishburger67, Lupine00, Baka, and others)
- Sairon350 for OAroused
- powerof3 whose SKSE mods were invaluable references
- NymphoElf for Advanced Nudity Detection

## Source Code
https://github.com/ozooma10/OSLAroused

## Feedback & Support
Let me know if you have suggestions, run into issues, or have ideas for arousal mechanics!
