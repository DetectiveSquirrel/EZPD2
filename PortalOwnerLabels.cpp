#include "Hack.h"
#include <string.h>
#include <wchar.h>

// ObjectData.szOwner is char[0x10] (D2Structs.h). Not sizeof(ObjectData::szOwner) — C++03 MSVC rejects it.
enum { kPortalOwnerFieldBytes = 0x10 };

// objects.txt Town Portal (vanilla / most mods). Red / quest portals use a different id (e.g. 60).
static BOOL IsPlayerTownPortalObject(LPUNITANY unit)
{
	if (!unit || unit->dwType != UNIT_TYPE_OBJECT || !unit->pObjectData)
		return FALSE;

	if (unit->dwTxtFileNo == OBJ_TXT_RED_PORTAL)
		return FALSE;

	if (unit->dwTxtFileNo == OBJ_TXT_TOWN_PORTAL)
		return TRUE;

	WCHAR *wName = (WCHAR *)GetUnitName(unit);
	if (wName && wcsstr(wName, L"Town Portal"))
		return TRUE;

	return FALSE;
}

static BOOL CopyPortalOwnerString(LPUNITANY unit, CHAR *out, size_t outChars)
{
	if (!out || outChars == 0)
		return FALSE;

	const char *src = unit->pObjectData->szOwner;
	size_t n = 0;
	while (n < (size_t)kPortalOwnerFieldBytes && src[n] != '\0')
		n++;

	while (n > 0 && (unsigned char)src[n - 1] <= ' ')
		n--;

	if (n == 0)
		return FALSE;

	if (n >= outChars)
		n = outChars - 1;

	memcpy(out, src, n);
	out[n] = '\0';
	return TRUE;
}

VOID DrawTownPortalOwnerLabels()
{
	if (!V_TownPortalOwnerLabelsEnabled || V_MainMenuOpen || !GameReady())
		return;

	if (IsFullScreenPanelOpen())
		return;

	if (!Me || !Me->pAct || !Me->pAct->pRoom1)
		return;

	RECT safeArea = GetSafeScreenAreaRect();
	const int fontSize = 6;
	const int labelOffsetY = -42;
	const int padX = 6;
	const int padTop = 14;
	const int padBottom = 6;

	for (LPROOM1 room = Me->pAct->pRoom1; room; room = room->pRoomNext)
	{
		for (LPUNITANY unit = room->pUnitFirst; unit; unit = unit->pListNext)
		{
			if (!IsPlayerTownPortalObject(unit))
				continue;

			CHAR ownerName[kPortalOwnerFieldBytes + 1];
			if (!CopyPortalOwnerString(unit, ownerName, sizeof(ownerName)))
				continue;

			LONG wx = (LONG)D2CLIENT_GetUnitX(unit);
			LONG wy = (LONG)D2CLIENT_GetUnitY(unit);
			POINT scr = {wx, wy};
			WorldToScreen(&scr);
			scr.y += labelOffsetY;

			if (!IsPointInRect(scr, safeArea))
				continue;

			DWORD textWidth = GetTextWidth(ownerName, fontSize);
			LONG half = (LONG)((textWidth + 1) / 2);
			INT left = (INT)scr.x - half - padX;
			INT right = (INT)scr.x + half + padX;
			INT top = (INT)scr.y - padTop;
			INT bottom = (INT)scr.y + padBottom;

			D2GFX_DrawRectangle(left, top, right, bottom, 0, 1);
			DrawTextB((INT)scr.x, (INT)scr.y, FONTCOLOR_LIGHTGREEN, fontSize, 1, "%s", ownerName);
		}
	}
}
