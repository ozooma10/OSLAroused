#include "Config.h"
#include "Libraries/SimpleIni/SimpleIni.h"

#include <RE/T/TESForm.h>

void Config::LoadINIs()
{
    if (LoadINI("Data/SKSE/Plugins/OSLAroused.ini")) {
        m_ConfigLoaded = true;
    }
    else {
        SKSE::log::error("Failed to load INI file.");
        m_ConfigLoaded = false;
        return;
    }

    //Load Custom INI
	LoadINI("Data/SKSE/Plugins/OSLAroused_Custom.ini");
}

bool Config::LoadINI(std::string fileName)
{
    CSimpleIniA ini(false, true, false);
    SI_Error rc = ini.LoadFile(fileName.c_str());
    if (rc < 0)
    {
        return false;
    }

    // Get a list of keywords in Keyword Section
    CSimpleIniA::TNamesDepend keywords;
    ini.GetAllValues("RegisteredKeywords", "KeywordEditorId", keywords);

    // Get the log level from the System section
    const char* logLevelStr = ini.GetValue("System", "LogLevel", "0");
    m_LogLevel = std::stoi(logLevelStr);
    //Log loglevel name
    switch (m_LogLevel) {
	case 0:
		SKSE::log::info("Log Level: Trace");
		break;
	case 1:
		SKSE::log::info("Log Level: Debug");
		break;
	case 2:
		SKSE::log::info("Log Level: Info");
		break;
	case 3:
		SKSE::log::info("Log Level: Warn");
		break;
	default:
		SKSE::log::info("Log Level: Error");
		break;
    }
	spdlog::default_logger()->set_level(static_cast<spdlog::level::level_enum>(m_LogLevel));


    SKSE::log::info("Trying to Register {} Keywords", keywords.size());
    // Iterate and log each section name
    for (auto &keyword : keywords)
    {

        auto keywordForm = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(keyword.pItem);
        if (keywordForm != nullptr)
        {
            m_RegisteredKeywordEditorIds.emplace_back(keywordForm->formID, keyword.pItem);
        }
        else
        {
			SKSE::log::warn("Keyword: {} failed to register. Failed to find Keyword Form.", keyword.pItem);
		}
    }

    return true;
}

bool Config::RegisterKeyword(std::string keywordEditorId)
{
    auto keywordForm = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(keywordEditorId);
    if (!keywordForm) {
		SKSE::log::error("RegisterKeyword: Failed to find keyword form.");
		return false;
	}
    m_RegisteredKeywordEditorIds.emplace_back(keywordForm->formID, keywordEditorId);

    CSimpleIniA ini(false, true, false);
    SI_Error rc = ini.LoadFile("Data/SKSE/Plugins/OSLAroused_Custom.ini");
    if (rc < 0) {
        //This is fine, just means the file doesnt exist yet
    }

    rc = ini.SetValue("RegisteredKeywords", "KeywordEditorId", keywordEditorId.c_str());
    if (rc < 0) {
        SKSE::log::error("RegisterKeyword: Failed to set value in INI file. Error: {}", rc);
		return false;
    }

    rc = ini.SaveFile("Data/SKSE/Plugins/OSLAroused_Custom.ini");
    if (rc < 0) {
		SKSE::log::error("RegisterKeyword: Failed to save INI file. Error: {}", rc);
		return false;
    }

    return true;
}
