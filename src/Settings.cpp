#include "PCH.h"
#include "Settings.h"
#include "Config.h"
#include "Integrations/ANDFactionIndices.h"

void Settings::SetANDFactionBaseline(int index, float value)
{
    using namespace Integrations::ANDFactionIndex;

    // Validate index first
    if (!IsValidIndex(index)) {
        SKSE::log::warn("SetANDFactionBaseline: Invalid faction index {} (valid range: 0-{})",
                     index, COUNT - 1);
        return;
    }

    // Clamp value to valid range
    const float clampedValue = std::clamp(value, 0.0f, 100.0f);
    if (value != clampedValue) {
        SKSE::log::debug("SetANDFactionBaseline: Clamping {} value from {} to {}",
                      GetFactionName(index), value, clampedValue);
    }

    {
        Locker locker(m_Lock);
        switch (index) {
        case NUDE: m_ANDFactionBaselines.Nude = clampedValue; break;
        case TOPLESS: m_ANDFactionBaselines.Topless = clampedValue; break;
        case BOTTOMLESS: m_ANDFactionBaselines.Bottomless = clampedValue; break;
        case SHOWING_CHEST: m_ANDFactionBaselines.ShowingChest = clampedValue; break;
        case SHOWING_ASS: m_ANDFactionBaselines.ShowingAss = clampedValue; break;
        case SHOWING_GENITALS: m_ANDFactionBaselines.ShowingGenitals = clampedValue; break;
        case SHOWING_BRA: m_ANDFactionBaselines.ShowingBra = clampedValue; break;
        case SHOWING_UNDERWEAR: m_ANDFactionBaselines.ShowingUnderwear = clampedValue; break;
        }
    }

    // Save to INI file
    Config::GetSingleton()->SaveANDFactionBaseline(index, clampedValue);
    SKSE::log::debug("SetANDFactionBaseline: {} baseline set to {}",
                  GetFactionName(index), clampedValue);
}