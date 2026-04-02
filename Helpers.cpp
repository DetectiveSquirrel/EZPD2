#include "Hack.h"

BOOL RemovePEHeader(HMODULE DLL)
{
	PIMAGE_DOS_HEADER DOSHD = (PIMAGE_DOS_HEADER)(DLL);
	PIMAGE_NT_HEADERS NTHD;

	if (DOSHD->e_magic == IMAGE_DOS_SIGNATURE)
	{
		NTHD = (PIMAGE_NT_HEADERS)((DWORD)DOSHD + DOSHD->e_lfanew);

		if (NTHD->Signature == IMAGE_NT_SIGNATURE)
		{
			DWORD OldProtect;
			VirtualProtect((LPVOID)NTHD, sizeof(IMAGE_NT_HEADERS), PAGE_READWRITE, &OldProtect);
			::memset(NTHD, NULL, sizeof(IMAGE_NT_HEADERS));

			VirtualProtect((LPVOID)NTHD, sizeof(IMAGE_NT_HEADERS), OldProtect, &OldProtect);
			VirtualProtect((LPVOID)DOSHD, sizeof(IMAGE_DOS_HEADER), PAGE_READWRITE, &OldProtect);

			::memset(DOSHD, NULL, sizeof(IMAGE_DOS_HEADER));
			VirtualProtect((LPVOID)DOSHD, sizeof(IMAGE_DOS_HEADER), OldProtect, &OldProtect);
			return TRUE;
		}
	}

	return FALSE;
}

VOID PrintText(DWORD Color, char *szText, ...)
{
	if (GameReady())
	{
		char szBuffer[152] = {0};
		va_list Args;
		va_start(Args, szText);
		vsprintf_s(szBuffer, 152, szText, Args);
		va_end(Args);
		wchar_t Buffer[0x130];
		MultiByteToWideChar(0, 1, szBuffer, 152, Buffer, 304);
		D2CLIENT_PrintGameString(Buffer, Color);
	}
}

BOOL GameReady()
{
	if (!Me) return FALSE;
	LPUNITANY pUnit = D2CLIENT_GetPlayerUnit();
	if (!VALIDPTR(pUnit)) return FALSE;
	if (*p_D2WIN_FirstControl) return FALSE;
	if (!VALIDPTR(pUnit->pInventory)) return FALSE;
	if (!VALIDPTR(pUnit->pPath)) return FALSE;
	if (!pUnit->pPath->xPos) return FALSE;
	if (!VALIDPTR(pUnit->pPath->pRoom1)) return FALSE;
	if (!VALIDPTR(pUnit->pPath->pRoom1->pRoom2)) return FALSE;
	if (!VALIDPTR(pUnit->pPath->pRoom1->pRoom2->pLevel)) return FALSE;
	if (!pUnit->pPath->pRoom1->pRoom2->pLevel->dwLevelNo) return FALSE;
	if (!VALIDPTR(pUnit->pAct)) return FALSE;
	if (!GetPlayerArea()) return FALSE;
	return TRUE;
}

DWORD GetPlayerArea()
{
	return D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
}

VOID ScreenToAutomap(POINT *pPos, INT nX, INT nY)
{
	pPos->x = ((nX - nY) / 2 / (*p_D2CLIENT_Divisor)) - (*p_D2CLIENT_Offset).x + 8;
	pPos->y = ((nX + nY) / 4 / (*p_D2CLIENT_Divisor)) - (*p_D2CLIENT_Offset).y - 8;

	if (D2CLIENT_GetAutomapSize())
	{
		--pPos->x;
		pPos->y += 5;
	}
}

BOOL GetMapName(BYTE iMapID, LPSTR lpszBuffer, DWORD dwMaxChars)
{
	if (lpszBuffer == NULL)
		return FALSE;

	lpszBuffer[0] = '\0';
	::memset(lpszBuffer, 0, sizeof(TCHAR) * dwMaxChars);
	if (dwMaxChars == 0)
		return FALSE;

	switch (iMapID)
	{
		///////////////////////////////////////////////////
		// Act 1 Maps
		///////////////////////////////////////////////////
	case MAP_A1_ROGUE_ENCAMPMENT:
		strncpy(lpszBuffer, "Rogue Encampment", dwMaxChars);
		break;

	case MAP_A1_BLOOD_MOOR:
		strncpy(lpszBuffer, "Blood Moor", dwMaxChars);
		break;

	case MAP_A1_COLD_PLAINS:
		strncpy(lpszBuffer, "Cold Plains", dwMaxChars);
		break;

	case MAP_A1_STONY_FIELD:
		strncpy(lpszBuffer, "Stony Field", dwMaxChars);
		break;

	case MAP_A1_DARK_WOOD:
		strncpy(lpszBuffer, "Dark Wood", dwMaxChars);
		break;

	case MAP_A1_BLACK_MARSH:
		strncpy(lpszBuffer, "Black Marsh", dwMaxChars);
		break;

	case MAP_A1_DEN_OF_EVIL:
		strncpy(lpszBuffer, "Den of Evil", dwMaxChars);
		break;

	case MAP_A1_CAVE_LEVEL_1:
		strncpy(lpszBuffer, "Cave Level 1", dwMaxChars);
		break;

	case MAP_A1_UNDERGROUND_PASSAGE_LEVEL_1:
		strncpy(lpszBuffer, "Underground Passage Level 1", dwMaxChars);
		break;

	case MAP_A1_HOLE_LEVEL_1:
		strncpy(lpszBuffer, "Hole Level 1", dwMaxChars);
		break;

	case MAP_A1_PIT_LEVEL_1:
		strncpy(lpszBuffer, "Pit Level 1", dwMaxChars);
		break;

	case MAP_A1_CAVE_LEVEL_2:
		strncpy(lpszBuffer, "Cave Level 2", dwMaxChars);
		break;

	case MAP_A1_UNDERGROUND_PASSAGE_LEVEL_2:
		strncpy(lpszBuffer, "Underground Passage Level 2", dwMaxChars);
		break;

	case MAP_A1_HOLE_LEVEL_2:
		strncpy(lpszBuffer, "Hole Level 2", dwMaxChars);
		break;

	case MAP_A1_PIT_LEVEL_2:
		strncpy(lpszBuffer, "Pit Level 2", dwMaxChars);
		break;

	case MAP_A1_BURIAL_GROUNDS:
		strncpy(lpszBuffer, "Burial Grounds", dwMaxChars);
		break;

	case MAP_A1_CRYPT:
		strncpy(lpszBuffer, "Crypt", dwMaxChars);
		break;

	case MAP_A1_MAUSOLEUM:
		strncpy(lpszBuffer, "Mausoleum", dwMaxChars);
		break;

	case MAP_A1_FORGOTTEN_TOWER:
		strncpy(lpszBuffer, "Forgotten Tower", dwMaxChars);
		break;

	case MAP_A1_TOWER_CELLAR_LEVEL_1:
		strncpy(lpszBuffer, "Tower Cellar Level 1", dwMaxChars);
		break;

	case MAP_A1_TOWER_CELLAR_LEVEL_2:
		strncpy(lpszBuffer, "Tower Cellar Level 2", dwMaxChars);
		break;

	case MAP_A1_TOWER_CELLAR_LEVEL_3:
		strncpy(lpszBuffer, "Tower Cellar Level 3", dwMaxChars);
		break;

	case MAP_A1_TOWER_CELLAR_LEVEL_4:
		strncpy(lpszBuffer, "Tower Cellar Level 4", dwMaxChars);
		break;

	case MAP_A1_TOWER_CELLAR_LEVEL_5:
		strncpy(lpszBuffer, "Tower Cellar Level 5", dwMaxChars);
		break;

	case MAP_A1_TAMOE_HIGHLAND:
		strncpy(lpszBuffer, "Tamoe Highland", dwMaxChars);
		break;

	case MAP_A1_MONASTERY_GATE:
		strncpy(lpszBuffer, "Monastery Gate", dwMaxChars);
		break;

	case MAP_A1_OUTER_CLOISTER:
		strncpy(lpszBuffer, "Outer Cloister", dwMaxChars);
		break;

	case MAP_A1_BARRACKS:
		strncpy(lpszBuffer, "Barracks", dwMaxChars);
		break;

	case MAP_A1_JAIL_LEVEL_1:
		strncpy(lpszBuffer, "Jail Level 1", dwMaxChars);
		break;

	case MAP_A1_JAIL_LEVEL_2:
		strncpy(lpszBuffer, "Jail Level 2", dwMaxChars);
		break;

	case MAP_A1_JAIL_LEVEL_3:
		strncpy(lpszBuffer, "Jail Level 3", dwMaxChars);
		break;

	case MAP_A1_INNER_CLOISTER:
	case MAP_A1_INNER_CLOISTER_2:
		strncpy(lpszBuffer, "Inner Cloister", dwMaxChars);
		break;

	case MAP_A1_CATACOMBS_LEVEL_1:
		strncpy(lpszBuffer, "Catacombs Level 1", dwMaxChars);
		break;

	case MAP_A1_CATACOMBS_LEVEL_2:
		strncpy(lpszBuffer, "Catacombs Level 2", dwMaxChars);
		break;

	case MAP_A1_CATACOMBS_LEVEL_3:
		strncpy(lpszBuffer, "Catacombs Level 3", dwMaxChars);
		break;

	case MAP_A1_CATACOMBS_LEVEL_4:
		strncpy(lpszBuffer, "Catacombs Level 4", dwMaxChars);
		break;

	case MAP_A1_TRISTRAM:
		strncpy(lpszBuffer, "Tristram", dwMaxChars);
		break;

	case MAP_A1_THE_SECRET_COW_LEVEL:
		strncpy(lpszBuffer, "The Secret Cow Level", dwMaxChars);
		break;

		///////////////////////////////////////////////////
		// Act 2 Maps
		///////////////////////////////////////////////////
	case MAP_A2_LUT_GHOLEIN:
		strncpy(lpszBuffer, "Lut Gholein", dwMaxChars);
		break;

	case MAP_A2_ROCKY_WASTE:
		strncpy(lpszBuffer, "Rocky Waste", dwMaxChars);
		break;

	case MAP_A2_DRY_HILLS:
		strncpy(lpszBuffer, "Dry Hills", dwMaxChars);
		break;

	case MAP_A2_FAR_OASIS:
		strncpy(lpszBuffer, "Far Oasis", dwMaxChars);
		break;

	case MAP_A2_LOST_CITY:
		strncpy(lpszBuffer, "Lost City", dwMaxChars);
		break;

	case MAP_A2_VALLEY_OF_SNAKES:
		strncpy(lpszBuffer, "Valley of Snakes", dwMaxChars);
		break;

	case MAP_A2_CANYON_OF_THE_MAGI:
		strncpy(lpszBuffer, "Cayon of the Magi", dwMaxChars);
		break;

	case MAP_A2_SEWERS_LEVEL_1:
		strncpy(lpszBuffer, "Sewers Level 1", dwMaxChars);
		break;

	case MAP_A2_SEWERS_LEVEL_2:
		strncpy(lpszBuffer, "Sewers Level 2", dwMaxChars);
		break;

	case MAP_A2_SEWERS_LEVEL_3:
		strncpy(lpszBuffer, "Sewers Level 3", dwMaxChars);
		break;

	case MAP_A2_HAREM_LEVEL_1:
		strncpy(lpszBuffer, "Harem Level 1", dwMaxChars);
		break;

	case MAP_A2_HAREM_LEVEL_2:
		strncpy(lpszBuffer, "Harem Level 2", dwMaxChars);
		break;

	case MAP_A2_PALACE_CELLAR_LEVEL_1:
		strncpy(lpszBuffer, "Palace Cellar Level 1", dwMaxChars);
		break;

	case MAP_A2_PALACE_CELLAR_LEVEL_2:
		strncpy(lpszBuffer, "Palace Cellar Level 2", dwMaxChars);
		break;

	case MAP_A2_PALACE_CELLAR_LEVEL_3:
		strncpy(lpszBuffer, "Palace Cellar Level 3", dwMaxChars);
		break;

	case MAP_A2_STONY_TOMB_LEVEL_1:
		strncpy(lpszBuffer, "Stony Tomb Level 1", dwMaxChars);
		break;

	case MAP_A2_HALLS_OF_THE_DEAD_LEVEL_1:
		strncpy(lpszBuffer, "Halls of the Dead Level 1", dwMaxChars);
		break;

	case MAP_A2_HALLS_OF_THE_DEAD_LEVEL_2:
		strncpy(lpszBuffer, "Halls of the Dead Level 2", dwMaxChars);
		break;

	case MAP_A2_CLAW_VIPER_TEMPLE_LEVEL_1:
		strncpy(lpszBuffer, "Claw Viper Temple Level 1", dwMaxChars);
		break;

	case MAP_A2_STONY_TOMB_LEVEL_2:
		strncpy(lpszBuffer, "Stony Tomb Level 2", dwMaxChars);
		break;

	case MAP_A2_HALLS_OF_THE_DEAD_LEVEL_3:
		strncpy(lpszBuffer, "Halls of the Dead Level 3", dwMaxChars);
		break;

	case MAP_A2_CLAW_VIPER_TEMPLE_LEVEL_2:
		strncpy(lpszBuffer, "Claw Viper Temple Level 2", dwMaxChars);
		break;

	case MAP_A2_MAGGOT_LAIR_LEVEL_1:
		strncpy(lpszBuffer, "Maggot Lair Level 1", dwMaxChars);
		break;

	case MAP_A2_MAGGOT_LAIR_LEVEL_2:
		strncpy(lpszBuffer, "Maggot Lair Level 2", dwMaxChars);
		break;

	case MAP_A2_MAGGOT_LAIR_LEVEL_3:
		strncpy(lpszBuffer, "Maggot Lair Level 3", dwMaxChars);
		break;

	case MAP_A2_ANCIENT_TUNNELS:
		strncpy(lpszBuffer, "Ancient Tunnels", dwMaxChars);
		break;

	case MAP_A2_TAL_RASHAS_TOMB_1:
	case MAP_A2_TAL_RASHAS_TOMB_2:
	case MAP_A2_TAL_RASHAS_TOMB_3:
	case MAP_A2_TAL_RASHAS_TOMB_4:
	case MAP_A2_TAL_RASHAS_TOMB_5:
	case MAP_A2_TAL_RASHAS_TOMB_6:
	case MAP_A2_TAL_RASHAS_TOMB_7:
		strncpy(lpszBuffer, "Tal Rasha's Tomb", dwMaxChars);
		break;

	case MAP_A2_TAL_RASHAS_CHAMBER:
		strncpy(lpszBuffer, "Tal Rasha's Chamber", dwMaxChars);
		break;

	case MAP_A2_ARCANE_SANCTUARY:
		strncpy(lpszBuffer, "Arcane Sanctuary", dwMaxChars);
		break;

		///////////////////////////////////////////////////
		// Act 3 Maps
		///////////////////////////////////////////////////
	case MAP_A3_KURAST_DOCKS:
		strncpy(lpszBuffer, "Kurast Docks", dwMaxChars);
		break;

	case MAP_A3_SPIDER_FOREST:
		strncpy(lpszBuffer, "Spider Forest", dwMaxChars);
		break;

	case MAP_A3_GREAT_MARSH:
		strncpy(lpszBuffer, "Great Marsh", dwMaxChars);
		break;

	case MAP_A3_FLAYER_JUNGLE:
		strncpy(lpszBuffer, "Flayer Jungle", dwMaxChars);
		break;

	case MAP_A3_LOWER_KURAST:
		strncpy(lpszBuffer, "Lower Kurast", dwMaxChars);
		break;

	case MAP_A3_KURAST_BAZAAR:
		strncpy(lpszBuffer, "Kurast Bazaar", dwMaxChars);
		break;

	case MAP_A3_UPPER_KURAST:
		strncpy(lpszBuffer, "Upper Kurast", dwMaxChars);
		break;

	case MAP_A3_KURAST_CAUSEWAY:
		strncpy(lpszBuffer, "Kurast Causeway", dwMaxChars);
		break;

	case MAP_A3_TRAVINCAL:
		strncpy(lpszBuffer, "Travincal", dwMaxChars);
		break;

	case MAP_A3_ARCHNID_LAIR:
		strncpy(lpszBuffer, "Arachnid Lair", dwMaxChars);
		break;

	case MAP_A3_SPIDER_CAVERN:
		strncpy(lpszBuffer, "Spider Cavern", dwMaxChars);
		break;

	case MAP_A3_SWAMPY_PIT_LEVEL_1:
		strncpy(lpszBuffer, "Swampy Pit Level 1", dwMaxChars);
		break;

	case MAP_A3_SWAMPY_PIT_LEVEL_2:
		strncpy(lpszBuffer, "Swampy Pit Level 2", dwMaxChars);
		break;

	case MAP_A3_FLAYER_DUNGEON_LEVEL_1:
		strncpy(lpszBuffer, "Flayer Dungeon Level 1", dwMaxChars);
		break;

	case MAP_A3_FLAYER_DUNGEON_LEVEL_2:
		strncpy(lpszBuffer, "Flayer Dungeon Level 2", dwMaxChars);
		break;

	case MAP_A3_SWAMPY_PIT_LEVEL_3:
		strncpy(lpszBuffer, "Swampy Pit Level 3", dwMaxChars);
		break;

	case MAP_A3_FLAYER_DUNGEON_LEVEL_3:
		strncpy(lpszBuffer, "Flayer Dungeon Level 3", dwMaxChars);
		break;

	case MAP_A3_SEWERS_LEVEL_1:
		strncpy(lpszBuffer, "Sewers Level 1", dwMaxChars);
		break;

	case MAP_A3_SEWERS_LEVEL_2:
		strncpy(lpszBuffer, "Sewers Level 2", dwMaxChars);
		break;

	case MAP_A3_RUINED_TEMPLE:
		strncpy(lpszBuffer, "Ruined Temple", dwMaxChars);
		break;

	case MAP_A3_DISUSED_FANE:
		strncpy(lpszBuffer, "Disused Fane", dwMaxChars);
		break;

	case MAP_A3_FORGOTTEN_RELIQUARY:
		strncpy(lpszBuffer, "Forgotten Reliquary", dwMaxChars);
		break;

	case MAP_A3_FORGOTTEN_TEMPLE:
		strncpy(lpszBuffer, "Forgotten Temple", dwMaxChars);
		break;

	case MAP_A3_RUINED_FANE:
		strncpy(lpszBuffer, "Ruined Fane", dwMaxChars);
		break;

	case MAP_A3_DISUSED_RELIQUARY:
		strncpy(lpszBuffer, "Disused Reliquary", dwMaxChars);
		break;

	case MAP_A3_DURANCE_OF_HATE_LEVEL_1:
		strncpy(lpszBuffer, "Durance of Hate Level 1", dwMaxChars);
		break;

	case MAP_A3_DURANCE_OF_HATE_LEVEL_2:
		strncpy(lpszBuffer, "Durance of Hate Level 2", dwMaxChars);
		break;

	case MAP_A3_DURANCE_OF_HATE_LEVEL_3:
		strncpy(lpszBuffer, "Durance of Hate Level 3", dwMaxChars);
		break;

		///////////////////////////////////////////////////
		// Act 4 Maps
		///////////////////////////////////////////////////
	case MAP_A4_THE_PANDEMONIUM_FORTRESS:
		strncpy(lpszBuffer, "The Pandemonium Fortress", dwMaxChars);
		break;

	case MAP_A4_OUTER_STEPPES:
		strncpy(lpszBuffer, "Outer Steppes", dwMaxChars);
		break;

	case MAP_A4_PLAINS_OF_DESPAIR:
		strncpy(lpszBuffer, "Plains of Despair", dwMaxChars);
		break;

	case MAP_A4_CITY_OF_THE_DAMNED:
		strncpy(lpszBuffer, "City of the Damned", dwMaxChars);
		break;

	case MAP_A4_RIVER_OF_FLAME:
		strncpy(lpszBuffer, "River of Flame", dwMaxChars);
		break;

	case MAP_A4_THE_CHAOS_SANCTUARY:
		strncpy(lpszBuffer, "The Chaos Sanctuary", dwMaxChars);
		break;

		///////////////////////////////////////////////////
		// Act 5 Maps
		///////////////////////////////////////////////////
	case MAP_A5_HARROGATH:
		strncpy(lpszBuffer, "Harrogath", dwMaxChars);
		break;

	case MAP_A5_THE_BLOODY_FOOTHILLS:
		strncpy(lpszBuffer, "The Bloody Foothills", dwMaxChars);
		break;

	case MAP_A5_FRIGID_HIGHLANDS:
		strncpy(lpszBuffer, "Frigid Highlands", dwMaxChars);
		break;

	case MAP_A5_ARREAT_PLATEAU:
		strncpy(lpszBuffer, "Arreat Plateau", dwMaxChars);
		break;

	case MAP_A5_CRYSTALLINE_PASSAGE:
		strncpy(lpszBuffer, "Crystalline Passage", dwMaxChars);
		break;

	case MAP_A5_FROZEN_RIVER:
		strncpy(lpszBuffer, "Frozen River", dwMaxChars);
		break;

	case MAP_A5_GLACIAL_TRAIL:
		strncpy(lpszBuffer, "Glacial Trail", dwMaxChars);
		break;

	case MAP_A5_DRIFTER_CAVERN:
		strncpy(lpszBuffer, "Drifter Cavern", dwMaxChars);
		break;

	case MAP_A5_FROZEN_TUNDRA:
		strncpy(lpszBuffer, "Frozen Tundra", dwMaxChars);
		break;

	case MAP_A5_THE_ANCIENTS_WAY:
		strncpy(lpszBuffer, "The Ancients' Way", dwMaxChars);
		break;

	case MAP_A5_ICY_CELLAR:
		strncpy(lpszBuffer, "Icy Cellar", dwMaxChars);
		break;

	case MAP_A5_ARREAT_SUMMIT:
		strncpy(lpszBuffer, "Arreat Summit", dwMaxChars);
		break;

	case MAP_A5_HALLS_OF_PAIN:
		strncpy(lpszBuffer, "Halls of Pain", dwMaxChars);
		break;

	case MAP_A5_ABADDON:
		strncpy(lpszBuffer, "Abaddon", dwMaxChars);
		break;

	case MAP_A5_PIT_OF_ACHERON:
		strncpy(lpszBuffer, "Pit of Acheron", dwMaxChars);
		break;

	case MAP_A5_INFERNAL_PIT:
		strncpy(lpszBuffer, "Infernal Pit", dwMaxChars);
		break;

	case MAP_A5_NIHLATHAKS_TEMPLE:
		strncpy(lpszBuffer, "Nihlathak's Temple", dwMaxChars);
		break;

	case MAP_A5_HALLS_OF_ANGUISH:
		strncpy(lpszBuffer, "Halls of Anguish", dwMaxChars);
		break;

	case MAP_A5_HALLS_OF_VAUGHT:
		strncpy(lpszBuffer, "Halls of Vaught", dwMaxChars);
		break;

	case MAP_A5_WORLDSTONE_KEEP_LEVEL_1:
		strncpy(lpszBuffer, "Worldstone Keep Level 1", dwMaxChars);
		break;

	case MAP_A5_WORLDSTONE_KEEP_LEVEL_2:
		strncpy(lpszBuffer, "Worldstone Keep Level 2", dwMaxChars);
		break;

	case MAP_A5_WORLDSTONE_KEEP_LEVEL_3:
		strncpy(lpszBuffer, "Worldstone Keep Level 3", dwMaxChars);
		break;

	case MAP_A5_THRONE_OF_DESTRUCTION:
		strncpy(lpszBuffer, "Throne of Destruction", dwMaxChars);
		break;

	case MAP_A5_WORLDSTONE_KEEP:
		strncpy(lpszBuffer, "The Worldstone Chamber", dwMaxChars);
		break;

	default:
		return 0;
		break;
	}

	return FALSE;
}

Level *GetUnitLevel(UnitAny *pUnit)
{
	if (pUnit)
		if (pUnit->pPath)
			if (pUnit->pPath->pRoom1)
				if (pUnit->pPath->pRoom1->pRoom2)
					return pUnit->pPath->pRoom1->pRoom2->pLevel;

	return FALSE;
}

BOOL IsLeftPanelOpen()
{
	return *p_D2CLIENT_ScreenCovered == 2;
}

BOOL IsRightPanelOpen()
{
	return *p_D2CLIENT_ScreenCovered == 1;
}

BOOL IsFullScreenPanelOpen()
{
	return *p_D2CLIENT_ScreenCovered == 3;
}

RECT GetSafeScreenAreaRect()
{
	RECT drawingArea = {0};

	DWORD screenWidth = *p_D2CLIENT_ScreenSizeX;
	DWORD screenHeight = *p_D2CLIENT_ScreenSizeY;

	drawingArea.left = IsLeftPanelOpen() ? screenWidth * 0.60 : screenWidth * 0.10;
	drawingArea.right = IsRightPanelOpen() ? screenWidth * 0.40 : screenWidth * 0.90;
	drawingArea.top = screenHeight * 0.10;
	drawingArea.bottom = screenHeight * 0.80;

	return drawingArea;
}

POINT AdjustAutomapLabelPosition(POINT point)
{
	RECT drawingZone = GetSafeScreenAreaRect();
	if (IsPointInRect(point, drawingZone))
	{
		return point;
	}

	POINT nearestPoint = point;

	if (nearestPoint.x < drawingZone.left)
		nearestPoint.x = drawingZone.left;
	else if (nearestPoint.x > drawingZone.right)
		nearestPoint.x = drawingZone.right;

	if (nearestPoint.y < drawingZone.top)
		nearestPoint.y = drawingZone.top;
	else if (nearestPoint.y > drawingZone.bottom)
		nearestPoint.y = drawingZone.bottom;

	return nearestPoint;
}

BOOL IsPointInRect(POINT &pt, RECT &rect)
{
	return pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom;
}

BOOL IsValidMonster(LPUNITANY Unit)
{
	if (!Unit)
		return FALSE;

	if (Unit->dwMode == NPC_MODE_DEATH || Unit->dwMode == NPC_MODE_DEAD)
		return FALSE;

	if (Unit->dwTxtFileNo >= 110 && Unit->dwTxtFileNo <= 113 || Unit->dwTxtFileNo == 608 && Unit->dwMode == NPC_MODE_USESKILL1)
		return FALSE;

	if (Unit->dwTxtFileNo == 68 && Unit->dwMode == NPC_MODE_SEQUENCE)
		return FALSE;

	if ((Unit->dwTxtFileNo == 258 || Unit->dwTxtFileNo == 261) && Unit->dwMode == NPC_MODE_SEQUENCE)
		return FALSE;

	if ((Unit->dwTxtFileNo == 356 || Unit->dwTxtFileNo == 357 || Unit->dwTxtFileNo == 424 || Unit->dwTxtFileNo == 425 ||
		 Unit->dwTxtFileNo == 418 || Unit->dwTxtFileNo == 419 || Unit->dwTxtFileNo == 421) &&
		Unit->dwOwnerId == Me->dwUnitId)
		return FALSE;

	DWORD Bad[] =
		{
			146, 147, 148, 150, 154, 155, 175, 176, 177, 178, 198, 199, 200, 201, 202, 210, 244, 245, 246, 251, 252, 253, 254, 255, 257, 264,
			265, 266, 270, 283, 297, 326, 327, 328, 329, 330, 331, 338, 351, 352, 353, 359, 366, 367, 405, 406, 408, 410, 411, 412, 413, 414,
			415, 416, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 528, 535, 542, 543,
			1057 // Demonic Sentinel
		};

	for (DWORD i = 0; i < ArraySize(Bad) - 1; i++)
		if (Unit->dwTxtFileNo == Bad[i])
			return FALSE;

	WCHAR *Name = (WCHAR *)GetUnitName(Unit);
	CHAR MonsterName[50];

	WideCharToMultiByte(CP_ACP, 0, Name, -1, MonsterName, (INT)sizeof(MonsterName), 0, 0);

	if (!strcmp(MonsterName, "an evil force") || !strcmp(MonsterName, "dummy") || !strcmp(MonsterName, "Trapped Soul"))
		return FALSE;

	return TRUE;
}

BOOL IsTownLevel(INT nLevel)
{
	return nLevel == MAP_A1_ROGUE_ENCAMPMENT ||
		   nLevel == MAP_A2_LUT_GHOLEIN ||
		   nLevel == MAP_A3_KURAST_DOCKS ||
		   nLevel == MAP_A4_THE_PANDEMONIUM_FORTRESS ||
		   nLevel == MAP_A5_HARROGATH;
}

BOOL IsPlayerInTown()
{
	Level *nLevel = GetUnitLevel(Me);
	if (nLevel)
	{
		DWORD levelNo = nLevel->dwLevelNo;
		if (IsTownLevel(levelNo))
		{
			return TRUE;
		}
	}
	return FALSE;
}

LPCSTR GetKeyName(int vkCode)
{
	if (vkCode == 0)
	{
		return "None";
	}

	static char keyName[32];

	UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

	switch (vkCode)
	{
	// Special cases
	case VK_F1:
		return "F1";
	case VK_F2:
		return "F2";
	case VK_F3:
		return "F3";
	case VK_F4:
		return "F4";
	case VK_F5:
		return "F5";
	case VK_F6:
		return "F6";
	case VK_F7:
		return "F7";
	case VK_F8:
		return "F8";
	case VK_F9:
		return "F9";
	case VK_F10:
		return "F10";
	case VK_F11:
		return "F11";
	case VK_F12:
		return "F12";
	case VK_F13:
		return "F13";
	case VK_F14:
		return "F14";
	case VK_F15:
		return "F15";
	case VK_INSERT:
		return "Insert";
	case VK_DELETE:
		return "Delete";
	case VK_HOME:
		return "Home";
	case VK_END:
		return "End";
	case VK_PRIOR:
		return "PgUp";
	case VK_NEXT:
		return "PgDown";
	case VK_LEFT:
		return "Left";
	case VK_RIGHT:
		return "Right";
	case VK_UP:
		return "Up";
	case VK_DOWN:
		return "Down";
	case VK_SPACE:
		return "Space";
	case VK_BROWSER_BACK:
		return "Browser Back";
	case VK_BROWSER_FORWARD:
		return "Browser Forward";
	default:
		// For standard keys, get name from Windows
		if (scanCode != 0)
		{
			GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName));
			return keyName;
		}
		return "?";
	}
}

LPUNITANY GetClosestUnit(DWORD dwType)
{
	if (!Me || !Me->pAct)
		return NULL;

	LPUNITANY pClosestUnit = NULL;
	DWORD dwClosestDistance = 0xFFFFFFFF;

	for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
	{
		for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
		{
			if (Unit && Unit->dwType == dwType)
			{
				// if (dwType == UNIT_TYPE_NPC && !IsValidMonster(Unit))
					// continue;

				if (Unit->pPath && Me->pPath)
				{
					INT unitX = D2CLIENT_GetUnitX(Unit);
					INT unitY = D2CLIENT_GetUnitY(Unit);
					INT nX = abs(unitX - Me->pPath->xPos);
					INT nY = abs(unitY - Me->pPath->yPos);
					DWORD dwDistance = nX * nX + nY * nY;

					if (dwDistance < dwClosestDistance)
					{
						dwClosestDistance = dwDistance;
						pClosestUnit = Unit;
					}
				}
			}
		}
	}

	return pClosestUnit;
}

DWORD GetPlayerVitalsPercent(BOOL bLife)
{
	if (!Me || !Me->pStats)
		return -1;

	if (bLife)
	{
		DWORD currentHP = GetUnitStat(Me, STAT_HP) / 256;
		DWORD maxHP = GetUnitStat(Me, STAT_MAXHP) / 256;
		return (currentHP * 100) / maxHP;
	}
	else
	{
		DWORD currentMana = GetUnitStat(Me, STAT_MANA) / 256;
		DWORD maxMana = GetUnitStat(Me, STAT_MAXMANA) / 256;
		return (currentMana * 100) / maxMana;
	}
}

DWORD GetUnitLifePercent(UnitAny *pUnit)
{
	if (!pUnit)
		return -1;

	DWORD unitHPPercent = GetUnitStat(pUnit, STAT_HP) / 327.68;

	if (unitHPPercent > 100)
		return 100;

	return unitHPPercent;
}

INT GetItemLocation(UnitAny *pItem)
{
	if (!pItem || !pItem->pItemData)
		return -1;

	switch (pItem->pItemData->ItemLocation)
	{
	case STORAGE_INVENTORY:
		return STORAGE_INVENTORY;

	case STORAGE_CUBE:
		return STORAGE_CUBE;

	case STORAGE_STASH:
		return STORAGE_STASH;

	case STORAGE_NULL:
		switch (pItem->pItemData->NodePage)
		{
		case NODEPAGE_EQUIP:
			return STORAGE_EQUIP;

		case NODEPAGE_BELTSLOTS:
			return STORAGE_BELT;
		}
	}

	return STORAGE_NULL;
}

BOOL GetItemCodeFromItem(UnitAny *pItem, LPSTR szBuffer, DWORD dwMax)
{
	if (!pItem)
		return FALSE;

	ItemTxt *pTxt = D2COMMON_GetItemText(pItem->dwTxtFileNo);
	::memcpy(szBuffer, pTxt->szCode, min(3, dwMax));
	return (int)strlen(szBuffer);
}

VOID SimulateKeyPress(INT vKey)
{
	// Simulate key press
	INPUT input[2] = {0};

	// Key down
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = vKey;
	input[0].ki.dwFlags = 0;

	// Key up
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = vKey;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP;

	// Send the input
	SendInput(2, input, sizeof(INPUT));
}

VOID SimulateKeyDown(INT vKey)
{
	// Simulate key down
	INPUT input = {0};

	// Key down
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vKey;
	input.ki.dwFlags = 0;

	// Send the input
	SendInput(1, &input, sizeof(INPUT));
}

VOID SimulateKeyUp(INT vKey)
{
	// Simulate key up
	INPUT input = {0};

	// Key up
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vKey;
	input.ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(1, &input, sizeof(INPUT));
}

LPCSTR GetPotionTypeName(INT potionType)
{
	switch (potionType)
	{
	case POTION_TYPE_REJUV:
		return "Rejuv";
	case POTION_TYPE_LIFE:
		return "Life";
	case POTION_TYPE_MANA:
		return "Mana";
	default:
		return "Unknown";
	}
}

INT SetValidPotionForRefillSlot(INT potionType)
{
	// Wrap around to valid values, skipping POTION_TYPE_NONE
	if (potionType <= POTION_TYPE_NONE)
		return POTION_TYPE_MANA;
	else if (potionType > POTION_TYPE_MANA)
		return POTION_TYPE_REJUV;

	return potionType;
}

POINT GetInventorySlotPixelCoordinates(INT x, INT y)
{
	POINT point;
	point.x = 549 + (x * 28) + 14;
	point.y = 249 + (y * 28) + 14;
	return point;
}

VOID SimulateLeftClick(POINT point)
{
	SetCursorPos(point.x, point.y);
	// use winapi for left click
	INPUT input[2] = {0};

	// Key down
	input[0].type = INPUT_MOUSE;
	input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	input[0].mi.dx = point.x;
	input[0].mi.dy = point.y;

	// Key up
	input[1].type = INPUT_MOUSE;
	input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	input[1].mi.dx = point.x;
	input[1].mi.dy = point.y;

	// Send the input
	SendInput(2, input, sizeof(INPUT));
}

BOOL IsPotionToFillInInventory()
{
	for (LPUNITANY Item = Me->pInventory->pFirstItem; Item; Item = Item->pItemData->pNextInvItem)
	{
		if (Item && Item->pItemData && GetItemLocation(Item) == STORAGE_INVENTORY && Item->pItemPath)
		{
			INT x = Item->pItemPath->dwPosX;
			INT y = Item->pItemPath->dwPosY;

			if (x == V_ItemSlotLocationToFill.x && y == V_ItemSlotLocationToFill.y)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

VOID IncreaseTickCount()
{
	V_TickCount++;
	if (V_TickCount > 80)
	{
		V_TickCount = 0;
	}
}

DWORD GetTextWidth(LPSTR text, INT font)
{
	if (!text)
		return 0;

	WCHAR wBuffer[512] = {0};
	MultiByteToWideChar(CP_ACP, 0, text, -1, wBuffer, 512);

	DWORD textWidth = 0, fileNo = 0;
	DWORD originalSize = D2WIN_SetTextSize(font);
	D2WIN_GetTextWidthFileNo(wBuffer, &textWidth, &fileNo);
	D2WIN_SetTextSize(originalSize);

	return textWidth;
}

BOOL RectsOverlap(const RECT &r1, const RECT &r2)
{
	return (r1.left < r2.right && r1.right > r2.left &&
			r1.top < r2.bottom && r1.bottom > r2.top);
}

BOOL IsMercClassId(DWORD dwClassID)
{
	return dwClassID == MERC_A1 || dwClassID == MERC_A2 || dwClassID == MERC_A3 || dwClassID == MERC_A4LIGHT || dwClassID == MERC_A4DARK || dwClassID == MERC_A5 || dwClassID == MERC_A5B;
}

LPUNITANY FindMercUnit(LPUNITANY pOwner)
{
	for (LPROOM1 pRoom1 = Me->pAct->pRoom1; pRoom1; pRoom1 = pRoom1->pRoomNext)
		for (LPUNITANY pUnit = pRoom1->pUnitFirst; pUnit; pUnit = pUnit->pListNext)
			if (IsMercClassId(pUnit->dwTxtFileNo) && D2CLIENT_GetMonsterOwner(pUnit->dwUnitId) == pOwner->dwUnitId)
				return pUnit;

	return NULL;
}

void WorldToScreen(POINT *pPos)
{
	D2COMMON_MapToAbsScreen(&pPos->x, &pPos->y);
	pPos->x -= D2CLIENT_GetMouseXOffset();
	pPos->y -= D2CLIENT_GetMouseYOffset();
}

void ScreenToWorld(POINT *pPos)
{
	D2COMMON_AbsScreenToMap(&pPos->x, &pPos->y);
	pPos->x += D2CLIENT_GetMouseXOffset();
	pPos->y += D2CLIENT_GetMouseYOffset();
}

BOOL IsVendor(LPUNITANY pUnit)
{
	if (!pUnit || pUnit->dwType != UNIT_TYPE_NPC)
	{
		return false;
	}

	switch (pUnit->dwTxtFileNo)
	{
	case NPCID_Cain1:
	case NPCID_Kashya:
	case NPCID_Akara:
	case NPCID_Warriv1:
	case NPCID_Gheed:
	case NPCID_Charsi:
	case NPCID_Greiz:
	case NPCID_Elzix:
	case NPCID_Drognan:
	case NPCID_Fara:
	case NPCID_Lysander:
	case NPCID_Cain2:
	case NPCID_Meshif1:
	case NPCID_Cain3:
	case NPCID_Hratli:
	case NPCID_Alkor:
	case NPCID_Ormus:
	case NPCID_Asheara:
	case NPCID_Cain4:
	case NPCID_Tyrael:
	case NPCID_Jamella:
	case NPCID_Halbu:
	case NPCID_Larzuk:
	case NPCID_Cain5:
	case NPCID_QualKehk:
	case NPCID_Malah:
	case NPCID_Anya:
		return true;
	default:
		return false;
	}
}

// Helper function to create a clean, lowercase version of an item name for searching.
// This removes Diablo II color codes (e.g., ÿc1) from the string.
void CreateCleanItemName(const char* input, char* output, size_t outSize)
{
	size_t j = 0;
	for (size_t i = 0; input[i] != '\0' && j < outSize - 1; ++i)
	{
		// Color codes start with 0xFF (ÿ)
		if ((unsigned char)input[i] == 0xFF)
		{
			// Most color codes are followed by a digit and another character.
			i += 2;
		}
		else
		{
			output[j++] = tolower(input[i]);
		}
	}
	output[j] = '\0';
}