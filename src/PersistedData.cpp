#include "PersistedData.h"
#include "Managers/SceneManager.h"

namespace PersistedData
{
	//BaseData is based off how powerof3's did it in Afterlife
	template <typename T>
	bool BaseData<T>::Save(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version)
	{
		if (!serializationInterface->OpenRecord(type, version)) {
			logger::error("Failed to open record for Data Serialization!");
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
			logger::error("Failed to save {} data records", numRecords);
			return false;
		}

		for (const auto& [formId, value] : m_Data) {
			if (!serializationInterface->WriteRecordData(formId)) {
				logger::error("Failed to save data for FormID: ({:X})", formId);
				return false;
			}

			if (!serializationInterface->WriteRecordData(value)) {
				logger::error("Failed to save value data for form: {}", formId);
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
				logger::error("Failed to resolve formID {} {}"sv, formId, fixedId);
				continue;
			}

			serializationInterface->ReadRecordData(value);
			m_Data[formId] = value;
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
			logger::error("Failed to open record for Data Serialization!");
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
			logger::error("Failed to save {} data records", numRecords);
			return false;
		}

		for (const auto& [formId, formList] : m_Data) {
			if (!serializationInterface->WriteRecordData(formId)) {
				logger::error("Failed to save data for FormID: ({:X})", formId);
				return false;
			}

			const auto numForms = formList.size();
			if (!serializationInterface->WriteRecordData(numForms)) {
				logger::error("Failed to save {} sub form list records", numForms);
				return false;
			}

			for (const auto& subFormId : formList) {
				if (!serializationInterface->WriteRecordData(subFormId)) {
					logger::error("Failed to save data for sub FormID: ({})", subFormId);
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
				logger::error("Failed to resolve formID {} {}"sv, formId, fixedId);
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
			logger::critical("Failed to save Arousal Data");
		}

		const auto baseLibidoData = BaseLibidoData::GetSingleton();
		if (!baseLibidoData->Save(serializationInterface, kBaseLibidoDataKey, kSerializationVersion)) {
			logger::critical("Failed to save Base Libido Data");
		}

		const auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
		if (!arousalMultiplierData->Save(serializationInterface, kAroualMultiplierDataKey, kSerializationVersion)) {
			logger::critical("Failed to save Arousal Multiplier Data");
		}

		const auto lastCheckData = LastCheckTimeData::GetSingleton();
		if (!lastCheckData->Save(serializationInterface, kLastCheckTimeDataKey, kSerializationVersion)) {
			logger::critical("Failed to save Arousal Last Check Time Data");
		}

		const auto lastOrgasmData = LastOrgasmTimeData::GetSingleton();
		if (!lastOrgasmData->Save(serializationInterface, kLastOrgasmTimeDataKey, kSerializationVersion)) {
			logger::critical("Failed to save Last Orgasm Time Data");
		}

		const auto armorKeywordData = ArmorKeywordData::GetSingleton();
		if (!armorKeywordData->Save(serializationInterface, kArmorKeywordDataKey, kSerializationVersion)) {
			logger::critical("Failed to save Armor Keyword Data");
		}

		logger::info("OSLArousal Data Saved");
	}

	void LoadCallback(SKSE::SerializationInterface* serializationInterface)
	{
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		logger::info("OSLArousal Load Start");

		while (serializationInterface->GetNextRecordInfo(type, version, length)) {
			logger::info("Trying Load for {}", DecodeTypeCode(type));

			if (version != kSerializationVersion) {
				logger::critical("Loaded data has incorrect version. Recieved ({}) - Expected ({}) for Data Key ({})"sv, version, kSerializationVersion, DecodeTypeCode(type));
				continue;
			}

			switch (type) {
			case kArousalDataKey:
				{
					auto arousalData = ArousalData::GetSingleton();
					if (!arousalData->Load(serializationInterface)) {
						logger::critical("Failed to Load Arousal Data"sv);
					}
				}
				break;
			case kBaseLibidoDataKey:
				{
					auto baseLibidoData = BaseLibidoData::GetSingleton();
					if (!baseLibidoData->Load(serializationInterface)) {
						logger::critical("Failed to Load Base Libido Data"sv);
					}
				}
				break;
			case kAroualMultiplierDataKey:
				{
					auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
					if (!arousalMultiplierData->Load(serializationInterface)) {
						logger::critical("Failed to Load Arousal Multiplier Data"sv);
					}
				}
				break;
			case kLastCheckTimeDataKey:
				{
					auto lastCheckData = LastCheckTimeData::GetSingleton();
					if (!lastCheckData->Load(serializationInterface)) {
						logger::critical("Failed to Load LastCheckTime Data"sv);
					}
				}
				break;
			case kLastOrgasmTimeDataKey:
				{
					auto lastOrgasmData = LastOrgasmTimeData::GetSingleton();
					if (!lastOrgasmData->Load(serializationInterface)) {
						logger::critical("Failed to Load LastOrgasm Data"sv);
					}
				}
				break;
			case kArmorKeywordDataKey:
				{
					auto armorKeywordData = ArmorKeywordData::GetSingleton();
					if (!armorKeywordData->Load(serializationInterface)) {
						logger::critical("Failed to Load armorKeywordData Data"sv);
					}
				}
				break;
			default:
				logger::critical("Unrecognized Record Type: {}"sv, DecodeTypeCode(type));
				break;
			}
		}

		logger::info("OSLArousal Data loaded");
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

		//End All Scenes as well
		SceneManager::GetSingleton()->ClearScenes();

		logger::info("Reverting Data State...");
	}
}
