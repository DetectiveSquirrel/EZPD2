#include "Hack.h"

Reveal::Reveal()
{
	m_LastLevel = 0;
}

Reveal::~Reveal()
{
}

static DWORD PreviousLevel = 0;

VOID Reveal::RevealAutomap()
{
	if (!V_MapHackEnabled)
		return;

	Level *CurrentLevel = GetUnitLevel(Me);
	if (!CurrentLevel)
		return;

		// exclude 3 rathma zones and both pvp areas
	if (CurrentLevel->dwLevelNo == 161 ||
		CurrentLevel->dwLevelNo == 162 ||
		CurrentLevel->dwLevelNo == 163 ||
		CurrentLevel->dwLevelNo == 157||
		CurrentLevel->dwLevelNo == 159)
		return;

	if (!Me || !Me->pPath || !Me->pPath->pRoom1)
		return;

	Act *pAct = Me->pAct;
	if (!pAct || !pAct->pMisc)
		return;

	if (CurrentLevel->dwLevelNo != PreviousLevel)
	{
		PreviousLevel = CurrentLevel->dwLevelNo;
		ClearTrackedPresetUnits();
		DestroyLevelExits();

		LPLEVEL pLevel = GetLevel(pAct->pMisc, CurrentLevel->dwLevelNo);

		if (!pLevel)
			return;

		if (!pLevel->pRoom2First)
		{
			D2COMMON_InitLevel(pLevel);
		}

		InitAutomapLayer(CurrentLevel->dwLevelNo);
		RevealLevel(pLevel);
		if (CreateCollisionMap())
		{
			AddBoundaryLevelExits();
		}
	}

	if (!Me->pPath->pRoom1 || !Me->pPath->pRoom1->pRoom2 || !Me->pPath->pRoom1->pRoom2->pLevel)
		return;

	InitAutomapLayer(Me->pPath->pRoom1->pRoom2->pLevel->dwLevelNo);
}

VOID Reveal::RevealLevel(LPLEVEL pLevel)
{
	if (!pLevel)
		return;

	for (LPROOM2 pRoom2 = pLevel->pRoom2First; pRoom2; pRoom2 = pRoom2->pRoom2Next)
	{
		if (!pRoom2)
			continue;
		RevealRoom(pRoom2);
	}
}

VOID Reveal::RevealRoom(LPROOM2 pRoom2)
{
	if (!pRoom2)
		return;

	BOOL bAdded = FALSE;

	if (pRoom2->dwPresetType == 2)
	{
		if (!pRoom2->pType2Info)
			return;

		DWORD dwPresetNo = *pRoom2->pType2Info;

		if (dwPresetNo == 38 || dwPresetNo == 39)
			return;
		if (dwPresetNo >= 401 && dwPresetNo <= 405)
			return;
		if (dwPresetNo == 836 || dwPresetNo == 863)
			return;
	}

	if (!pRoom2->pRoom1)
	{
		AddAutomapRoom(pRoom2);
		bAdded = TRUE;
	}

	RevealRoom1(pRoom2);

	if (bAdded)
		RemoveAutomapRoom(pRoom2);
}

VOID Reveal::RevealRoom1(LPROOM2 pRoom)
{
	if (!pRoom || !pRoom->pRoom1)
		return;

	D2CLIENT_RevealAutomapRoom(pRoom->pRoom1, 1, *p_D2CLIENT_AutomapLayer);
	Level *CurrentLevel = GetUnitLevel(Me);
	if (!CurrentLevel)
		return;

	for (LPPRESETUNIT pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
	{
		if (!pUnit)
			continue;

		INT unitWorldX = (pRoom->dwPosX * 5) + pUnit->dwPosX;
		INT unitWorldY = (pRoom->dwPosY * 5) + pUnit->dwPosY;

		if (pUnit->dwType == UNIT_TYPE_TILE)
		{
			DWORD dwTargetLevel = GetTileLevelNo(pRoom, pUnit->dwTxtFileNo);
			if (dwTargetLevel)
			{
				BOOL bAlreadyExists = FALSE;
				for (INT i = 0; i < m_LevelExits.GetSize(); i++)
				{
					INT xDiff = m_LevelExits[i]->ptPos.x - (unitWorldX - (8 << 1));
					INT yDiff = m_LevelExits[i]->ptPos.y - (unitWorldY - 10);

					// Increase/Decrease number if detection is off
					if (m_LevelExits[i]->dwTargetLevelNo == dwTargetLevel &&
						xDiff >= -70 && xDiff <= 70 &&
						yDiff >= -70 && yDiff <= 70)
					{
						bAlreadyExists = TRUE;
						break;
					}
				}

				if (bAlreadyExists)
				{
					continue;
				}

				LPCAVEDESC LevelDesc = new CAVEDESC;
				CHAR szLevel[0x40] = "";

				::memset(LevelDesc, 0, sizeof(CAVEDESC));
				GetMapName((INT)dwTargetLevel, szLevel, 0x40);

				sprintf_s(LevelDesc->szName, sizeof(LevelDesc->szName), "%s", szLevel);
				LevelDesc->ptPos.x = unitWorldX - (8 << 1);
				LevelDesc->ptPos.y = unitWorldY - 10;
				LevelDesc->dwAct = Me->pAct->dwAct;
				LevelDesc->dwLevelNo = pRoom->pLevel->dwLevelNo;
				LevelDesc->dwTargetLevelNo = dwTargetLevel;

				// If we are in canyon of magi, only show the level name if it is the staff tomb level
				if (pRoom->pLevel->dwLevelNo == MAP_A2_CANYON_OF_THE_MAGI)
				{
					if (dwTargetLevel == pRoom->pLevel->pMisc->dwStaffTombLevel)
					{
						strcpy_s(LevelDesc->szName, sizeof(LevelDesc->szName), "True Tomb");
					}
					else
					{
						delete LevelDesc;
						continue;
					}
				}

				m_LevelExits.Add(LevelDesc);
			}
		}

		const char *specificUnitName = nullptr;
		bool isTrackableUnit = false;

		if (pUnit->dwType == UNIT_TYPE_NPC)
		{
			if (CurrentLevel->dwLevelNo < 132)
			{
				for (size_t i = 0; i < sizeof(trackedCampaignMonsters) / sizeof(trackedCampaignMonsters[0]); ++i)
				{
					if (pUnit->dwTxtFileNo == trackedCampaignMonsters[i].dwClassId)
					{
						isTrackableUnit = true;
						specificUnitName = trackedCampaignMonsters[i].szName;
						break;
					}
				}
			}
			else if (CurrentLevel->dwLevelNo != 195 && CurrentLevel->dwLevelNo != 196)
			{
				for (size_t i = 0; i < sizeof(trackedMapMonsters) / sizeof(trackedMapMonsters[0]); ++i)
				{
					if (pUnit->dwTxtFileNo == trackedMapMonsters[i].dwClassId)
					{
						isTrackableUnit = true;
						specificUnitName = trackedMapMonsters[i].szName;
						break;
					}
				}
			}
		}

		else if (pUnit->dwType == UNIT_TYPE_OBJECT && CurrentLevel->dwLevelNo < 132)
		{
			for (size_t i = 0; i < sizeof(trackedCampaignObjects) / sizeof(trackedCampaignObjects[0]); ++i)
			{
				if (pUnit->dwTxtFileNo == trackedCampaignObjects[i].dwClassId)
				{
					isTrackableUnit = true;
					specificUnitName = trackedCampaignObjects[i].szName;
					break;
				}
			}
		}

		if (isTrackableUnit)
		{
			BOOL bAlreadyTracked = FALSE;
			for (INT k = 0; k < m_TrackedPresetUnits.GetSize(); k++)
			{
				if (m_TrackedPresetUnits[k].ptPos.x == unitWorldX &&
					m_TrackedPresetUnits[k].ptPos.y == unitWorldY &&
					m_TrackedPresetUnits[k].dwClassId == pUnit->dwTxtFileNo &&
					m_TrackedPresetUnits[k].dwUnitType == pUnit->dwType)
				{
					bAlreadyTracked = TRUE;
					break;
				}
			}

			if (!bAlreadyTracked)
			{
				PRESETUNITINFO unitInfo;
				::memset(&unitInfo, 0, sizeof(PRESETUNITINFO));
				unitInfo.dwClassId = pUnit->dwTxtFileNo;
				unitInfo.dwUnitType = pUnit->dwType;
				unitInfo.ptPos.x = unitWorldX;
				unitInfo.ptPos.y = unitWorldY;
				unitInfo.dwLevelNo = pRoom->pLevel->dwLevelNo;

				if (specificUnitName)
				{
					strncpy_s(unitInfo.szName, sizeof(unitInfo.szName), specificUnitName, _TRUNCATE);
				}

				m_TrackedPresetUnits.Add(unitInfo);
			}
		}
	}
}

VOID Reveal::AddRoomCell(INT xPos, INT yPos, INT nCell, LPROOM2 pRoom)
{
	if (nCell == 376 && pRoom->pLevel->dwLevelNo == MAP_A4_RIVER_OF_FLAME)
		return;

	LPAUTOMAPCELL pCell = D2CLIENT_NewAutomapCell();

	xPos += pRoom->dwPosX * 5;
	yPos += pRoom->dwPosY * 5;

	pCell->nCellNo = nCell;
	pCell->xPixel = (((xPos - yPos) * 16) / 10) + 1;
	pCell->yPixel = (((xPos + yPos) * 8) / 10) - 3;
	D2CLIENT_AddAutomapCell(pCell, &((*p_D2CLIENT_AutomapLayer)->pObjects));
}

VOID Reveal::AddAutomapRoom(LPROOM2 pRoom2)
{
	LPUNITANY pUnit = *p_D2CLIENT_PlayerUnit;
	D2COMMON_AddRoomData(pRoom2->pLevel->pMisc->pAct,
						 pRoom2->pLevel->dwLevelNo,
						 pRoom2->dwPosX,
						 pRoom2->dwPosY,
						 pUnit->pPath->pRoom1);
}

VOID Reveal::RemoveAutomapRoom(LPROOM2 pRoom2)
{
	LPUNITANY pUnit = *p_D2CLIENT_PlayerUnit;
	D2COMMON_RemoveRoomData(pRoom2->pLevel->pMisc->pAct,
							pRoom2->pLevel->dwLevelNo,
							pRoom2->dwPosX,
							pRoom2->dwPosY,
							pUnit->pPath->pRoom1);
}

BOOL Reveal::GetCaveExits(LPCAVEDESC *lpLevel, INT nMaxExits)
{
	INT nTotal = 0;

	for (INT i = 0; i < m_LevelExits.GetSize(); i++)
	{
		if (nTotal > nMaxExits)
			return FALSE;

		lpLevel[nTotal] = m_LevelExits[i];
		nTotal++;
	}

	return nTotal;
}

DWORD Reveal::GetTileLevelNo(LPROOM2 lpRoom2, DWORD dwTileNo)
{
	if (!lpRoom2)
		return NULL;

	for (LPROOMTILE pRoomTile = lpRoom2->pRoomTiles; pRoomTile; pRoomTile = pRoomTile->pNext)
	{
		if (!pRoomTile || !pRoomTile->nNum)
			continue;

		if (*(pRoomTile->nNum) == dwTileNo)
		{
			if (!pRoomTile->pRoom2 || !pRoomTile->pRoom2->pLevel)
				continue;

			return pRoomTile->pRoom2->pLevel->dwLevelNo;
		}
	}

	return NULL;
}

LPLEVEL Reveal::GetLevel(LPACTMISC pMisc, DWORD dwLevelNo)
{
	for (LPLEVEL pLevel = pMisc->pLevelFirst; pLevel; pLevel = pLevel->pNextLevel)
		if (pLevel->dwLevelNo == dwLevelNo)
			return pLevel;

	return D2COMMON_GetLevel(pMisc, dwLevelNo);
}

LPAUTOMAPLAYER2 Reveal::InitAutomapLayer(DWORD levelno)
{
	LPAUTOMAPLAYER2 pLayer = D2COMMON_GetLayer(levelno);
	return (LPAUTOMAPLAYER2)InitAutomapLayerSTUB(pLayer->nLayerNo);
}

BOOL Reveal::CreateCollisionMap()
{
	if (!Me)
		return FALSE;

	if (!Me->pPath || !Me->pPath->pRoom1)
		return FALSE;

	if (!Me->pPath->pRoom1->pRoom2)
		return FALSE;

	LPLEVEL pLevel = Me->pPath->pRoom1->pRoom2->pLevel;
	if (!pLevel)
		return FALSE;

	// If the level is the same as last time, we already have a collision map
	if (pLevel->dwLevelNo == m_LastLevel)
		return TRUE;

	// Destroy the old map if it exists
	if (m_Map.IsCreated())
		m_Map.Destroy();

	m_LastLevel = pLevel->dwLevelNo;
	m_LevelOrigin.x = pLevel->dwPosX * 5;
	m_LevelOrigin.y = pLevel->dwPosY * 5;
	m_SizeX = pLevel->dwSizeX * 5;
	m_SizeY = pLevel->dwSizeY * 5;

	// Check for valid size
	if (m_SizeX <= 0 || m_SizeY <= 0 || m_SizeX > 10000 || m_SizeY > 10000)
		return FALSE;

	m_Map.Lock();
	if (!m_Map.Create(pLevel->dwSizeX * 5, pLevel->dwSizeY * 5, MAP_DATA_NULL))
	{
		m_Map.Unlock();
		return FALSE;
	}

	CArrayEx<DWORD, DWORD> aSkip;
	AddCollisionData(Me->pPath->pRoom1->pRoom2, aSkip);

	FillGaps();
	FillGaps();

	m_Map.Unlock();
	return TRUE;
}

BOOL Reveal::IsGap(INT nX, INT nY)
{
	if (m_Map[nX][nY] % 2)
		return FALSE;

	INT nSpaces = 0;
	INT i = 0;

	for (i = nX - 2; i <= nX + 2 && nSpaces < 3; i++)
	{
		if (i < 0 || i >= m_Map.GetCX() || (m_Map[i][nY] % 2))
			nSpaces = 0;
		else
			nSpaces++;
	}

	if (nSpaces < 3)
		return TRUE;

	nSpaces = 0;
	for (i = nY - 2; i <= nY + 2 && nSpaces < 3; i++)
	{
		if (i < 0 || i >= m_Map.GetCY() || (m_Map[nX][i] % 2))
			nSpaces = 0;
		else
			nSpaces++;
	}

	return nSpaces < 3;
}

BOOL Reveal::FillGaps()
{
	if (!m_Map.IsCreated())
		return FALSE;

	INT nCX = m_Map.GetCX();
	INT nCY = m_Map.GetCY();

	for (INT x = 0; x < nCX; x++)
	{
		for (INT y = 0; y < nCY; y++)
		{
			if (IsGap(x, y))
				m_Map[x][y] = MAP_DATA_FILL;
		}
	}

	return TRUE;
}

BOOL Reveal::AddCollisionData(LPCOLLMAP pColl)
{
	if (!pColl)
		return FALSE;

	INT nX = pColl->dwPosGameX - m_LevelOrigin.x;
	INT nY = pColl->dwPosGameY - m_LevelOrigin.y;
	INT nCX = pColl->dwSizeGameX;
	INT nCY = pColl->dwSizeGameY;

	if (!m_Map.IsValidIndex(nX, nY))
		return FALSE;

	INT nLimitX = nX + nCX;
	INT nLimitY = nY + nCY;

	LPWORD p = pColl->pMapStart;
	if (!p)
		return FALSE;

	for (INT y = nY; y < nLimitY; y++)
	{
		for (INT x = nX; x < nLimitX; x++)
		{
			m_Map[x][y] = *p;

			if (m_Map[x][y] == 1024)
				m_Map[x][y] = MAP_DATA_AVOID;

			p++;
		}
	}

	return TRUE;
}

BOOL Reveal::AddCollisionData(LPROOM2 pRoom2, CArrayEx<DWORD, DWORD> &aSkip)
{
	if (!pRoom2)
		return FALSE;

	if (pRoom2->pLevel->dwLevelNo != m_LastLevel)
		return FALSE;

	if (aSkip.Find((DWORD)pRoom2) != -1)
		return TRUE;

	aSkip.Add((DWORD)pRoom2);

	BOOL bAdd = FALSE;
	if (!pRoom2->pRoom1)
	{
		AddAutomapRoom(pRoom2);
		bAdd = TRUE;
	}

	if (pRoom2->pRoom1)
		AddCollisionData(pRoom2->pRoom1->Coll);

	LPROOM2 *pRoomNear = pRoom2->pRoom2Near;
	for (DWORD i = 0; i < pRoom2->dwRoomsNear; i++)
		AddCollisionData(pRoomNear[i], aSkip);

	if (bAdd)
		RemoveAutomapRoom(pRoom2);

	return TRUE;
}

BOOL Reveal::GetLevelExits(LPLEVELEXIT *lpLevel, INT nMaxExits)
{
	POINT ptExitPoints[0x40][2];
	INT nTotalPoints = 0, nCurrentExit = 0;

	for (INT i = 0; i < m_Map.GetCX(); i++)
	{
		if (!(m_Map[i][0] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = i;
			ptExitPoints[nTotalPoints][0].y = 0;

			for (i++; i < m_Map.GetCX(); i++)
			{
				if (m_Map[i][0] % 2)
				{
					ptExitPoints[nTotalPoints][1].x = i - 1;
					ptExitPoints[nTotalPoints][1].y = 0;
					break;
				}
			}

			nTotalPoints++;
			break;
		}
	}

	for (INT i = 0; i < m_Map.GetCX(); i++)
	{
		if (!(m_Map[i][m_Map.GetCY() - 1] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = i;
			ptExitPoints[nTotalPoints][0].y = m_Map.GetCY() - 1;

			for (i++; i < m_Map.GetCX(); i++)
			{
				if ((m_Map[i][m_Map.GetCY() - 1] % 2))
				{
					ptExitPoints[nTotalPoints][1].x = i - 1;
					ptExitPoints[nTotalPoints][1].y = m_Map.GetCY() - 1;
					break;
				}
			}

			nTotalPoints++;
			break;
		}
	}

	for (INT i = 0; i < m_Map.GetCY(); i++)
	{
		if (!(m_Map[0][i] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = 0;
			ptExitPoints[nTotalPoints][0].y = i;

			for (i++; i < m_Map.GetCY(); i++)
			{
				if ((m_Map[0][i] % 2))
				{
					ptExitPoints[nTotalPoints][1].x = 0;
					ptExitPoints[nTotalPoints][1].y = i - 1;
					break;
				}
			}

			nTotalPoints++;
			break;
		}
	}

	for (INT i = 0; i < m_Map.GetCY(); i++)
	{
		if (!(m_Map[m_Map.GetCX() - 1][i] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = m_Map.GetCX() - 1;
			ptExitPoints[nTotalPoints][0].y = i;

			for (i++; i < m_Map.GetCY(); i++)
			{
				if ((m_Map[m_Map.GetCX() - 1][i] % 2))
				{
					ptExitPoints[nTotalPoints][1].x = m_Map.GetCX() - 1;
					ptExitPoints[nTotalPoints][1].y = i - 1;
					break;
				}
			}

			nTotalPoints++;
			break;
		}
	}

	LPPOINT ptCenters = new POINT[nTotalPoints];

	for (INT i = 0; i < nTotalPoints; i++)
	{
		INT nXDiff = ptExitPoints[i][1].x - ptExitPoints[i][0].x;
		INT nYDiff = ptExitPoints[i][1].y - ptExitPoints[i][0].y;
		INT nXCenter = 0, nYCenter = 0;

		if (nXDiff > 0)
		{
			if (nXDiff % 2)
				nXCenter = ptExitPoints[i][0].x + ((nXDiff - (nXDiff % 2)) / 2);
			else
				nXCenter = ptExitPoints[i][0].x + (nXDiff / 2);
		}

		if (nYDiff > 0)
		{
			if (nYDiff % 2)
				nYCenter = ptExitPoints[i][0].y + ((nYDiff - (nYDiff % 2)) / 2);
			else
				nYCenter = ptExitPoints[i][0].y + (nYDiff / 2);
		}

		ptCenters[i].x = nXCenter ? nXCenter : ptExitPoints[i][0].x;
		ptCenters[i].y = nYCenter ? nYCenter : ptExitPoints[i][0].y;
	}

	for (LPROOM2 pRoom = GetUnitLevel(Me)->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next)
	{
		LPROOM2 *pNear = pRoom->pRoom2Near;

		for (DWORD i = 0; i < pRoom->dwRoomsNear; i++)
		{
			if (pNear[i]->pLevel->dwLevelNo != GetUnitLevel(Me)->dwLevelNo)
			{
				INT nRoomX = pRoom->dwPosX * 5;
				INT nRoomY = pRoom->dwPosY * 5;

				for (INT j = 0; j < nTotalPoints; j++)
				{
					if ((ptCenters[j].x + m_LevelOrigin.x) >= (WORD)nRoomX && (ptCenters[j].x + m_LevelOrigin.x) <= (WORD)(nRoomX + (pRoom->dwSizeX * 5)))
					{
						if ((ptCenters[j].y + m_LevelOrigin.y) >= (WORD)nRoomY && (ptCenters[j].y + m_LevelOrigin.y) <= (WORD)(nRoomY + (pRoom->dwSizeY * 5)))
						{
							if (nCurrentExit >= nMaxExits)
							{
								delete[] ptCenters;
								return FALSE;
							}

							lpLevel[nCurrentExit] = new LEVELEXIT;
							lpLevel[nCurrentExit]->dwTargetLevel = pNear[i]->pLevel->dwLevelNo;
							lpLevel[nCurrentExit]->ptPos.x = ptCenters[j].x + m_LevelOrigin.x;
							lpLevel[nCurrentExit]->ptPos.y = ptCenters[j].y + m_LevelOrigin.y;
							nCurrentExit++;
						}
					}
				}

				break;
			}
		}

		BOOL bAdded = FALSE;

		if (!pRoom->pRoom1)
		{
			D2COMMON_AddRoomData(Me->pAct, pRoom->pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, Me->pPath->pRoom1);
			bAdded = TRUE;
		}

		for (PresetUnit *pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
		{
			if (nCurrentExit >= nMaxExits)
			{
				if (bAdded)
					D2COMMON_RemoveRoomData(Me->pAct, pRoom->pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, Me->pPath->pRoom1);

				delete[] ptCenters;
				return FALSE;
			}

			if (pUnit->dwType == UNIT_TYPE_TILE)
			{
				DWORD dwTargetLevel = GetTileLevelNo(pRoom, pUnit->dwTxtFileNo);

				if (dwTargetLevel)
				{
					BOOL Exists = FALSE;

					for (INT i = 0; i < nCurrentExit; i++)
					{
						INT xDiff = lpLevel[i]->ptPos.x - ((pRoom->dwPosX * 5) + pUnit->dwPosX);
						INT yDiff = lpLevel[i]->ptPos.y - ((pRoom->dwPosY * 5) + pUnit->dwPosY);

						// Increase/Decrease number if detection is off
						if (xDiff >= -70 && xDiff <= 70 &&
							yDiff >= -70 && yDiff <= 70)
						{
							Exists = TRUE;
							break;
						}
					}

					if (!Exists)
					{
						lpLevel[nCurrentExit] = new LEVELEXIT;
						lpLevel[nCurrentExit]->dwTargetLevel = dwTargetLevel;
						lpLevel[nCurrentExit]->ptPos.x = (pRoom->dwPosX * 5) + pUnit->dwPosX;
						lpLevel[nCurrentExit]->ptPos.y = (pRoom->dwPosY * 5) + pUnit->dwPosY;
						nCurrentExit++;
					}
				}
			}
		}

		if (bAdded)
			D2COMMON_RemoveRoomData(Me->pAct, pRoom->pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, Me->pPath->pRoom1);
	}

	delete[] ptCenters;
	return TRUE;
}

VOID Reveal::DestroyLevelExits()
{
	if (!m_LevelExits.IsEmpty())
	{
		for (INT i = 0; i < m_LevelExits.GetSize(); i++)
		{
			if (m_LevelExits[i])
				delete m_LevelExits[i];
		}

		m_LevelExits.RemoveAll();
	}
}

WORD Reveal::GetCollisionInfo(INT nX, INT nY)
{
	nX -= m_LevelOrigin.x;
	nY -= m_LevelOrigin.y;

	if (!m_Map.IsValidIndex(nX, nY))
		return NULL;

	return m_Map[nX][nY];
}

BOOL Reveal::ExportCollisionMap(CMatrix<WORD, WORD> &cMatrix)
{
	m_Map.Lock();
	m_Map.ExportData(cMatrix);
	m_Map.Unlock();

	return TRUE;
}

VOID Reveal::ClearTrackedPresetUnits()
{
	if (!m_TrackedPresetUnits.IsEmpty())
	{
		m_TrackedPresetUnits.RemoveAll();
	}
}

BOOL Reveal::GetTrackedPresetUnits(CArrayEx<PRESETUNITINFO, PRESETUNITINFO &> &outArray)
{
	outArray.RemoveAll();
	for (INT i = 0; i < m_TrackedPresetUnits.GetSize(); i++)
	{
		outArray.Add(m_TrackedPresetUnits[i]);
	}
	return TRUE;
}

VOID Reveal::AddBoundaryLevelExits()
{
	if (!Me || !m_Map.IsCreated())
		return;

	LPLEVEL pCurrentLevel = GetUnitLevel(Me);
	if (!pCurrentLevel)
		return;

	POINT ptExitPoints[0x40][2];
	INT nTotalPoints = 0;

	for (INT i = 0; i < m_Map.GetCX() && nTotalPoints < 0x40; i++)
	{
		if (!(m_Map[i][0] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = i;
			ptExitPoints[nTotalPoints][0].y = 0;
			for (i++; i < m_Map.GetCX(); i++)
			{
				if (m_Map[i][0] % 2)
				{
					ptExitPoints[nTotalPoints][1].x = i - 1;
					ptExitPoints[nTotalPoints][1].y = 0;
					break;
				}
			}
			if (ptExitPoints[nTotalPoints][1].x == 0 && ptExitPoints[nTotalPoints][0].x != 0)
			{
				ptExitPoints[nTotalPoints][1].x = m_Map.GetCX() - 1;
				ptExitPoints[nTotalPoints][1].y = 0;
			}
			nTotalPoints++;
			if (i >= m_Map.GetCX() && ptExitPoints[nTotalPoints - 1][1].x == 0)
			{
				ptExitPoints[nTotalPoints - 1][1].x = m_Map.GetCX() - 1;
				ptExitPoints[nTotalPoints - 1][1].y = 0;
			}
		}
	}

	for (INT i = 0; i < m_Map.GetCX() && nTotalPoints < 0x40; i++)
	{
		if (!(m_Map[i][m_Map.GetCY() - 1] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = i;
			ptExitPoints[nTotalPoints][0].y = m_Map.GetCY() - 1;
			for (i++; i < m_Map.GetCX(); i++)
			{
				if (m_Map[i][m_Map.GetCY() - 1] % 2)
				{
					ptExitPoints[nTotalPoints][1].x = i - 1;
					ptExitPoints[nTotalPoints][1].y = m_Map.GetCY() - 1;
					break;
				}
			}
			if (ptExitPoints[nTotalPoints][1].x == 0 && ptExitPoints[nTotalPoints][0].x != 0)
			{
				ptExitPoints[nTotalPoints][1].x = m_Map.GetCX() - 1;
				ptExitPoints[nTotalPoints][1].y = m_Map.GetCY() - 1;
			}
			nTotalPoints++;
			if (i >= m_Map.GetCX() && ptExitPoints[nTotalPoints - 1][1].x == 0)
			{
				ptExitPoints[nTotalPoints - 1][1].x = m_Map.GetCX() - 1;
				ptExitPoints[nTotalPoints - 1][1].y = m_Map.GetCY() - 1;
			}
		}
	}

	for (INT i = 0; i < m_Map.GetCY() && nTotalPoints < 0x40; i++)
	{
		if (!(m_Map[0][i] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = 0;
			ptExitPoints[nTotalPoints][0].y = i;
			for (i++; i < m_Map.GetCY(); i++)
			{
				if (m_Map[0][i] % 2)
				{
					ptExitPoints[nTotalPoints][1].x = 0;
					ptExitPoints[nTotalPoints][1].y = i - 1;
					break;
				}
			}
			if (ptExitPoints[nTotalPoints][1].y == 0 && ptExitPoints[nTotalPoints][0].y != 0)
			{
				ptExitPoints[nTotalPoints][1].x = 0;
				ptExitPoints[nTotalPoints][1].y = m_Map.GetCY() - 1;
			}
			nTotalPoints++;
			if (i >= m_Map.GetCY() && ptExitPoints[nTotalPoints - 1][1].y == 0)
			{
				ptExitPoints[nTotalPoints - 1][1].x = 0;
				ptExitPoints[nTotalPoints - 1][1].y = m_Map.GetCY() - 1;
			}
		}
	}

	for (INT i = 0; i < m_Map.GetCY() && nTotalPoints < 0x40; i++)
	{
		if (!(m_Map[m_Map.GetCX() - 1][i] % 2))
		{
			ptExitPoints[nTotalPoints][0].x = m_Map.GetCX() - 1;
			ptExitPoints[nTotalPoints][0].y = i;
			for (i++; i < m_Map.GetCY(); i++)
			{
				if (m_Map[m_Map.GetCX() - 1][i] % 2)
				{
					ptExitPoints[nTotalPoints][1].x = m_Map.GetCX() - 1;
					ptExitPoints[nTotalPoints][1].y = i - 1;
					break;
				}
			}
			if (ptExitPoints[nTotalPoints][1].y == 0 && ptExitPoints[nTotalPoints][0].y != 0)
			{
				ptExitPoints[nTotalPoints][1].x = m_Map.GetCX() - 1;
				ptExitPoints[nTotalPoints][1].y = m_Map.GetCY() - 1;
			}
			nTotalPoints++;
			if (i >= m_Map.GetCY() && ptExitPoints[nTotalPoints - 1][1].y == 0)
			{
				ptExitPoints[nTotalPoints - 1][1].x = m_Map.GetCX() - 1;
				ptExitPoints[nTotalPoints - 1][1].y = m_Map.GetCY() - 1;
			}
		}
	}

	if (nTotalPoints == 0)
		return;

	POINT *ptCenters = new POINT[nTotalPoints];
	for (INT i = 0; i < nTotalPoints; i++)
	{
		INT nXDiff = ptExitPoints[i][1].x - ptExitPoints[i][0].x;
		INT nYDiff = ptExitPoints[i][1].y - ptExitPoints[i][0].y;
		ptCenters[i].x = ptExitPoints[i][0].x + (nXDiff / 2);
		ptCenters[i].y = ptExitPoints[i][0].y + (nYDiff / 2);
	}

	for (LPROOM2 pRoom = pCurrentLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next)
	{
		if (!pRoom)
			continue;

		BOOL bRoomAddedForAutomap = FALSE;
		if (!pRoom->pRoom1)
		{
			AddAutomapRoom(pRoom);
			bRoomAddedForAutomap = TRUE;
		}

		LPROOM2 *pNear = pRoom->pRoom2Near;
		if (!pNear)
		{
			if (bRoomAddedForAutomap)
				RemoveAutomapRoom(pRoom);
			continue;
		}

		for (DWORD i = 0; i < pRoom->dwRoomsNear; i++)
		{
			if (pNear[i] && pNear[i]->pLevel && pNear[i]->pLevel->dwLevelNo != pCurrentLevel->dwLevelNo)
			{
				INT nRoomX_map = pRoom->dwPosX * 5 - m_LevelOrigin.x;
				INT nRoomY_map = pRoom->dwPosY * 5 - m_LevelOrigin.y;
				INT nRoomCX_map = pRoom->dwSizeX * 5;
				INT nRoomCY_map = pRoom->dwSizeY * 5;

				for (INT j = 0; j < nTotalPoints; j++)
				{
					if (ptCenters[j].x >= nRoomX_map && ptCenters[j].x < (nRoomX_map + nRoomCX_map) &&
						ptCenters[j].y >= nRoomY_map && ptCenters[j].y < (nRoomY_map + nRoomCY_map))
					{
						POINT worldPos;
						worldPos.x = ptCenters[j].x + m_LevelOrigin.x;
						worldPos.y = ptCenters[j].y + m_LevelOrigin.y;
						DWORD dwTargetLevel = pNear[i]->pLevel->dwLevelNo;

						// Check for duplicates before adding
						BOOL bExists = FALSE;
						for (INT k = 0; k < m_LevelExits.GetSize(); k++)
						{
							INT xDiff = m_LevelExits[k]->ptPos.x - (worldPos.x - (8 << 1));
							INT yDiff = m_LevelExits[k]->ptPos.y - (worldPos.y - 10);

							// Increase/Decrease number if detection is off
							if (m_LevelExits[k]->dwTargetLevelNo == dwTargetLevel &&
								xDiff >= -70 && xDiff <= 70 &&
								yDiff >= -70 && yDiff <= 70)
							{
								bExists = TRUE;
								break;
							}
						}

						if (!bExists)
						{
							LPCAVEDESC LevelDesc = new CAVEDESC;
							CHAR szLevelName[0x40] = "";
							::memset(LevelDesc, 0, sizeof(CAVEDESC));

							GetMapName((BYTE)dwTargetLevel, szLevelName, 0x40);
							sprintf_s(LevelDesc->szName, sizeof(LevelDesc->szName), "%s", szLevelName);
							LevelDesc->ptPos.x = worldPos.x - (8 << 1);
							LevelDesc->ptPos.y = worldPos.y - 10;
							LevelDesc->dwAct = Me->pAct->dwAct;
							LevelDesc->dwLevelNo = pCurrentLevel->dwLevelNo;
							LevelDesc->dwTargetLevelNo = dwTargetLevel;

							if (pNear[i]->pLevel->pMisc && dwTargetLevel == pNear[i]->pLevel->pMisc->dwStaffTombLevel)
							{
								strcat_s(LevelDesc->szName, sizeof(LevelDesc->szName), " (Staff Tomb)");
							}

							m_LevelExits.Add(LevelDesc);
							break;
						}
					}
				}
				if (pNear[i] && pNear[i]->pLevel && pNear[i]->pLevel->dwLevelNo != pCurrentLevel->dwLevelNo && m_LevelExits.GetSize() > 0 && m_LevelExits[m_LevelExits.GetSize() - 1]->dwTargetLevelNo == pNear[i]->pLevel->dwLevelNo)
				{
					break;
				}
			}
		}
		if (bRoomAddedForAutomap)
		{
			RemoveAutomapRoom(pRoom);
		}
	}
	delete[] ptCenters;
}
