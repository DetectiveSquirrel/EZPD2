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

enum AnyaBotPhase
{
    ANYA_IDLE,
    ANYA_PREP_VENDOR_CLICK,
    ANYA_HOVER_VENDOR,
    ANYA_HOLD_VENDOR_CLICK,
    ANYA_WAIT_VENDOR_MENU,
    ANYA_SEND_TRADE_DOWN,
    ANYA_SEND_TRADE_ENTER,
    ANYA_WAIT_SHOP_UI,
    ANYA_CHECK_SHOP_ITEMS,
    ANYA_PREP_PURCHASE_ITEM,
    ANYA_CLICK_PURCHASE_TAB,
    ANYA_WAIT_PURCHASE_TAB,
    ANYA_HOVER_PURCHASE_ITEM,
    ANYA_CLICK_PURCHASE_ITEM,
    ANYA_VERIFY_PURCHASE_ITEM,
    ANYA_CLOSE_VENDOR,
    ANYA_PREP_PORTAL_OUT,
    ANYA_HOVER_PORTAL_OUT,
    ANYA_CLICK_PORTAL_OUT,
    ANYA_WAIT_OUTSIDE,
    ANYA_PREP_PORTAL_BACK,
    ANYA_HOVER_PORTAL_BACK,
    ANYA_CLICK_PORTAL_BACK,
    ANYA_WAIT_TOWN
};

struct AnyaPurchaseTarget
{
    DWORD unitId;
    int   tabIndex;
    POINT clickPos;
};

struct AnyaBotCtx
{
    AnyaBotPhase phase;
    DWORD        tick;
    int          retries;
    bool         checkedItems;
    POINT        clickPos;
    int          purchaseRetries;
    int          purchaseIndex;
    int          purchaseCount;
    DWORD        postDetectUntilTick;
    DWORD        portalBackReadyTick;
    DWORD        vendorReadyTick;
    DWORD        retryReadyTick;
    AnyaPurchaseTarget purchaseTargets[64];
};

static AnyaBotCtx s_anya = { ANYA_IDLE, 0, 0, false, {0, 0}, 0, 0, 0, 0, 0, 0, 0, {} };

static const DWORD ANYA_INTERACT_DELAY_MS      = 250;
static const DWORD ANYA_SHOP_TO_TAB_DELAY_MS   = 1000;
static const DWORD ANYA_VENDOR_TAB_POST_MS     = 400;
static const DWORD ANYA_STAND_READY_POLL_MS      = 50;
static const DWORD ANYA_STAND_READY_TIMEOUT_MS = 8000;
static const DWORD ANYA_PORTAL_BACK_HOVER_MS     = 500;
static const DWORD ANYA_VENDOR_HOVER_MS          = 500;
static const DWORD ANYA_RETRY_DELAY_MS         = 750;
static const DWORD ANYA_VENDOR_HOLD_MS         = 110;
static const DWORD ANYA_VENDOR_MENU_TIMEOUT_MS = 2000;
static const DWORD ANYA_SHOP_TIMEOUT_MS        = 2000;
static const DWORD ANYA_TRANSITION_TIMEOUT_MS  = 3000;
static const int   ANYA_MAX_RETRIES            = 4;

static LPUNITANY FindAnyaVendorUnit()
{
    if (!Me || !Me->pAct)
        return NULL;

    for (LPROOM1 room = Me->pAct->pRoom1; room; room = room->pRoomNext)
    {
        for (LPUNITANY unit = room->pUnitFirst; unit; unit = unit->pListNext)
        {
            if (!unit || unit->dwType != UNIT_TYPE_NPC || !unit->pPath)
                continue;

            if (unit->dwTxtFileNo == NPCID_Anya || unit->dwTxtFileNo == NPCID_Akara || unit->dwTxtFileNo == NPCID_Charsi)
                return unit;
        }
    }

    return NULL;
}

static LPUNITANY FindPortalUnit()
{
    if (!Me || !Me->pAct)
        return NULL;

    for (LPROOM1 room = Me->pAct->pRoom1; room; room = room->pRoomNext)
    {
        for (LPUNITANY unit = room->pUnitFirst; unit; unit = unit->pListNext)
        {
            if (unit && (unit->dwTxtFileNo == OBJ_TXT_RED_PORTAL || unit->dwTxtFileNo == OBJ_TXT_RED_PORTAL_ACT1_VENDOR))
                return unit;
        }
    }

    return NULL;
}

static BOOL IsOutsideForAnyaRun(DWORD levelNo)
{
    // A5 vendor refresh -> Nihlathak's Temple. PD2 A1 personal red portal -> 157 (0x9D) or Moor arena 159 (0x9F).
    return levelNo == MAP_A5_NIHLATHAKS_TEMPLE || levelNo == MAP_PVP_MOOR_ARENA || levelNo == MAP_PD2_A1_RED_PORTAL_OUT_157;
}

// Poll while outside / in town until the player is fully idle (stand) so portal/vendor clicks are reliable.
static BOOL AnyaPlayerStandingOutsideForPortalBack()
{
    return Me &&
           IsOutsideForAnyaRun(GetPlayerArea()) &&
           Me->dwMode == PLAYER_MODE_STAND_OUTTOWN;
}

static BOOL AnyaPlayerStandingInTownForVendor()
{
    return Me &&
           IsTownLevel((INT)GetPlayerArea()) &&
           Me->dwMode == PLAYER_MODE_STAND_INTOWN;
}

// Vendor shop tabs (left to right): armor, weapon 1, weapon 2, misc.
// These match itemtypes.txt `nStorePage` / container-style values used by BH.
static int VendorTabIndexFromStorePage(BYTE storePage)
{
    switch (storePage)
    {
    case 0x82: // CONTAINER_ARMOR_TAB
        return 0;
    case 0x84: // CONTAINER_WEAPON_TAB_1
        return 1;
    case 0x86: // CONTAINER_WEAPON_TAB_2
        return 2;
    case 0x88: // CONTAINER_MISC_TAB
        return 3;
    default:
        break;
    }

    // Common PD2-style encoding appears to be direct 0..3 tab index.
    if (storePage <= 3)
        return (int)storePage;

    // Older encodings sometimes use 1..4.
    if (storePage >= 1 && storePage <= 4)
        return (int)storePage - 1;

    return 0;
}

// `D2COMMON_GetItemText` returns the real `items.txt` row (BH: `ItemsTxt`), which does not match our trimmed `ItemTxt` struct.
// Read `nType` at the known stable offset used by BH (`ItemsTxt::nType` @ 0x11E).
static WORD GetItemsTxtTypeIdFromGetItemText(LPITEMTXT pItemTxt)
{
    if (!pItemTxt)
        return 0xFFFF;
    return *(const WORD*)((const BYTE*)pItemTxt + 0x11E);
}

static int GetVendorTabIndexForItemTxtNo(DWORD dwTxtFileNo, WORD* outItemType, BYTE* outStorePage)
{
    if (outItemType)
        *outItemType = 0xFFFF;
    if (outStorePage)
        *outStorePage = 0xFF;

    LPITEMTXT pItemTxt = D2COMMON_GetItemText(dwTxtFileNo);
    if (!pItemTxt || !p_D2COMMON_sgptDataTable || !*p_D2COMMON_sgptDataTable)
        return 0;

    const WORD itemType = GetItemsTxtTypeIdFromGetItemText(pItemTxt);
    if (outItemType)
        *outItemType = itemType;

    sgptDataTable* pData = *p_D2COMMON_sgptDataTable;
    if (!pData->pItemsTypeTxt || itemType >= (WORD)pData->dwItemsTypeRecs)
        return 0;

    const BYTE storePage = pData->pItemsTypeTxt[itemType].nStorePage;
    if (outStorePage)
        *outStorePage = storePage;

    return VendorTabIndexFromStorePage(storePage);
}

static BOOL VendorHasItemUnitId(DWORD unitId)
{
    LPUNITANY vendor = D2CLIENT_GetCurrentInteractingNPC();
    if (!vendor || !vendor->pInventory)
        return FALSE;

    for (LPUNITANY pItem = vendor->pInventory->pFirstItem; pItem; pItem = (pItem->pItemData ? pItem->pItemData->pNextInvItem : NULL))
    {
        if (pItem && pItem->dwUnitId == unitId)
            return TRUE;
    }

    return FALSE;
}

static VOID FailAnyaStep(const char* reason)
{
    if (s_anya.retries < ANYA_MAX_RETRIES)
    {
        s_anya.retries++;
        PrintText(FONTCOLOR_RED, "Anya Bot retry %d/%d: %s", s_anya.retries, ANYA_MAX_RETRIES, reason);
        s_anya.phase = ANYA_IDLE;
        s_anya.retryReadyTick = GetTickCount() + ANYA_RETRY_DELAY_MS;
        s_anya.tick  = GetTickCount();
        return;
    }

    PrintText(FONTCOLOR_RED, "Anya Bot failed: %s", reason);
    ExitAnyaBot();
}

VOID AnyaBot()
{
    if (!V_AnyaBotRunning)
        return;

    if (!Me)
        return;

    Level* playerLevel = GetUnitLevel(Me);
    if (!playerLevel)
        return;

    DWORD now = GetTickCount();
    if ((LONG)(now - s_anya.retryReadyTick) < 0)
        return;

    switch (s_anya.phase)
    {
    case ANYA_IDLE:
        if (playerLevel->dwLevelNo == MAP_A5_HARROGATH || playerLevel->dwLevelNo == MAP_A1_ROGUE_ENCAMPMENT)
        {
            if (!s_anya.checkedItems)
            {
                s_anya.phase = ANYA_PREP_VENDOR_CLICK;
                s_anya.tick  = now;
                s_anya.vendorReadyTick = now;
            }
            else
            {
                s_anya.phase = ANYA_PREP_PORTAL_OUT;
                s_anya.tick  = now;
            }
        }
        else if (IsOutsideForAnyaRun(playerLevel->dwLevelNo))
        {
            s_anya.phase = ANYA_PREP_PORTAL_BACK;
            s_anya.tick  = now;
            s_anya.portalBackReadyTick = now;
        }
        break;

    case ANYA_PREP_VENDOR_CLICK:
        if (now - s_anya.tick > ANYA_STAND_READY_TIMEOUT_MS)
        {
            FailAnyaStep("timeout waiting to stand in town for vendor");
            return;
        }
        if ((LONG)(now - s_anya.vendorReadyTick) < 0)
            return;
        if (!AnyaPlayerStandingInTownForVendor())
        {
            s_anya.vendorReadyTick = now + ANYA_STAND_READY_POLL_MS;
            return;
        }
        if (GetUIVar(UI_NPCMENU))
        {
            s_anya.phase = ANYA_SEND_TRADE_DOWN;
            s_anya.tick  = now;
            s_anya.retries = 0;
            return;
        }
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;

        {
            LPUNITANY vendor = FindAnyaVendorUnit();
            if (!vendor)
            {
                FailAnyaStep("vendor not found");
                return;
            }

            POINT screenPos = { vendor->pPath->xPos, vendor->pPath->yPos };
            WorldToScreen(&screenPos);
            s_anya.clickPos = screenPos;
            SetCursorPos(screenPos.x, screenPos.y);
            s_anya.phase = ANYA_HOVER_VENDOR;
            s_anya.tick  = now;
        }
        break;

    case ANYA_HOVER_VENDOR:
        if (now - s_anya.tick < ANYA_VENDOR_HOVER_MS)
            return;
        SimulateLeftDown(s_anya.clickPos);
        s_anya.phase = ANYA_HOLD_VENDOR_CLICK;
        s_anya.tick  = now;
        return;

    case ANYA_HOLD_VENDOR_CLICK:
        if (now - s_anya.tick < ANYA_VENDOR_HOLD_MS)
            return;
        SimulateLeftUp(s_anya.clickPos);
        s_anya.phase = ANYA_WAIT_VENDOR_MENU;
        s_anya.tick  = now;
        return;

    case ANYA_WAIT_VENDOR_MENU:
        if (GetUIVar(UI_NPCMENU))
        {
            s_anya.phase = ANYA_SEND_TRADE_DOWN;
            s_anya.tick  = now;
            return;
        }

        if (now - s_anya.tick > ANYA_VENDOR_MENU_TIMEOUT_MS)
        {
            FailAnyaStep("vendor menu did not open");
            return;
        }
        return;

    case ANYA_SEND_TRADE_DOWN:
        if (GetUIVar(UI_NPCSHOP))
        {
            s_anya.phase = ANYA_CHECK_SHOP_ITEMS;
            s_anya.tick = now;
            return;
        }
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        SimulateKeyPress(VK_DOWN);
        s_anya.phase = ANYA_SEND_TRADE_ENTER;
        s_anya.tick  = now;
        return;

    case ANYA_SEND_TRADE_ENTER:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        SimulateKeyPress(VK_RETURN);
        s_anya.phase = ANYA_WAIT_SHOP_UI;
        s_anya.tick  = now;
        return;

    case ANYA_WAIT_SHOP_UI:
        if (GetUIVar(UI_NPCSHOP))
        {
            s_anya.phase = ANYA_CHECK_SHOP_ITEMS;
            s_anya.tick  = now;
            return;
        }

        if (now - s_anya.tick > ANYA_SHOP_TIMEOUT_MS)
        {
            FailAnyaStep("shop did not open");
            return;
        }
        return;

    case ANYA_CHECK_SHOP_ITEMS:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        {
            LPUNITANY vendor = D2CLIENT_GetCurrentInteractingNPC();
            if (vendor && vendor->pInventory && strlen(ITEM_NAME_SEARCH) > 0)
            {
                s_anya.purchaseCount = 0;
                s_anya.purchaseIndex = 0;
                s_anya.purchaseRetries = 0;

                for (LPUNITANY pItem = vendor->pInventory->pFirstItem; pItem; pItem = (pItem->pItemData ? pItem->pItemData->pNextInvItem : NULL))
                {
                    if (pItem && pItem->pItemData && pItem->pItemPath)
                    {
                        wchar_t wFullDesc[2048];
                        D2CLIENT_GetItemName(pItem, wFullDesc, sizeof(wFullDesc) / sizeof(wFullDesc[0]));
                        char szFullDesc[2048];
                        WideCharToMultiByte(CP_ACP, 0, wFullDesc, -1, szFullDesc, sizeof(szFullDesc), NULL, NULL);
                        char cleanedDesc[2048];
                        CreateCleanItemName(szFullDesc, cleanedDesc, sizeof(cleanedDesc));

                        if (strstr(cleanedDesc, ITEM_NAME_SEARCH) != NULL)
                        {
                            if (s_anya.purchaseCount >= (int)ArraySize(s_anya.purchaseTargets))
                                continue;

                            int tabIndex = GetVendorTabIndexForItemTxtNo(pItem->dwTxtFileNo, NULL, NULL);

                            AnyaPurchaseTarget& target = s_anya.purchaseTargets[s_anya.purchaseCount++];
                            target.unitId = pItem->dwUnitId;
                            target.tabIndex = tabIndex;
                            target.clickPos = GetVendorSlotPixelCoordinates((INT)pItem->pItemPath->dwPosX, (INT)pItem->pItemPath->dwPosY);
                        }
                    }
                }
            }
        }

        if (!V_AnyaAutoPurchaseEnabled)
        {
            if (s_anya.purchaseCount > 0)
            {
                PrintText(FONTCOLOR_YELLOW, "Found %d matching shop item(s). Auto purchase disabled — bot stopped (shop left open).", s_anya.purchaseCount);
                ResetAnyaBot();
                return;
            }
            s_anya.postDetectUntilTick = 0;
            s_anya.phase = ANYA_CLOSE_VENDOR;
            s_anya.tick  = now;
            return;
        }

        if (s_anya.purchaseCount > 0)
        {
            PrintText(FONTCOLOR_YELLOW, "Found %d matching shop item(s).", s_anya.purchaseCount);
            // Let vendor navigation tabs fully render after shop opens.
            s_anya.postDetectUntilTick = now + ANYA_SHOP_TO_TAB_DELAY_MS;
            s_anya.phase = ANYA_PREP_PURCHASE_ITEM;
            s_anya.tick  = now;
            return;
        }

        s_anya.postDetectUntilTick = 0;
        s_anya.phase = ANYA_CLOSE_VENDOR;
        s_anya.tick  = now;
        return;

    case ANYA_PREP_PURCHASE_ITEM:
        if (s_anya.purchaseIndex >= s_anya.purchaseCount)
        {
            s_anya.postDetectUntilTick = 0;
            s_anya.phase = ANYA_CLOSE_VENDOR;
            s_anya.tick = now;
            return;
        }
        // After detecting a shop match, wait for the configured post-shop tab delay.
        // Purchase retries skip this so we can quickly re-click the tab.
        if (s_anya.purchaseRetries == 0 && (LONG)(now - s_anya.postDetectUntilTick) < 0)
            return;
        s_anya.clickPos = GetVendorTabPixelCoordinates(s_anya.purchaseTargets[s_anya.purchaseIndex].tabIndex);
        SetCursorPos(s_anya.clickPos.x, s_anya.clickPos.y);
        s_anya.phase = ANYA_CLICK_PURCHASE_TAB;
        // Allow tab click immediately after the post-detect delay (avoid stacking another ANYA_INTERACT_DELAY_MS wait).
        s_anya.tick = now - ANYA_INTERACT_DELAY_MS;
        return;

    case ANYA_CLICK_PURCHASE_TAB:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        SimulateLeftClick(s_anya.clickPos);
        s_anya.phase = ANYA_WAIT_PURCHASE_TAB;
        s_anya.tick = now;
        return;

    case ANYA_WAIT_PURCHASE_TAB:
        if (now - s_anya.tick < ANYA_VENDOR_TAB_POST_MS)
            return;
        s_anya.phase = ANYA_HOVER_PURCHASE_ITEM;
        s_anya.tick = now;
        return;

    case ANYA_HOVER_PURCHASE_ITEM:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        s_anya.clickPos = s_anya.purchaseTargets[s_anya.purchaseIndex].clickPos;
        SetCursorPos(s_anya.clickPos.x, s_anya.clickPos.y);
        s_anya.phase = ANYA_CLICK_PURCHASE_ITEM;
        s_anya.tick = now;
        return;

    case ANYA_CLICK_PURCHASE_ITEM:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        SimulateRightClick(s_anya.clickPos);
        s_anya.phase = ANYA_VERIFY_PURCHASE_ITEM;
        s_anya.tick = now;
        return;

    case ANYA_VERIFY_PURCHASE_ITEM:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        {
            BOOL itemGoneFromVendor = !VendorHasItemUnitId(s_anya.purchaseTargets[s_anya.purchaseIndex].unitId);
            if (itemGoneFromVendor)
            {
                s_anya.purchaseIndex++;
                s_anya.purchaseRetries = 0;
                s_anya.postDetectUntilTick = now + ANYA_INTERACT_DELAY_MS;
                s_anya.phase = ANYA_PREP_PURCHASE_ITEM;
                s_anya.tick = now;
                return;
            }

            s_anya.purchaseRetries++;
            if (s_anya.purchaseRetries >= 4)
            {
                PrintText(FONTCOLOR_RED, "Failed to purchase item after 4 retries.");
                ExitAnyaBot();
                return;
            }

            // Retry: always re-select vendor tab (move + click + tab wait) before next right-click.
            s_anya.phase = ANYA_PREP_PURCHASE_ITEM;
            s_anya.tick = now;
        }
        return;

    case ANYA_CLOSE_VENDOR:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        D2CLIENT_CloseInteract();
        s_anya.checkedItems = true;
        s_anya.retries      = 0;
        s_anya.phase = ANYA_PREP_PORTAL_OUT;
        s_anya.tick  = now;
        return;

    case ANYA_PREP_PORTAL_OUT:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        {
            LPUNITANY portal = FindPortalUnit();
            if (!portal)
            {
                FailAnyaStep("portal to outside not found");
                return;
            }

            POINT screenPos = { D2CLIENT_GetUnitX(portal), D2CLIENT_GetUnitY(portal) };
            WorldToScreen(&screenPos);
            s_anya.clickPos = screenPos;
            SetCursorPos(screenPos.x, screenPos.y);
            s_anya.phase = ANYA_HOVER_PORTAL_OUT;
            s_anya.tick  = now;
        }
        break;

    case ANYA_HOVER_PORTAL_OUT:
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        s_anya.phase = ANYA_CLICK_PORTAL_OUT;
        s_anya.tick  = now;
        return;

    case ANYA_CLICK_PORTAL_OUT:
        SimulateLeftClick(s_anya.clickPos);
        s_anya.phase = ANYA_WAIT_OUTSIDE;
        s_anya.tick  = now;
        return;

    case ANYA_WAIT_OUTSIDE:
        if (IsOutsideForAnyaRun(GetPlayerArea()))
        {
            s_anya.retries = 0;
            s_anya.phase = ANYA_PREP_PORTAL_BACK;
            s_anya.portalBackReadyTick = now;
            s_anya.tick  = now;
            return;
        }

        if (now - s_anya.tick > ANYA_TRANSITION_TIMEOUT_MS)
        {
            // Portal retry cooldown.
            if (s_anya.retries < ANYA_MAX_RETRIES)
            {
                s_anya.retries++;
                PrintText(FONTCOLOR_RED, "Anya Bot retry %d/%d: %s", s_anya.retries, ANYA_MAX_RETRIES, "failed entering outside portal");
                s_anya.phase = ANYA_PREP_PORTAL_OUT;
                s_anya.retryReadyTick = now + ANYA_RETRY_DELAY_MS;
                s_anya.tick  = now;
                return;
            }
            FailAnyaStep("failed entering outside portal");
            return;
        }
        return;

    case ANYA_PREP_PORTAL_BACK:
        if (now - s_anya.tick > ANYA_STAND_READY_TIMEOUT_MS)
        {
            FailAnyaStep("timeout waiting to stand outside for return portal");
            return;
        }
        if ((LONG)(now - s_anya.portalBackReadyTick) < 0)
            return;
        if (!AnyaPlayerStandingOutsideForPortalBack())
        {
            s_anya.portalBackReadyTick = now + ANYA_STAND_READY_POLL_MS;
            return;
        }
        if (now - s_anya.tick < ANYA_INTERACT_DELAY_MS)
            return;
        {
            LPUNITANY portal = FindPortalUnit();
            if (!portal)
            {
                FailAnyaStep("return portal not found");
                return;
            }

            POINT screenPos = { D2CLIENT_GetUnitX(portal), D2CLIENT_GetUnitY(portal) };
            WorldToScreen(&screenPos);
            s_anya.clickPos = screenPos;
            SetCursorPos(screenPos.x, screenPos.y);
            s_anya.phase = ANYA_HOVER_PORTAL_BACK;
            s_anya.tick  = now;
        }
        break;

    case ANYA_HOVER_PORTAL_BACK:
        if (now - s_anya.tick < ANYA_PORTAL_BACK_HOVER_MS)
            return;
        s_anya.phase = ANYA_CLICK_PORTAL_BACK;
        s_anya.tick  = now;
        return;

    case ANYA_CLICK_PORTAL_BACK:
        SimulateLeftClick(s_anya.clickPos);
        s_anya.phase = ANYA_WAIT_TOWN;
        s_anya.tick  = now;
        return;

    case ANYA_WAIT_TOWN:
        if (IsTownLevel(GetPlayerArea()))
        {
            s_anya.checkedItems = false;
            s_anya.retries = 0;
            s_anya.phase = ANYA_PREP_VENDOR_CLICK;
            s_anya.tick  = now;
            s_anya.vendorReadyTick = now;
            return;
        }

        if (now - s_anya.tick > ANYA_TRANSITION_TIMEOUT_MS)
        {
            // Portal retry cooldown.
            if (s_anya.retries < ANYA_MAX_RETRIES)
            {
                s_anya.retries++;
                PrintText(FONTCOLOR_RED, "Anya Bot retry %d/%d: %s", s_anya.retries, ANYA_MAX_RETRIES, "failed returning to town");
                s_anya.phase = ANYA_PREP_PORTAL_BACK;
                s_anya.portalBackReadyTick = now;
                s_anya.retryReadyTick = now + ANYA_RETRY_DELAY_MS;
                s_anya.tick  = now;
                return;
            }
            FailAnyaStep("failed returning to town");
            return;
        }
        return;
    }
}


VOID ResetAnyaBot()
{
    V_AnyaBotRunning    = FALSE;
    s_anya.phase        = ANYA_IDLE;
    s_anya.tick         = 0;
    s_anya.retries      = 0;
    s_anya.checkedItems = false;
    s_anya.purchaseRetries = 0;
    s_anya.purchaseIndex = 0;
    s_anya.purchaseCount = 0;
    s_anya.postDetectUntilTick = 0;
    s_anya.portalBackReadyTick = 0;
    s_anya.vendorReadyTick = 0;
    s_anya.retryReadyTick = 0;
}

VOID ExitAnyaBot()
{
    ResetAnyaBot();
    PrintText(FONTCOLOR_RED, "Anya Bot stopped");
}








