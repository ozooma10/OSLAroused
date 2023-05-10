#pragma once
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
		Locker locker(m_Lock);
		m_EroticArmorBaseline = newVal;
		m_EroticArmorKeyword = keyword;
	}
	float GetEroticArmorBaseline() const
	{
		Locker locker(m_Lock);
		return m_EroticArmorBaseline;
	}
	RE::BGSKeyword* GetEroticArmorKeyword() const
	{
		Locker locker(m_Lock);
		return m_EroticArmorKeyword;
	}

private:
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

	float m_EroticArmorBaseline = 20.f;
	RE::BGSKeyword* m_EroticArmorKeyword = nullptr;

	mutable Lock m_Lock;
};
