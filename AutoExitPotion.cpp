#include "Hack.h"

VOID AutoPotExitMain()
{
  if (!Me || !Me->pStats)
    return;

  if (IsPlayerInTown())
    return;

  if (Me->dwMode == PLAYER_MODE_DEAD)
    return;

  DWORD currentLifePct = GetPlayerVitalsPercent(TRUE);
  DWORD currentManaPct = GetPlayerVitalsPercent(FALSE);

  if (currentLifePct == -1 || currentManaPct == -1)
    return;

  LPUNITANY pMerc = FindMercUnit(Me);

  if (V_AutoExitEnabled)
  {
    if (currentLifePct <= V_AutoExitLifeThreshold)
    {
      D2CLIENT_ExitGame();
      return;
    }
  }

  if (V_AutoPotEnabled && GetUIVar(UI_CHAT) == 0)
  {
    if (V_AutoPotRejuvEnabled)
    {
      if (currentLifePct <= V_AutoPotRejuvThreshold)
      {
        int hotkeyToUse = GetHotkeySlotForPotionType(POTION_TYPE_REJUV);
        if (hotkeyToUse != 0)
        {
          UsePotion(hotkeyToUse, FALSE);
          return;
        }
      }
    }

    if (V_AutoPotManaEnabled)
    {
      if (currentManaPct <= V_AutoPotManaThreshold)
      {
        if (!GetUnitState(Me, AFFECT_MANAPOT))
        {
          int hotkeyToUse = GetHotkeySlotForPotionType(POTION_TYPE_MANA);
          if (hotkeyToUse != 0)
          {
            UsePotion(hotkeyToUse, FALSE);
            return;
          }
        }
      }
    }

    if (V_AutoPotLifeEnabled)
    {
      if (currentLifePct <= V_AutoPotLifeThreshold)
      {
        if (!GetUnitState(Me, AFFECT_HEALTHPOT))
        {
          int hotkeyToUse = GetHotkeySlotForPotionType(POTION_TYPE_LIFE);
          if (hotkeyToUse != 0)
          {
            UsePotion(hotkeyToUse, FALSE);
            return;
          }
        }
      }
    }

    if (pMerc && (V_TickCount % 80 == 0))
    {
      DWORD mercLifePct = GetUnitLifePercent(pMerc);
      if (mercLifePct == -1 || mercLifePct == 0)
        return;

      if (V_MercAutoPotRejuvEnabled && mercLifePct <= V_MercAutoPotRejuvThreshold)
      {
        int hotkeyToUse = GetHotkeySlotForPotionType(POTION_TYPE_REJUV);
        if (hotkeyToUse != 0)
        {
          UsePotion(hotkeyToUse, TRUE);
          return;
        }
      }

      if (V_MercAutoPotLifeEnabled && mercLifePct <= V_MercAutoPotLifeThreshold && !GetUnitState(pMerc, AFFECT_HEALTHPOT))
      {
        int hotkeyToUse = GetHotkeySlotForPotionType(POTION_TYPE_LIFE);
        if (hotkeyToUse != 0)
        {
          UsePotion(hotkeyToUse, TRUE);
          return;
        }
      }
    }
  }
}

VOID RefillPotions()
{
  if (!V_AutoRefillEnabled)
    return;

  if (GetUIVar(UI_CHAT) == 1)
    return;

  if (V_IsRefillingPotions)
  {

    // If key is released -> exit and cleanup
    if (!KeyDown(V_RefillPotionsKey))
    {
      ExitRefillPotions();
      return;
    }

    // Set Empty belt slot Vars
    SetEmptyBeltSlotVars();
    if (!V_IsColumn1Empty && !V_IsColumn2Empty && !V_IsColumn3Empty &&
        !V_IsColumn4Empty)
    {
      return;
    }

    if (V_IsColumn1Empty)
    {
      V_ItemSlotLocationToFill =
          GetFirstPotionPosInInventory(V_RefillSlot1PotionType);
    }
    if (V_IsColumn2Empty && V_ItemSlotLocationToFill.x == -1 &&
        V_ItemSlotLocationToFill.y == -1)
    {
      V_ItemSlotLocationToFill =
          GetFirstPotionPosInInventory(V_RefillSlot2PotionType);
    }
    if (V_IsColumn3Empty && V_ItemSlotLocationToFill.x == -1 &&
        V_ItemSlotLocationToFill.y == -1)
    {
      V_ItemSlotLocationToFill =
          GetFirstPotionPosInInventory(V_RefillSlot3PotionType);
    }
    if (V_IsColumn4Empty && V_ItemSlotLocationToFill.x == -1 &&
        V_ItemSlotLocationToFill.y == -1)
    {
      V_ItemSlotLocationToFill =
          GetFirstPotionPosInInventory(V_RefillSlot4PotionType);
    }

    if (V_ItemSlotLocationToFill.x == -1 && V_ItemSlotLocationToFill.y == -1)
    {
      return;
    }

    // Open Inventory if not open
    if (GetUIVar(UI_INVENTORY) == 0)
    {
      SimulateKeyPress('I');
      return;
    }

    if (V_TickCount % 40 == 0)
    {
      POINT screenPos = GetInventorySlotPixelCoordinates(
          V_ItemSlotLocationToFill.x, V_ItemSlotLocationToFill.y);

      if (!KeyDown(VK_SHIFT))
      {
        SimulateKeyDown(VK_SHIFT);
      }
      SimulateLeftClick(screenPos);
      if (KeyDown(VK_SHIFT))
      {
        SimulateKeyUp(VK_SHIFT);
      }
    }

    // Reset Item Slot Location To Fill at the end so the checks for empty
    // slots does not get stuck when one slot is filled
    V_ItemSlotLocationToFill.x = -1;
    V_ItemSlotLocationToFill.y = -1;
  }
}

VOID ExitRefillPotions()
{
  V_ItemSlotLocationToFill.x = -1;
  V_ItemSlotLocationToFill.y = -1;
  V_IsColumn1Empty = FALSE;
  V_IsColumn2Empty = FALSE;
  V_IsColumn3Empty = FALSE;
  V_IsColumn4Empty = FALSE;

  if (GetUIVar(UI_INVENTORY) == 1)
  {
    SimulateKeyPress('I');
  }

  if (KeyDown(VK_SHIFT))
  {
    SimulateKeyUp(VK_SHIFT);
  }

  SetCursorPos(V_InitialCursorPos.x, V_InitialCursorPos.y);
  V_IsRefillingPotions = FALSE;
  // PrintText(1, "Exited Refill Potions");
}

VOID SetEmptyBeltSlotVars()
{
  INT potionSlots = GetAmountOfBeltSlots(GetCurrentlyEquippedBeltItemCode());
  DWORD rowsPerColumn = potionSlots / 4; // 2 for 8-slot belts, 3 for 12-slot belts, 4 for 16-slot belts

  DWORD column1Count = 0;
  DWORD column2Count = 0;
  DWORD column3Count = 0;
  DWORD column4Count = 0;

  for (LPUNITANY Item = Me->pInventory->pFirstItem; Item;
       Item = (Item->pItemData ? Item->pItemData->pNextInvItem : nullptr))
  {
    if (Item && Item->pItemData && GetItemLocation(Item) == STORAGE_BELT &&
        Item->pItemPath)
    {
      DWORD x = Item->pItemPath->dwPosX;

      // Column 1: x coordinates = 0,4,8,12
      if (x == 0 || x == 4 || x == 8 || x == 12)
        column1Count++;
      // Column 2: x coordinates = 1,5,9,13
      else if (x == 1 || x == 5 || x == 9 || x == 13)
        column2Count++;
      // Column 3: x coordinates = 2,6,10,14
      else if (x == 2 || x == 6 || x == 10 || x == 14)
        column3Count++;
      // Column 4: x coordinates = 3,7,11,15
      else if (x == 3 || x == 7 || x == 11 || x == 15)
        column4Count++;
    }
  }

  V_IsColumn1Empty = column1Count < rowsPerColumn;
  V_IsColumn2Empty = column2Count < rowsPerColumn;
  V_IsColumn3Empty = column3Count < rowsPerColumn;
  V_IsColumn4Empty = column4Count < rowsPerColumn;
}

INT GetHotkeySlotForPotionType(INT potionType)
{
  for (LPUNITANY Item = Me->pInventory->pFirstItem; Item;
       Item = (Item->pItemData ? Item->pItemData->pNextInvItem : nullptr))
  {
    if (Item && Item->pItemData && GetItemLocation(Item) == STORAGE_BELT &&
        Item->pItemPath)
    {
      char Code[4] = {0};
      GetItemCodeFromItem(Item, Code, 3);

      if (GetPotionTypeFromItemCode(Code) == potionType)
      {
        if (Item->pItemPath->dwPosX == 0)
          return 1;
        else if (Item->pItemPath->dwPosX == 1)
          return 2;
        else if (Item->pItemPath->dwPosX == 2)
          return 3;
        else if (Item->pItemPath->dwPosX == 3)
          return 4;
      }
    }
  }

  return 0;
}

INT GetPotionTypeFromItemCode(LPSTR lpszItemCode)
{
  if (lpszItemCode == NULL)
    return POTION_TYPE_NONE;

  if (!_stricmp(lpszItemCode, "rvl") || !_stricmp(lpszItemCode, "rvs"))
    return POTION_TYPE_REJUV;

  if (!_stricmp(lpszItemCode, "hp1") || !_stricmp(lpszItemCode, "hp2") ||
      !_stricmp(lpszItemCode, "hp3") || !_stricmp(lpszItemCode, "hp4") ||
      !_stricmp(lpszItemCode, "hp5"))
    return POTION_TYPE_LIFE;

  if (!_stricmp(lpszItemCode, "mp1") || !_stricmp(lpszItemCode, "mp2") ||
      !_stricmp(lpszItemCode, "mp3") || !_stricmp(lpszItemCode, "mp4") ||
      !_stricmp(lpszItemCode, "mp5"))
    return POTION_TYPE_MANA;

  return POTION_TYPE_NONE;
}

POINT GetFirstPotionPosInInventory(INT potionType)
{
  POINT point;

  for (LPUNITANY Item = Me->pInventory->pFirstItem; Item;
       Item = (Item->pItemData ? Item->pItemData->pNextInvItem : nullptr))
  {
    if (Item && Item->pItemData && GetItemLocation(Item) == STORAGE_INVENTORY &&
        Item->pItemPath)
    {
      char Code[4] = {0};
      GetItemCodeFromItem(Item, Code, 3);

      if (GetPotionTypeFromItemCode(Code) == potionType)
      {
        point.x = Item->pItemPath->dwPosX;
        point.y = Item->pItemPath->dwPosY;
        return point;
      }
    }
  }

  point.x = -1;
  point.y = -1;
  return point;
}

INT GetAmountOfBeltSlots(LPSTR itemCode)
{
  // empty string = 4 slots (no belt equipped)
  // lbl, vbl = 8 slots
  // mbl, tbl = 12 slots
  // any other code = 16 slots

  if (!itemCode || itemCode[0] == '\0')
    return 4;
  else if (!_stricmp(itemCode, "lbl") || !_stricmp(itemCode, "vbl"))
    return 8;
  else if (!_stricmp(itemCode, "mbl") || !_stricmp(itemCode, "tbl"))
    return 12;
  else
    return 16;
}

LPSTR GetCurrentlyEquippedBeltItemCode()
{
  static char beltCode[5] = "";

  if (!Me || !Me->pInventory)
    return beltCode;

  for (UnitAny *pItem = Me->pInventory->pFirstItem; pItem;
       pItem = pItem->pItemData->pNextInvItem)
  {
    if (!pItem->pItemData)
      continue;

    if (pItem->pItemData->BodyLocation == EQUIP_BELT)
    {
      GetItemCodeFromItem(pItem, beltCode, 4);
      return beltCode;
    }
  }

  memset(beltCode, 0, sizeof(beltCode));
  return beltCode;
}

VOID UsePotion(INT hotkeyToUse, BOOL isMercPotion)
{
  if (isMercPotion && !KeyDown(VK_SHIFT))
  {
    SimulateKeyDown(VK_SHIFT);
  }

  switch (hotkeyToUse)
  {
  case 1:
    SimulateKeyPress('1');
    break;
  case 2:
    SimulateKeyPress('2');
    break;
  case 3:
    SimulateKeyPress('3');
    break;
  case 4:
    SimulateKeyPress('4');
    break;
  }

  if (isMercPotion && KeyDown(VK_SHIFT))
  {
    SimulateKeyUp(VK_SHIFT);
  }
}