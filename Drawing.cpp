#include "Hack.h"
#include <wchar.h>
#include <excpt.h>
#include <string.h>

// -1: not centered (text starts at X).
//  1: centered (text is centered on X).
VOID DrawTextB(INT X, INT Y, DWORD Color, INT Font, INT Center, LPSTR Text, ...)
{
	POINT nPos = {X, Y};
	DWORD dwOldSize, wWidth, dwFileNo;

	CHAR szBuffer[800] = "";
	va_list Args;
	va_start(Args, Text);
	vsprintf_s(szBuffer, sizeof(szBuffer), Text, Args);
	va_end(Args);

	WCHAR wBuffer[0x130];
	MultiByteToWideChar(0, 1, szBuffer, -1, wBuffer, 0x130);

	dwOldSize = D2WIN_SetTextSize(Font);

	if (Center != -1)
	{
		D2WIN_GetTextWidthFileNo(wBuffer, &wWidth, &dwFileNo);
		nPos.x -= (wWidth >> Center);
	}

	D2WIN_DrawText(wBuffer, nPos.x, nPos.y, Color, -1);
	D2WIN_SetTextSize(dwOldSize);
}

VOID DrawCross(INT X, INT Y, DWORD Color)
{
	POINT Position = {X, Y};
	CHAR Lines[][2] = {0, -2, 4, -4, 8, -2, 4, 0, 8, 2, 4, 4, 0, 2, -4, 4, -8, 2, -4, 0, -8, -2, -4, -4, 0, -2};

	for (INT i = 0; i < ArraySize(Lines) - 1; i++)
		D2GFX_DrawLine(Position.x + Lines[i][0], Position.y + Lines[i][1], Position.x + Lines[i + 1][0], Position.y + Lines[i + 1][1], Color, -1);
}

static DWORD GetMarkerTextColor(DWORD Color)
{
	switch (Color)
	{
	case CROSSCOLOR_ACTUAL_BOSS:
		return FONTCOLOR_PINK;
	case CROSSCOLOR_BOSS:
		return FONTCOLOR_ORANGE;
	case CROSSCOLOR_CHAMP:
		return FONTCOLOR_BLUE;
	case CROSSCOLOR_MINION:
		return FONTCOLOR_YELLOW;
	case CROSSCOLOR_IMPORTANT:
		return FONTCOLOR_LIGHTGREEN;
	case CROSSCOLOR_NORMAL:
	default:
		return FONTCOLOR_DARKORANGE;
	}
}

static VOID DrawTextMarker(INT X, INT Y, DWORD Color, DWORD Font)
{
	DrawTextB(X, Y + 4, GetMarkerTextColor(Color), Font, 1, "x");
}

static VOID DrawMonsterMarker(INT X, INT Y, DWORD Color, DWORD Style, DWORD Font)
{
	if (Style == MONSTER_MARKER_STYLE_CROSS)
	{
		DrawCross(X, Y, Color);

		return;
	}

	DrawTextMarker(X, Y, Color, Font);
}

static BOOL IsOwnedByPlayer(LPUNITANY Unit)
{
	if (!Unit || !Me)
		return FALSE;

	if (Unit->dwOwnerId == Me->dwUnitId)
		return TRUE;

	return D2CLIENT_GetMonsterOwner(Unit->dwUnitId) == Me->dwUnitId;
}

static POINT GetPlayerAutomapPosition()
{
	POINT playerPos = {D2CLIENT_GetUnitX(Me), D2CLIENT_GetUnitY(Me)};
	ScreenToAutomap(&playerPos, playerPos.x * 32, playerPos.y * 32);

	return playerPos;
}

static VOID DrawLineFromPlayerToAutomapPoint(POINT playerPos, POINT targetPos, DWORD Color)
{
	if (!V_DrawPOILinesToTarget)
		targetPos = AdjustAutomapLabelPosition(targetPos);

	D2GFX_DrawLine(playerPos.x, playerPos.y, targetPos.x, targetPos.y, Color, -1);
}

static DWORD GetExitLineColor(LPCAVEDESC exitInfo, DWORD currentZone)
{
	if (!exitInfo)
		return 0x99;

	if (!strcmp(exitInfo->szName, "True Tomb"))
		return 0x69;

	return exitInfo->dwTargetLevelNo < currentZone ? 10 : 0x99;
}

static DWORD GetTrackedPresetLineColor(PRESETUNITINFO &unitInfo)
{
	return !strcmp(unitInfo.szName, "Waypoint") ? 0x7D : 0xA8;
}

static POINT GetExitLineWorldPosition(LPCAVEDESC exitInfo)
{
	if (exitInfo && (exitInfo->ptTargetPos.x || exitInfo->ptTargetPos.y))
		return exitInfo->ptTargetPos;

	return exitInfo->ptPos;
}

VOID DrawBox(INT X1, INT Y1, INT X2, INT Y2, INT LineColor)
{
	D2GFX_DrawLine(X1, Y1, X2, Y1, LineColor, -1);
	D2GFX_DrawLine(X1, Y2, X1, Y1, LineColor, -1);
	D2GFX_DrawLine(X2, Y2, X1, Y2, LineColor, -1);
	D2GFX_DrawLine(X2, Y1, X2, Y2, LineColor, -1);
}

VOID DrawCheckBox(INT X, INT Y, INT Size, INT FontSize, BOOL Checked, DWORD BoxColor, DWORD TextColor, LPSTR Text, ...)
{
	DrawBox(X, Y, X + Size, Y + Size, BoxColor);

	if (Checked)
	{
		D2GFX_DrawLine(X + 2, Y + 2, X + Size - 2, Y + Size - 2, BoxColor, -1);
		D2GFX_DrawLine(X + Size - 2, Y + 2, X + 2, Y + Size - 2, BoxColor, -1);
	}

	CHAR szBuffer[800] = "";
	va_list Args;
	va_start(Args, Text);
	vsprintf_s(szBuffer, sizeof(szBuffer), Text, Args);
	va_end(Args);

	DrawTextB(X + Size * 1.5, Y + Size, TextColor, FontSize, -1, "%s", szBuffer);
}

VOID DrawIncreaseDecrease(INT X, INT Y, BOOL Increase, DWORD BoxColor)
{
	DWORD boxSize = 10;

	DrawBox(X, Y, X + boxSize, Y + boxSize, BoxColor);

	if (Increase)
	{
		D2GFX_DrawLine(X + 1, Y + (boxSize / 2), X + boxSize - 1, Y + (boxSize / 2), BoxColor, -1);
		D2GFX_DrawLine(X + (boxSize / 2), Y + 1, X + (boxSize / 2), Y + boxSize - 1, BoxColor, -1);
	}
	else
	{
		D2GFX_DrawLine(X + 1, Y + (boxSize / 2), X + boxSize - 1, Y + (boxSize / 2), BoxColor, -1);
	}
}

VOID DrawTrackedEntitiesLabels()
{
	if (!V_MapHackEnabled || !V_Reveal || !*p_D2CLIENT_AutomapOn || IsFullScreenPanelOpen() || V_MainMenuOpen)
		return;

	DWORD CurrentZone = GetPlayerArea();
	if (CurrentZone == 188 || CurrentZone == 185) // Lucion / Uber Tristram
		return;

	std::vector<RECT> drawnLabelRects;
	const RECT drawingZone = GetSafeScreenAreaRect();
	const POINT playerPos = GetPlayerAutomapPosition();
	const int FONT_SIZE = 15;
	const int LABEL_HEIGHT = 15;

	if (V_ShowZoneTransitions)
	{
		for (size_t i = 0; i < V_Reveal->m_LevelExits.GetSize(); ++i)
		{
			LPCAVEDESC pExitInfo = V_Reveal->m_LevelExits[i];
			POINT initialPos = {pExitInfo->ptPos.x, pExitInfo->ptPos.y};
			POINT linePos = GetExitLineWorldPosition(pExitInfo);

			ScreenToAutomap(&linePos, linePos.x * 32, linePos.y * 32);
			DrawLineFromPlayerToAutomapPoint(playerPos, linePos, GetExitLineColor(pExitInfo, CurrentZone));
			ScreenToAutomap(&initialPos, pExitInfo->ptPos.x * 32, pExitInfo->ptPos.y * 32);
			initialPos = AdjustAutomapLabelPosition(initialPos);

			DWORD textWidth = GetTextWidth(pExitInfo->szName, FONT_SIZE);
			RECT labelRect;
			POINT finalPos = initialPos;
			bool spotFound = false;

			// Try pushing down
			for (int tries = 0; tries < 20 && !spotFound; ++tries)
			{
				labelRect.left = finalPos.x - ((LONG)textWidth / 2);
				labelRect.top = finalPos.y;
				labelRect.right = finalPos.x + ((LONG)textWidth / 2);
				labelRect.bottom = finalPos.y + LABEL_HEIGHT;

				if (labelRect.bottom > drawingZone.bottom)
					break;

				bool collision = false;
				for (size_t j = 0; j < drawnLabelRects.size(); ++j)
				{
					if (RectsOverlap(labelRect, drawnLabelRects[j]))
					{
						collision = true;
						break;
					}
				}
				if (!collision)
				{
					spotFound = true;
				}
				else
				{
					finalPos.y += LABEL_HEIGHT;
				}
			}

			// If not found, try pushing up
			if (!spotFound)
			{
				finalPos = initialPos;
				finalPos.y -= LABEL_HEIGHT;
				for (int tries = 0; tries < 20 && !spotFound; ++tries)
				{
					labelRect.left = finalPos.x - ((LONG)textWidth / 2);
					labelRect.top = finalPos.y;
					labelRect.right = finalPos.x + ((LONG)textWidth / 2);
					labelRect.bottom = finalPos.y + LABEL_HEIGHT;

					if (labelRect.top < drawingZone.top)
						break;

					bool collision = false;
					for (size_t j = 0; j < drawnLabelRects.size(); ++j)
					{
						if (RectsOverlap(labelRect, drawnLabelRects[j]))
						{
							collision = true;
							break;
						}
					}
					if (!collision)
					{
						spotFound = true;
					}
					else
					{
						finalPos.y -= LABEL_HEIGHT;
					}
				}
			}

			if (spotFound)
			{
				DrawTextB(finalPos.x, finalPos.y, FONTCOLOR_LIGHTGREEN, FONT_SIZE, 1, "%s", pExitInfo->szName);
				drawnLabelRects.push_back(labelRect);
			}
		}
	}

	if (V_ShowPreloads)
	{
		for (size_t i = 0; i < V_Reveal->m_TrackedPresetUnits.GetSize(); ++i)
		{
			PRESETUNITINFO &unitInfo = V_Reveal->m_TrackedPresetUnits[i];
			POINT initialPos = {unitInfo.ptPos.x, unitInfo.ptPos.y};

			ScreenToAutomap(&initialPos, unitInfo.ptPos.x * 32, unitInfo.ptPos.y * 32);
			DrawLineFromPlayerToAutomapPoint(playerPos, initialPos, GetTrackedPresetLineColor(unitInfo));
			initialPos = AdjustAutomapLabelPosition(initialPos);

			DWORD textWidth = GetTextWidth(unitInfo.szName, FONT_SIZE);
			RECT labelRect;
			POINT finalPos = initialPos;
			bool spotFound = false;

			// Try pushing down
			for (int tries = 0; tries < 20 && !spotFound; ++tries)
			{
				labelRect.left = finalPos.x - ((LONG)textWidth / 2);
				labelRect.top = finalPos.y;
				labelRect.right = finalPos.x + ((LONG)textWidth / 2);
				labelRect.bottom = finalPos.y + LABEL_HEIGHT;

				if (labelRect.bottom > drawingZone.bottom)
					break;

				bool collision = false;
				for (size_t j = 0; j < drawnLabelRects.size(); ++j)
				{
					if (RectsOverlap(labelRect, drawnLabelRects[j]))
					{
						collision = true;
						break;
					}
				}
				if (!collision)
				{
					spotFound = true;
				}
				else
				{
					finalPos.y += LABEL_HEIGHT;
				}
			}

			// If not found, try pushing up
			if (!spotFound)
			{
				finalPos = initialPos;
				finalPos.y -= LABEL_HEIGHT;
				for (int tries = 0; tries < 20 && !spotFound; ++tries)
				{
					labelRect.left = finalPos.x - ((LONG)textWidth / 2);
					labelRect.top = finalPos.y;
					labelRect.right = finalPos.x + ((LONG)textWidth / 2);
					labelRect.bottom = finalPos.y + LABEL_HEIGHT;

					if (labelRect.top < drawingZone.top)
						break;

					bool collision = false;
					for (size_t j = 0; j < drawnLabelRects.size(); ++j)
					{
						if (RectsOverlap(labelRect, drawnLabelRects[j]))
						{
							collision = true;
							break;
						}
					}
					if (!collision)
					{
						spotFound = true;
					}
					else
					{
						finalPos.y -= LABEL_HEIGHT;
					}
				}
			}

			if (spotFound)
			{
				DrawTextB(finalPos.x, finalPos.y, FONTCOLOR_LIGHTGREEN, FONT_SIZE, 1, "%s", unitInfo.szName);
				drawnLabelRects.push_back(labelRect);
			}
		}
	}
}

VOID DrawNearbyEntities()
{
	if (!V_NearbyEntitiesEnabled || !*p_D2CLIENT_AutomapOn || IsFullScreenPanelOpen() || V_MainMenuOpen)
		return;

	// Define our entity groups that we'll collect and draw in order
	struct EntityToDraw
	{
		LPUNITANY Unit;
		POINT DrawingPos;
		DWORD Color;
		BOOL IsSpecial;
		CHAR Name[128];
		BYTE Type; // 0 = NPC, 1 = Object
	};

	std::vector<EntityToDraw> normalMonsters;
	std::vector<EntityToDraw> minionMonsters;
	std::vector<EntityToDraw> championMonsters;
	std::vector<EntityToDraw> bossMonsters;
	std::vector<EntityToDraw> objects;
	std::vector<EntityToDraw> interestingMonsters;
	std::vector<EntityToDraw> actualBossMonsters;

	// First pass: collect all entities we want to draw
	for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
	{
		for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
		{
			if (Unit && Unit->dwType == UNIT_TYPE_NPC)
			{
				if (!IsValidMonster(Unit))
					continue;

				if (IsOwnedByPlayer(Unit))
					continue;

				if (IsMercClassId(Unit->dwTxtFileNo))
					continue;

				DWORD Color = 0;
				BOOL IsSpecial = FALSE;
				EntityToDraw entity = {0};
				entity.Unit = Unit;
				entity.Type = 0; // NPC

				// Skip if unit has no path
				if (!Unit->pPath)
					continue;

				// entity.DrawingPos.x = Unit->pPath->xPos;
				// entity.DrawingPos.y = Unit->pPath->yPos;
				entity.DrawingPos.x = D2CLIENT_GetUnitX(Unit);
				entity.DrawingPos.y = D2CLIENT_GetUnitY(Unit);
				ScreenToAutomap(&entity.DrawingPos, entity.DrawingPos.x * 32, entity.DrawingPos.y * 32);

				// Get monster name if it's special
				WCHAR *MonsterName = (WCHAR *)GetUnitName(Unit);
				WideCharToMultiByte(CP_ACP, 0, MonsterName, -1, entity.Name, sizeof(entity.Name), 0, 0);

				// Check if TextFileNo is in the ActualBossTextFileNo array
				if (std::find(std::begin(ActualBossTextFileNo), std::end(ActualBossTextFileNo), Unit->dwTxtFileNo) != std::end(ActualBossTextFileNo))
				{
					Color = CROSSCOLOR_ACTUAL_BOSS;
					IsSpecial = TRUE;
					entity.Color = Color;
					entity.IsSpecial = IsSpecial;
					actualBossMonsters.push_back(entity);
				}
				// Check if TextFileNo is in the InterestingMonsterTextFileNo array
				else if (std::find(std::begin(InterestingMonsterTextFileNo), std::end(InterestingMonsterTextFileNo), Unit->dwTxtFileNo) != std::end(InterestingMonsterTextFileNo))
				{
					Color = CROSSCOLOR_IMPORTANT;
					IsSpecial = TRUE;
					entity.Color = Color;
					entity.IsSpecial = IsSpecial;
					interestingMonsters.push_back(entity);
				}
				else if (Unit->pMonsterData->fBoss & 1 && !Unit->pMonsterData->fChamp)
				{
					if (!V_DrawChampBossMonsters)
						continue;

					Color = CROSSCOLOR_BOSS;
					entity.Color = Color;
					entity.IsSpecial = FALSE;
					bossMonsters.push_back(entity);
				}
				else if (Unit->pMonsterData->fChamp & 1)
				{
					if (!V_DrawChampBossMonsters)
						continue;

					Color = CROSSCOLOR_CHAMP;
					entity.Color = Color;
					entity.IsSpecial = FALSE;
					championMonsters.push_back(entity);
				}
				else if (Unit->pMonsterData->fMinion & 1)
				{
					if (!V_DrawChampBossMonsters)
						continue;

					Color = CROSSCOLOR_MINION;
					entity.Color = Color;
					entity.IsSpecial = FALSE;
					minionMonsters.push_back(entity);
				}
				else
				{
					if (!V_DrawNormalMonsters)
						continue;

					Color = CROSSCOLOR_NORMAL;
					entity.Color = Color;
					entity.IsSpecial = FALSE;
					normalMonsters.push_back(entity);
				}
			}
			else if (Unit && Unit->dwType == UNIT_TYPE_OBJECT)
			{
				EntityToDraw entity = {0};
				entity.Unit = Unit;
				entity.Type = 1; // Object

				WCHAR *wName = (WCHAR *)GetUnitName(Unit);
				WideCharToMultiByte(CP_ACP, 0, wName, -1, entity.Name, sizeof(entity.Name), 0, 0);

				// Check if OBJ_MODE_OPENED
				if (Unit->dwMode == OBJ_MODE_OPENED)
					continue;

				if (strstr(entity.Name, "Shrine") != NULL || strstr(entity.Name, "Evil Urn") != NULL)
				{
					if (!V_DrawShrines)
						continue;

					BYTE shrineType = 0;
					if (Unit->pObjectData)
					{
						shrineType = Unit->pObjectData->Type;
						// 12 = Skill, 14 = Stamina, 15 = Exp
						if (shrineType == 12 || shrineType == 14 || shrineType == 15)
						{
							if (!V_DrawGoodShrines)
								continue;
						}
						else
						{
							if (!V_DrawOtherShrines)
								continue;
						}
					}

					if (Unit->pObjectPath)
					{
						entity.DrawingPos.x = Unit->pObjectPath->dwPosX;
						entity.DrawingPos.y = Unit->pObjectPath->dwPosY;
						ScreenToAutomap(&entity.DrawingPos, entity.DrawingPos.x * 32, entity.DrawingPos.y * 32);
						entity.DrawingPos = AdjustAutomapLabelPosition(entity.DrawingPos);
						entity.Color = CROSSCOLOR_SHRINE;
						objects.push_back(entity);
					}
				}
				else if (strstr(entity.Name, "Spire of Darkness") != NULL)
				{
					if (Unit->pObjectPath)
					{
						entity.DrawingPos.x = Unit->pObjectPath->dwPosX;
						entity.DrawingPos.y = Unit->pObjectPath->dwPosY;
						ScreenToAutomap(&entity.DrawingPos, entity.DrawingPos.x * 32, entity.DrawingPos.y * 32);
						entity.DrawingPos = AdjustAutomapLabelPosition(entity.DrawingPos);
						entity.Color = CROSSCOLOR_IMPORTANT;
						objects.push_back(entity);
					}
				}
			}
		}
	}

	// Second pass: draw non-monsters first, then monsters in order of importance.
	const int FONT_SIZE = 15;

	// 1. Objects cross
	for (size_t i = 0; i < objects.size(); i++)
	{
		DrawCross(objects[i].DrawingPos.x, objects[i].DrawingPos.y, objects[i].Color);
	}

	// 2. Normal monsters
	for (size_t i = 0; i < normalMonsters.size(); i++)
	{
		DrawMonsterMarker(normalMonsters[i].DrawingPos.x, normalMonsters[i].DrawingPos.y, normalMonsters[i].Color,
						  V_MonsterMarkerStyle, V_MonsterMarkerFontSize);
	}

	// 3. Magic / champion monsters
	for (size_t i = 0; i < championMonsters.size(); i++)
	{
		DrawMonsterMarker(championMonsters[i].DrawingPos.x, championMonsters[i].DrawingPos.y, championMonsters[i].Color,
						  V_MonsterMarkerStyle, V_MonsterMarkerFontSize);
	}

	// 4. Rare / minion monsters
	for (size_t i = 0; i < minionMonsters.size(); i++)
	{
		DrawMonsterMarker(minionMonsters[i].DrawingPos.x, minionMonsters[i].DrawingPos.y, minionMonsters[i].Color,
						  V_MonsterMarkerStyle, V_MonsterMarkerFontSize);
	}

	// 5. Unique / boss monsters
	for (size_t i = 0; i < bossMonsters.size(); i++)
	{
		DrawMonsterMarker(bossMonsters[i].DrawingPos.x, bossMonsters[i].DrawingPos.y, bossMonsters[i].Color,
						  V_MonsterMarkerStyle, V_MonsterMarkerFontSize);
	}

	// 6. Important monsters
	for (size_t i = 0; i < interestingMonsters.size(); i++)
	{
		DrawMonsterMarker(interestingMonsters[i].DrawingPos.x, interestingMonsters[i].DrawingPos.y, interestingMonsters[i].Color,
						  V_MonsterMarkerStyle, V_MonsterMarkerFontSize);
	}

	// 7. Actual boss monsters
	for (size_t i = 0; i < actualBossMonsters.size(); i++)
	{
		DrawMonsterMarker(actualBossMonsters[i].DrawingPos.x, actualBossMonsters[i].DrawingPos.y, actualBossMonsters[i].Color,
						  V_MonsterMarkerStyle, V_MonsterMarkerFontSize);
	}

	// 8. Object labels
	for (size_t i = 0; i < objects.size(); i++)
	{
		DrawTextB(objects[i].DrawingPos.x, objects[i].DrawingPos.y - 8,
				  (objects[i].Color == CROSSCOLOR_IMPORTANT) ? FONTCOLOR_LIGHTGREEN : FONTCOLOR_LIGHTGREY,
				  FONT_SIZE, 1, "%s", objects[i].Name);
	}

	// 9. Interesting monsters labels
	for (size_t i = 0; i < interestingMonsters.size(); i++)
	{
		// Draw Shadow of Mendeln, but not Shadow of Terror, Shadow of Hate, Shadow of Destruction
		if (strstr(interestingMonsters[i].Name, "Terror") == NULL && strstr(interestingMonsters[i].Name, "Hate") == NULL &&
			strstr(interestingMonsters[i].Name, "Destruction") == NULL && strstr(interestingMonsters[i].Name, "Veiled Portal") == NULL)
		{
			DrawTextB(interestingMonsters[i].DrawingPos.x, interestingMonsters[i].DrawingPos.y - 8, FONTCOLOR_LIGHTGREEN, FONT_SIZE, 1, interestingMonsters[i].Name);
		}
	}

	// 10. Actual boss monsters labels
	for (size_t i = 0; i < actualBossMonsters.size(); i++)
	{
		DrawTextB(actualBossMonsters[i].DrawingPos.x, actualBossMonsters[i].DrawingPos.y - 8, FONTCOLOR_PINK, FONT_SIZE, 1, actualBossMonsters[i].Name);
	}
}

VOID DrawMonsterHealthPercent()
{
	if (!V_DrawMonsterHealthPercent || IsFullScreenPanelOpen() || V_MainMenuOpen || !Me || !Me->pAct)
		return;

	for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
	{
		for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
		{
			if (!Unit || Unit->dwType != UNIT_TYPE_NPC || !Unit->pPath || !Unit->pMonsterData)
				continue;

			if (!IsValidMonster(Unit) || IsOwnedByPlayer(Unit) || IsMercClassId(Unit->dwTxtFileNo))
				continue;

			if (!V_DrawNormalMonsters && !(Unit->pMonsterData->fBoss || Unit->pMonsterData->fChamp || Unit->pMonsterData->fMinion))
				continue;

			if (!V_DrawChampBossMonsters && (Unit->pMonsterData->fBoss || Unit->pMonsterData->fChamp || Unit->pMonsterData->fMinion))
				continue;

			DWORD lifePercent = GetUnitLifePercent(Unit);
			if (lifePercent > 100)
				continue;

			POINT screenPos = {D2CLIENT_GetUnitX(Unit), D2CLIENT_GetUnitY(Unit)};
			WorldToScreen(&screenPos);
			DrawTextB(screenPos.x, screenPos.y - 25, FONTCOLOR_WHITE, 6, 1, "%d%%", lifePercent);
		}
	}
}

VOID DrawMonsterClassIds()
{
	if (!V_DrawMonsterClassIds || IsFullScreenPanelOpen() || V_MainMenuOpen || !Me || !Me->pAct)
		return;

	for (LPROOM1 Room = Me->pAct->pRoom1; Room; Room = Room->pRoomNext)
	{
		for (LPUNITANY Unit = Room->pUnitFirst; Unit; Unit = Unit->pListNext)
		{
			if (!Unit || Unit->dwType != UNIT_TYPE_NPC || !Unit->pPath || !Unit->pMonsterData)
				continue;

			if (!IsValidMonster(Unit) || IsOwnedByPlayer(Unit) || IsMercClassId(Unit->dwTxtFileNo))
				continue;

			POINT screenPos = {D2CLIENT_GetUnitX(Unit), D2CLIENT_GetUnitY(Unit)};
			WorldToScreen(&screenPos);
			DrawTextB(screenPos.x, screenPos.y - 38, FONTCOLOR_GOLD, 6, 1, "txt:%d", Unit->dwTxtFileNo);
		}
	}
}