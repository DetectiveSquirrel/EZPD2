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

                POINT screenPos = { vendor->pPath->xPos, vendor->pPath->yPos };
                WorldToScreen(&screenPos);
                screenPos.y -= vendorShortcuts[i].y_offset;

                SimulateLeftClick(screenPos);
                SetCursorPos(initalMouseX, initalMouseY);
                return;
            }
        }
    }
}

static DWORD anyaStateTick = 0;
static AnyaBotState anyaState = ANYA_IDLE;
static int anyaRetries = 0;

VOID AnyaBot()
{
    if (!V_AnyaBotRunning)
        return;

    Level* playerLevel = GetUnitLevel(Me);
    if (!playerLevel)
        return;

    // Safety check: if we are stuck in a state for too long (e.g. 10 seconds), reset
    if (GetTickCount() - anyaStateTick > 10000 && anyaState != ANYA_IDLE)
    {
        // Exception: Shopping might take longer, but we have internal logic for that.
        // For now, let's just log and reset if truly stuck.
        PrintText(FONTCOLOR_RED, "AnyaBot stuck, resetting state.");
        anyaState = ANYA_IDLE;
        anyaStateTick = GetTickCount();
    }

    // State Machine
    switch (anyaState)
    {
    case ANYA_IDLE:
        // entry point. Decide where to go.
        if (playerLevel->dwLevelNo == MAP_A5_HARROGATH || playerLevel->dwLevelNo == MAP_A1_ROGUE_ENCAMPMENT)
        {
            if (!V_AnyaBotCheckedItems)
            {
                anyaState = ANYA_MOVING_TO_VENDOR;
                anyaStateTick = GetTickCount();
            }
            else
            {
                anyaState = ANYA_MOVING_TO_PORTAL;
                anyaStateTick = GetTickCount();
            }
        }
        else if (playerLevel->dwLevelNo == MAP_A5_NIHLATHAKS_TEMPLE || playerLevel->dwLevelNo == MAP_PVP_MOOR_ARENA)
        {
            // We are outside, need to return
            anyaState = ANYA_RETURNING;
            anyaStateTick = GetTickCount();
        }
        break;

    case ANYA_MOVING_TO_VENDOR:
        // Logic: Find vendor, check distance, click if close enough
        if (GetTickCount() - anyaStateTick < 500) return; // Wait a bit after state change

        if (GetUIVar(UI_NPCMENU))
        {
            // Already interacting
            anyaState = ANYA_INTERACTING_VENDOR;
            anyaStateTick = GetTickCount();
            return;
        }

        for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
        {
            for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
            {
                if (Unit && Unit->dwType == UNIT_TYPE_NPC && (Unit->dwTxtFileNo == NPCID_Anya || Unit->dwTxtFileNo == NPCID_Akara || Unit->dwTxtFileNo == NPCID_Charsi))
                {
                    WORD xPos = Unit->pPath->xPos;
                    WORD yPos = Unit->pPath->yPos;
                    POINT screenPos = { xPos, yPos };
                    WorldToScreen(&screenPos);

                    SimulateLeftClick(screenPos);

                    // Assume we clicked, wait for UI
                    anyaState = ANYA_INTERACTING_VENDOR;
                    anyaStateTick = GetTickCount();
                    anyaRetries = 0;
                    return;
                }
            }
        }
        break;

    case ANYA_INTERACTING_VENDOR:
        // Wait for NPC menu
        if (GetTickCount() - anyaStateTick > 3000)
        {
            // Timeout waiting for menu
            if (anyaRetries < 3)
            {
                anyaRetries++;
                anyaState = ANYA_MOVING_TO_VENDOR; // Try moving/clicking again
                anyaStateTick = GetTickCount();
                return;
            }
            else
            {
                PrintText(FONTCOLOR_RED, "Failed to open vendor menu.");
                ExitAnyaBot();
                return;
            }
        }

        if (GetUIVar(UI_NPCMENU) && !GetUIVar(UI_NPCSHOP))
        {
            // Menu open, click trade
            // Simple hardcoded keypress for now, but timed
            if (GetTickCount() - anyaStateTick > 500)
            {
                SimulateKeyPress(VK_DOWN);
                SimulateKeyPress(VK_RETURN);

                // Move to next state, waiting for shop
                anyaState = ANYA_SHOPPING;
                anyaStateTick = GetTickCount();
                anyaRetries = 0;
            }
        }
        break;

    case ANYA_SHOPPING:
        // Wait for shop UI
        if (GetTickCount() - anyaStateTick > 3000)
        {
            if (!GetUIVar(UI_NPCSHOP))
            {
                // Failed to open shop? try again from scratch
                anyaState = ANYA_MOVING_TO_VENDOR;
                anyaStateTick = GetTickCount();
                return;
            }
        }

        if (GetUIVar(UI_NPCSHOP))
        {
            // Shop is open. Check items.
            // Give it a moment to populate? Usually instant but let's be safe
            if (GetTickCount() - anyaStateTick < 500) return;

            LPUNITANY vendor = D2CLIENT_GetCurrentInteractingNPC();
            if (vendor && vendor->pInventory && strlen(ITEM_NAME_SEARCH) > 0)
            {
                for (LPUNITANY pItem = vendor->pInventory->pFirstItem; pItem; pItem = (pItem->pItemData ? pItem->pItemData->pNextInvItem : NULL))
                {
                    if (pItem && pItem->pItemData)
                    {
                        wchar_t wFullDesc[2048];
                        D2CLIENT_GetItemName(pItem, wFullDesc, sizeof(wFullDesc) / sizeof(wFullDesc[0]));
                        char szFullDesc[2048];
                        WideCharToMultiByte(CP_ACP, 0, wFullDesc, -1, szFullDesc, sizeof(szFullDesc), NULL, NULL);
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
            }

            // Done checking
            D2CLIENT_CloseInteract();
            V_AnyaBotCheckedItems = TRUE;
            anyaState = ANYA_IDLE; // Loop back to idle to decide next move (which will be portal)
            anyaStateTick = GetTickCount();
        }
        break;

    case ANYA_MOVING_TO_PORTAL:
        if (GetTickCount() - anyaStateTick < 500) return;

        // Find portal (Red Portal is ID 60 usually? The code used 60)
        for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
        {
            for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
            {
                if (Unit && Unit->dwTxtFileNo == 60)
                {
                    WORD xPos = D2CLIENT_GetUnitX(Unit);
                    WORD yPos = D2CLIENT_GetUnitY(Unit);
                    POINT screenPos = { xPos, yPos };
                    WorldToScreen(&screenPos);

                    SimulateLeftClick(screenPos);

                    anyaState = ANYA_ENTERING_PORTAL;
                    anyaStateTick = GetTickCount();
                    return;
                }
            }
        }
        break;

    case ANYA_ENTERING_PORTAL:
        // Wait for level change
        if (GetTickCount() - anyaStateTick > 5000)
        {
            // Stuck? Retry
            anyaState = ANYA_MOVING_TO_PORTAL;
            anyaStateTick = GetTickCount();
        }

        // If level changes, state machine will pick it up in ANYA_IDLE or we can check here
        if (Me->dwMode != PLAYER_MODE_STAND_INTOWN && Me->dwMode != PLAYER_MODE_WALK_INTOWN && Me->dwMode != PLAYER_MODE_RUN)
        {
            // Seems we are out?
            // Actually better to let IDLE handle the location check on next loop
            anyaState = ANYA_IDLE;
        }
        break;

    case ANYA_RETURNING:
        // We are outside. Need to click portal back.
        if (GetTickCount() - anyaStateTick < 500) return;

        V_AnyaBotCheckedItems = FALSE; // Reset for next shop run

        for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
        {
            for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
            {
                if (Unit && Unit->dwTxtFileNo == 60) // Portal ID
                {
                    WORD xPos = D2CLIENT_GetUnitX(Unit);
                    WORD yPos = D2CLIENT_GetUnitY(Unit);
                    POINT screenPos = { xPos, yPos };
                    WorldToScreen(&screenPos);

                    SimulateLeftClick(screenPos);

                    // Wait for town
                    anyaState = ANYA_IDLE; // Let IDLE detect when we are back in town
                    anyaStateTick = GetTickCount();
                    return;
                }
            }
        }
        break;
    }
}


VOID ResetAnyaBot()
{
    V_AnyaBotRunning = FALSE;
    V_AnyaBotCheckedItems = FALSE;
    anyaState = ANYA_IDLE;
    anyaStateTick = 0;
    anyaRetries = 0;
}

VOID ExitAnyaBot()
{
    ResetAnyaBot();
    PrintText(FONTCOLOR_RED, "Anya Bot stopped");
}