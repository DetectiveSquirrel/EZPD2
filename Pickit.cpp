#include "Hack.h"

VOID PickUpItems()
{
    if (!V_PickitEnabled)
        return;

    // Don't run pickit if potions are being refilled
    if (V_IsRefillingPotions)
        return;

    if (GetUIVar(UI_CHAT) == 1)
        return;

    if (!Me || !Me->pStats)
        return;

    if (Me->dwMode == PLAYER_MODE_DEAD)
        return;

    if (V_IsPickingUpItems)
    {
        // If key is released -> exit and cleanup
        if (!KeyDown(V_PickitKey))
        {
            ExitPickingUpItems();
            return;
        }

        if (V_TickCount % 40 != 0)
            return;

        // Get closest item
        LPUNITANY closestItem = GetClosestUnit(UNIT_TYPE_ITEM);
        if (!closestItem)
            return;

        // Get item world coordinates
        INT itemWorldX = D2CLIENT_GetUnitX(closestItem);
        INT itemWorldY = D2CLIENT_GetUnitY(closestItem);

        // Convert item world coordinates to screen coordinates
        POINT itemScreenPos = {itemWorldX, itemWorldY};
        WorldToScreen(&itemScreenPos);

        // Check if item is in pickit area
        RECT pickitArea = GetSafeScreenAreaRect();
        if (!IsPointInRect(itemScreenPos, pickitArea))
            return;

        // Print item name + screen coordinates
        // CHAR szItemName[128] = "Unknown Item";
        // WCHAR wItemName[128] = {0};
        // D2CLIENT_GetItemNameString(closestItem, wItemName, 128);
        // if (wItemName[0] != L'\0')
        // {
        //     WideCharToMultiByte(CP_ACP, 0, wItemName, -1, szItemName, sizeof(szItemName), 0, 0);
        // }
        // PrintText(1, "Name: %s, Screen Coords: %d, %d", szItemName, itemScreenPos.x, itemScreenPos.y);
        
        SimulateLeftClick(itemScreenPos);
    }
}

VOID ExitPickingUpItems()
{
    SetCursorPos(V_InitialCursorPos.x, V_InitialCursorPos.y);
    V_IsPickingUpItems = FALSE;
    PrintText(1, "Exited Pickit");
}
