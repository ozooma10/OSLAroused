#pragma once
#include "PCH.h"

namespace PersistedData
{
	template <typename T>
	//BaseData is based off how powerof3's did it in Afterlife
	class BaseData
	{
	public:
		T GetData(RE::FormID formId, T missing)
		{
			Locker locker(m_Lock);
			auto it = m_Data.find(formId);
			if (it != m_Data.end()) {
				return it->second;
			}
			return missing;
		}

		void SetData(RE::FormID formId, T value)
		{
			Locker locker(m_Lock);
			m_Data[formId] = value;
		}


		//Runs sanitizer over stored values inplace, replacing each with the value sanitizer() returns. 
		//Used for post-load self-healing of corrupt/out-of-range persisted values. 
		//Returns the number of values that were actually changed.
		template <typename Fn>
		std::size_t SanitizeValues(Fn sanitizer)
		{
			Locker locker(m_Lock);
			std::size_t corrected = 0;
			for (auto& [formId, value] : m_Data) {
				T newValue = sanitizer(value);
				if (newValue != value) {
					value = newValue;
					++corrected;
				}
			}
			return corrected;
		}

		virtual const char* GetType() = 0;

		virtual bool Save(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version);
		virtual bool Save(SKSE::SerializationInterface* serializationInterface);
		virtual bool Load(SKSE::SerializationInterface* serializationInterface);

		void Clear();

		virtual void DumpToLog() = 0;

	protected:
		std::map<RE::FormID, T> m_Data;

		using Lock = std::recursive_mutex;
		using Locker = std::lock_guard<Lock>;
		mutable Lock m_Lock;
	};

	class BaseFormInt : public BaseData<int>
	{
	public:
		virtual void DumpToLog() override
		{
			Locker locker(m_Lock);
			for (const auto& [formId, value] : m_Data) {
				SKSE::log::info("Dump Row From {} - FormID: {} - value: {}", GetType(), formId, value);
			}
			SKSE::log::info("{} Rows Dumped For Type {}", m_Data.size(), GetType());
		}
	};

	class BaseFormFloat : public BaseData<float>
	{
	public:
		virtual void DumpToLog() override
		{
			Locker locker(m_Lock);
			for (const auto& [formId, value] : m_Data) {
				SKSE::log::info("Dump Row From {} - FormID: {} - value: {}", GetType(), formId, value);
			}
			SKSE::log::info("{} Rows Dumped For Type {}", m_Data.size(), GetType());
		}
	};

	class BaseFormBool : public BaseData<bool>
	{
	public:
		virtual void DumpToLog() override
		{
			Locker locker(m_Lock);
			for (const auto& [formId, value] : m_Data) {
				SKSE::log::info("Dump Row From {} - FormID: {} - value: {}", GetType(), formId, value);
			}
			SKSE::log::info("{} Rows Dumped For Type {}", m_Data.size(), GetType());
		}
	};

	class BaseFormArrayData : public BaseData<std::set<RE::FormID>>
	{
	public:
		virtual bool Save(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version) override;
		virtual bool Save(SKSE::SerializationInterface* serializationInterface) override;
		virtual bool Load(SKSE::SerializationInterface* serializationInterface) override;

		std::map<RE::FormID, std::set<RE::FormID>> GetData() const { return m_Data; }

		void AppendData(RE::FormID formId, RE::FormID subFormId)
		{
			Locker locker(m_Lock);
			if (const auto it = m_Data.find(formId); it != m_Data.end()) {
				it->second.insert(subFormId);
			} else {
				m_Data[formId] = { subFormId };
			}
		}

		void RemoveData(RE::FormID formId, RE::FormID subFormId)
		{
			Locker locker(m_Lock);

			if (const auto it = m_Data.find(formId); it != m_Data.end()) {
				it->second.erase(subFormId);
			}
		}

		virtual void DumpToLog() override
		{
			SKSE::log::info("{} Rows Not Dumped For List Type {}", m_Data.size(), GetType());
		}
	};

	class SettingsData final : public BaseFormInt
	{
	public:
		enum Setting {
			ArousalMode,
		};

		static SettingsData* GetSingleton()
		{
			static SettingsData singleton;
			return &singleton;
		}

		int GetArousalMode()
		{
			//0 = osl mode
			return GetData(Setting::ArousalMode, 0);
		}

		void SetArousalMode(int mode)
		{
			SetData(Setting::ArousalMode, (int)mode);
		}

		const char* GetType() override
		{
			return "Settings";
		};
	};

	class ArousalData final : public BaseFormFloat
	{
	public:
		static ArousalData* GetSingleton()
		{
			static ArousalData singleton;
			return &singleton;
		}

		const char* GetType() override
		{
			return "Arousal";
		}
	};

	class BaseLibidoData final : public BaseFormFloat
	{
	public:
		static BaseLibidoData* GetSingleton()
		{
			static BaseLibidoData singleton;
			return &singleton;
		}

		const char* GetType() override
		{
			return "BaseLibido";
		}
	};

	class ArousalMultiplierData final : public BaseFormFloat
	{
	public:
		static ArousalMultiplierData* GetSingleton()
		{
			static ArousalMultiplierData singleton;
			return &singleton;
		}

		const char* GetType() override
		{
			return "ArousalMultiplier";
		}
	};

	class LastCheckTimeData final : public BaseFormFloat
	{
	public:
		static LastCheckTimeData* GetSingleton()
		{
			static LastCheckTimeData singleton;
			return &singleton;
		}

		const char* GetType() override
		{
			return "LastCheckTime";
		}
	};

	
	class LastOrgasmTimeData final : public BaseFormFloat
	{
	public:
		static LastOrgasmTimeData* GetSingleton()
		{
			static LastOrgasmTimeData singleton;
			return &singleton;
		}

		const char* GetType() override
		{
			return "LastOrgasmTime";
		}
	};

	class ArmorKeywordData final : public BaseFormArrayData
	{
	public:
		static ArmorKeywordData* GetSingleton()
		{
			static ArmorKeywordData singleton;
			return &singleton;
		}

		const char* GetType() override
		{
			return "ArmorKeywords";
		}
	};

	class IsArousalLockedData final : public BaseFormBool
	{
	public:
		static IsArousalLockedData* GetSingleton()
		{
			static IsArousalLockedData singleton;
			return &singleton;
		}
		const char* GetType() override
		{
			return "IsArousalLocked";
		}
	};

	class IsActorExhibitionistData final : public BaseFormBool
	{
	public:
		static IsActorExhibitionistData* GetSingleton()
		{
			static IsActorExhibitionistData singleton;
			return &singleton;
		}
		const char* GetType() override
		{
			return "IsActorExhibitionist";
		}
	};

	// Keyed by ARMOR FormID (not actor): true means the worn piece "counts as
	// clothing" and suppresses nudity arousal for its wearer.
	class CountsAsClothingData final : public BaseFormBool
	{
	public:
		static CountsAsClothingData* GetSingleton()
		{
			static CountsAsClothingData singleton;
			return &singleton;
		}
		const char* GetType() override
		{
			return "CountsAsClothing";
		}
	};

	constexpr std::uint32_t kSerializationVersion = 1;
	constexpr std::uint32_t kArousalDataKey = 'OSLA';
	constexpr std::uint32_t kBaseLibidoDataKey = 'OSLB';
	constexpr std::uint32_t kAroualMultiplierDataKey = 'OSLM';
	constexpr std::uint32_t kLastCheckTimeDataKey = 'OSLC';
	constexpr std::uint32_t kLastOrgasmTimeDataKey = 'OSLO';
	constexpr std::uint32_t kArmorKeywordDataKey = 'OSLK';
	constexpr std::uint32_t kIsArousalLockedDataKey = 'OSLL';
	constexpr std::uint32_t kIsActorExhibitionistDataKey = 'OSLE';
	constexpr std::uint32_t kCountsAsClothingDataKey = 'OSLG';
	constexpr std::uint32_t kSettingsDataKey = 'OSLS';

	std::string DecodeTypeCode(std::uint32_t typeCode);

	void SaveCallback(SKSE::SerializationInterface* serializationInterface);
	void LoadCallback(SKSE::SerializationInterface* serializationInterface);
	void RevertCallback(SKSE::SerializationInterface* serializationInterface);

	// Post-load self-healing: clamps/repairs persisted per-actor values that could otherwise corrupt
	// the arousal system (out-of-range, negative, or non-finite values from a corrupted or externally
	// edited save). Legitimate-but-extreme in-range values are preserved (e.g. a multiplier of 0, which
	// is a valid "no arousal gains" setting). Safe to call after LoadCallback.
	void SanitizeLoadedData();

	void ResetSystemForModeSwitch();
};
