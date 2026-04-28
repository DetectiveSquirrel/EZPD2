#include "Hack.h"
#include "Menu.h"
#include <cstring>

std::vector<MenuItem> g_menuColumn1;
std::vector<MenuItem> g_menuColumn2;
std::vector<MenuItem> g_menuColumn3;
std::vector<MenuItem> g_menuMap;
std::vector<MenuItem> g_menuMonsters;
std::vector<MenuItem> g_menuDebug;
std::vector<MenuItem> g_menuAutomation;
std::vector<MenuItem> g_menuHotkeys;

enum MenuTab
{
    MENU_TAB_MAP,
    MENU_TAB_MONSTERS,
    MENU_TAB_DEBUG,
    MENU_TAB_AUTOMATION,
    MENU_TAB_HOTKEYS,
    MENU_TAB_COUNT
};

static int g_selectedMenuTab = MENU_TAB_MAP;
static int g_menuScrollOffsets[MENU_TAB_COUNT] = {0};
static const char *g_menuTabNames[MENU_TAB_COUNT] = {"Map", "Monsters", "Debug", "Automation", "Hotkeys"};

static const int MENU_VIEW_TOP = 90;
static const int MENU_VIEW_BOTTOM = 570;
static const int MENU_WIDTH = 535;
static const int MENU_HEIGHT = 580;

static int GetMenuLeft()
{
    return max(0, ((INT)*p_D2CLIENT_ScreenSizeX - MENU_WIDTH) / 2);
}

static int GetMenuTop()
{
    return max(0, ((INT)*p_D2CLIENT_ScreenSizeY - MENU_HEIGHT) / 2);
}

// Forward declarations for callback functions
void ToggleMenuKeyInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_MENU_KEY;
}

void ToggleRefillPotionsKeyInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_REFILL_POTIONS_KEY;
}

void TogglePickitKeyInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_PICKIT_KEY;
}

void ToggleAnyaBotKeyInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_ANYA_BOT_KEY;
}

void ToggleTradeInviteReplyKeyInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_TRADE_INVITE_REPLY_KEY;
}

void ToggleBuildMAChargesKeyInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_BUILD_MA_CHARGES_KEY;
}

void ToggleMAAutomateKeyInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_MA_AUTOMATE_KEY;
}

void ToggleMartialArtSkillButtonInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_MARTIAL_ART_SKILL_BUTTON;
}

void ToggleFinisherSkillButtonInput()
{
    V_IsHotkeyInputMode = TRUE;
    V_InputModeType = MODE_FINISHER_SKILL_BUTTON;
}

const char *GetMonsterMarkerStyleName(DWORD style)
{
    return style == MONSTER_MARKER_STYLE_CROSS ? "Cross" : "x";
}

bool IsMonsterMarkerStyleSlider(MenuItem &item)
{
    return item.intValue == &V_MonsterMarkerStyle;
}

bool IsMonsterMarkerFontSlider(MenuItem &item)
{
    return item.intValue == &V_MonsterMarkerFontSize;
}

void AddMonsterMarkerMenuItems(std::vector<MenuItem> &column)
{
    MenuItem item;

    item = MenuItem();
    item.label = "All Monster Style";
    item.type = IntSlider;
    item.intValue = &V_MonsterMarkerStyle;
    item.intMin = MONSTER_MARKER_STYLE_CROSS;
    item.intMax = MONSTER_MARKER_STYLE_X;
    item.indent = 1;
    item.parentIndex = -1;
    column.push_back(item);

    item = MenuItem();
    item.label = "All Monster Font ID";
    item.type = IntSlider;
    item.intValue = &V_MonsterMarkerFontSize;
    item.intMin = 0;
    item.intMax = 15;
    item.indent = 1;
    item.parentIndex = -1;
    column.push_back(item);
}

void AddMenuRange(std::vector<MenuItem> &dest, std::vector<MenuItem> &source, size_t start, size_t end)
{
    if (end > source.size())
        end = source.size();

    for (size_t i = start; i < end; ++i)
    {
        dest.push_back(source[i]);
    }
}

void AddMenuItemByLabel(std::vector<MenuItem> &dest, std::vector<MenuItem> &source, const char *label)
{
    for (size_t i = 0; i < source.size(); ++i)
    {
        if (source[i].label && strcmp(source[i].label, label) == 0)
        {
            dest.push_back(source[i]);
            return;
        }
    }
}

void BuildMenuTabs()
{
    g_menuMap.clear();
    g_menuMonsters.clear();
    g_menuDebug.clear();
    g_menuAutomation.clear();
    g_menuHotkeys.clear();

    AddMenuRange(g_menuMap, g_menuColumn1, 0, 5);
    AddMenuItemByLabel(g_menuMap, g_menuColumn3, "Town Portal Labels");

    AddMenuRange(g_menuMonsters, g_menuColumn1, 5, 11);
    AddMenuItemByLabel(g_menuMonsters, g_menuColumn3, "Monster Icons");
    AddMenuItemByLabel(g_menuMonsters, g_menuColumn3, "All Monster Style");
    AddMenuItemByLabel(g_menuMonsters, g_menuColumn3, "All Monster Font ID");
    AddMenuItemByLabel(g_menuMonsters, g_menuColumn3, "Monster HP Percent");
    AddMenuItemByLabel(g_menuMonsters, g_menuColumn3, "Monster Class IDs");

    AddMenuItemByLabel(g_menuDebug, g_menuColumn3, "Debug Drawing");
    AddMenuItemByLabel(g_menuDebug, g_menuColumn3, "Mouse / Player Coords");
    AddMenuItemByLabel(g_menuDebug, g_menuColumn3, "Nearest Item");
    AddMenuItemByLabel(g_menuDebug, g_menuColumn3, "Player Inventory");
    AddMenuItemByLabel(g_menuDebug, g_menuColumn3, "Player Belt");
    AddMenuItemByLabel(g_menuDebug, g_menuColumn3, "Closest Monster Stats");
    AddMenuItemByLabel(g_menuDebug, g_menuColumn3, "Current Room Info");

    AddMenuRange(g_menuAutomation, g_menuColumn2, 0, g_menuColumn2.size());
    AddMenuItemByLabel(g_menuAutomation, g_menuColumn3, "Vendor Preview");
    AddMenuItemByLabel(g_menuAutomation, g_menuColumn3, "Vendor Shortcuts");
    AddMenuItemByLabel(g_menuAutomation, g_menuColumn3, "Anya Auto Purchase");
    AddMenuItemByLabel(g_menuAutomation, g_menuColumn3, "Trade Reply (/r game / pass)");
    AddMenuItemByLabel(g_menuAutomation, g_menuColumn3, "Martial Arts Charges");
    AddMenuItemByLabel(g_menuAutomation, g_menuColumn3, "Martial Arts Automate");
    AddMenuItemByLabel(g_menuAutomation, g_menuColumn3, "Enable Build MA Charges");
    AddMenuItemByLabel(g_menuAutomation, g_menuColumn3, "Enable MA Automate");

    AddMenuRange(g_menuHotkeys, g_menuColumn1, 11, g_menuColumn1.size());
    AddMenuItemByLabel(g_menuHotkeys, g_menuColumn3, "MA Skill Bindings");
    AddMenuItemByLabel(g_menuHotkeys, g_menuColumn3, "Martial Art Skill Button");
    AddMenuItemByLabel(g_menuHotkeys, g_menuColumn3, "Finisher Skill Button");
}

void InitMenu()
{
    g_menuColumn1.clear();
    g_menuColumn2.clear();
    g_menuColumn3.clear();
    MenuItem item;

    // --- COLUMN 1 ---

    item = MenuItem();
    item.label = "Map Hack / Drawing";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);

    // Map Hack Category
    item = MenuItem();
    item.label = "Map Hack";
    item.type = Category;
    item.boolValue = &V_MapHackEnabled;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);
    int mapHackParent = g_menuColumn1.size() - 1;

    item = MenuItem();
    item.label = "Zone Transitions";
    item.type = Checkbox;
    item.boolValue = &V_ShowZoneTransitions;
    item.indent = 1;
    item.parentIndex = mapHackParent;
    g_menuColumn1.push_back(item);
    
    item = MenuItem();
    item.label = "Preloads (Waypoints, Quest Objects, etc.)";
    item.type = Checkbox;
    item.boolValue = &V_ShowPreloads;
    item.indent = 1;
    item.parentIndex = mapHackParent;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "POI Lines To Target";
    item.type = Checkbox;
    item.boolValue = &V_DrawPOILinesToTarget;
    item.indent = 1;
    item.parentIndex = mapHackParent;
    g_menuColumn1.push_back(item);

    // Nearby Entities Category
    item = MenuItem();
    item.label = "Draw Nearby Entities";
    item.type = Category;
    item.boolValue = &V_NearbyEntitiesEnabled;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);
    int nearbyParent = g_menuColumn1.size() - 1;

    item = MenuItem();
    item.label = "Champ / Boss Packs";
    item.type = Checkbox;
    item.boolValue = &V_DrawChampBossMonsters;
    item.indent = 1;
    item.parentIndex = nearbyParent;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Normal Monsters";
    item.type = Checkbox;
    item.boolValue = &V_DrawNormalMonsters;
    item.indent = 1;
    item.parentIndex = nearbyParent;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Shrines / Evil Urns";
    item.type = Checkbox;
    item.boolValue = &V_DrawShrines;
    item.indent = 1;
    item.parentIndex = nearbyParent;
    g_menuColumn1.push_back(item);
    int shrinesParent = g_menuColumn1.size() - 1;

    item = MenuItem();
    item.label = "Good Shrines (XP/Skill/Stamina)";
    item.type = Checkbox;
    item.boolValue = &V_DrawGoodShrines;
    item.indent = 2;
    item.parentIndex = shrinesParent;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Other Shrines";
    item.type = Checkbox;
    item.boolValue = &V_DrawOtherShrines;
    item.indent = 2;
    item.parentIndex = shrinesParent;
    g_menuColumn1.push_back(item);

    // Hotkeys
    item = MenuItem();
    item.label = "Hotkeys";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Toggle Menu";
    item.type = Button;
    item.callback = ToggleMenuKeyInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Refill Potions";
    item.type = Button;
    item.callback = ToggleRefillPotionsKeyInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Pickit";
    item.type = Button;
    item.callback = TogglePickitKeyInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Anya Bot";
    item.type = Button;
    item.callback = ToggleAnyaBotKeyInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Trade Reply";
    item.type = Button;
    item.callback = ToggleTradeInviteReplyKeyInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "Build MA Charges";
    item.type = Button;
    item.callback = ToggleBuildMAChargesKeyInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);

    item = MenuItem();
    item.label = "MA Automate";
    item.type = Button;
    item.callback = ToggleMAAutomateKeyInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn1.push_back(item);
    
    // --- COLUMN 2 ---

    item = MenuItem();
    item.label = "Autopot / Auto Exit";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn2.push_back(item);

    // Auto Pot Category
    item = MenuItem();
    item.label = "Auto Pot";
    item.type = Category;
    item.boolValue = &V_AutoPotEnabled;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn2.push_back(item);
    int autoPotParent = g_menuColumn2.size() - 1;

    item = MenuItem();
    item.label = "Rejuv Pot";
    item.type = Checkbox;
    item.boolValue = &V_AutoPotRejuvEnabled;
    item.indent = 1;
    item.parentIndex = autoPotParent;
    g_menuColumn2.push_back(item);
    int rejuvPotParent = g_menuColumn2.size() - 1;

    item = MenuItem();
    item.label = "Threshold";
    item.type = IntSlider;
    item.intValue = &V_AutoPotRejuvThreshold;
    item.intMin = 0;
    item.intMax = 99;
    item.indent = 2;
    item.parentIndex = rejuvPotParent;
    g_menuColumn2.push_back(item);

    item = MenuItem();
    item.label = "Mana Pot";
    item.type = Checkbox;
    item.boolValue = &V_AutoPotManaEnabled;
    item.indent = 1;
    item.parentIndex = autoPotParent;
    g_menuColumn2.push_back(item);
    int manaPotParent = g_menuColumn2.size() - 1;

    item = MenuItem();
    item.label = "Threshold";
    item.type = IntSlider;
    item.intValue = &V_AutoPotManaThreshold;
    item.intMin = 0;
    item.intMax = 99;
    item.indent = 2;
    item.parentIndex = manaPotParent;
    g_menuColumn2.push_back(item);

    item = MenuItem();
    item.label = "Life Pot";
    item.type = Checkbox;
    item.boolValue = &V_AutoPotLifeEnabled;
    item.indent = 1;
    item.parentIndex = autoPotParent;
    g_menuColumn2.push_back(item);
    int lifePotParent = g_menuColumn2.size() - 1;

    item = MenuItem();
    item.label = "Threshold";
    item.type = IntSlider;
    item.intValue = &V_AutoPotLifeThreshold;
    item.intMin = 0;
    item.intMax = 99;
    item.indent = 2;
    item.parentIndex = lifePotParent;
    g_menuColumn2.push_back(item);

    item = MenuItem();
    item.label = "Merc Rejuv Pot";
    item.type = Checkbox;
    item.boolValue = &V_MercAutoPotRejuvEnabled;
    item.indent = 1;
    item.parentIndex = autoPotParent;
    g_menuColumn2.push_back(item);
    int mercRejuvPotParent = g_menuColumn2.size() - 1;

    item = MenuItem();
    item.label = "Threshold";
    item.type = IntSlider;
    item.intValue = &V_MercAutoPotRejuvThreshold;
    item.intMin = 0;
    item.intMax = 99;
    item.indent = 2;
    item.parentIndex = mercRejuvPotParent;
    g_menuColumn2.push_back(item);

    item = MenuItem();
    item.label = "Merc Life Pot";
    item.type = Checkbox;
    item.boolValue = &V_MercAutoPotLifeEnabled;
    item.indent = 1;
    item.parentIndex = autoPotParent;
    g_menuColumn2.push_back(item);
    int mercLifePotParent = g_menuColumn2.size() - 1;

    item = MenuItem();
    item.label = "Threshold";
    item.type = IntSlider;
    item.intValue = &V_MercAutoPotLifeThreshold;
    item.intMin = 0;
    item.intMax = 99;
    item.indent = 2;
    item.parentIndex = mercLifePotParent;
    g_menuColumn2.push_back(item);

    // Auto Refill Category
    item = MenuItem();
    item.label = "Refill Potions";
    item.type = Category;
    item.boolValue = &V_AutoRefillEnabled;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn2.push_back(item);
    int autoRefillParent = g_menuColumn2.size() - 1;

    item = MenuItem();
    item.label = "Slot 1";
    item.type = IntSlider;
    item.intValue = (DWORD *)&V_RefillSlot1PotionType;
    item.intMin = 0;
    item.intMax = 4;
    item.indent = 1;
    item.parentIndex = autoRefillParent;
    g_menuColumn2.push_back(item);
    item = MenuItem();
    item.label = "Slot 2";
    item.type = IntSlider;
    item.intValue = (DWORD *)&V_RefillSlot2PotionType;
    item.intMin = 0;
    item.intMax = 4;
    item.indent = 1;
    item.parentIndex = autoRefillParent;
    g_menuColumn2.push_back(item);
    item = MenuItem();
    item.label = "Slot 3";
    item.type = IntSlider;
    item.intValue = (DWORD *)&V_RefillSlot3PotionType;
    item.intMin = 0;
    item.intMax = 4;
    item.indent = 1;
    item.parentIndex = autoRefillParent;
    g_menuColumn2.push_back(item);
    item = MenuItem();
    item.label = "Slot 4";
    item.type = IntSlider;
    item.intValue = (DWORD *)&V_RefillSlot4PotionType;
    item.intMin = 0;
    item.intMax = 4;
    item.indent = 1;
    item.parentIndex = autoRefillParent;
    g_menuColumn2.push_back(item);

    // Auto Exit Category
    item = MenuItem();
    item.label = "Auto Exit";
    item.type = Category;
    item.boolValue = &V_AutoExitEnabled;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn2.push_back(item);
    int autoExitParent = g_menuColumn2.size() - 1;

    item = MenuItem();
    item.label = "Threshold";
    item.type = IntSlider;
    item.intValue = &V_AutoExitLifeThreshold;
    item.intMin = 0;
    item.intMax = 99;
    item.indent = 1;
    item.parentIndex = autoExitParent;
    g_menuColumn2.push_back(item);

    item = MenuItem();
    item.label = "Pickit (Do not use!)";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn2.push_back(item);

    item = MenuItem();
    item.label = "Pickit Enabled";
    item.type = Checkbox;
    item.boolValue = &V_PickitEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn2.push_back(item);

    // --- COLUMN 3 ---

    item = MenuItem();
    item.label = "Misc";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Vendor Preview";
    item.type = Checkbox;
    item.boolValue = &V_VendorPreviewEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Town Portal Labels";
    item.type = Checkbox;
    item.boolValue = &V_TownPortalOwnerLabelsEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Monster Icons";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    AddMonsterMarkerMenuItems(g_menuColumn3);

    item = MenuItem();
    item.label = "Monster HP Percent";
    item.type = Checkbox;
    item.boolValue = &V_DrawMonsterHealthPercent;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Monster Class IDs";
    item.type = Checkbox;
    item.boolValue = &V_DrawMonsterClassIds;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Debug Drawing";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item.label = "Mouse / Player Coords";
    item.type = Checkbox;
    item.boolValue = &V_DebugMouseCoordinates;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Nearest Item";
    item.type = Checkbox;
    item.boolValue = &V_DebugNearestItem;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Player Inventory";
    item.type = Checkbox;
    item.boolValue = &V_DebugPlayerInventory;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Player Belt";
    item.type = Checkbox;
    item.boolValue = &V_DebugPlayerBelt;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Closest Monster Stats";
    item.type = Checkbox;
    item.boolValue = &V_DebugClosestMonster;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Current Room Info";
    item.type = Checkbox;
    item.boolValue = &V_DebugCurrentRoom;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Vendor Shortcuts";
    item.type = Checkbox;
    item.boolValue = &V_VendorShortcutEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Anya Auto Purchase";
    item.type = Checkbox;
    item.boolValue = &V_AnyaAutoPurchaseEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Trade Reply (/r game / pass)";
    item.type = Checkbox;
    item.boolValue = &V_TradeInviteReplyEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Martial Arts Charges";
    item.type = Checkbox;
    item.boolValue = &V_MartialArtsChargesEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Martial Arts Automate";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Enable Build MA Charges";
    item.type = Checkbox;
    item.boolValue = &V_BuildMAChargesEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Enable MA Automate";
    item.type = Checkbox;
    item.boolValue = &V_MAAutomateEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "MA Skill Bindings";
    item.type = Label;
    item.indent = 0;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Martial Art Skill Button";
    item.type = Button;
    item.callback = ToggleMartialArtSkillButtonInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    item = MenuItem();
    item.label = "Finisher Skill Button";
    item.type = Button;
    item.callback = ToggleFinisherSkillButtonInput;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);

    BuildMenuTabs();
}

void DrawMenuColumn(std::vector<MenuItem> &column, int base_x, int &currentY, int scrollOffset)
{
    const int itemHeight = 20;
    const int indentWidth = 20;

    for (size_t i = 0; i < column.size(); ++i)
    {
        MenuItem &item = column[i];

        if (i > 0)
        {
            bool isNewGroup = (item.type == Label || item.type == Category) && item.indent == 0;
            if (isNewGroup)
            {
                bool isCategoryFollowingLabel = (item.type == Category && column[i - 1].type == Label && column[i - 1].indent == 0);
                if (!isCategoryFollowingLabel)
                {
                    currentY += itemHeight;
                }
            }
        }

        bool isThresholdSlider = item.type == IntSlider && strcmp(item.label, "Threshold") == 0 && item.parentIndex != -1;

        if (isThresholdSlider)
        {
            MenuItem &parent = column[item.parentIndex];
            item.x = parent.x + 120;
            item.y = parent.y;
        }
        else
        {
            item.x = base_x + (item.indent * indentWidth);
            item.y = currentY - scrollOffset;
        }

        int menuTop = GetMenuTop();
        bool isVisible = item.y >= menuTop + MENU_VIEW_TOP - itemHeight && item.y <= menuTop + MENU_VIEW_BOTTOM;

        if (isVisible)
        {
            switch (item.type)
            {
            case Category:
            case Checkbox:
                item.width = 12;
                item.height = 12;
                DrawCheckBox(item.x, item.y, item.width, 6, *item.boolValue, COLOR_WHITE, FONTCOLOR_WHITE, "%s", (LPSTR)item.label);
                break;
            case IntSlider:
                if (IsMonsterMarkerStyleSlider(item))
                {
                    DrawIncreaseDecrease(item.x, item.y + 5, FALSE, COLOR_WHITE);
                    DrawTextB(item.x + 20, item.y + 15, FONTCOLOR_WHITE, 6, -1, "%s: %s", (LPSTR)item.label, GetMonsterMarkerStyleName(*item.intValue));
                    DrawIncreaseDecrease(item.x + 130, item.y + 5, TRUE, COLOR_WHITE);
                }
                else if (IsMonsterMarkerFontSlider(item))
                {
                    DrawIncreaseDecrease(item.x, item.y + 5, FALSE, COLOR_WHITE);
                    DrawTextB(item.x + 20, item.y + 15, FONTCOLOR_WHITE, 6, -1, "%s: %d", (LPSTR)item.label, *item.intValue);
                    DrawIncreaseDecrease(item.x + 130, item.y + 5, TRUE, COLOR_WHITE);
                }
                else if (item.label == "Slot 1" || item.label == "Slot 2" || item.label == "Slot 3" || item.label == "Slot 4")
                {
                    const int decrease_button_x = item.x + 50;
                    const int potion_name_x = decrease_button_x + 25;
                    const int increase_button_x = potion_name_x + 55;

                    DrawTextB(item.x, item.y + 15, FONTCOLOR_WHITE, 6, -1, "%s:", (LPSTR)item.label);
                    DrawIncreaseDecrease(decrease_button_x, item.y + 5, FALSE, COLOR_WHITE);
                    DrawTextB(potion_name_x, item.y + 15, FONTCOLOR_WHITE, 6, -1, "%s", GetPotionTypeName(*(int *)item.intValue));
                    DrawIncreaseDecrease(increase_button_x, item.y + 5, TRUE, COLOR_WHITE);
                }
                else if (isThresholdSlider)
                {
                    DrawIncreaseDecrease(item.x, item.y, FALSE, COLOR_WHITE);
                    DrawTextB(item.x + 20, item.y + 10, FONTCOLOR_WHITE, 6, -1, "%d%%", *item.intValue);
                    DrawIncreaseDecrease(item.x + 50, item.y, TRUE, COLOR_WHITE);
                }
                else
                {
                    DrawIncreaseDecrease(item.x, item.y + 5, FALSE, COLOR_WHITE);
                    DrawTextB(item.x + 20, item.y + 15, FONTCOLOR_WHITE, 6, -1, "%s: %d%%", (LPSTR)item.label, *item.intValue);
                    DrawIncreaseDecrease(item.x + 100, item.y + 5, TRUE, COLOR_WHITE);
                }
                break;
            case Button:
            {
                int textX = item.x;
                int valueX = item.x + 150;

                // Align these two rows with checkbox text in the same subsection.
                if (item.callback == ToggleMartialArtSkillButtonInput || item.callback == ToggleFinisherSkillButtonInput)
                {
                    textX = item.x + 18;
                    valueX = item.x + 168;
                }

                DrawTextB(textX, item.y, FONTCOLOR_WHITE, 6, -1, "%s", (LPSTR)item.label);
                if (item.callback == ToggleMenuKeyInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_MENU_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_MainMenuKey));
                }
                else if (item.callback == ToggleRefillPotionsKeyInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_REFILL_POTIONS_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_RefillPotionsKey));
                }
                else if (item.callback == TogglePickitKeyInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_PICKIT_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_PickitKey));
                }
                else if (item.callback == ToggleAnyaBotKeyInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_ANYA_BOT_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_AnyaBotKey));
                }
                else if (item.callback == ToggleTradeInviteReplyKeyInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_TRADE_INVITE_REPLY_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_TradeInviteReplyKey));
                }
                else if (item.callback == ToggleBuildMAChargesKeyInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_BUILD_MA_CHARGES_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_BuildMAChargesKey));
                }
                else if (item.callback == ToggleMAAutomateKeyInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_MA_AUTOMATE_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_MAAutomateKey));
                }
                else if (item.callback == ToggleMartialArtSkillButtonInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_MARTIAL_ART_SKILL_BUTTON ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_MartialArtSkillButton));
                }
                else if (item.callback == ToggleFinisherSkillButtonInput)
                {
                    DrawTextB(valueX, item.y, V_InputModeType == MODE_FINISHER_SKILL_BUTTON ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_FinisherSkillButton));
                }
                break;
            }
            case Label:
                DrawTextB(item.x, item.y, FONTCOLOR_GOLD, 8, -1, "%s", (LPSTR)item.label);
                break;
            }
        }

        if (!isThresholdSlider)
        {
            currentY += itemHeight;
        }
    }
}

std::vector<MenuItem> &GetSelectedMenuItems()
{
    switch (g_selectedMenuTab)
    {
    case MENU_TAB_MONSTERS:
        return g_menuMonsters;
    case MENU_TAB_DEBUG:
        return g_menuDebug;
    case MENU_TAB_AUTOMATION:
        return g_menuAutomation;
    case MENU_TAB_HOTKEYS:
        return g_menuHotkeys;
    case MENU_TAB_MAP:
    default:
        return g_menuMap;
    }
}

void DrawMenuTabs()
{
    const int tabX = GetMenuLeft() + 5;
    const int tabY = GetMenuTop() + 40;
    const int tabWidth = 100;
    const int tabHeight = 20;

    for (int i = 0; i < MENU_TAB_COUNT; ++i)
    {
        int x = tabX + (i * tabWidth);
        DWORD color = i == g_selectedMenuTab ? FONTCOLOR_GOLD : FONTCOLOR_WHITE;

        DrawBox(x, tabY, x + tabWidth - 8, tabY + tabHeight, COLOR_WHITE);
        DrawTextB(x + 8, tabY + 15, color, 6, -1, "%s", g_menuTabNames[i]);
    }
}

bool HandleMenuTabClick(int mouseX, int mouseY)
{
    const int tabX = GetMenuLeft() + 5;
    const int tabY = GetMenuTop() + 40;
    const int tabWidth = 100;
    const int tabHeight = 20;

    if (mouseY < tabY || mouseY > tabY + tabHeight)
        return false;

    for (int i = 0; i < MENU_TAB_COUNT; ++i)
    {
        int x = tabX + (i * tabWidth);
        if (mouseX >= x && mouseX <= x + tabWidth - 8)
        {
            g_selectedMenuTab = i;
            return true;
        }
    }

    return false;
}

void DrawMenu()
{
    if (!V_MainMenuOpen)
        return;

    int menuLeft = GetMenuLeft();
    int menuTop = GetMenuTop();

    D2GFX_DrawRectangle(menuLeft, menuTop, menuLeft + MENU_WIDTH, menuTop + MENU_HEIGHT, 0, 1);
    DrawTextB(menuLeft + 5, menuTop + 20, FONTCOLOR_GOLD, 7, -1, "EZPD2 Settings");
    DrawMenuTabs();

    int currentY = menuTop + MENU_VIEW_TOP;
    DrawMenuColumn(GetSelectedMenuItems(), menuLeft + 15, currentY, g_menuScrollOffsets[g_selectedMenuTab]);

    if (g_menuScrollOffsets[g_selectedMenuTab] > 0)
        DrawTextB(menuLeft + MENU_WIDTH - 55, menuTop + MENU_VIEW_TOP - 5, FONTCOLOR_LIGHTGREY, 6, -1, "scroll: %d", g_menuScrollOffsets[g_selectedMenuTab]);
}

bool HandleMenuClickColumn(std::vector<MenuItem> &column, int mouseX, int mouseY)
{
    for (size_t i = 0; i < column.size(); ++i)
    {
        MenuItem &item = column[i];
        bool clicked = false;

        int menuTop = GetMenuTop();
        if (item.y < menuTop + MENU_VIEW_TOP || item.y > menuTop + MENU_VIEW_BOTTOM)
            continue;

        switch (item.type)
        {
        case Category:
        case Checkbox:
            if (mouseX >= item.x && mouseX <= item.x + item.width && mouseY >= item.y && mouseY <= item.y + item.height)
            {
                *item.boolValue = !(*item.boolValue);
                clicked = true;
            }
            break;
        case IntSlider:
            if (item.label == "Slot 1" || item.label == "Slot 2" || item.label == "Slot 3" || item.label == "Slot 4")
            {
                const int decrease_button_x = item.x + 50;
                const int potion_name_x = decrease_button_x + 25;
                const int increase_button_x = potion_name_x + 55;

                if (mouseX >= decrease_button_x && mouseX <= decrease_button_x + 10 && mouseY >= item.y + 5 && mouseY <= item.y + 15)
                {
                    *(int *)item.intValue = SetValidPotionForRefillSlot(*(int *)item.intValue - 1);
                    clicked = true;
                }
                if (mouseX >= increase_button_x && mouseX <= increase_button_x + 10 && mouseY >= item.y + 5 && mouseY <= item.y + 15)
                {
                    *(int *)item.intValue = SetValidPotionForRefillSlot(*(int *)item.intValue + 1);
                    clicked = true;
                }
            }
            else
            {
                bool isThresholdSlider = item.type == IntSlider && strcmp(item.label, "Threshold") == 0 && item.parentIndex != -1;
                int y_offset = isThresholdSlider ? 0 : 5;
                int increase_button_x = isThresholdSlider ? item.x + 50 : item.x + 100;

                if (IsMonsterMarkerStyleSlider(item) || IsMonsterMarkerFontSlider(item))
                    increase_button_x = item.x + 130;

                int step = 1;
                if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
                    step = 5;
                else if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
                    step = 20;

                // Decrease button
                if (mouseX >= item.x && mouseX <= item.x + 10 && mouseY >= item.y + y_offset && mouseY <= item.y + y_offset + 10)
                {
                    int currentValue = *item.intValue;
                    currentValue -= step;
                    if (currentValue < item.intMin)
                        currentValue = item.intMin;
                    *item.intValue = currentValue;
                    clicked = true;
                }
                // Increase button
                if (mouseX >= increase_button_x && mouseX <= increase_button_x + 10 && mouseY >= item.y + y_offset && mouseY <= item.y + y_offset + 10)
                {
                    *item.intValue += step;
                    if (*item.intValue > item.intMax)
                        *item.intValue = item.intMax;
                    clicked = true;
                }
            }
            break;
        case Button:
            if (mouseX >= item.x && mouseX <= item.x + 200 && mouseY >= item.y - 5 && mouseY <= item.y + 15)
            {
                if (item.callback)
                {
                    item.callback();
                }
                clicked = true;
            }
            break;
        case Label:
            break;
        }

        if (clicked)
        {
            return true;
        }
    }
    return false;
}

void HandleMenuScroll(int delta)
{
    if (!V_MainMenuOpen)
        return;

    int &scrollOffset = g_menuScrollOffsets[g_selectedMenuTab];

    if (delta > 0)
        scrollOffset -= 40;
    else if (delta < 0)
        scrollOffset += 40;

    if (scrollOffset < 0)
        scrollOffset = 0;
    if (scrollOffset > 1000)
        scrollOffset = 1000;
}

bool HandleMenuClick(int mouseX, int mouseY)
{
    if (!V_MainMenuOpen)
        return false;

    if (HandleMenuTabClick(mouseX, mouseY))
        return true;

    return HandleMenuClickColumn(GetSelectedMenuItems(), mouseX, mouseY);
}
