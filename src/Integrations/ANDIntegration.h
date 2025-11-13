#pragma once

#include "../PCH.h"
#include "../Utilities/Ticker.h"
#include "../Utilities/LRUCache.h"

namespace Integrations
{
    /**
     * Integration with Advanced Nudity Detection (A.N.D.) mod
     *
     * This class provides integration with the A.N.D. mod, which uses factions
     * to track detailed nudity states. It calculates a nudity score (0-50) based
     * on which A.N.D. factions an actor belongs to, and converts that to an
     * arousal baseline modifier.
     *
     * The integration gracefully falls back to simple nudity detection if A.N.D.
     * is not present in the load order.
     */
    class ANDIntegration
    {
    public:
        struct ANDNudityState
        {
            bool isNude = false;
            bool isTopless = false;
            bool isBottomless = false;
            bool isShowingChest = false;
            bool isShowingAss = false;
            bool isShowingGenitals = false;
            bool isShowingBra = false;
            bool isShowingUnderwear = false;
            float calculatedScore = 0.0f;
        };

        [[nodiscard]] static ANDIntegration *GetSingleton()
        {
            static ANDIntegration singleton;
            return &singleton;
        }

        bool Initialize();

        /**
         * @brief Checks if A.N.D. integration is available and enabled
         */
        [[nodiscard]] bool IsAvailable() const { return m_IsAvailable; }

        /**
         * @brief Calculates nudity score (0-50) based on A.N.D. faction membership
         *
         * Score calculation follows these rules:
         * - Nude faction: Returns 50 immediately (hard override)
         * - Chest region (priority): Topless(20) > ShowingChest(12) > ShowingBra(8)
         * - Front bottom (priority): Bottomless(30) > ShowingGenitals(15) > ShowingUnderwear(8)
         * - Back bottom: ShowingAss(8) only if not Bottomless
         * - Special synergy: Topless + Bottomless (without Nude) = 37
         *
         * @param actor The actor to check
         * @return Nudity score between 0 and 50
         */
        [[nodiscard]] float GetANDNudityScore(RE::Actor *actor);

        /**
         * @brief Gets the arousal baseline modifier based on nudity state
         *
         * Uses A.N.D. integration if available and enabled, otherwise falls back
         * to legacy nudity/erotic armor detection.
         *
         * @param actor The actor to check
         * @return Arousal baseline modifier (typically 0-30)
         */
        [[nodiscard]] float GetNudityBaselineModifier(RE::Actor *actor);

        /**
         * @brief Gets the cached A.N.D. nudity state for an actor
         * @param actor The actor to check
         * @return The cached nudity state
         */
        [[nodiscard]] ANDNudityState GetActorNudityState(RE::Actor *actor);

        /**
         * @brief Invalidates the nudity state cache for an actor
         * @param actor The actor whose cache should be invalidated
         */
        void InvalidateActorCache(RE::Actor *actor);

        /**
         * @brief Clears all cached nudity states
         */
        void ClearCache();

    private:
        ANDIntegration(): m_ActorNudityCache(std::function<ANDNudityState(RE::Actor*)>([this](RE::Actor* actor) {
              return FetchActorNudityState(actor);
          }), 100) {}
        ~ANDIntegration() = default;
        ANDIntegration(const ANDIntegration &) = delete;
        ANDIntegration &operator=(const ANDIntegration &) = delete;
        ANDIntegration(ANDIntegration &&) = delete;
        ANDIntegration &operator=(ANDIntegration &&) = delete;

        /**
         * @brief Fetches the current A.N.D. faction state for an actor
         * @param actor The actor to check
         * @return The nudity state based on faction membership
         */
        [[nodiscard]] ANDNudityState FetchActorNudityState(RE::Actor *actor);

        /**
         * @brief Checks if the actor is wearing erotic armor (legacy)
         * @param actor The actor to check
         * @return true if wearing armor with erotic keywords
         */
        [[nodiscard]] bool IsActorWearingEroticArmorLegacy(RE::Actor *actor) const;

        /**
         * @brief Checks if the actor is nude (legacy)
         * @param actor The actor to check
         * @return true if not wearing body armor
         */
        [[nodiscard]] bool IsActorNudeLegacy(RE::Actor *actor) const;

        // A.N.D. Faction FormIDs (resolved at initialization)
        RE::TESFaction *m_ANDNudeFaction = nullptr;
        RE::TESFaction *m_ANDToplessFaction = nullptr;
        RE::TESFaction *m_ANDBottomlessFaction = nullptr;
        RE::TESFaction *m_ANDShowingChestFaction = nullptr;
        RE::TESFaction *m_ANDShowingAssFaction = nullptr;
        RE::TESFaction *m_ANDShowingGenitalsFaction = nullptr;
        RE::TESFaction *m_ANDShowingBraFaction = nullptr;
        RE::TESFaction *m_ANDShowingUnderwearFaction = nullptr;

        // Base FormIDs for A.N.D. factions (before resolution)
        // These will be configured based on the actual A.N.D. mod structure
        // For now, using placeholder values that will be updated when A.N.D. FormIDs are known
        static constexpr uint32_t AND_MOD_FORM_BASE = 0x800; // Placeholder base
        static constexpr uint32_t AND_NUDE_FACTION_ID = AND_MOD_FORM_BASE;
        static constexpr uint32_t AND_TOPLESS_FACTION_ID = AND_MOD_FORM_BASE + 1;
        static constexpr uint32_t AND_BOTTOMLESS_FACTION_ID = AND_MOD_FORM_BASE + 2;
        static constexpr uint32_t AND_SHOWING_CHEST_FACTION_ID = AND_MOD_FORM_BASE + 3;
        static constexpr uint32_t AND_SHOWING_ASS_FACTION_ID = AND_MOD_FORM_BASE + 4;
        static constexpr uint32_t AND_SHOWING_GENITALS_FACTION_ID = AND_MOD_FORM_BASE + 5;
        static constexpr uint32_t AND_SHOWING_BRA_FACTION_ID = AND_MOD_FORM_BASE + 6;
        static constexpr uint32_t AND_SHOWING_UNDERWEAR_FACTION_ID = AND_MOD_FORM_BASE + 7;

        // Baseline contribution values for each nudity state
        static constexpr float NUDE_BASELINE = 50.0f;
        static constexpr float TOPLESS_BASELINE = 20.0f;
        static constexpr float BOTTOMLESS_BASELINE = 30.0f;
        static constexpr float SHOWING_CHEST_BASELINE = 12.0f;
        static constexpr float SHOWING_GENITALS_BASELINE = 15.0f;
        static constexpr float SHOWING_ASS_BASELINE = 8.0f;
        static constexpr float SHOWING_BRA_BASELINE = 8.0f;
        static constexpr float SHOWING_UNDERWEAR_BASELINE = 8.0f;

        // Special synergy score for Topless + Bottomless without Nude
        static constexpr float TOPLESS_BOTTOMLESS_SYNERGY = 37.0f;

        // Cache for actor nudity states (100 entries like other integrations)
        Utilities::LRUCache<RE::Actor *, ANDNudityState> m_ActorNudityCache;

        // Integration availability flag
        bool m_IsAvailable = false;

        // Mod index for A.N.D. (resolved at initialization)
        uint16_t m_ANDModIndex = 0xFFFF;

        // Thread safety
        mutable std::recursive_mutex m_Lock;
        using Locker = std::lock_guard<std::recursive_mutex>;
    };
}