#pragma once

#include "Managers/LibidoManager.h"

namespace ArousalManager
{
	float GetArousal(RE::Actor* actorRef, bool bUpdateState = true);
	float SetArousal(RE::Actor* actorRef, float value);
	float ModifyArousal(RE::Actor* actorRef, float value);

	float CalculateArousal(RE::Actor* actorRef, float timePassed);

	//modified functions to allow paralel proc from different plugin
	extern "C" DLLEXPORT float GetArousalExt(RE::Actor* actorRef);
	extern "C" DLLEXPORT float SetArousalExt(RE::Actor* actorRef, float value, bool sendevent);
	extern "C" DLLEXPORT float ModifyArousalExt(RE::Actor* actorRef, float value,bool sendevent);
}
