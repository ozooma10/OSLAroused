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
			SKSE::log::error("Keyword: {} failed to register.", keyword.pItem);
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
    }

    m_RegisteredKeywordEditorIds.emplace_back(keywordForm->formID, keywordEditorId);
    return true;
}
