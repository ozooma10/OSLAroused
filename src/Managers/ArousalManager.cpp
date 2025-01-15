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
		return SystemManager::GetSingleton()->GetArousalSystem().GetArousal(actorRef, bUpdateState);
    }

    float SetArousal(RE::Actor* actorRef, float value)
    {
		return SystemManager::GetSingleton()->GetArousalSystem().SetArousal(actorRef, value, true);
    }

    float ModifyArousal(RE::Actor* actorRef, float modValue)
    {
		return SystemManager::GetSingleton()->GetArousalSystem().ModifyArousal(actorRef, modValue, true);
    }

    float GetArousalExt(RE::Actor* actorRef)
    {
		return SystemManager::GetSingleton()->GetArousalSystem().GetArousal(actorRef, false);
    }

    float SetArousalExt(RE::Actor* actorRef, float value, bool sendevent)
    {
		return SystemManager::GetSingleton()->GetArousalSystem().SetArousal(actorRef, value, sendevent);
    }

    float ModifyArousalExt(RE::Actor* actorRef, float modValue, bool sendevent)
    {
		return SystemManager::GetSingleton()->GetArousalSystem().ModifyArousal(actorRef, modValue, sendevent);
    }
}
