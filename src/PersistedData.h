#pragma once
namespace PersistedData
{
	template <typename T>
	//BaseData is based off how powerof3's did it in Afterlife
	class BaseData
	{
	public:
		float GetData(RE::FormID formId, T missing)
		{
			Locker locker(m_Lock);
			if (auto idx = m_Data.find(formId) != m_Data.end()) {
				return m_Data[formId];
			}
			return missing;
		}

		void SetData(RE::FormID formId, T value)
		{
			Locker locker(m_Lock);
			m_Data[formId] = value;
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
				logger::info("Dump Row From {} - FormID: {} - value: {}", GetType(), formId, value);
			}
			logger::info("{} Rows Dumped For Type {}", m_Data.size(), GetType());
		}
	};

	class BaseFormFloat : public BaseData<float>
	{
	public:
		virtual void DumpToLog() override
		{
			Locker locker(m_Lock);
			for (const auto& [formId, value] : m_Data) {
				logger::info("Dump Row From {} - FormID: {} - value: {}", GetType(), formId, value);
			}
			logger::info("{} Rows Dumped For Type {}", m_Data.size(), GetType());
		}
	};

	class BaseFormBool : public BaseData<bool>
	{
	public:
		virtual void DumpToLog() override
		{
			Locker locker(m_Lock);
			for (const auto& [formId, value] : m_Data) {
				logger::info("Dump Row From {} - FormID: {} - value: {}", GetType(), formId, value);
			}
			logger::info("{} Rows Dumped For Type {}", m_Data.size(), GetType());
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
			logger::info("{} Rows Not Dumped For List Type {}", m_Data.size(), GetType());
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

	constexpr std::uint32_t kSerializationVersion = 1;
	constexpr std::uint32_t kArousalDataKey = 'OSLA';
	constexpr std::uint32_t kBaseLibidoDataKey = 'OSLB';
	constexpr std::uint32_t kAroualMultiplierDataKey = 'OSLM';
	constexpr std::uint32_t kLastCheckTimeDataKey = 'OSLC';
	constexpr std::uint32_t kLastOrgasmTimeDataKey = 'OSLO';
	constexpr std::uint32_t kArmorKeywordDataKey = 'OSLK';
	constexpr std::uint32_t kIsArousalLockedDataKey = 'OSLL';
	constexpr std::uint32_t kIsActorExhibitionistDataKey = 'OSLE';
	constexpr std::uint32_t kSettingsDataKey = 'OSLS';

	std::string DecodeTypeCode(std::uint32_t typeCode);

	void SaveCallback(SKSE::SerializationInterface* serializationInterface);
	void LoadCallback(SKSE::SerializationInterface* serializationInterface);
	void RevertCallback(SKSE::SerializationInterface* serializationInterface);

	void ResetSystemForModeSwitch();
};
