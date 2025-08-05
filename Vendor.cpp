#include "Hack.h"

VOID DrawVendorPreview()
{
    if (!V_VendorPreviewEnabled)
        return;

    if (V_MainMenuOpen)
        return;

    LPUNITANY vendor = D2CLIENT_GetCurrentInteractingNPC();
    if (!vendor)
        return;

    // Get the vendor's inventory
    LPINVENTORY vendorInventory = vendor->pInventory;
    if (!vendorInventory)
        return;

    int x = 10;
    int y = 80;
    int lineHeight = 70;

    if (strlen(ITEM_NAME_SEARCH) == 0)
        return; // Don't search for an empty string

    for (LPUNITANY pItem = vendor->pInventory->pFirstItem; pItem; pItem = (pItem->pItemData ? pItem->pItemData->pNextInvItem : NULL))
    {
        if (pItem && pItem->pItemData)
        {
            wchar_t wFullDesc[2048];
            D2CLIENT_GetItemName(pItem, wFullDesc, sizeof(wFullDesc) / sizeof(wFullDesc[0]));

            char szFullDesc[2048];
            WideCharToMultiByte(CP_ACP, 0, wFullDesc, -1, szFullDesc, sizeof(szFullDesc), NULL, NULL);

            // Create a clean, lowercase version of the full description for searching
            char cleanedDesc[2048];
            CreateCleanItemName(szFullDesc, cleanedDesc, sizeof(cleanedDesc));

            if (strstr(cleanedDesc, ITEM_NAME_SEARCH) != NULL)
            {
                DWORD textWidth = GetTextWidth(szFullDesc, 6);
                D2GFX_DrawRectangle(x - 5, y - 30, x + textWidth + 5, y + 5, 0, 1);
                DrawTextB(x, y, FONTCOLOR_WHITE, 6, -1, szFullDesc);
                y += lineHeight;
            }
        }
    }
}

VOID VendorShortcut()
{
    if (!V_VendorShortcutEnabled)
        return;

    if (!Me)
        return;

    if (V_MainMenuOpen || !IsPlayerInTown())
        return;

    LPUNITANY vendor = D2CLIENT_GetCurrentInteractingNPC();
    if (!vendor)
        return;

    BOOL vendorActive = GetUIVar(UI_NPCMENU) == 1;
    BOOL tradeActive = GetUIVar(UI_NPCSHOP) == 1;

    if (vendorActive && !tradeActive)
    {
        for (int i = 0; i < numVendorShortcuts; i++)
        {
            if (vendor->dwTxtFileNo == vendorShortcuts[i].textfileno)
            {
                if (!KeyDown(VK_CONTROL) && !KeyDown(VK_LCONTROL) && !KeyDown(VK_RCONTROL))
                    return;

                DWORD initalMouseX = MouseX;
                DWORD initalMouseY = MouseY;

                POINT screenPos = {vendor->pPath->xPos, vendor->pPath->yPos};
                WorldToScreen(&screenPos);
                screenPos.y -= vendorShortcuts[i].y_offset;

                SimulateLeftClick(screenPos);
                SetCursorPos(initalMouseX, initalMouseY);
                return;
            }
        }
    }
}

VOID AnyaBot()
{
    if (!V_AnyaBotRunning)
        return;

    Level *playerLevel = GetUnitLevel(Me);
    if (!playerLevel)
        return;

    BOOL vendorActive = GetUIVar(UI_NPCMENU) == 1;
    BOOL tradeActive = GetUIVar(UI_NPCSHOP) == 1;

    // If in town and items not checked yet
    if ((playerLevel->dwLevelNo == MAP_A5_HARROGATH || playerLevel->dwLevelNo == MAP_A1_ROGUE_ENCAMPMENT) && !V_AnyaBotCheckedItems && !vendorActive)
    {
        for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
        {
            for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
            {
                if (Unit && Unit->dwType == UNIT_TYPE_NPC && (Unit->dwTxtFileNo == NPCID_Anya || Unit->dwTxtFileNo == NPCID_Akara || Unit->dwTxtFileNo == NPCID_Charsi))
                {
                    WORD xPos = Unit->pPath->xPos;
                    WORD yPos = Unit->pPath->yPos;
                    POINT screenPos = {xPos, yPos};
                    WorldToScreen(&screenPos);

                    // Check if item is in pickit area
                    RECT clickableArea = GetSafeScreenAreaRect();
                    if (!IsPointInRect(screenPos, clickableArea))
                    {
                        PrintText(FONTCOLOR_RED, "Vendor is outside of clickable area");
                        ExitAnyaBot();
                        return;
                    }

                    // Need to mouseover vendor first, else the click will not register
                    if (V_TickCount == 35)
                    {
                        SetCursorPos(screenPos.x, screenPos.y);
                        return;
                    }
                    else if (V_TickCount == 45)
                    {
                        SimulateLeftClick(screenPos);
                        return;
                    }
                }
            }
        }
    }

    // Click on "Trade" once the vendor becomes active
    if (vendorActive && !tradeActive && !V_AnyaBotCheckedItems)
    {
        if (V_TickCount == 25)
        {
            // LPUNITANY vendor = D2CLIENT_GetCurrentInteractingNPC();
            // if (!vendor)
            //     return;

            // WORD xPos = D2CLIENT_GetUnitX(vendor);
            // WORD yPos = D2CLIENT_GetUnitY(vendor);
            // POINT screenPos = {xPos, yPos};
            // WorldToScreen(&screenPos);

            // screenPos.y -= 125;

            // SimulateLeftClick(screenPos);
            // return;

            SimulateKeyPress(VK_DOWN);
            SimulateKeyPress(VK_RETURN);
            return;
        }
    }

    // Check for items once Trade window is active
    if (tradeActive && !V_AnyaBotCheckedItems)
    {
        LPUNITANY vendor = D2CLIENT_GetCurrentInteractingNPC();
        if (!vendor)
            return;

        // Get the vendor's inventory
        LPINVENTORY vendorInventory = vendor->pInventory;
        if (!vendorInventory)
            return;

        if (strlen(ITEM_NAME_SEARCH) == 0)
            return; // Don't search for an empty string

        for (LPUNITANY pItem = vendor->pInventory->pFirstItem; pItem; pItem = (pItem->pItemData ? pItem->pItemData->pNextInvItem : NULL))
        {
            if (pItem && pItem->pItemData)
            {
                wchar_t wFullDesc[2048];
                D2CLIENT_GetItemName(pItem, wFullDesc, sizeof(wFullDesc) / sizeof(wFullDesc[0]));

                char szFullDesc[2048];
                WideCharToMultiByte(CP_ACP, 0, wFullDesc, -1, szFullDesc, sizeof(szFullDesc), NULL, NULL);

                // Create a clean, lowercase version of the full description for searching
                char cleanedDesc[2048];
                CreateCleanItemName(szFullDesc, cleanedDesc, sizeof(cleanedDesc));

                if (strstr(cleanedDesc, ITEM_NAME_SEARCH) != NULL)
                {
                    PrintText(FONTCOLOR_YELLOW, "Found Item to buy");
                    ExitAnyaBot();
                    return;
                }
            }
        }

        D2CLIENT_CloseInteract();
        V_AnyaBotCheckedItems = TRUE;
        return;
    }

    if ((playerLevel->dwLevelNo == MAP_A5_HARROGATH || playerLevel->dwLevelNo == MAP_A1_ROGUE_ENCAMPMENT) && V_AnyaBotCheckedItems && Me->dwMode == PLAYER_MODE_STAND_INTOWN)
    {
        for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
        {
            for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
            {
                if (Unit && Unit->dwTxtFileNo == 60)
                {
                    WORD xPos = D2CLIENT_GetUnitX(Unit);
                    WORD yPos = D2CLIENT_GetUnitY(Unit);
                    POINT screenPos = {xPos, yPos};
                    WorldToScreen(&screenPos);

                    // Check if item is in pickit area
                    RECT clickableArea = GetSafeScreenAreaRect();
                    if (!IsPointInRect(screenPos, clickableArea))
                    {
                        PrintText(FONTCOLOR_RED, "Portal is outside of clickable area");
                        ExitAnyaBot();
                        return;
                    }

                    if (V_TickCount == 35)
                    {
                        SetCursorPos(screenPos.x, screenPos.y);
                        return;
                    }
                    else if (V_TickCount == 45)
                    {
                        SimulateLeftClick(screenPos);
                        return;
                    }
                }
            }
        }
    }

    if ((playerLevel->dwLevelNo == MAP_A5_NIHLATHAKS_TEMPLE || playerLevel->dwLevelNo == MAP_PVP_MOOR_ARENA) && Me->dwMode == PLAYER_MODE_STAND_OUTTOWN)
    {
        V_AnyaBotCheckedItems = FALSE;

        for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
        {
            for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
            {
                if (Unit && Unit->dwTxtFileNo == 60)
                {
                    WORD xPos = D2CLIENT_GetUnitX(Unit);
                    WORD yPos = D2CLIENT_GetUnitY(Unit);
                    POINT screenPos = {xPos, yPos};
                    WorldToScreen(&screenPos);

                    // Check if item is in pickit area
                    RECT clickableArea = GetSafeScreenAreaRect();
                    if (!IsPointInRect(screenPos, clickableArea))
                    {
                        PrintText(FONTCOLOR_RED, "Portal is outside of clickable area");
                        ExitAnyaBot();
                        return;
                    }

                    if (V_TickCount == 35)
                    {
                        SetCursorPos(screenPos.x, screenPos.y);
                        return;
                    }
                    else if (V_TickCount == 45)
                    {
                        SimulateLeftClick(screenPos);
                        return;
                    }
                }
            }
        }
    }
}

VOID ExitAnyaBot()
{
    V_AnyaBotRunning = FALSE;
    V_AnyaBotCheckedItems = FALSE;

    PrintText(FONTCOLOR_RED, "Anya Bot stopped");
}