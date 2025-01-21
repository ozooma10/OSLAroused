#include "ArousalManager.h"
#include "PersistedData.h"
#include "Utilities/Utils.h"
#include "Papyrus/Papyrus.h"
#include "Settings.h"

using namespace PersistedData;

namespace ArousalManager
{
    float GetArousal(RE::Actor* actorRef, bool bUpdateState)
    {
		return GetSingleton()->GetArousalSystem().GetArousal(actorRef, bUpdateState);
    }

    float SetArousal(RE::Actor* actorRef, float value)
    {
		return GetSingleton()->GetArousalSystem().SetArousal(actorRef, value, true);
    }

    float ModifyArousal(RE::Actor* actorRef, float modValue)
    {
		return GetSingleton()->GetArousalSystem().ModifyArousal(actorRef, modValue, true);
    }

    float GetArousalExt(RE::Actor* actorRef)
    {
		return GetSingleton()->GetArousalSystem().GetArousal(actorRef, false);
    }

    float SetArousalExt(RE::Actor* actorRef, float value, bool sendevent)
    {
		return GetSingleton()->GetArousalSystem().SetArousal(actorRef, value, sendevent);
    }

    float ModifyArousalExt(RE::Actor* actorRef, float modValue, bool sendevent)
    {
		return GetSingleton()->GetArousalSystem().ModifyArousal(actorRef, modValue, sendevent);
    }

    ArousalManager::ArousalManager()
    {
        auto arousalMode = (IArousalSystem::ArousalMode)PersistedData::SettingsData::GetSingleton()->GetArousalMode();
		logger::trace("ArousalManager::ArousalManager Initializing with {}", (int)arousalMode);
        if (arousalMode == IArousalSystem::ArousalMode::kOSL)
        {
            m_pArousalSystem = std::make_unique<ArousalSystemOSL>();
        }
        else
        {
            m_pArousalSystem = std::make_unique<ArousalSystemSLA>();
        }
    }
}
