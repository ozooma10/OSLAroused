#pragma once

namespace PapyrusActor
{
	std::vector<RE::TESForm*> GetAllEquippedArmor(RE::StaticFunctionTag* base, RE::Actor* actorRef);

	std::vector<int> GetActiveDeviceTypeIds(RE::StaticFunctionTag* base, RE::Actor* actorRef);


	bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);
};
