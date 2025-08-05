#include "Hack.h"
#include "Menu.h"
#include <cstring>

std::vector<MenuItem> g_menuColumn1;
std::vector<MenuItem> g_menuColumn2;
std::vector<MenuItem> g_menuColumn3;

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
    item.label = "Vendor Shortcuts";
    item.type = Checkbox;
    item.boolValue = &V_VendorShortcutEnabled;
    item.indent = 1;
    item.parentIndex = -1;
    g_menuColumn3.push_back(item);
}

void DrawMenuColumn(std::vector<MenuItem> &column, int base_x, int &currentY)
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
            item.y = currentY;
        }

        switch (item.type)
        {
        case Category:
        case Checkbox:
            item.width = 12;
            item.height = 12;
            DrawCheckBox(item.x, item.y, item.width, 6, *item.boolValue, COLOR_WHITE, FONTCOLOR_WHITE, (LPSTR)item.label);
            break;
        case IntSlider:
            if (item.label == "Slot 1" || item.label == "Slot 2" || item.label == "Slot 3" || item.label == "Slot 4")
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
            DrawTextB(item.x, item.y, FONTCOLOR_WHITE, 6, -1, (LPSTR)item.label);
            if (item.callback == ToggleMenuKeyInput)
            {
                DrawTextB(item.x + 150, item.y, V_InputModeType == MODE_MENU_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_MainMenuKey));
            }
            else if (item.callback == ToggleRefillPotionsKeyInput)
            {
                DrawTextB(item.x + 150, item.y, V_InputModeType == MODE_REFILL_POTIONS_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_RefillPotionsKey));
            }
            else if (item.callback == TogglePickitKeyInput)
            {
                DrawTextB(item.x + 150, item.y, V_InputModeType == MODE_PICKIT_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_PickitKey));
            }
            else if (item.callback == ToggleAnyaBotKeyInput)
            {
                DrawTextB(item.x + 150, item.y, V_InputModeType == MODE_ANYA_BOT_KEY ? FONTCOLOR_BLUE : FONTCOLOR_GOLD, 6, -1, "%s", GetKeyName(V_AnyaBotKey));
            }
            break;
        case Label:
            DrawTextB(item.x, item.y, FONTCOLOR_GOLD, 8, -1, (LPSTR)item.label);
            break;
        }

        if (!isThresholdSlider)
        {
            currentY += itemHeight;
        }
    }
}

void DrawMenu()
{
    if (!V_MainMenuOpen)
        return;

    D2GFX_DrawRectangle(25, 10, 900, 450, 0, 1);
    DrawTextB(30, 30, FONTCOLOR_GOLD, 7, -1, "EZPD2 Settings");

    int currentY = 60;
    DrawMenuColumn(g_menuColumn1, 30, currentY);
    currentY = 60;
    DrawMenuColumn(g_menuColumn2, 300, currentY);
    currentY = 60;
    DrawMenuColumn(g_menuColumn3, 570, currentY);
}

bool HandleMenuClickColumn(std::vector<MenuItem> &column, int mouseX, int mouseY)
{
    for (size_t i = 0; i < column.size(); ++i)
    {
        MenuItem &item = column[i];
        bool clicked = false;
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

void HandleMenuClick(int mouseX, int mouseY)
{
    if (!V_MainMenuOpen)
        return;

    if (HandleMenuClickColumn(g_menuColumn1, mouseX, mouseY))
        return;
    if (HandleMenuClickColumn(g_menuColumn2, mouseX, mouseY))
        return;
    if (HandleMenuClickColumn(g_menuColumn3, mouseX, mouseY))
        return;
}
