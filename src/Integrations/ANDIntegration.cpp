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
            SKSE::log::error("Failed to get TESDataHandler");
            return false;
        }

        // Look for A.N.D. mod in load order
        const char* andModName = "Advanced Nudity Detection.esp";
        const auto andMod = dataHandler->LookupModByName(andModName);
        if (!andMod) {
            SKSE::log::info("{} mod not found in load order, integration disabled", andModName);
            m_IsAvailable = false;
            return false;
        }

        m_ANDModIndex = andMod->GetPartialIndex();

        // Resolve all A.N.D. factions
        auto ResolveAndCastFaction = [&](uint32_t baseId, const char* name) -> RE::TESFaction* {
            RE::FormID formId = Utilities::Forms::ResolveFormId(m_ANDModIndex, baseId);
            if (!formId) {
                SKSE::log::warn("Failed to resolve A.N.D. faction: {} (0x{:08X})", name, baseId);
                return nullptr;
            }
            
            RE::TESFaction* faction = RE::TESForm::LookupByID<RE::TESFaction>(formId); 
            if(!faction) {
                SKSE::log::warn("Resolved A.N.D. faction is not a TESFaction: {} (0x{:08X})", name, formId);
                return nullptr;
            }
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
            SKSE::log::warn("Failed to resolve core A.N.D. factions, integration disabled");
            m_IsAvailable = false;
            return false;
        }

        m_IsAvailable = true;
        SKSE::log::info("A.N.D. Integration initialized successfully");
        return true;
    }

    float ANDIntegration::GetANDNudityScore(RE::Actor* actor)
    {
        if (!actor) {
            SKSE::log::trace("GetANDNudityScore: Actor is null");
            return 0.0f;
        }

        Locker locker(m_Lock);

        if (!m_IsAvailable) {
            SKSE::log::trace("GetANDNudityScore: A.N.D. integration not available");
            return 0.0f;
        }

        try {
            return FetchActorNudityState(actor).calculatedScore;
        }
        catch (const std::exception& e) {
            SKSE::log::error("GetANDNudityScore: Exception caught for actor {:08X}: {}",
                         actor->formID, e.what());
            return 0.0f;
        }
    }

    ANDIntegration::ANDNudityState ANDIntegration::FetchActorNudityState(RE::Actor* actor)
    {
        ANDNudityState state{};

        if (!actor || !m_IsAvailable) {
            return state;
        }

        // Check faction membership for each A.N.D. faction with error handling
        auto CheckFaction = [&](RE::TESFaction* faction, const char* factionName) -> bool {
            if (!faction) {
                SKSE::log::trace("CheckFaction: {} faction is null", factionName);
                return false;
            }

            try {
                return actor->GetFactionRank(faction, actor->IsPlayer()) > 0;
            }
            catch (const std::exception& e) {
                SKSE::log::error("CheckFaction: Error checking {} faction for actor {:08X}: {}",
                             factionName, actor->formID, e.what());
                return false;
            }
        };

        state.isNude = CheckFaction(m_ANDNudeFaction, "Nude");

        // Get configurable baseline values from Settings
        const auto settings = Settings::GetSingleton();
        auto baselines = settings->GetANDFactionBaselines();

        // Step 1: Hard override for Nude
        if (state.isNude) {
            state.calculatedScore = baselines.Nude;
            SKSE::log::trace("Actor {} is nude, score: {}", actor->GetDisplayFullName(), state.calculatedScore);
            return state;
        }

        state.isTopless = CheckFaction(m_ANDToplessFaction, "Topless");
        state.isBottomless = CheckFaction(m_ANDBottomlessFaction, "Bottomless");
        state.isShowingChest = CheckFaction(m_ANDShowingChestFaction, "ShowingChest");
        state.isShowingAss = CheckFaction(m_ANDShowingAssFaction, "ShowingAss");
        state.isShowingGenitals = CheckFaction(m_ANDShowingGenitalsFaction, "ShowingGenitals");
        state.isShowingBra = CheckFaction(m_ANDShowingBraFaction, "ShowingBra");
        state.isShowingUnderwear = CheckFaction(m_ANDShowingUnderwearFaction, "ShowingUnderwear");


        SKSE::log::trace("Actor {} A.N.D. Nudity State - Nude: {}, Topless: {}, Bottomless: {}, ShowingChest: {}, ShowingAss: {}, ShowingGenitals: {}, ShowingBra: {}, ShowingUnderwear: {}",
                     actor->GetDisplayFullName(),
                     state.isNude,
                     state.isTopless,
                     state.isBottomless,
                     state.isShowingChest,
                     state.isShowingAss,
                     state.isShowingGenitals,
                     state.isShowingBra,
                     state.isShowingUnderwear);

        // Step 2: Chest region (priority: Topless > ShowingChest > ShowingBra)
        float chestScore = 0.0f;
        if (state.isTopless) {
            chestScore = baselines.Topless;
        } else if (state.isShowingChest) {
            chestScore = baselines.ShowingChest;
        } else if (state.isShowingBra) {
            chestScore = baselines.ShowingBra;
        }

        // Step 3: Front bottom region (priority: Bottomless > ShowingGenitals > ShowingUnderwear)
        float frontScore = 0.0f;
        if (state.isBottomless) {
            frontScore = baselines.Bottomless;
        } else if (state.isShowingGenitals) {
            frontScore = baselines.ShowingGenitals;
        } else if (state.isShowingUnderwear) {
            frontScore = baselines.ShowingUnderwear;
        }

        // Step 4: Back bottom / ass region
        float assScore = 0.0f;
        if (state.isBottomless) {
            assScore = 0.0f;  // Already covered by bottomless
        } else if (state.isShowingAss) {
            assScore = baselines.ShowingAss;
        }

        // Step 5: Calculate base score
        float baseScore = chestScore + frontScore + assScore;

        // Step 6: Special synergy case
        // If Topless and Bottomless are both true but Nude is false
        if (state.isTopless && state.isBottomless && !state.isNude) {
            // Use a synergy bonus that's proportional to the configured values
            baseScore = (baselines.Topless + baselines.Bottomless) * 0.74f;
            SKSE::log::trace("Actor {} has topless+bottomless synergy, score: {}",
                         actor->GetDisplayFullName(), baseScore);
        }

        // Step 7: Clamp to valid range [0, 100] (using max configured value)
        float maxValue = std::max({baselines.Nude, baselines.Topless + baselines.Bottomless,
                                   baselines.ShowingChest + baselines.ShowingGenitals + baselines.ShowingAss});
        state.calculatedScore = std::clamp(baseScore, 0.0f, maxValue);

        SKSE::log::trace("Actor {} nudity score: {} (chest:{}, front:{}, ass:{})",
                     actor->GetDisplayFullName(), state.calculatedScore, chestScore, frontScore, assScore);

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

        // If we have an A.N.D. score, return it directly
        if (andScore > 0.0f) {
            SKSE::log::trace("Actor {} using A.N.D. nudity modifier: {}",
                         actor->GetDisplayFullName(), andScore);
            return andScore;
        }

        // Fall back to legacy nudity detection
        if (IsActorNudeLegacy(actor)) {
            float nudeBaseline = settings->GetNudeArousalBaseline();
            SKSE::log::trace("Actor {} using legacy nude baseline: {}", actor->GetDisplayFullName(), nudeBaseline);
            return nudeBaseline;
        }

        // Check for erotic armor as secondary fallback
        const float eroticBaseline = GetActorEroticArmorBaselineLegacy(actor);
        if (eroticBaseline > 0.0f) {
            SKSE::log::trace("Actor {} using erotic armor baseline: {}", actor->GetDisplayFullName(), eroticBaseline);
            return eroticBaseline;
        }

        return 0.0f;
    }

    std::vector<float> ANDIntegration::GetANDFactionContributions(RE::Actor* actor)
    {
        std::vector<float> contributions;

        if (!actor) {
            SKSE::log::trace("GetANDFactionContributions: Actor is null");
            return contributions;
        }

        Locker locker(m_Lock);

        if (!m_IsAvailable) {
            SKSE::log::trace("GetANDFactionContributions: A.N.D. integration not available");
            return contributions;
        }

        try {
            // Get the nudity state
            ANDNudityState nudityState = FetchActorNudityState(actor);

        // Get configurable baseline values from Settings
        const auto settings = Settings::GetSingleton();
        auto baselines = settings->GetANDFactionBaselines();

        // Initialize all contributions to 0
        contributions.reserve(8);
        float nudeContrib = 0.0f;
        float toplessContrib = 0.0f;
        float bottomlessContrib = 0.0f;
        float chestContrib = 0.0f;
        float assContrib = 0.0f;
        float genitalsContrib = 0.0f;
        float braContrib = 0.0f;
        float underwearContrib = 0.0f;

        // Calculate actual contributions based on priority rules
        // Step 1: Hard override for Nude
        if (nudityState.isNude) {
            nudeContrib = baselines.Nude;
            // When nude, all other contributions are 0
        } else {
            // Step 2: Chest region (priority: Topless > ShowingChest > ShowingBra)
            if (nudityState.isTopless) {
                toplessContrib = baselines.Topless;
                // ShowingChest and ShowingBra are overridden by Topless
            } else if (nudityState.isShowingChest) {
                chestContrib = baselines.ShowingChest;
                // ShowingBra is overridden by ShowingChest
            } else if (nudityState.isShowingBra) {
                braContrib = baselines.ShowingBra;
            }

            // Step 3: Front bottom region (priority: Bottomless > ShowingGenitals > ShowingUnderwear)
            if (nudityState.isBottomless) {
                bottomlessContrib = baselines.Bottomless;
                // ShowingGenitals and ShowingUnderwear are overridden by Bottomless
            } else if (nudityState.isShowingGenitals) {
                genitalsContrib = baselines.ShowingGenitals;
                // ShowingUnderwear is overridden by ShowingGenitals
            } else if (nudityState.isShowingUnderwear) {
                underwearContrib = baselines.ShowingUnderwear;
            }

            // Step 4: Back bottom / ass region
            if (!nudityState.isBottomless && nudityState.isShowingAss) {
                // ShowingAss only contributes if not Bottomless
                assContrib = baselines.ShowingAss;
            }

            // Step 5: Special synergy case
            // If Topless and Bottomless are both true but Nude is false, override individual contributions
            if (nudityState.isTopless && nudityState.isBottomless) {
                // Use a synergy bonus that's proportional to the configured values
                // Default was 37 when topless=20 and bottomless=30 (total 50 -> 37)
                // So we'll use 74% of the sum as the synergy value
                float synergy = (baselines.Topless + baselines.Bottomless) * 0.74f;

                // Distribute proportionally
                float sum = baselines.Topless + baselines.Bottomless;
                float toplessRatio = (sum > 0.0f) ? (baselines.Topless / sum) : 0.5f;
                toplessContrib = synergy * toplessRatio;
                bottomlessContrib = synergy * (1.0f - toplessRatio);

                // Clear other contributions that would be redundant
                chestContrib = 0.0f;
                assContrib = 0.0f;
                genitalsContrib = 0.0f;
                braContrib = 0.0f;
                underwearContrib = 0.0f;
            }
        }

        // Build the result vector in the documented order
        contributions.push_back(nudeContrib);        // [0] Nude
        contributions.push_back(toplessContrib);     // [1] Topless
        contributions.push_back(bottomlessContrib);  // [2] Bottomless
        contributions.push_back(chestContrib);       // [3] ShowingChest
        contributions.push_back(assContrib);         // [4] ShowingAss
        contributions.push_back(genitalsContrib);    // [5] ShowingGenitals
        contributions.push_back(braContrib);         // [6] ShowingBra
        contributions.push_back(underwearContrib);   // [7] ShowingUnderwear

            SKSE::log::trace("Actor {} A.N.D. faction contributions: Nude:{:.1f}, Topless:{:.1f}, Bottomless:{:.1f}, Chest:{:.1f}, Ass:{:.1f}, Genitals:{:.1f}, Bra:{:.1f}, Underwear:{:.1f}",
                         actor->GetDisplayFullName(),
                         contributions[0], contributions[1], contributions[2], contributions[3],
                         contributions[4], contributions[5], contributions[6], contributions[7]);

            return contributions;
        }
        catch (const std::exception& e) {
            SKSE::log::error("GetANDFactionContributions: Exception caught for actor {}: {}",
                         actor->GetDisplayFullName(), e.what());
            // Return empty vector on error
            return std::vector<float>();
        }
    }

    bool ANDIntegration::IsActorNudeLegacy(RE::Actor* actor) const
    {
        if (!actor) {
            return false;
        }

        // Use existing utility function for legacy nude check
        return Utilities::Actor::IsNakedCached(actor);
    }

    bool ANDIntegration::IsActorWearingEroticArmorLegacy(RE::Actor* actor) const
    {
        return GetActorEroticArmorBaselineLegacy(actor) > 0.0f;
    }

    float ANDIntegration::GetActorEroticArmorBaselineLegacy(RE::Actor* actor) const
    {
        if (!actor) {
            return 0.0f;
        }

        const auto settings = Settings::GetSingleton();
        const auto wornArmorKeywords = Utilities::Actor::GetWornArmorKeywords(actor);
        return settings->GetEroticArmorBaselineForKeywords(wornArmorKeywords);
    }
}
