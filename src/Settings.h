#pragma once
#include "Integrations/ANDFactionIndices.h"

#include <set>
#include <unordered_map>

using Lock = std::recursive_mutex;
using Locker = std::lock_guard<Lock>;

enum DeviceType
{
	Belt = 0,
	Collar,
	LegCuffs,
	ArmCuffs,
	Bra,
	Gag,
	PiercingsNipple,
	PiercingsVaginal,
	Blindfold,
	Harness,
	PlugVaginal,
	PlugAnal,
	Corset,
	Boots,
	Gloves,
	Hood,
	Suit,
	HeavyBondage,
	BondageMittens
};

struct DeviceArousalBaselineChange
{
	float Belt = 20;
	float Collar = 5;
	float LegCuffs = 5;
	float ArmCuffs = 5;
	float Bra = 10;
	float Gag = 10;
	float PiercingsNipple = 10;
	float PiercingsVaginal = 10;
	float Blindfold = 5;
	float Harness = 10;
	float PlugVaginal = 20;
	float PlugAnal = 20;
	float Corset = 10;
	float Boots = 5;
	float Gloves = 5;
	float Hood = 0;
	float Suit = 5;
	float HeavyBondage = 10;
	float BondageMittens = 10;
};


class Settings
{
public:
	// SLA decay base for exponential decay calculations
	// Used in formula: exposure * pow(kSLADecayBase, -time/halfLife)
	// Value 1.5 means exposure reduces by factor of 1.5^(-1) = 0.667 per half-life
	static constexpr float kSLADecayBase = 1.5f;

	static Settings* GetSingleton()
	{
		static Settings singleton;
		return &singleton;
	}

	void SetMinLibidoValue(bool bPlayerVal, float newVal)
	{
		Locker locker(m_Lock);
		if (bPlayerVal) {
			m_PlayerMinLibidoValue = newVal;
		} else {
			m_NPCMinLibidoValue= newVal;
		}
	}
	float GetMinLibidoValue(bool bPlayerVal) const
	{
		Locker locker(m_Lock);
		return bPlayerVal ? m_PlayerMinLibidoValue : m_NPCMinLibidoValue;
	}

	void SetArousalChangeRate(float newVal)
	{
		Locker locker(m_Lock);
		m_ArousalChangeRate = 1.f - (newVal / 100);
	}
	float GetArousalChangeRate() const
	{
		Locker locker(m_Lock);
		return m_ArousalChangeRate;
	}

	void SetLibidoChangeRate(float newVal)
	{
		Locker locker(m_Lock);
		m_LibidoChangeRate = 1.f - (newVal / 100);
	}
	float GetLibidoChangeRate() const
	{
		Locker locker(m_Lock);
		return m_LibidoChangeRate;
	}

	void SetNudeArousalBaseline(float newVal) { 
		Locker locker(m_Lock);
		m_IsNudeBaseline = newVal; 
	}
	float GetNudeArousalBaseline() const { 
		Locker locker(m_Lock);
		return m_IsNudeBaseline;
	}

	void SetNudeViewingBaseline(float newVal)
	{
		Locker locker(m_Lock);
		m_ViewingNudeBaseline = newVal;
	}
	float GetNudeViewingBaseline() const
	{
		Locker locker(m_Lock);
		return m_ViewingNudeBaseline;
	}

	void SetSceneParticipantBaseline(float newVal)
	{
		Locker locker(m_Lock);
		m_SceneParticipateBaseline = newVal;
	}
	float GetSceneParticipantBaseline() const
	{
		Locker locker(m_Lock);
		return m_SceneParticipateBaseline;
	}

	void SetSceneViewingBaseline(float newVal)
	{
		Locker locker(m_Lock);
		m_SceneViewingBaseline = newVal;
	}
	float GetSceneViewingBaseline() const
	{
		Locker locker(m_Lock);
		return m_SceneViewingBaseline;
	}

	void SetSceneVictimGainsArousal(bool newVal)
	{
		Locker locker(m_Lock);
		m_SceneVictimGainsArousal = newVal;
	}
	bool GetSceneVictimGainsArousal() const
	{
		Locker locker(m_Lock);
		return m_SceneVictimGainsArousal;
	}

	void SetScanDistance(float newVal)
	{
		Locker locker(m_Lock);
		m_ScanDistance = newVal;
	}
	float GetScanDistance() const
	{
		Locker locker(m_Lock);
		return m_ScanDistance;
	}

	void SetDeviceBaseline(DeviceArousalBaselineChange newVal)
	{
		Locker locker(m_Lock);
		m_DeviceBaseline = newVal;
	}
	DeviceArousalBaselineChange GetDeviceBaseline() const
	{
		Locker locker(m_Lock);
		return m_DeviceBaseline;
	}

	void SetEroticArmorBaseline(float newVal, RE::BGSKeyword* keyword)
	{
		if (!keyword) {
			return;
		}
		SetEroticArmorBaseline(newVal, keyword->formID);
	}

	void SetEroticArmorBaseline(float newVal, RE::FormID keywordFormId)
	{
		Locker locker(m_Lock);
		m_EroticArmorBaselines[keywordFormId] = newVal;
	}

	float GetEroticArmorBaseline(RE::BGSKeyword* keyword) const
	{
		return keyword ? GetEroticArmorBaseline(keyword->formID) : 0.0f;
	}

	float GetEroticArmorBaseline(RE::FormID keywordFormId) const
	{
		Locker locker(m_Lock);
		if (const auto it = m_EroticArmorBaselines.find(keywordFormId); it != m_EroticArmorBaselines.end()) {
			return it->second;
		}
		return 0.0f;
	}

	std::unordered_map<RE::FormID, float> GetEroticArmorBaselines() const
	{
		Locker locker(m_Lock);
		return m_EroticArmorBaselines;
	}

	float GetEroticArmorBaselineForKeywords(const std::set<RE::FormID>& keywordFormIds) const
	{
		Locker locker(m_Lock);
		float totalBaseline = 0.0f;
		for (const auto keywordFormId : keywordFormIds) {
			if (const auto it = m_EroticArmorBaselines.find(keywordFormId); it != m_EroticArmorBaselines.end()) {
				totalBaseline += it->second;
			}
		}
		return totalBaseline;
	}

	void ClearEroticArmorBaselines()
	{
		Locker locker(m_Lock);
		m_EroticArmorBaselines.clear();
	}

	// A.N.D. Integration Settings
	void SetUseANDIntegration(bool newVal)
	{
		Locker locker(m_Lock);
		m_UseANDIntegration = newVal;
	}
	bool GetUseANDIntegration() const
	{
		Locker locker(m_Lock);
		return m_UseANDIntegration;
	}

	// A.N.D. Individual faction baseline values
	struct ANDFactionBaselines {
		float Nude = 50.0f;           
		float Topless = 20.0f;        
		float Bottomless = 30.0f;     
		float ShowingChest = 12.0f;   
		float ShowingAss = 8.0f;      
		float ShowingGenitals = 15.0f;
		float ShowingBra = 8.0f;      
		float ShowingUnderwear = 8.0f;
	};

	void SetANDFactionBaselines(const ANDFactionBaselines& baselines)
	{
		Locker locker(m_Lock);
		m_ANDFactionBaselines = baselines;
	}

	ANDFactionBaselines GetANDFactionBaselines() const
	{
		Locker locker(m_Lock);
		return m_ANDFactionBaselines;
	}

	void SetANDFactionBaseline(int index, float value);

	float GetANDFactionBaseline(int index) const
	{
		using namespace Integrations::ANDFactionIndex;
		Locker locker(m_Lock);
		switch (index) {
		case NUDE: return m_ANDFactionBaselines.Nude;
		case TOPLESS: return m_ANDFactionBaselines.Topless;
		case BOTTOMLESS: return m_ANDFactionBaselines.Bottomless;
		case SHOWING_CHEST: return m_ANDFactionBaselines.ShowingChest;
		case SHOWING_ASS: return m_ANDFactionBaselines.ShowingAss;
		case SHOWING_GENITALS: return m_ANDFactionBaselines.ShowingGenitals;
		case SHOWING_BRA: return m_ANDFactionBaselines.ShowingBra;
		case SHOWING_UNDERWEAR: return m_ANDFactionBaselines.ShowingUnderwear;
		default: return 0.0f;
		}
	}

	float GetTimeRateHalfLife() const
	{
		Locker locker(m_Lock);
		return m_TimeRateHalfLife;
	}
	void SetTimeRateHalfLife(float newVal)
	{
		newVal = std::clamp(newVal, 0.f, 10.f);
		Locker locker(m_Lock);
		m_TimeRateHalfLife = newVal;
	}

	float GetDefaultExposureRate() const
	{
		Locker locker(m_Lock);
		return m_DefaultExposureRate;
	}


	void SetDefaultExposureRate(float newVal)
	{
		newVal = std::clamp(newVal, 0.f, 10.f);
		Locker locker(m_Lock);
		m_DefaultExposureRate = newVal;
	}

	float GetArousalUpdateInterval() {
		return m_ArousalUpdateInterval;
	}

	// Arousal the player gains when finishing a (non-interrupted) sleep.
	// 0 disables the sleep effect entirely.
	void SetSleepArousalGain(float newVal)
	{
		Locker locker(m_Lock);
		m_SleepArousalGain = std::clamp(newVal, 0.f, 100.f);
	}
	float GetSleepArousalGain() const
	{
		Locker locker(m_Lock);
		return m_SleepArousalGain;
	}

	// Direct, lasting arousal an OSL-mode observer gains per update interval while
	// witnessing a naked actor (on top of the passive viewing-baseline lift).
	// Scaled by elapsed time, nudity level and gender preference. 0 disables.
	void SetSpectatorArousalGain(float newVal)
	{
		Locker locker(m_Lock);
		m_SpectatorArousalGain = std::clamp(newVal, 0.f, 100.f);
	}
	float GetSpectatorArousalGain() const
	{
		Locker locker(m_Lock);
		return m_SpectatorArousalGain;
	}

private:

	float m_ArousalUpdateInterval = 0.1f;

	float m_PlayerMinLibidoValue = 30.f;
	float m_NPCMinLibidoValue = 80.f;
	float m_ArousalChangeRate = 0.2f;
	float m_LibidoChangeRate = 0.1f;

	float m_IsNudeBaseline = 30.f;
	float m_ViewingNudeBaseline = 20.f;

	float m_SceneParticipateBaseline = 50.f;
	float m_SceneViewingBaseline = 30.f;
	bool m_SceneVictimGainsArousal = false;

	float m_ScanDistance = 5120.f;

	DeviceArousalBaselineChange m_DeviceBaseline;

	std::unordered_map<RE::FormID, float> m_EroticArmorBaselines;

	// A.N.D. Integration settings
	bool m_UseANDIntegration = true;  // Default true if A.N.D. is present
	ANDFactionBaselines m_ANDFactionBaselines;  // Individual faction baseline values

	//SLA property
	float m_TimeRateHalfLife = 2.f;
	float m_DefaultExposureRate = 2.f;

	// Sleep effect (player only). 0 disables.
	float m_SleepArousalGain = 10.f;

	// OSL-mode direct spectator arousal gain per interval. 0 disables.
	float m_SpectatorArousalGain = 2.f;

	mutable Lock m_Lock;
};
