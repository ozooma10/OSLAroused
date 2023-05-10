#pragma once
#include <Utilities/LRUCache.h>

namespace DeviousDeviceKeywordFormIds
{
	const static RE::FormID DeviousBelt = 0x3330;
	const static RE::FormID DeviousCollar = 0x3df7;
	const static RE::FormID DeviousLegCuffs = 0x3df8;
	const static RE::FormID DeviousArmCuffs = 0x3df9;
	const static RE::FormID DeviousBra = 0x3dfa;
	const static RE::FormID DeviousGag = 0x7eb8;
	const static RE::FormID DeviousPiercingsNipple = 0xca39;
	const static RE::FormID DeviousPiercingsVaginal = 0x23e70;
	const static RE::FormID DeviousBlindfold = 0x11b1a;
	const static RE::FormID DeviousHarness = 0x17c43;
	const static RE::FormID DeviousPlugVaginal = 0x1dd7c;
	const static RE::FormID DeviousPlugAnal = 0x1dd7d;
	const static RE::FormID DeviousCorset = 0x27f28;
	const static RE::FormID DeviousBoots = 0x27f29;
	const static RE::FormID DeviousGloves = 0x2afa1;
	const static RE::FormID DeviousHood = 0x2afa2;
	const static RE::FormID DeviousSuit = 0x2afa3;
	const static RE::FormID DeviousHeavyBondage = 0x5226c;
	const static RE::FormID DeviousBondageMittens = 0x5ff81;
}
using namespace DeviousDeviceKeywordFormIds;

namespace ToysKeywordFormIds
{
	const static RE::FormID Vaginal = 0x816;
	const static RE::FormID Hands = 0x81c;
	const static RE::FormID Feet = 0x81d;
	const static RE::FormID Mouth = 0x81e;
	const static RE::FormID Neck = 0x81f;
	const static RE::FormID Wrists = 0x820;
	const static RE::FormID Anal = 0x821;
	const static RE::FormID Pelvis = 0x822;
	const static RE::FormID Genital = 0x823;
	const static RE::FormID Nipples = 0x824;
	const static RE::FormID Legs = 0x825;
	const static RE::FormID Eyes = 0x826;
	const static RE::FormID Breasts = 0x827;
	const static RE::FormID Torso = 0x828;
	const static RE::FormID Arms = 0x829;
}


struct WornDeviceField
{
	bool Belt: 1 = false;
	bool Collar: 1 = false;
	bool LegCuffs: 1 = false;
	bool ArmCuffs: 1 = false;
	bool Bra: 1 = false;
	bool Gag: 1 = false;
	bool PiercingsNipple: 1 = false;
	bool PiercingsVaginal: 1 = false;
	bool Blindfold: 1 = false;
	bool Harness: 1 = false;
	bool PlugVaginal: 1 = false;
	bool PlugAnal: 1 = false;
	bool Corset: 1 = false;
	bool Boots: 1 = false;
	bool Gloves: 1 = false;
	bool Hood: 1 = false;
	bool Suit: 1 = false;
	bool HeavyBondage: 1 = false;
	bool BondageMittens: 1 = false;
};


//Gets worn devices, ignores equipmentToIgnore if specified (for case of unequip event partial propogation)
WornDeviceField GetWornDevices(RE::Actor* actorRef, RE::TESForm* equipmentToIgnore);
WornDeviceField GetWornDevicesCache(RE::Actor* actorRef);

class DevicesIntegration
{
public:
	static DevicesIntegration* GetSingleton()
	{
		static DevicesIntegration singleton;
		return &singleton;
	}

	DevicesIntegration() :
		m_ActorWornDeviceCache(std::function<WornDeviceField(RE::Actor*)>(GetWornDevicesCache), 100) 
	{
		Initialize();
	}

	float GetArousalBaselineFromDevices(RE::Actor* actorRef);

	WornDeviceField GetActorWornDevices(RE::Actor* actorRef);
	void ActiveEquipmentChanged(RE::Actor* actorRef, RE::TESForm* equipmentForm, bool bEquipped);

private:
	void Initialize();

	Utilities::LRUCache<RE::Actor*, WornDeviceField> m_ActorWornDeviceCache;

	uint32_t m_DDAssetsModIndex;
	uint32_t m_DDIntegrationModIndex;
	uint32_t m_ToysModIndex;

	RE::TESForm* equipmentForm;

public:
	RE::FormID DDBelt;
	RE::FormID DDCollar;
	RE::FormID DDLegCuffs;
	RE::FormID DDArmCuffs;
	RE::FormID DDBra;
	RE::FormID DDGag;
	RE::FormID DDPiercingsNipple;
	RE::FormID DDPiercingsVaginal;
	RE::FormID DDBlindfold;
	RE::FormID DDHarness;
	RE::FormID DDPlugVaginal;
	RE::FormID DDPlugAnal;
	RE::FormID DDCorset;
	RE::FormID DDBoots;
	RE::FormID DDGloves;
	RE::FormID DDHood;
	RE::FormID DDSuit;
	RE::FormID DDHeavyBondage;
	RE::FormID DDBondageMittens;

	RE::FormID ToysVaginal;
	RE::FormID ToysHands;
	RE::FormID ToysFeet;
	RE::FormID ToysMouth;
	RE::FormID ToysNeck;
	RE::FormID ToysWrists;
	RE::FormID ToysAnal;
	RE::FormID ToysPelvis;
	RE::FormID ToysGenital;
	RE::FormID ToysNipples;
	RE::FormID ToysLegs;
	RE::FormID ToysEyes;
	RE::FormID ToysBreasts;
	RE::FormID ToysTorso;
	RE::FormID ToysArms;
};
