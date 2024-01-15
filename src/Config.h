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

private:
    bool LoadINI(std::string filename);
    bool m_ConfigLoaded;

    std::vector<KeywordEntry> m_RegisteredKeywordEditorIds;
};