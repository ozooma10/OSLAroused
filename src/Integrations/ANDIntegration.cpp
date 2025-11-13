#include "ANDIntegration.h"
#include "../Settings.h"
#include "../Utilities/Utils.h"
#include "../Managers/ActorStateManager.h"

namespace Integrations
{
    bool ANDIntegration::Initialize()
    {
        Locker locker(m_Lock);

        const auto dataHandler = RE::TESDataHandler::GetSingleton();
        if (!dataHandler) {
            logger::error("Failed to get TESDataHandler");
            return false;
        }

        // Look for A.N.D. mod in load order
        // TODO: Update with actual A.N.D. plugin name when known
        const char* andModName = "AdvancedNudityDetection.esp";
        const auto andMod = dataHandler->LookupModByName(andModName);

        if (!andMod) {
            logger::info("A.N.D. mod not found in load order, integration disabled");
            m_IsAvailable = false;
            return false;
        }

        m_ANDModIndex = andMod->GetPartialIndex();
        logger::info("Found A.N.D. mod with index: {:04X}", m_ANDModIndex);

        // Resolve faction FormIDs
        auto ResolveFormId = [this, dataHandler, andModName](uint32_t baseFormId) -> RE::TESForm* {
            RE::FormID formId = (static_cast<RE::FormID>(m_ANDModIndex) << 24) | (baseFormId & 0xFFFFFF);
            return dataHandler->LookupForm(formId, andModName);
        };

        // Resolve all A.N.D. factions
        auto ResolveAndCastFaction = [&](uint32_t baseId, const char* name) -> RE::TESFaction* {
            auto* form = ResolveFormId(baseId);
            if (!form) {
                logger::warn("Failed to resolve A.N.D. faction: {} (0x{:08X})", name, baseId);
                return nullptr;
            }
            auto* faction = form->As<RE::TESFaction>();
            if (!faction) {
                logger::warn("Form is not a faction: {} (0x{:08X})", name, baseId);
                return nullptr;
            }
            logger::debug("Resolved A.N.D. faction: {} -> 0x{:08X}", name, faction->formID);
            return faction;
        };

        m_ANDNudeFaction = ResolveAndCastFaction(AND_NUDE_FACTION_ID, "Nude");
        m_ANDToplessFaction = ResolveAndCastFaction(AND_TOPLESS_FACTION_ID, "Topless");
        m_ANDBottomlessFaction = ResolveAndCastFaction(AND_BOTTOMLESS_FACTION_ID, "Bottomless");
        m_ANDShowingChestFaction = ResolveAndCastFaction(AND_SHOWING_CHEST_FACTION_ID, "ShowingChest");
        m_ANDShowingAssFaction = ResolveAndCastFaction(AND_SHOWING_ASS_FACTION_ID, "ShowingAss");
        m_ANDShowingGenitalsFaction = ResolveAndCastFaction(AND_SHOWING_GENITALS_FACTION_ID, "ShowingGenitals");
        m_ANDShowingBraFaction = ResolveAndCastFaction(AND_SHOWING_BRA_FACTION_ID, "ShowingBra");
        m_ANDShowingUnderwearFaction = ResolveAndCastFaction(AND_SHOWING_UNDERWEAR_FACTION_ID, "ShowingUnderwear");

        // Check if we resolved at least the core factions
        if (!m_ANDNudeFaction || !m_ANDToplessFaction || !m_ANDBottomlessFaction) {
            logger::warn("Failed to resolve core A.N.D. factions, integration disabled");
            m_IsAvailable = false;
            return false;
        }

        m_IsAvailable = true;
        logger::info("A.N.D. Integration initialized successfully");
        return true;
    }

    float ANDIntegration::GetANDNudityScore(RE::Actor* actor)
    {
        if (!actor) {
            return 0.0f;
        }

        Locker locker(m_Lock);

        if (!m_IsAvailable) {
            return 0.0f;
        }

        // Get cached state (will fetch if not cached)
        ANDNudityState state = m_ActorNudityCache(actor);
        return state.calculatedScore;
    }

    ANDIntegration::ANDNudityState ANDIntegration::FetchActorNudityState(RE::Actor* actor)
    {
        ANDNudityState state{};

        if (!actor || !m_IsAvailable) {
            return state;
        }

        // Check faction membership for each A.N.D. faction
        auto CheckFaction = [&](RE::TESFaction* faction) -> bool {
            return faction && actor->IsInFaction(faction);
        };

        state.isNude = CheckFaction(m_ANDNudeFaction);
        state.isTopless = CheckFaction(m_ANDToplessFaction);
        state.isBottomless = CheckFaction(m_ANDBottomlessFaction);
        state.isShowingChest = CheckFaction(m_ANDShowingChestFaction);
        state.isShowingAss = CheckFaction(m_ANDShowingAssFaction);
        state.isShowingGenitals = CheckFaction(m_ANDShowingGenitalsFaction);
        state.isShowingBra = CheckFaction(m_ANDShowingBraFaction);
        state.isShowingUnderwear = CheckFaction(m_ANDShowingUnderwearFaction);

        // Calculate score based on the combination logic specified

        // Step 1: Hard override for Nude
        if (state.isNude) {
            state.calculatedScore = NUDE_BASELINE;  // 50.0f
            logger::trace("Actor {:08X} is nude, score: {}", actor->formID, state.calculatedScore);
            return state;
        }

        // Step 2: Chest region (priority: Topless > ShowingChest > ShowingBra)
        float chestScore = 0.0f;
        if (state.isTopless) {
            chestScore = TOPLESS_BASELINE;  // 20.0f
        } else if (state.isShowingChest) {
            chestScore = SHOWING_CHEST_BASELINE;  // 12.0f
        } else if (state.isShowingBra) {
            chestScore = SHOWING_BRA_BASELINE;  // 8.0f
        }

        // Step 3: Front bottom region (priority: Bottomless > ShowingGenitals > ShowingUnderwear)
        float frontScore = 0.0f;
        if (state.isBottomless) {
            frontScore = BOTTOMLESS_BASELINE;  // 30.0f
        } else if (state.isShowingGenitals) {
            frontScore = SHOWING_GENITALS_BASELINE;  // 15.0f
        } else if (state.isShowingUnderwear) {
            frontScore = SHOWING_UNDERWEAR_BASELINE;  // 8.0f
        }

        // Step 4: Back bottom / ass region
        float assScore = 0.0f;
        if (state.isBottomless) {
            assScore = 0.0f;  // Already covered by bottomless
        } else if (state.isShowingAss) {
            assScore = SHOWING_ASS_BASELINE;  // 8.0f
        }

        // Step 5: Calculate base score
        float baseScore = chestScore + frontScore + assScore;

        // Step 6: Special synergy case
        // If Topless and Bottomless are both true but Nude is false
        if (state.isTopless && state.isBottomless && !state.isNude) {
            baseScore = TOPLESS_BOTTOMLESS_SYNERGY;  // 37.0f
            logger::trace("Actor {:08X} has topless+bottomless synergy, score: {}",
                         actor->formID, baseScore);
        }

        // Step 7: Clamp to valid range [0, 50]
        state.calculatedScore = std::clamp(baseScore, 0.0f, 50.0f);

        logger::trace("Actor {:08X} nudity score: {} (chest:{}, front:{}, ass:{})",
                     actor->formID, state.calculatedScore, chestScore, frontScore, assScore);

        return state;
    }

    float ANDIntegration::GetNudityBaselineModifier(RE::Actor* actor)
    {
        if (!actor) {
            return 0.0f;
        }

        const auto settings = Settings::GetSingleton();

        // Check if A.N.D. integration is enabled in settings
        bool useANDIntegration = settings->GetUseANDIntegration();

        // Get A.N.D. score if integration is available and enabled
        float andScore = 0.0f;
        if (useANDIntegration && m_IsAvailable) {
            andScore = GetANDNudityScore(actor);
        }

        // If we have an A.N.D. score, apply the multiplier and return
        if (andScore > 0.0f) {
            float multiplier = settings->GetANDNudityMultiplier();
            float modifier = andScore * multiplier;

            logger::trace("Actor {:08X} using A.N.D. nudity modifier: {} (score:{} * mult:{})",
                         actor->formID, modifier, andScore, multiplier);
            return modifier;
        }

        // Fall back to legacy nudity detection
        if (IsActorNudeLegacy(actor)) {
            float nudeBaseline = settings->GetNudeArousalBaseline();
            logger::trace("Actor {:08X} using legacy nude baseline: {}", actor->formID, nudeBaseline);
            return nudeBaseline;
        }

        // Check for erotic armor as secondary fallback
        if (IsActorWearingEroticArmorLegacy(actor)) {
            // Get erotic armor baseline - Settings already has this functionality
            float eroticBaseline = 0.0f;
            const auto wornArmorKeywords = Utilities::Actor::GetWornArmorKeywords(actor);

            for (const auto keywordFormId : wornArmorKeywords) {
                float keywordBaseline = settings->GetEroticArmorBaseline(keywordFormId);
                if (keywordBaseline > eroticBaseline) {
                    eroticBaseline = keywordBaseline;  // Use highest value
                }
            }

            if (eroticBaseline > 0.0f) {
                logger::trace("Actor {:08X} using erotic armor baseline: {}", actor->formID, eroticBaseline);
                return eroticBaseline;
            }
        }

        return 0.0f;
    }

    ANDIntegration::ANDNudityState ANDIntegration::GetActorNudityState(RE::Actor* actor)
    {
        if (!actor) {
            return ANDNudityState{};
        }

        Locker locker(m_Lock);
        return m_ActorNudityCache(actor);
    }

    void ANDIntegration::InvalidateActorCache(RE::Actor* actor)
    {
        if (!actor) {
            return;
        }

        Locker locker(m_Lock);
        m_ActorNudityCache.PurgeItem(actor);

        logger::trace("Invalidated A.N.D. cache for actor {:08X}", actor->formID);
    }

    void ANDIntegration::ClearCache()
    {
        Locker locker(m_Lock);
        // LRUCache doesn't have a Clear method, so we'll need to recreate it
        // For now, this is a no-op as we can't clear without recreating the cache
        // Individual actors can still be invalidated with InvalidateActorCache

        logger::trace("Clear all A.N.D. caches requested (no-op - use InvalidateActorCache for individual actors)");
    }

    bool ANDIntegration::IsActorNudeLegacy(RE::Actor* actor) const
    {
        if (!actor) {
            return false;
        }

        // Use existing utility function for legacy nude check
        return Utilities::Actor::IsNaked(actor);
    }

    bool ANDIntegration::IsActorWearingEroticArmorLegacy(RE::Actor* actor) const
    {
        if (!actor) {
            return false;
        }

        // Check if actor is wearing any armor with erotic keywords
        const auto settings = Settings::GetSingleton();
        const auto wornArmorKeywords = Utilities::Actor::GetWornArmorKeywords(actor);

        for (const auto keywordFormId : wornArmorKeywords) {
            if (settings->GetEroticArmorBaseline(keywordFormId) > 0.0f) {
                return true;  // Found at least one erotic armor keyword
            }
        }

        return false;
    }
}