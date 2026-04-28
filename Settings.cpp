#include "Hack.h"
#include <fstream>
#include <map>
#include <sstream>
#include <string>

static const char *SETTINGS_FILE = "EZPD2Settings.json";

enum SettingType {
    SETTING_INT,
    SETTING_BOOL,
    SETTING_DWORD
};

struct Setting {
    const char *key;
    void *valuePtr;
    SettingType type;
    int defaultValue;
};

static Setting g_settings[] = {
    {"MainMenuKey", &V_MainMenuKey, SETTING_INT, VK_F7},
    {"RefillPotionsKey", &V_RefillPotionsKey, SETTING_INT, 0},
    {"PickitKey", &V_PickitKey, SETTING_INT, 0},
    {"BuildMAChargesKey", &V_BuildMAChargesKey, SETTING_INT, 0},
    {"MAAutomateKey", &V_MAAutomateKey, SETTING_INT, 0},
    {"MapHackEnabled", &V_MapHackEnabled, SETTING_BOOL, TRUE},
    {"ShowZoneTransitions", &V_ShowZoneTransitions, SETTING_BOOL, TRUE},
    {"ShowPreloads", &V_ShowPreloads, SETTING_BOOL, TRUE},
    {"DrawPOILinesToTarget", &V_DrawPOILinesToTarget, SETTING_BOOL, TRUE},
    {"TownPortalOwnerLabelsEnabled", &V_TownPortalOwnerLabelsEnabled, SETTING_BOOL, TRUE},
    {"NearbyEntitiesEnabled", &V_NearbyEntitiesEnabled, SETTING_BOOL, TRUE},
    {"DrawChampBossMonsters", &V_DrawChampBossMonsters, SETTING_BOOL, TRUE},
    {"DrawNormalMonsters", &V_DrawNormalMonsters, SETTING_BOOL, TRUE},
    {"MonsterMarkerStyle", &V_MonsterMarkerStyle, SETTING_DWORD, MONSTER_MARKER_STYLE_CROSS},
    {"MonsterMarkerFontSize", &V_MonsterMarkerFontSize, SETTING_DWORD, 12},
    {"MonsterNormalColor", &V_MonsterNormalColor, SETTING_DWORD, BH_AUTOMAP_MONSTER_COLOR_NORMAL},
    {"MonsterMinionColor", &V_MonsterMinionColor, SETTING_DWORD, BH_AUTOMAP_MONSTER_COLOR_MINION},
    {"MonsterChampionColor", &V_MonsterChampionColor, SETTING_DWORD, BH_AUTOMAP_MONSTER_COLOR_CHAMPION},
    {"MonsterBossColor", &V_MonsterBossColor, SETTING_DWORD, BH_AUTOMAP_MONSTER_COLOR_BOSS},
    {"DrawListedBossMarkers", &V_DrawListedBossMarkers, SETTING_BOOL, TRUE},
    {"ListedBossCrossColor", &V_ListedBossCrossColor, SETTING_DWORD, BH_AUTOMAP_MONSTER_COLOR_BOSS},
    {"ListedBossNameColor", &V_ListedBossNameColor, SETTING_DWORD, FONTCOLOR_PINK},
    {"ListedBossHpInName", &V_ListedBossHpInName, SETTING_BOOL, TRUE},
    {"MonsterResistTextOffsetX", &V_MonsterResistTextOffsetX, SETTING_DWORD, 10},
    {"MonsterResistTextOffsetY", &V_MonsterResistTextOffsetY, SETTING_DWORD, 10},
    {"DrawMonsterResistances", &V_DrawMonsterResistances, SETTING_BOOL, FALSE},
    {"DrawMonsterEnchantments", &V_DrawMonsterEnchantments, SETTING_BOOL, FALSE},
    {"DrawShrines", &V_DrawShrines, SETTING_BOOL, TRUE},
    {"DrawGoodShrines", &V_DrawGoodShrines, SETTING_BOOL, TRUE},
    {"DrawOtherShrines", &V_DrawOtherShrines, SETTING_BOOL, TRUE},
    {"DrawMonsterHealthPercent", &V_DrawMonsterHealthPercent, SETTING_BOOL, FALSE},
    {"DrawMonsterClassIds", &V_DrawMonsterClassIds, SETTING_BOOL, FALSE},
    {"DebugMouseCoordinates", &V_DebugMouseCoordinates, SETTING_BOOL, FALSE},
    {"DebugNearestItem", &V_DebugNearestItem, SETTING_BOOL, FALSE},
    {"DebugPlayerInventory", &V_DebugPlayerInventory, SETTING_BOOL, FALSE},
    {"DebugPlayerBelt", &V_DebugPlayerBelt, SETTING_BOOL, FALSE},
    {"DebugClosestMonster", &V_DebugClosestMonster, SETTING_BOOL, FALSE},
    {"DebugCurrentRoom", &V_DebugCurrentRoom, SETTING_BOOL, FALSE},
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

static const int g_numSettings = (int)(sizeof(g_settings) / sizeof(g_settings[0]));

static std::string GetEzpd2ModuleDirectory()
{
    char dllPath[MAX_PATH] = {0};
    GetModuleFileName(V_DLL, dllPath, MAX_PATH);
    std::string fullPath(dllPath);
    size_t lastSlash = fullPath.find_last_of("\\/");
    if (lastSlash != std::string::npos)
        return fullPath.substr(0, lastSlash + 1);
    return "";
}

static void DeleteLegacyIniIfPresent()
{
    std::string legacyIni = GetEzpd2ModuleDirectory() + "EZPD2Settings.ini";
    DeleteFileA(legacyIni.c_str());
}

static void ClampMonsterAutomapColorsToPalette()
{
    V_MonsterNormalColor &= 0xFFu;
    V_MonsterMinionColor &= 0xFFu;
    V_MonsterChampionColor &= 0xFFu;
    V_MonsterBossColor &= 0xFFu;
    V_ListedBossCrossColor &= 0xFFu;
    if (V_ListedBossNameColor > FONTCOLOR_RED)
        V_ListedBossNameColor = FONTCOLOR_RED;
}

static void InitDefaultSettings()
{
    for (int i = 0; i < g_numSettings; ++i)
    {
        Setting &s = g_settings[i];
        switch (s.type)
        {
        case SETTING_INT:
            *(static_cast<int *>(s.valuePtr)) = s.defaultValue;
            break;
        case SETTING_BOOL:
            *(static_cast<BOOL *>(s.valuePtr)) = s.defaultValue;
            break;
        case SETTING_DWORD:
            *(static_cast<DWORD *>(s.valuePtr)) = (DWORD)s.defaultValue;
            break;
        }
    }
    ClampMonsterAutomapColorsToPalette();
}

// --- Minimal JSON (one object, string keys, integer values only) ---

static void SkipWs(const char *&p)
{
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        ++p;
}

static bool ParseJsonString(const char *&p, std::string &out)
{
    out.clear();
    if (*p != '"')
        return false;
    ++p;
    while (*p != '\0')
    {
        if (*p == '"')
        {
            ++p;
            return true;
        }
        if (*p == '\\' && p[1] != '\0')
        {
            ++p;
            out += *p++;
            continue;
        }
        out += *p++;
    }
    return false;
}

static bool ParseJsonInt(const char *&p, long long &out)
{
    SkipWs(p);
    bool neg = false;
    if (*p == '-')
    {
        neg = true;
        ++p;
    }
    if (*p < '0' || *p > '9')
        return false;
    unsigned long long n = 0;
    while (*p >= '0' && *p <= '9')
    {
        n = n * 10u + (unsigned)(*p - '0');
        ++p;
    }
    if (neg)
    {
        out = 0LL - (long long)n;
        return true;
    }
    out = (long long)n;
    return true;
}

static bool ParseJsonObject(const char *&p, std::map<std::string, long long> &kv)
{
    SkipWs(p);
    if (*p != '{')
        return false;
    ++p;
    SkipWs(p);
    if (*p == '}')
    {
        ++p;
        return true;
    }
    for (;;)
    {
        std::string key;
        if (!ParseJsonString(p, key))
            return false;
        SkipWs(p);
        if (*p != ':')
            return false;
        ++p;
        SkipWs(p);
        long long v;
        if (!ParseJsonInt(p, v))
            return false;
        kv[key] = v;
        SkipWs(p);
        if (*p == '}')
        {
            ++p;
            return true;
        }
        if (*p != ',')
            return false;
        ++p;
        SkipWs(p);
    }
}

static bool ReadEntireFile(const std::string &path, std::string &out)
{
    std::ifstream f(path.c_str(), std::ios::binary);
    if (!f.is_open())
        return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    out = ss.str();
    return true;
}

static void ApplySettingsFromMap(const std::map<std::string, long long> &kv)
{
    for (int i = 0; i < g_numSettings; ++i)
    {
        Setting &s = g_settings[i];
        std::map<std::string, long long>::const_iterator it = kv.find(s.key);
        if (it == kv.end())
            continue;
        long long v = it->second;
        switch (s.type)
        {
        case SETTING_INT:
            *(static_cast<int *>(s.valuePtr)) = (int)v;
            break;
        case SETTING_BOOL:
            *(static_cast<BOOL *>(s.valuePtr)) = (v != 0) ? TRUE : FALSE;
            break;
        case SETTING_DWORD:
            if (v < 0)
                v = 0;
            *(static_cast<DWORD *>(s.valuePtr)) = (DWORD)v;
            break;
        }
    }
    ClampMonsterAutomapColorsToPalette();
}

static std::string BuildSettingsJson()
{
    std::ostringstream o;
    o << "{\n  \"settingsVersion\": 1";
    for (int i = 0; i < g_numSettings; ++i)
    {
        Setting &s = g_settings[i];
        o << ",\n  \"" << s.key << "\": ";
        switch (s.type)
        {
        case SETTING_INT:
            o << *(static_cast<int *>(s.valuePtr));
            break;
        case SETTING_BOOL:
            o << (*(static_cast<BOOL *>(s.valuePtr)) ? 1 : 0);
            break;
        case SETTING_DWORD:
            o << *(static_cast<DWORD *>(s.valuePtr));
            break;
        }
    }
    o << "\n}\n";
    return o.str();
}

bool SaveSettings()
{
    std::string path = GetEzpd2ModuleDirectory() + SETTINGS_FILE;
    std::string json = BuildSettingsJson();
    std::ofstream file(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open())
        return false;
    file.write(json.data(), (std::streamsize)json.size());
    file.flush();
    bool ok = file.good();
    file.close();
    if (ok)
        DeleteLegacyIniIfPresent();
    return ok;
}

bool LoadSettings()
{
    InitDefaultSettings();

    std::string path = GetEzpd2ModuleDirectory() + SETTINGS_FILE;
    std::string raw;
    if (!ReadEntireFile(path, raw) || raw.empty())
    {
        SaveSettings();
        return false;
    }

    const char *p = raw.c_str();
    if (raw.size() >= 3 && (unsigned char)p[0] == 0xEF && (unsigned char)p[1] == 0xBB && (unsigned char)p[2] == 0xBF)
        p += 3;

    std::map<std::string, long long> kv;
    if (!ParseJsonObject(p, kv))
    {
        SaveSettings();
        return false;
    }
    SkipWs(p);
    if (*p != '\0')
    {
        SaveSettings();
        return false;
    }

    ApplySettingsFromMap(kv);
    DeleteLegacyIniIfPresent();
    return true;
}
