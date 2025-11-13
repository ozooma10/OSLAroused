# OSLAroused: System Overview

## What is OSLAroused?

OSLAroused is a high-performance arousal framework for Skyrim Special Edition that tracks and manages character arousal levels dynamically based on in-game situations and time. It provides two interchangeable calculation modes for compatibility with different mod ecosystems.

## Core Concept

The system maintains an arousal value (0-100) for each character that changes based on:
- **Time** - Arousal naturally converges toward a baseline over time
- **Situation** - Environmental factors modify the baseline (nudity, scenes, equipment)
- **Events** - Direct modifications from game events or other mods

## Two Calculation Modes

### OSL Mode (Native)
- Stores arousal directly and uses exponential convergence
- Formula: `newArousal = lerp(current, baseline, 1 - 0.2^gameHours)`
- Converges 80% toward baseline per game hour
- Baseline = max(situational modifiers, base libido)

### SLA Mode (Compatibility)
- Calculates arousal from exposure and time since orgasm
- Formula: `arousal = (daysSinceOrgasm × timeRate) + exposure`
- Exposure decays with 2-day half-life: `exposure × 1.5^(-time/2)`
- Adds exposure when viewing nudity/scenes

## Mathematical Models

### Exponential Convergence (OSL Mode)

The OSL mode uses an exponential convergence model where arousal approaches the baseline over time:

```
newArousal = lerp(currentArousal, baseline, 1 - ε^gameHoursPassed)
```

Where:
- `ε = 0.2` for arousal changes (fast convergence)
- `ε = 0.1` for libido changes (slower convergence)

**Convergence rates:**
- After 1 game hour: 80% of the way to baseline
- After 2 game hours: 96% of the way to baseline
- After 3 game hours: 99.2% of the way to baseline

This creates smooth, natural transitions that quickly approach equilibrium.

### Half-Life Decay (SLA Mode)

The SLA mode uses a half-life decay model for exposure:

```
exposure_decayed = exposure × 1.5^(-timeSinceUpdate / halfLife)
```

Where:
- `halfLife = 2.0` game days (default)
- Exposure reduces by 50% every 2 game days without stimuli

TimeRate also decays based on days since orgasm using the same model:
```
timeRate_decayed = baseTimeRate × 1.5^(-daysSinceOrgasm / halfLife)
```

## Baseline Calculation

The system calculates a dynamic baseline based on the current situation. These modifiers stack additively and are clamped to [0, 100]:

| Situation | Baseline Modifier |
|-----------|------------------|
| Actor is nude | +30 |
| Actor is participating in scene | +50 |
| Actor is viewing nude character | +20 |
| Actor is viewing scene | +30 |
| Wearing erotic armor | +20 |
| Devious Devices (per device) | +5 to +20 |

### Device Contributions
- Chastity Belt: +20
- Vaginal/Anal Plugs: +20 each
- Collar: +5
- Leg/Arm Cuffs: +5 each
- Bra: +10
- Gag: +10
- Nipple/Vaginal Piercings: +10 each
- Blindfold: +5
- Harness: +10
- Corset: +10
- Boots/Gloves: +5 each
- Heavy Bondage: +10
- Bondage Mittens: +10

## Update System

### Timing
- **Real-time ticker**: Runs every 15 seconds
- **Game-time threshold**: Updates when 0.1 game hours pass (~6 minutes real-time at 20:1 timescale)
- **Spectating checks**: Continuous for actors in same cell

### Update Process
1. Ticker checks elapsed game time since last update
2. For each actor in the current cell:
   - Calculate time-based arousal changes
   - Apply situational modifiers
   - Check for spectating events (SLA mode)
   - Update stored values

### Spectating (SLA Mode Only)
When an actor views a nude character:
- Preferred gender: +4 exposure per update
- Non-preferred gender: +2 exposure per update
- Exhibitionist bonus: +2 additional exposure for the nude actor

## Mode Comparison

| Aspect | OSL Mode | SLA Mode |
|--------|----------|----------|
| **Storage** | Direct arousal value | Calculated from exposure + time |
| **Decay** | Exponential (ε=0.2) | Half-life (2 days) |
| **Speed** | Fast (80%/hour) | Slow (50%/2 days) |
| **Complexity** | Simple | Complex (orgasm tracking) |
| **Spectating** | Not implemented | +2 to +4 exposure |
| **Best For** | Quick responsiveness | Long-term progression |

## Data Persistence

All arousal data is saved per-character in SKSE co-saves:
- **ArousalData** (OSLA): Current arousal/exposure value
- **BaseLibidoData** (OSLB): Base libido/time rate
- **ArousalMultiplierData** (OSLM): Arousal/exposure multiplier
- **LastCheckTimeData** (OSLC): Last update timestamp
- **LastOrgasmTimeData** (OSLO): Last orgasm timestamp (SLA mode)
- **ArmorKeywordData** (OSLK): Cached worn armor keywords
- **IsArousalLockedData** (OSLL): Lock state flag
- **IsActorExhibitionistData** (OSLE): Exhibitionist flag
- **SettingsData** (OSLS): Active mode (0=OSL, 1=SLA)

## Integration Points

### For Mod Authors

**Papyrus API:**
- `GetArousal(Actor)`: Get current arousal (0-100)
- `SetArousal(Actor, float)`: Set arousal directly
- `ModifyArousal(Actor, float)`: Add/subtract arousal (with multiplier)
- `GetLibido(Actor)`: Get base libido/time rate
- `SetLibido(Actor, float)`: Set base libido

**C++ DLL Exports:**
- `GetArousalExt(uint32_t formId)`: Direct arousal access
- `SetArousalExt(uint32_t formId, float value)`: Direct arousal setting
- `ModifyArousalExt(uint32_t formId, float value)`: Modify with multiplier

**Events:**
- `OSLAroused_ActorArousalUpdated`: Fired on arousal changes
- Includes actor reference and new arousal value

## Performance Considerations

- **LRU Cache**: Recently accessed actors cached for fast lookup
- **Keyword Caching**: Worn armor keywords cached to avoid repeated scans
- **Cell-Based Updates**: Only updates actors in loaded cells
- **Configurable Update Rate**: Can adjust ticker frequency if needed

## Configuration

Key settings (in OSLAroused.ini or via MCM):
- `ArousalChangeRate`: Epsilon for OSL arousal (default: 0.2)
- `LibidoChangeRate`: Epsilon for OSL libido (default: 0.1)
- `TimeRateHalfLife`: Half-life for SLA decay (default: 2.0 days)
- `ScanDistance`: Range for detecting nearby actors (default: 5120 units)
- Various baseline values for different situations

## System Flow Example

### OSL Mode Example
1. Actor enters scene → baseline increases to 50
2. Current arousal: 20, baseline: 50
3. After 1 hour: `arousal = lerp(20, 50, 0.8) = 44`
4. After 2 hours: `arousal = lerp(44, 50, 0.8) = 48.8`
5. Scene ends → baseline drops to base libido (e.g., 10)
6. Arousal begins converging down toward 10

### SLA Mode Example
1. Actor with 10 exposure views nude character
2. Exposure increases by 4 (preferred gender)
3. New exposure: 14
4. Days since orgasm: 1
5. Time rate: 10
6. `arousal = (1 × 10) + 14 = 24`
7. After 2 days without stimuli: `exposure = 14 × 0.5 = 7`
8. `arousal = (3 × 10) + 7 = 37`

## Design Principles

1. **Smooth Transitions**: Mathematical models ensure gradual changes
2. **Situational Awareness**: Environment directly affects arousal
3. **Compatibility**: Dual-mode system works with existing mods
4. **Performance**: Optimized caching and update strategies
5. **Persistence**: All data survives save/load cycles
6. **Extensibility**: Clean API for mod integration

## Questions for Validation

1. **Convergence Rates**: Are ε=0.2 (arousal) and ε=0.1 (libido) producing the intended speed of change?
2. **Baseline Stacking**: Should modifiers stack additively as currently implemented, or would multiplicative/diminishing returns be more realistic?
3. **Update Frequency**: Is the 15-second real-time ticker optimal for performance vs. responsiveness?
4. **SLA Half-Life**: Is the 2-day half-life appropriate for the intended gameplay pacing?
5. **Device Values**: Are the device arousal contributions balanced for gameplay?