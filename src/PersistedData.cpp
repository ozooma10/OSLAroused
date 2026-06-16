#include "PersistedData.h"
#include "Managers/SceneManager.h"

#include <algorithm>
#include <cmath>

namespace PersistedData
{
	//BaseData is based off how powerof3's did it in Afterlife
	template <typename T>
	bool BaseData<T>::Save(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version)
	{
		if (!serializationInterface->OpenRecord(type, version)) {
			SKSE::log::error("Failed to open record for Data Serialization!");
			return false;
		}

		return Save(serializationInterface);
	}

	template <typename T>
	bool BaseData<T>::Save(SKSE::SerializationInterface* serializationInterface)
	{
		assert(serializationInterface);
		Locker locker(m_Lock);

		const auto numRecords = m_Data.size();
		if (!serializationInterface->WriteRecordData(numRecords)) {
			SKSE::log::error("Failed to save {} data records", numRecords);
			return false;
		}

		for (const auto& [formId, value] : m_Data) {
			if (!serializationInterface->WriteRecordData(formId)) {
				SKSE::log::error("Failed to save data for FormID: ({:X})", formId);
				return false;
			}

			if (!serializationInterface->WriteRecordData(value)) {
				SKSE::log::error("Failed to save value data for form: {}", formId);
				return false;
			}
		}
		return true;
	}

	template <typename T>
	bool BaseData<T>::Load(SKSE::SerializationInterface* serializationInterface)
	{
		assert(serializationInterface);

		std::size_t recordDataSize;
		serializationInterface->ReadRecordData(recordDataSize);

		Locker locker(m_Lock);
		m_Data.clear();

		RE::FormID formId;
		T value;

		for (auto i = 0; i < recordDataSize; i++) {
			serializationInterface->ReadRecordData(formId);
			//Ensure form still exists
			RE::FormID fixedId;
			if (!serializationInterface->ResolveFormID(formId, fixedId)) {
				SKSE::log::error("Failed to resolve formID {} {}"sv, formId, fixedId);
				continue;
			}

			serializationInterface->ReadRecordData(value);
			m_Data[fixedId] = value;
		}
		return true;
	}

	template <typename T>
	void BaseData<T>::Clear()
	{
		Locker locker(m_Lock);
		m_Data.clear();
	}

	bool BaseFormArrayData::Save(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version)
	{
		if (!serializationInterface->OpenRecord(type, version)) {
			SKSE::log::error("Failed to open record for Data Serialization!");
			return false;
		}

		return Save(serializationInterface);
	}

	bool BaseFormArrayData::Save(SKSE::SerializationInterface* serializationInterface)
	{
		assert(serializationInterface);
		Locker locker(m_Lock);

		const auto numRecords = m_Data.size();
		if (!serializationInterface->WriteRecordData(numRecords)) {
			SKSE::log::error("Failed to save {} data records", numRecords);
			return false;
		}

		for (const auto& [formId, formList] : m_Data) {
			if (!serializationInterface->WriteRecordData(formId)) {
				SKSE::log::error("Failed to save data for FormID: ({:X})", formId);
				return false;
			}

			const auto numForms = formList.size();
			if (!serializationInterface->WriteRecordData(numForms)) {
				SKSE::log::error("Failed to save {} sub form list records", numForms);
				return false;
			}

			for (const auto& subFormId : formList) {
				if (!serializationInterface->WriteRecordData(subFormId)) {
					SKSE::log::error("Failed to save data for sub FormID: ({})", subFormId);
					return false;
				}
			}
		}
		return true;
	}

	bool BaseFormArrayData::Load(SKSE::SerializationInterface* serializationInterface)
	{
		assert(serializationInterface);

		std::size_t recordDataSize;
		serializationInterface->ReadRecordData(recordDataSize);

		Locker locker(m_Lock);
		m_Data.clear();

		RE::FormID formId;
		std::size_t subFormIdCount;
		RE::FormID subFormId;

		for (auto i = 0; i < recordDataSize; i++) {
			serializationInterface->ReadRecordData(formId);
			//Ensure form still exists
			RE::FormID fixedId;
			if (!serializationInterface->ResolveFormID(formId, fixedId)) {
				SKSE::log::error("Failed to resolve formID {} {}"sv, formId, fixedId);
				continue;
			}

			std::set<RE::FormID> subFormIds;
			serializationInterface->ReadRecordData(subFormIdCount);
			for (auto subFormIndex = 0; subFormIndex < subFormIdCount; subFormIndex++) {
				serializationInterface->ReadRecordData(subFormId);
				subFormIds.insert(subFormId);
			}
			m_Data[formId] = subFormIds;
		}
		return true;
	}

	std::string DecodeTypeCode(std::uint32_t typeCode)
	{
		char buf[4];
		buf[3] = char(typeCode);
		buf[2] = char(typeCode >> 8);
		buf[1] = char(typeCode >> 16);
		buf[0] = char(typeCode >> 24);
		return std::string(buf, buf + 4);
	}

	void SaveCallback(SKSE::SerializationInterface* serializationInterface)
	{
		const auto arousalData = ArousalData::GetSingleton();
		if (!arousalData->Save(serializationInterface, kArousalDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Arousal Data");
		}

		const auto baseLibidoData = BaseLibidoData::GetSingleton();
		if (!baseLibidoData->Save(serializationInterface, kBaseLibidoDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Base Libido Data");
		}

		const auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
		if (!arousalMultiplierData->Save(serializationInterface, kAroualMultiplierDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Arousal Multiplier Data");
		}

		const auto lastCheckData = LastCheckTimeData::GetSingleton();
		if (!lastCheckData->Save(serializationInterface, kLastCheckTimeDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Arousal Last Check Time Data");
		}

		const auto lastOrgasmData = LastOrgasmTimeData::GetSingleton();
		if (!lastOrgasmData->Save(serializationInterface, kLastOrgasmTimeDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Last Orgasm Time Data");
		}

		const auto armorKeywordData = ArmorKeywordData::GetSingleton();
		if (!armorKeywordData->Save(serializationInterface, kArmorKeywordDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Armor Keyword Data");
		}

		const auto isArousalLockedData = IsArousalLockedData::GetSingleton();
		if (!isArousalLockedData->Save(serializationInterface, kIsArousalLockedDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Is Arousal Locked Data");
		}

		const auto isActorExhibitionistData = IsActorExhibitionistData::GetSingleton();
		if (!isActorExhibitionistData->Save(serializationInterface, kIsActorExhibitionistDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Is Actor Exhibitionist Data");
		}

		const auto settingsData = SettingsData::GetSingleton();
		if (!settingsData->Save(serializationInterface, kSettingsDataKey, kSerializationVersion)) {
			SKSE::log::critical("Failed to save Settings Data");
		}

		SKSE::log::trace("OSLArousal Data Saved");
	}

	void LoadCallback(SKSE::SerializationInterface* serializationInterface)
	{
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		SKSE::log::trace("OSLArousal Load Start");

		while (serializationInterface->GetNextRecordInfo(type, version, length)) {
			SKSE::log::trace("Trying Load for {}", DecodeTypeCode(type));

			if (version != kSerializationVersion) {
				SKSE::log::critical("Loaded data has incorrect version. Recieved ({}) - Expected ({}) for Data Key ({})"sv, version, kSerializationVersion, DecodeTypeCode(type));
				continue;
			}

			switch (type) {
			case kArousalDataKey:
				{
					auto arousalData = ArousalData::GetSingleton();
					if (!arousalData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load Arousal Data"sv);
					}
				}
				break;
			case kBaseLibidoDataKey:
				{
					auto baseLibidoData = BaseLibidoData::GetSingleton();
					if (!baseLibidoData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load Base Libido Data"sv);
					}
				}
				break;
			case kAroualMultiplierDataKey:
				{
					auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
					if (!arousalMultiplierData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load Arousal Multiplier Data"sv);
					}
				}
				break;
			case kLastCheckTimeDataKey:
				{
					auto lastCheckData = LastCheckTimeData::GetSingleton();
					if (!lastCheckData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load LastCheckTime Data"sv);
					}
				}
				break;
			case kLastOrgasmTimeDataKey:
				{
					auto lastOrgasmData = LastOrgasmTimeData::GetSingleton();
					if (!lastOrgasmData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load LastOrgasm Data"sv);
					}
				}
				break;
			case kArmorKeywordDataKey:
				{
					auto armorKeywordData = ArmorKeywordData::GetSingleton();
					if (!armorKeywordData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load armorKeywordData Data"sv);
					}
				}
				break;
			case kIsArousalLockedDataKey:
				{
					auto isArousalLockedData = IsArousalLockedData::GetSingleton();
					if (!isArousalLockedData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load IsArousalLocked Data"sv);
					}
				}
				break;
			case kIsActorExhibitionistDataKey:
				{
					auto isActorExhibitionistData = IsActorExhibitionistData::GetSingleton();
					if (!isActorExhibitionistData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load IsActorExhibitionist Data"sv);
					}
				}
				break;
			case kSettingsDataKey:
				{
					auto settingsData = SettingsData::GetSingleton();
					if (!settingsData->Load(serializationInterface)) {
						SKSE::log::critical("Failed to Load Settings Data"sv);
					}
				}
				break;
			default:
				SKSE::log::critical("Unrecognized Record Type: {}"sv, DecodeTypeCode(type));
				break;
			}
		}

		//Self-heal any corrupt/stuck persisted values before they reach the arousal system
		SanitizeLoadedData();

		SKSE::log::trace("OSLArousal Data loaded");
	}

	void SanitizeLoadedData()
	{
		// Arousal and base libido are always in [0, 100]; a non-finite (NaN/inf) value would propagate through every calculation, so reset it to 0.
		const auto clampScore = [](float value) -> float {
			if (!std::isfinite(value)) {
				return 0.f;
			}
			return std::clamp(value, 0.f, 100.f);
		};

		std::size_t corrected = 0;
		corrected += ArousalData::GetSingleton()->SanitizeValues(clampScore);
		corrected += BaseLibidoData::GetSingleton()->SanitizeValues(clampScore);

		// A multiplier of 0 (or negative/non-finite) is the classic "arousal is stuck" pathology: every ModifyArousal gain is multiplied to nothing.
		// Reset those to the mode-appropriate neutral default and clamp the rest to a sane range.
		// SettingsData stores the mode as a raw int where 0 == OSL (see SettingsData::GetArousalMode).
		// OSL's neutral multiplier is 1.0; SLA's neutral ExposureRate default is 2.0.
		const bool bSLAMode = SettingsData::GetSingleton()->GetArousalMode() != 0;
		const float defaultMultiplier = bSLAMode ? 2.f : 1.f;
		corrected += ArousalMultiplierData::GetSingleton()->SanitizeValues([&](float value) -> float {
			if (!std::isfinite(value) || value <= 0.f) {
				return defaultMultiplier;
			}
			return std::clamp(value, 0.01f, 100.f);
		});

		// Timestamps are game-time-in-days snapshots; a negative or non-finite value means "no valid record" which we represent as 0 
		// (treated as the epoch / "never" elsewhere).
		const auto clampTimestamp = [](float value) -> float {
			return std::isfinite(value) && value >= 0.f ? value : 0.f;
		};
		corrected += LastCheckTimeData::GetSingleton()->SanitizeValues(clampTimestamp);
		corrected += LastOrgasmTimeData::GetSingleton()->SanitizeValues(clampTimestamp);

		if (corrected > 0) {
			SKSE::log::warn("SanitizeLoadedData repaired {} corrupt/stuck persisted value(s) on load", corrected);
		} else {
			SKSE::log::trace("SanitizeLoadedData: all persisted values within expected ranges");
		}
	}

	void RevertCallback(SKSE::SerializationInterface*)
	{
		auto arousalData = ArousalData::GetSingleton();
		arousalData->Clear();
		auto baseLibidoData = BaseLibidoData::GetSingleton();
		baseLibidoData->Clear();
		auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
		arousalMultiplierData->Clear();
		auto lastCheckData = LastCheckTimeData::GetSingleton();
		lastCheckData->Clear();
		auto lastOrgasmData = LastOrgasmTimeData::GetSingleton();
		lastOrgasmData->Clear();
		auto armorKeywordData = ArmorKeywordData::GetSingleton();
		armorKeywordData->Clear();
		auto isArousalLockedData = IsArousalLockedData::GetSingleton();
		isArousalLockedData->Clear();
		auto isActorExhibitionistData = IsActorExhibitionistData::GetSingleton();
		isActorExhibitionistData->Clear();
		auto settingsData = SettingsData::GetSingleton();
		settingsData->Clear();

		//End All Scenes as well
		SceneManager::GetSingleton()->ClearScenes();

		SKSE::log::trace("Reverting Data State...");
	}

	void ResetSystemForModeSwitch()
	{
		SKSE::log::info("Resetting System for Mode Switch...");
		//Reset relevant data for mode switch
		auto arousalData = ArousalData::GetSingleton();
		arousalData->Clear();

		auto baseLibidoData = BaseLibidoData::GetSingleton();
		baseLibidoData->Clear();

		auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
		arousalMultiplierData->Clear();

		auto lastCheckData = LastCheckTimeData::GetSingleton();
		lastCheckData->Clear();
	}
}
