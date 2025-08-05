#include "Hack.h"

VOID DrawMouseCoordinates()
{
	if (V_MainMenuOpen)
		return;

	DrawTextB(10, 10, FONTCOLOR_WHITE, 6, -1, "Mouse X: %d", *p_D2CLIENT_MouseX);
	DrawTextB(10, 20, FONTCOLOR_WHITE, 6, -1, "Mouse Y: %d", *p_D2CLIENT_MouseY);

	DWORD playerX = Me->pPath->xPos;
	DWORD playerY = Me->pPath->yPos;

	DrawTextB(10, 30, FONTCOLOR_WHITE, 6, -1, "Player X: %d", playerX);
	DrawTextB(10, 40, FONTCOLOR_WHITE, 6, -1, "Player Y: %d", playerY);

	DrawTextB(10, 50, FONTCOLOR_WHITE, 6, -1, "Offset X: %d", Me->pPath->xOffset);
	DrawTextB(10, 60, FONTCOLOR_WHITE, 6, -1, "Offset Y: %d", Me->pPath->yOffset);

	DrawTextB(10, 70, FONTCOLOR_WHITE, 6, -1, "Screen X: %d", *p_D2CLIENT_ScreenSizeX);
	DrawTextB(10, 80, FONTCOLOR_WHITE, 6, -1, "Screen Y: %d", *p_D2CLIENT_ScreenSizeY);

	// Convert playerX and playerY to screen coordinates
	POINT screenPos = {playerX, playerY};
	WorldToScreen(&screenPos);
	DrawTextB(10, 90, FONTCOLOR_WHITE, 6, -1, "ScreenPos X: %d", screenPos.x);
	DrawTextB(10, 100, FONTCOLOR_WHITE, 6, -1, "ScreenPos Y: %d", screenPos.y);

	DWORD UI1 = GetUIVar(UI_NPCMENU);
	DrawTextB(10, 110, FONTCOLOR_WHITE, 6, -1, "UI1: %d", UI1);

	DWORD UI2 = GetUIVar(UI_NPCSHOP);
	DrawTextB(10, 120, FONTCOLOR_WHITE, 6, -1, "UI2: %d", UI2);

	DWORD playerMode = Me->dwMode;
	DrawTextB(10, 130, FONTCOLOR_WHITE, 6, -1, "Player Mode: %d", playerMode);

	Level *playerLevel = GetUnitLevel(Me);
	DrawTextB(10, 140, FONTCOLOR_WHITE, 6, -1, "Player Level: %d", playerLevel->dwLevelNo);
}

VOID DrawClosestMonsterStats()
{
	if (V_MainMenuOpen)
		return;

	LPUNITANY closestMonster = GetClosestUnit(UNIT_TYPE_NPC);
	if (closestMonster)
	{
		int yPos = 30;

		// Basic unit info
		DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Unit ID: %d", closestMonster->dwUnitId);
		yPos += 10;

		DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "TxtFileNo: %d", closestMonster->dwTxtFileNo);
		yPos += 10;

		DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Mode: %d", closestMonster->dwMode);
		yPos += 10;

		// Get unit name
		WCHAR *wName = (WCHAR *)GetUnitName(closestMonster);
		CHAR szName[128] = "";
		WideCharToMultiByte(CP_ACP, 0, wName, -1, szName, sizeof(szName), 0, 0);
		DrawTextB(10, yPos, FONTCOLOR_GOLD, 6, -1, "Name: %s", szName);
		yPos += 10;

		// Monster data flags
		if (closestMonster->pMonsterData)
		{
			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Monster Flags:");
			yPos += 10;

			DrawTextB(20, yPos, FONTCOLOR_WHITE, 6, -1, "Unknown: %d", closestMonster->pMonsterData->fUnk);
			yPos += 10;

			DrawTextB(20, yPos, FONTCOLOR_WHITE, 6, -1, "Normal: %d", closestMonster->pMonsterData->fNormal);
			yPos += 10;

			DrawTextB(20, yPos, FONTCOLOR_WHITE, 6, -1, "Champion: %d", closestMonster->pMonsterData->fChamp);
			yPos += 10;

			DrawTextB(20, yPos, FONTCOLOR_WHITE, 6, -1, "Boss: %d", closestMonster->pMonsterData->fBoss);
			yPos += 10;

			DrawTextB(20, yPos, FONTCOLOR_WHITE, 6, -1, "Minion: %d", closestMonster->pMonsterData->fMinion);
			yPos += 10;

			// Display unique number if applicable
			if (closestMonster->pMonsterData->wUniqueNo != 0)
			{
				DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Unique No: %d", closestMonster->pMonsterData->wUniqueNo);
				yPos += 10;
			}
		}

		// Position info
		if (closestMonster->pPath)
		{
			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Position: %d, %d", closestMonster->pPath->xPos, closestMonster->pPath->yPos);
			yPos += 10;
		}

		// Stats if available
		if (closestMonster->pStats)
		{
			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Stats Count: %d", closestMonster->pStats->wStatCount1);
			yPos += 10;

			// Display stats
			DrawTextB(10, yPos, FONTCOLOR_GOLD, 6, -1, "Stats List:");
			yPos += 10;

			DWORD Life = GetUnitStat(closestMonster, STAT_HP) >> 8;
			DWORD MaxLife = GetUnitStat(closestMonster, STAT_MAXHP) >> 8;
			DWORD LifePercent = GetUnitLifePercent(closestMonster);
			DWORD PhysicalResist = GetUnitStat(closestMonster, STAT_DMGREDUCTIONPCT);
			DWORD MagicResist = GetUnitStat(closestMonster, STAT_MAGICDMGREDUCTIONPCT);
			DWORD FireResist = GetUnitStat(closestMonster, STAT_FIRERESIST);
			DWORD LightningResist = GetUnitStat(closestMonster, STAT_LIGHTNINGRESIST);
			DWORD ColdResist = GetUnitStat(closestMonster, STAT_COLDRESIST);
			DWORD PoisonResist = GetUnitStat(closestMonster, STAT_POISONRESIST);

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Life: %d", Life);
			yPos += 10;

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Max Life: %d", MaxLife);
			yPos += 10;

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Life Percent: %d%%", LifePercent);
			yPos += 10;

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Physical Resist: %d", PhysicalResist);
			yPos += 10;

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Magic Resist: %d", MagicResist);
			yPos += 10;

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Fire Resist: %d", FireResist);
			yPos += 10;

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Lightning Resist: %d", LightningResist);
			yPos += 10;

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Cold Resist: %d", ColdResist);
			yPos += 10;

			DrawTextB(10, yPos, FONTCOLOR_WHITE, 6, -1, "Poison Resist: %d", PoisonResist);
			yPos += 10;
		}
	}
}

VOID DrawPlayerInventoryItems()
{

	if (V_MainMenuOpen)
		return;

	if (!Me || !Me->pInventory || !Me->pInventory->pFirstItem)
		return;

	// Base position for drawing
	INT baseX = 100;
	INT baseY = 100;
	INT itemSpacing = 150; // Space between items
	INT lineHeight = 15;   // Height between lines of text

	INT itemCount = 0;
	LPUNITANY currentItem = Me->pInventory->pFirstItem;

	while (currentItem && itemCount < 4)
	{
		INT currentX = baseX + (itemCount * itemSpacing);
		INT currentY = baseY;

		// Get item name
		WCHAR wItemName[128] = {0};
		CHAR szItemName[128] = "Unknown Item";

		D2CLIENT_GetItemNameString(currentItem, wItemName, 128);
		if (wItemName[0] != L'\0')
		{
			WideCharToMultiByte(CP_ACP, 0, wItemName, -1, szItemName, sizeof(szItemName), 0, 0);
		}
		else
		{
			// Try to get item code if name not available
			if (currentItem->pItemData)
			{
				char itemCode[5] = {0};
				GetItemCodeFromItem(currentItem, itemCode, 4);
				sprintf_s(szItemName, "Item Code: %s", itemCode);
			}
		}

		// Draw item name
		DrawTextB(currentX, currentY, FONTCOLOR_GOLD, 6, -1, "%s", szItemName);
		currentY += lineHeight;

		// Draw item level
		if (currentItem->pItemData)
		{
			DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "iLvl: %d", currentItem->pItemData->dwItemLevel);
			currentY += lineHeight;
		}

		// Draw item quality
		if (currentItem->pItemData)
		{
			const char *qualityNames[] = {
				"Low Quality", "Normal", "Superior", "Magic",
				"Set", "Rare", "Unique", "Crafted"};
			DWORD quality = currentItem->pItemData->dwQuality;
			if (quality < 8)
			{
				DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Quality: %s", qualityNames[quality]);
			}
			else
			{
				DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Quality: %d", quality);
			}
			currentY += lineHeight;
		}

		// Draw item flags
		if (currentItem->pItemData)
		{
			if (currentItem->pItemData->dwFlags & 0x00000010)
				DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Ethereal");
			else
				DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Not Ethereal");
			currentY += lineHeight;

			if (currentItem->pItemData->dwFlags & 0x00000400)
				DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Identified");
			else
				DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Unidentified");
			currentY += lineHeight;
		}

		// Draw item position
		if (currentItem->pItemPath)
		{
			DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Pos: %d,%d",
					  currentItem->pItemPath->dwPosX,
					  currentItem->pItemPath->dwPosY);
			currentY += lineHeight;
		}

		// Draw item location
		if (currentItem->pItemData)
		{
			const char *locationNames[] = {
				"Inventory", "Equipped", "Belt", "Ground",
				"Cursor", "Stash", "Cube"};
			BYTE location = currentItem->pItemData->ItemLocation;
			if (location < 7)
			{
				DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Location: %s", locationNames[location]);
			}
			else
			{
				DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "Location: %d", location);
			}
			currentY += lineHeight;

			WORD wordXPos = currentItem->pPath->xPos;
			WORD wordYPos = currentItem->pPath->yPos;

			DrawTextB(currentX, currentY, FONTCOLOR_WHITE, 6, -1, "X: %d, Y: %d", wordXPos, wordYPos);
			currentY += lineHeight;
		}

		// Move to next item
		if (currentItem->pItemData)
			currentItem = currentItem->pItemData->pNextInvItem;
		else
			currentItem = NULL;

		itemCount++;
	}

	if (itemCount == 0)
	{
		DrawTextB(baseX, baseY, FONTCOLOR_WHITE, 6, -1, "No items in inventory");
	}
}

VOID DrawPlayerBeltItems()
{
	if (V_MainMenuOpen)
		return;

	if (!Me || !Me->pInventory || !Me->pInventory->pFirstItem)
		return;

	// Base position for drawing
	INT baseX = 400;
	INT baseY = 100;
	INT lineHeight = 15; // Height between lines of text

	// Draw header
	DrawTextB(baseX, baseY, FONTCOLOR_GOLD, 6, -1, "Belt Items - Raw Data");
	baseY += lineHeight * 2;

	// Count total belt items
	INT totalBeltItems = 0;
	for (LPUNITANY Item = Me->pInventory->pFirstItem; Item; Item = Item->pItemData->pNextInvItem)
	{
		if (Item && Item->pItemData && GetItemLocation(Item) == STORAGE_BELT)
		{
			totalBeltItems++;
		}
	}

	DrawTextB(baseX, baseY, FONTCOLOR_WHITE, 6, -1, "Total Belt Items: %d", totalBeltItems);
	baseY += lineHeight * 2;

	// Column headers
	DrawTextB(baseX, baseY, FONTCOLOR_WHITE, 6, -1, "#  Code   ItemPath(X,Y)   Path(X,Y)   NodePage   ItemLoc");
	baseY += lineHeight;

	// List all belt items with detailed information
	INT itemIndex = 0;
	INT currentY = baseY;

	for (LPUNITANY Item = Me->pInventory->pFirstItem; Item; Item = Item->pItemData->pNextInvItem)
	{
		if (Item && Item->pItemData && GetItemLocation(Item) == STORAGE_BELT)
		{
			char itemCode[5] = {0};
			GetItemCodeFromItem(Item, itemCode, 4);

			// ItemPath coordinates
			INT itemPathX = -1;
			INT itemPathY = -1;
			if (Item->pItemPath)
			{
				itemPathX = Item->pItemPath->dwPosX;
				itemPathY = Item->pItemPath->dwPosY;
			}

			// Path coordinates
			INT pathX = -1;
			INT pathY = -1;
			if (Item->pPath)
			{
				pathX = Item->pPath->xPos;
				pathY = Item->pPath->yPos;
			}

			// Item data
			BYTE nodePage = 255;
			BYTE itemLocation = 255;
			if (Item->pItemData)
			{
				nodePage = Item->pItemData->NodePage;
				itemLocation = Item->pItemData->ItemLocation;
			}

			// Draw item info
			DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1,
					  "%2d  %-5s  (%2d,%2d)        (%2d,%2d)      %3d       %3d",
					  itemIndex + 1,
					  itemCode,
					  itemPathX, itemPathY,
					  pathX, pathY,
					  nodePage,
					  itemLocation);

			currentY += lineHeight;
			itemIndex++;
		}
	}

	// If no belt items found
	if (itemIndex == 0)
	{
		DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "No items found in belt");
	}

	// Now show the column counts as calculated by SetEmptyBeltSlotVars logic
	currentY += lineHeight * 2;
	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 6, -1, "Column Counts (SetEmptyBeltSlotVars Logic):");
	currentY += lineHeight;

	// Count items using the same logic as SetEmptyBeltSlotVars
	DWORD column1Count = 0;
	DWORD column2Count = 0;
	DWORD column3Count = 0;
	DWORD column4Count = 0;

	for (LPUNITANY Item = Me->pInventory->pFirstItem; Item; Item = Item->pItemData->pNextInvItem)
	{
		if (Item && Item->pItemData && GetItemLocation(Item) == STORAGE_BELT && Item->pItemPath)
		{
			DWORD x = Item->pItemPath->dwPosX;

			if (x == 0)
				column1Count++;
			if (x == 1)
				column2Count++;
			if (x == 2)
				column3Count++;
			if (x == 3)
				column4Count++;
		}
	}

	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1,
			  "Column 1: %d/4, Column 2: %d/4, Column 3: %d/4, Column 4: %d/4",
			  column1Count, column2Count, column3Count, column4Count);
	currentY += lineHeight;

	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1,
			  "IsEmpty Flags: [1]:%d [2]:%d [3]:%d [4]:%d",
			  V_IsColumn1Empty, V_IsColumn2Empty, V_IsColumn3Empty, V_IsColumn4Empty);

	// Try alternative counting method using pPath instead of pItemPath
	currentY += lineHeight * 2;
	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 6, -1, "Alternative Column Counts (Using pPath):");
	currentY += lineHeight;

	column1Count = 0;
	column2Count = 0;
	column3Count = 0;
	column4Count = 0;

	for (LPUNITANY Item = Me->pInventory->pFirstItem; Item; Item = Item->pItemData->pNextInvItem)
	{
		if (Item && Item->pItemData && GetItemLocation(Item) == STORAGE_BELT && Item->pPath)
		{
			WORD x = Item->pPath->xPos;

			if (x == 0)
				column1Count++;
			if (x == 1)
				column2Count++;
			if (x == 2)
				column3Count++;
			if (x == 3)
				column4Count++;
		}
	}

	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1,
			  "Column 1: %d/4, Column 2: %d/4, Column 3: %d/4, Column 4: %d/4",
			  column1Count, column2Count, column3Count, column4Count);
}

VOID DrawNearestItem()
{
	if (V_MainMenuOpen || !GameReady())
		return;

	LPUNITANY pItem = GetClosestUnit(UNIT_TYPE_ITEM);

	// Ensure we have a valid item and it's on the ground
	if (!pItem || !pItem->pItemData || pItem->dwMode != ITEM_MODE_ON_GROUND)
	{
		return;
	}

	// Base position for drawing
	INT baseX = 250;
	INT baseY = 10;
	INT lineHeight = 12;
	INT currentY = baseY;

	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 8, -1, "Nearest Item on Ground:");
	currentY += lineHeight * 2;

	// --- Basic Info ---
	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "Unit ID: %d", pItem->dwUnitId);
	currentY += lineHeight;
	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "TxtFileNo: %d", pItem->dwTxtFileNo);
	currentY += lineHeight;
	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "Mode: %d", pItem->dwMode);
	currentY += lineHeight;

	// --- Name and Code ---
	CHAR szItemName[128] = "Unknown Item";
	WCHAR wItemName[128] = {0};
	D2CLIENT_GetItemNameString(pItem, wItemName, 128);
	if (wItemName[0] != L'\0')
	{
		WideCharToMultiByte(CP_ACP, 0, wItemName, -1, szItemName, sizeof(szItemName), 0, 0);
	}
	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 6, -1, "Name: %s", szItemName);
	currentY += lineHeight;

	char itemCode[5] = {0};
	GetItemCodeFromItem(pItem, itemCode, 4);
	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "Item Code: %s", itemCode);
	currentY += lineHeight * 2;

	// --- Properties ---
	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 6, -1, "Properties:");
	currentY += lineHeight;

	DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "iLvl: %d", pItem->pItemData->dwItemLevel);
	currentY += lineHeight;

	const char *qualityNames[] = {"", "Inferior", "Normal", "Superior", "Magic", "Set", "Rare", "Unique", "Crafted"};
	DWORD quality = pItem->pItemData->dwQuality;
	if (quality > 0 && quality < 9)
	{
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Quality: %s", qualityNames[quality]);
	}
	else
	{
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Quality: %d", quality);
	}
	currentY += lineHeight;

	// Sockets
	DWORD sockets = GetUnitStat(pItem, STAT_SOCKETS);
	if (pItem->pItemData->dwFlags & ITEM_HASSOCKETS)
	{
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Sockets: %d", sockets);
	}
	else
	{
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Sockets: No");
	}
	currentY += lineHeight;

	// --- Flags ---
	if (pItem->pItemData->dwFlags & ITEM_IDENTIFIED)
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Identified: Yes");
	else
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Identified: No");
	currentY += lineHeight;

	if (pItem->pItemData->dwFlags & ITEM_ETHEREAL)
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Ethereal: Yes");
	else
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Ethereal: No");
	currentY += lineHeight * 2;

	// --- Position ---
	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 6, -1, "Position:");
	currentY += lineHeight;

	if (pItem->pPath)
	{
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "World Coords: (%d, %d)", pItem->pPath->xPos, pItem->pPath->yPos);
		currentY += lineHeight;

		LONG unitX = (LONG)D2CLIENT_GetUnitX(pItem);
		LONG unitY = (LONG)D2CLIENT_GetUnitY(pItem);
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Unit X: %d, Unit Y: %d", unitX, unitY);
		currentY += lineHeight;

		POINT unitPos = {unitX, unitY};
		WorldToScreen(&unitPos);

		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Screen Coords: (%d, %d)", unitPos.x, unitPos.y);

		currentY += lineHeight;
	}
	else
	{
		DrawTextB(baseX + 10, currentY, FONTCOLOR_WHITE, 6, -1, "Position data unavailable.");
		currentY += lineHeight;
	}
}

VOID DrawCurrentRoomInfo()
{
	if (V_MainMenuOpen)
		return;

	if (!Me || !Me->pPath || !Me->pPath->pRoom1)
		return;

	// Base position for drawing
	INT baseX = 600;
	INT baseY = 50;
	INT lineHeight = 15;
	INT currentY = baseY;
	INT infoColumnX = 600;

	// Draw header
	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 8, -1, "Current Room Information");
	currentY += lineHeight * 2;

	// Get current room info
	LPROOM1 currentRoom = Me->pPath->pRoom1;
	LPROOM2 currentRoom2 = currentRoom->pRoom2;
	LPLEVEL currentLevel = currentRoom2->pLevel;

	// Draw level and room info
	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "Level: %d", currentLevel->dwLevelNo);
	currentY += lineHeight;

	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "Room Position: (%d, %d)", currentRoom2->dwPosX, currentRoom2->dwPosY);
	currentY += lineHeight;

	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "Room Size: %d x %d", currentRoom2->dwSizeX, currentRoom2->dwSizeY);
	currentY += lineHeight;

	DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "Room Preset Type: %d", currentRoom2->dwPresetType);
	currentY += lineHeight * 2;

	// Draw header for preset units
	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 7, -1, "Preset Units in Current Room:");
	currentY += lineHeight * 2;

	// Column headers
	DrawTextB(baseX, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "Type");
	DrawTextB(baseX + 60, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "TxtFileNo");
	DrawTextB(baseX + 130, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "Position");
	DrawTextB(baseX + 220, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "Name/Info");
	currentY += lineHeight;

	// Draw separator line
	for (INT i = 0; i < 350; i += 5)
	{
		DrawTextB(baseX + i, currentY, FONTCOLOR_DARKGREY, 6, -1, "-");
	}
	currentY += lineHeight;

	// Track if we found any preset units
	BOOL foundPresetUnits = FALSE;

	// Draw all preset units in the room
	if (currentRoom2->pPreset)
	{
		for (LPPRESETUNIT pUnit = currentRoom2->pPreset; pUnit; pUnit = pUnit->pPresetNext)
		{
			foundPresetUnits = TRUE;

			// Unit type name
			const char *typeName = "Unknown";
			switch (pUnit->dwType)
			{
			case UNIT_TYPE_PLAYER:
				typeName = "Player";
				break;
			case UNIT_TYPE_NPC:
				typeName = "NPC";
				break;
			case UNIT_TYPE_OBJECT:
				typeName = "Object";
				break;
			case UNIT_TYPE_MISSILE:
				typeName = "Missile";
				break;
			case UNIT_TYPE_ITEM:
				typeName = "Item";
				break;
			case UNIT_TYPE_TILE:
				typeName = "Tile";
				break;
			default:
				typeName = "Unknown";
				break;
			}

			// Draw unit type
			DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "%s", typeName);

			// Draw TxtFileNo
			DrawTextB(baseX + 60, currentY, FONTCOLOR_WHITE, 6, -1, "%d", pUnit->dwTxtFileNo);

			// Draw position
			INT unitWorldX = (currentRoom2->dwPosX * 5) + pUnit->dwPosX;
			INT unitWorldY = (currentRoom2->dwPosY * 5) + pUnit->dwPosY;
			DrawTextB(baseX + 130, currentY, FONTCOLOR_WHITE, 6, -1, "(%d, %d)", unitWorldX, unitWorldY);

			// Check if this is already a tracked unit
			BOOL isTracked = FALSE;
			const char *unitName = NULL;

			if (pUnit->dwType == UNIT_TYPE_NPC)
			{
				for (size_t i = 0; trackedCampaignMonsters[i].szName != NULL; ++i)
				{
					if (pUnit->dwTxtFileNo == trackedCampaignMonsters[i].dwClassId)
					{
						isTracked = TRUE;
						unitName = trackedCampaignMonsters[i].szName;
						break;
					}
				}
			}
			else if (pUnit->dwType == UNIT_TYPE_OBJECT)
			{
				for (size_t i = 0; trackedCampaignObjects[i].szName != NULL; ++i)
				{
					if (pUnit->dwTxtFileNo == trackedCampaignObjects[i].dwClassId)
					{
						isTracked = TRUE;
						unitName = trackedCampaignObjects[i].szName;
						break;
					}
				}
			}
			else if (pUnit->dwType == UNIT_TYPE_TILE)
			{
				// Check if it's a level transition
				DWORD dwTargetLevel = V_Reveal->GetTileLevelNo(currentRoom2, pUnit->dwTxtFileNo);
				if (dwTargetLevel)
				{
					CHAR szLevel[0x40] = "";
					GetMapName((INT)dwTargetLevel, szLevel, 0x40);
					DrawTextB(baseX + 220, currentY, FONTCOLOR_CYAN, 6, -1, "Exit to: %s", szLevel);
				}
			}

			// Draw unit name if tracked
			if (isTracked)
			{
				DrawTextB(baseX + 220, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "%s [TRACKED]", unitName);
			}
			else
			{
				DrawTextB(baseX + 220, currentY, FONTCOLOR_RED, 6, -1, "Not tracked");
			}

			currentY += lineHeight;

			// If we've displayed too many units, add a note and stop
			if (currentY > 500)
			{
				DrawTextB(baseX, currentY, FONTCOLOR_YELLOW, 6, -1, "Too many units to display...");
				break;
			}
		}
	}

	if (!foundPresetUnits)
	{
		DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "No preset units found in this room.");
		currentY += lineHeight;
	}

	// Draw header for live units in the room
	currentY += lineHeight;
	DrawTextB(baseX, currentY, FONTCOLOR_GOLD, 7, -1, "Special Units in Current Room:");
	currentY += lineHeight * 2;

	// Column headers
	DrawTextB(baseX, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "Type");
	DrawTextB(baseX + 60, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "TxtFileNo");
	DrawTextB(baseX + 130, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "Position");
	DrawTextB(baseX + 220, currentY, FONTCOLOR_LIGHTGREEN, 6, -1, "Name/Info");
	currentY += lineHeight;

	// Draw separator line
	for (INT i = 0; i < 350; i += 5)
	{
		DrawTextB(baseX + i, currentY, FONTCOLOR_DARKGREY, 6, -1, "-");
	}
	currentY += lineHeight;

	// Track if we found any live units
	BOOL foundLiveUnits = FALSE;

	// Draw all live units in the room
	for (LPUNITANY pUnit = currentRoom->pUnitFirst; pUnit; pUnit = pUnit->pListNext)
	{
		if (!pUnit)
			continue;

		// Skip missiles and items
		if (pUnit->dwType == UNIT_TYPE_MISSILE || pUnit->dwType == UNIT_TYPE_ITEM)
			continue;

		// Skip normal monsters (only show special monsters)
		if (pUnit->dwType == UNIT_TYPE_NPC)
		{
			// Skip if it's not a boss, champion, or unique monster
			if (pUnit->pMonsterData &&
				!pUnit->pMonsterData->fBoss &&
				!pUnit->pMonsterData->fChamp &&
				!pUnit->pMonsterData->fMinion &&
				!pUnit->pMonsterData->fUnk)
			{
				continue;
			}
		}

		foundLiveUnits = TRUE;

		// Unit type name
		const char *typeName = "Unknown";
		switch (pUnit->dwType)
		{
		case UNIT_TYPE_PLAYER:
			typeName = "Player";
			break;
		case UNIT_TYPE_NPC:
			typeName = "NPC";
			break;
		case UNIT_TYPE_OBJECT:
			typeName = "Object";
			break;
		default:
			typeName = "Unknown";
			break;
		}

		// Draw unit type
		DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "%s", typeName);

		// Draw TxtFileNo
		DrawTextB(baseX + 60, currentY, FONTCOLOR_WHITE, 6, -1, "%d", pUnit->dwTxtFileNo);

		// Draw position
		if (pUnit->pPath)
		{

			DrawTextB(baseX + 130, currentY, FONTCOLOR_WHITE, 6, -1, "(%d, %d)", D2CLIENT_GetUnitX(pUnit), D2CLIENT_GetUnitY(pUnit));
		}

		// Draw name
		WCHAR *wName = (WCHAR *)GetUnitName(pUnit);
		if (wName)
		{
			CHAR szName[128] = "";
			WideCharToMultiByte(CP_ACP, 0, wName, -1, szName, sizeof(szName), 0, 0);
			DrawTextB(baseX + 220, currentY, FONTCOLOR_WHITE, 6, -1, "%s", szName);
		}

		// Draw additional info for specific unit types
		if (pUnit->dwType == UNIT_TYPE_NPC && pUnit->pMonsterData)
		{
			// Check if it's a boss, champion, etc.
			if (pUnit->pMonsterData->fBoss)
				DrawTextB(baseX + 320, currentY, FONTCOLOR_GOLD, 6, -1, "[Boss]");
			else if (pUnit->pMonsterData->fChamp)
				DrawTextB(baseX + 320, currentY, FONTCOLOR_ORANGE, 6, -1, "[Champion]");
			else if (pUnit->pMonsterData->fMinion)
				DrawTextB(baseX + 320, currentY, FONTCOLOR_YELLOW, 6, -1, "[Minion]");
		}
		else if (pUnit->dwType == UNIT_TYPE_OBJECT && pUnit->pObjectData)
		{
			// For objects, show the object type
			DrawTextB(baseX + 320, currentY, FONTCOLOR_CYAN, 6, -1, "Type: %d", pUnit->pObjectData->Type);
		}

		currentY += lineHeight;

		// If we've displayed too many units, add a note and stop
		if (currentY > 500)
		{
			DrawTextB(baseX, currentY, FONTCOLOR_YELLOW, 6, -1, "Too many units to display...");
			break;
		}
	}

	if (!foundLiveUnits)
	{
		DrawTextB(baseX, currentY, FONTCOLOR_WHITE, 6, -1, "No live units found in this room.");
	}
}