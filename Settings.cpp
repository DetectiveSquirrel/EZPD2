#include "Hack.h"
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

const char *SETTINGS_FILE = "EZPD2Settings.ini";

enum SettingType {
    SETTING_INT,
    SETTING_BOOL,
    SETTING_DWORD
};

struct Setting {
    const char* key;
    void* valuePtr;
    SettingType type;
    int defaultValue; // Used for int, BOOL, and DWORD
};

// Centralized settings definition
Setting g_settings[] = {
    {"MainMenuKey", &V_MainMenuKey, SETTING_INT, VK_F7},
    {"RefillPotionsKey", &V_RefillPotionsKey, SETTING_INT, 0},
    {"PickitKey", &V_PickitKey, SETTING_INT, 0},
    {"BuildMAChargesKey", &V_BuildMAChargesKey, SETTING_INT, 0},
    {"MAAutomateKey", &V_MAAutomateKey, SETTING_INT, 0},
    {"MapHackEnabled", &V_MapHackEnabled, SETTING_BOOL, TRUE},
    {"ShowZoneTransitions", &V_ShowZoneTransitions, SETTING_BOOL, TRUE},
    {"ShowPreloads", &V_ShowPreloads, SETTING_BOOL, TRUE},
    {"TownPortalOwnerLabelsEnabled", &V_TownPortalOwnerLabelsEnabled, SETTING_BOOL, TRUE},
    {"NearbyEntitiesEnabled", &V_NearbyEntitiesEnabled, SETTING_BOOL, TRUE},
    {"DrawChampBossMonsters", &V_DrawChampBossMonsters, SETTING_BOOL, TRUE},
    {"DrawNormalMonsters", &V_DrawNormalMonsters, SETTING_BOOL, TRUE},
    {"NormalMonsterMarkerStyle", &V_NormalMonsterMarkerStyle, SETTING_DWORD, MONSTER_MARKER_STYLE_X},
    {"NormalMonsterMarkerFontSize", &V_NormalMonsterMarkerFontSize, SETTING_DWORD, 12},
    {"MinionMonsterMarkerStyle", &V_MinionMonsterMarkerStyle, SETTING_DWORD, MONSTER_MARKER_STYLE_X},
    {"MinionMonsterMarkerFontSize", &V_MinionMonsterMarkerFontSize, SETTING_DWORD, 12},
    {"ChampionMonsterMarkerStyle", &V_ChampionMonsterMarkerStyle, SETTING_DWORD, MONSTER_MARKER_STYLE_X},
    {"ChampionMonsterMarkerFontSize", &V_ChampionMonsterMarkerFontSize, SETTING_DWORD, 12},
    {"BossMonsterMarkerStyle", &V_BossMonsterMarkerStyle, SETTING_DWORD, MONSTER_MARKER_STYLE_X},
    {"BossMonsterMarkerFontSize", &V_BossMonsterMarkerFontSize, SETTING_DWORD, 12},
    {"ImportantMonsterMarkerStyle", &V_ImportantMonsterMarkerStyle, SETTING_DWORD, MONSTER_MARKER_STYLE_X},
    {"ImportantMonsterMarkerFontSize", &V_ImportantMonsterMarkerFontSize, SETTING_DWORD, 12},
    {"ActualBossMonsterMarkerStyle", &V_ActualBossMonsterMarkerStyle, SETTING_DWORD, MONSTER_MARKER_STYLE_X},
    {"ActualBossMonsterMarkerFontSize", &V_ActualBossMonsterMarkerFontSize, SETTING_DWORD, 12},
    {"DrawShrines", &V_DrawShrines, SETTING_BOOL, TRUE},
    {"DrawGoodShrines", &V_DrawGoodShrines, SETTING_BOOL, TRUE},
    {"DrawOtherShrines", &V_DrawOtherShrines, SETTING_BOOL, TRUE},
    {"AutoExitEnabled", &V_AutoExitEnabled, SETTING_BOOL, FALSE},
    {"AutoExitLifeThreshold", &V_AutoExitLifeThreshold, SETTING_DWORD, 10},
    {"AutoPotEnabled", &V_AutoPotEnabled, SETTING_BOOL, FALSE},
    {"AutoPotLifeEnabled", &V_AutoPotLifeEnabled, SETTING_BOOL, FALSE},
    {"AutoPotLifeThreshold", &V_AutoPotLifeThreshold, SETTING_DWORD, 70},
    {"AutoPotManaEnabled", &V_AutoPotManaEnabled, SETTING_BOOL, FALSE},
    {"AutoPotManaThreshold", &V_AutoPotManaThreshold, SETTING_DWORD, 20},
    {"AutoPotRejuvEnabled", &V_AutoPotRejuvEnabled, SETTING_BOOL, FALSE},
    {"AutoPotRejuvThreshold", &V_AutoPotRejuvThreshold, SETTING_DWORD, 40},
    {"MercAutoPotRejuvEnabled", &V_MercAutoPotRejuvEnabled, SETTING_BOOL, FALSE},
    {"MercAutoPotRejuvThreshold", &V_MercAutoPotRejuvThreshold, SETTING_DWORD, 40},
    {"MercAutoPotLifeEnabled", &V_MercAutoPotLifeEnabled, SETTING_BOOL, FALSE},
    {"MercAutoPotLifeThreshold", &V_MercAutoPotLifeThreshold, SETTING_DWORD, 70},
    {"AutoRefillEnabled", &V_AutoRefillEnabled, SETTING_BOOL, FALSE},
    {"RefillSlot1PotionType", &V_RefillSlot1PotionType, SETTING_INT, POTION_TYPE_REJUV},
    {"RefillSlot2PotionType", &V_RefillSlot2PotionType, SETTING_INT, POTION_TYPE_REJUV},
    {"RefillSlot3PotionType", &V_RefillSlot3PotionType, SETTING_INT, POTION_TYPE_REJUV},
    {"RefillSlot4PotionType", &V_RefillSlot4PotionType, SETTING_INT, POTION_TYPE_MANA},
    {"PickitEnabled", &V_PickitEnabled, SETTING_BOOL, FALSE},
    {"VendorPreviewEnabled", &V_VendorPreviewEnabled, SETTING_BOOL, FALSE},
    {"VendorShortcutEnabled", &V_VendorShortcutEnabled, SETTING_BOOL, FALSE},
    {"AnyaAutoPurchaseEnabled", &V_AnyaAutoPurchaseEnabled, SETTING_BOOL, TRUE},
    {"AnyaBotKey", &V_AnyaBotKey, SETTING_INT, 0},
    {"TradeInviteReplyEnabled", &V_TradeInviteReplyEnabled, SETTING_BOOL, FALSE},
    {"TradeInviteReplyKey", &V_TradeInviteReplyKey, SETTING_INT, 0},
    {"MartialArtsChargesEnabled", &V_MartialArtsChargesEnabled, SETTING_BOOL, TRUE},
    {"BuildMAChargesEnabled", &V_BuildMAChargesEnabled, SETTING_BOOL, FALSE},
    {"MAAutomateEnabled", &V_MAAutomateEnabled, SETTING_BOOL, FALSE},
    {"MartialArtSkillButton", &V_MartialArtSkillButton, SETTING_INT, VK_RBUTTON},
    {"FinisherSkillButton", &V_FinisherSkillButton, SETTING_INT, VK_LBUTTON},
};

const int g_numSettings = sizeof(g_settings) / sizeof(g_settings[0]);

std::string GetDllDirectory()
{
    char dllPath[MAX_PATH] = {0};
    GetModuleFileName(V_DLL, dllPath, MAX_PATH);
    std::string fullPath(dllPath);
    size_t lastSlash = fullPath.find_last_of("\\");
    if (lastSlash != std::string::npos) {
        return fullPath.substr(0, lastSlash + 1);
    }
    return "";
}

void InitDefaultSettings() {
    for (int i = 0; i < g_numSettings; ++i) {
        Setting& s = g_settings[i];
        switch (s.type) {
            case SETTING_INT:
                *(static_cast<int*>(s.valuePtr)) = s.defaultValue;
                break;
            case SETTING_BOOL:
                *(static_cast<BOOL*>(s.valuePtr)) = s.defaultValue;
                break;
            case SETTING_DWORD:
                *(static_cast<DWORD*>(s.valuePtr)) = s.defaultValue;
                break;
        }
    }
}

bool SaveSettings() {
    std::string settingsPath = GetDllDirectory() + SETTINGS_FILE;
    std::ofstream file(settingsPath.c_str());
    if (!file.is_open()) {
        return false;
    }

    for (int i = 0; i < g_numSettings; ++i) {
        Setting& s = g_settings[i];
        file << s.key << "=";
        switch (s.type) {
            case SETTING_INT:
                file << *(static_cast<int*>(s.valuePtr));
                break;
            case SETTING_BOOL:
                file << (*(static_cast<BOOL*>(s.valuePtr)) ? "1" : "0");
                break;
            case SETTING_DWORD:
                file << *(static_cast<DWORD*>(s.valuePtr));
                break;
        }
        file << std::endl;
    }

    file.close();
    return true;
}

// Helper to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool LoadSettings() {
    InitDefaultSettings();

    std::string settingsPath = GetDllDirectory() + SETTINGS_FILE;
    std::ifstream file(settingsPath.c_str());
    if (!file.is_open()) {
        SaveSettings(); // Create a default settings file
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments
        size_t comment_pos = line.find(';');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        // Find separator
        size_t separator_pos = line.find('=');
        if (separator_pos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, separator_pos));
        std::string value = trim(line.substr(separator_pos + 1));

        if (key.empty()) {
            continue;
        }

        for (int i = 0; i < g_numSettings; ++i) {
            if (_stricmp(g_settings[i].key, key.c_str()) == 0) {
                Setting& s = g_settings[i];
                switch (s.type) {
                    case SETTING_INT:
                        *(static_cast<int*>(s.valuePtr)) = atoi(value.c_str());
                        break;
                    case SETTING_BOOL:
                        *(static_cast<BOOL*>(s.valuePtr)) = (atoi(value.c_str()) != 0);
                        break;
                    case SETTING_DWORD:
                        *(static_cast<DWORD*>(s.valuePtr)) = strtoul(value.c_str(), NULL, 10);
                        break;
                }
                break; // Found key, move to next line
            }
        }
    }

    file.close();
    return true;
}