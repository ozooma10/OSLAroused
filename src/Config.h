#pragma once
#include "PCH.h"

class Config
{
public:
    struct KeywordEntry
    {
        RE::FormID FormId;
        std::string EditorId;

        KeywordEntry(RE::FormID formId, std::string editorId)
            : FormId(formId), EditorId(editorId)
        {
        }
    };

    static Config *GetSingleton()
    {
        static Config singleton;
        return &singleton;
    }

    void LoadINIs();

    std::vector<KeywordEntry> GetRegisteredKeywords()
    {
		return m_RegisteredKeywordEditorIds;
	}
    bool RegisterKeyword(std::string keywordEditorId);
    bool SaveKeywordBaseline(RE::FormID keywordFormId, float value);
    bool SaveANDFactionBaseline(int index, float value);
    bool SaveSleepArousalGain(float value);
    bool SaveSpectatorArousalGain(float value);

private:
    bool LoadINI(std::string filename, bool useDefaults);
    // Ensures the canonical SexLab Aroused / SLS armor keywords are registered if
    // their forms exist in the load order, even when the user's INI omits them.
    void BackfillKnownKeywords();
    bool IsKeywordRegistered(RE::FormID keywordFormId) const;
    const KeywordEntry* FindRegisteredKeyword(RE::FormID keywordFormId) const;
    bool m_ConfigLoaded = false;

    std::vector<KeywordEntry> m_RegisteredKeywordEditorIds;
    int m_LogLevel = 0;
};
