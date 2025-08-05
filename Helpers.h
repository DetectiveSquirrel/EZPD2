#define ArraySize(X) (sizeof(X) / sizeof(X[0]))
#define KeyDown(Code) ((GetAsyncKeyState(Code) & 0x8000) ? TRUE : FALSE)
#define VALIDPTR(x) ((x) && (!IsBadReadPtr(x, sizeof(x))))

BOOL RemovePEHeader(HMODULE DLL);
VOID PrintText(DWORD Color, char *szText, ...);
BOOL GameReady(VOID);
DWORD GetPlayerArea(VOID);
VOID ScreenToAutomap(POINT *pPos, INT nX, INT nY);
BOOL GetMapName(BYTE iMapID, LPSTR lpszBuffer, DWORD dwMaxChars);
Level *GetUnitLevel(UnitAny *pUnit);
BOOL IsLeftPanelOpen(VOID);
BOOL IsRightPanelOpen(VOID);
BOOL IsFullScreenPanelOpen(VOID);
RECT GetSafeScreenAreaRect(VOID);
POINT AdjustAutomapLabelPosition(POINT point);
BOOL IsPointInRect(POINT &pt, RECT &rect);
BOOL IsValidMonster(LPUNITANY Unit);
BOOL IsTownLevel(INT nLevel);
BOOL IsPlayerInTown();
LPCSTR GetKeyName(int vkCode);
LPUNITANY GetClosestUnit(DWORD dwType);
DWORD GetPlayerVitalsPercent(BOOL bLife);
DWORD GetUnitLifePercent(UnitAny* pUnit);
INT GetItemLocation(UnitAny* pItem);
BOOL GetItemCodeFromItem(UnitAny* pItem, LPSTR szBuffer, DWORD dwMax);
VOID SimulateKeyPress(INT vKey);
VOID SimulateKeyDown(INT vKey);
VOID SimulateKeyUp(INT vKey);
LPCSTR GetPotionTypeName(INT potionType);
INT SetValidPotionForRefillSlot(INT potionType);
POINT GetInventorySlotPixelCoordinates(INT x, INT y);
VOID SimulateLeftClick(POINT point);
BOOL IsPotionToFillInInventory();
VOID IncreaseTickCount();
DWORD GetTextWidth(LPSTR text, INT font);
BOOL RectsOverlap(const RECT& r1, const RECT& r2);
BOOL IsMercClassId(DWORD dwClassID);
LPUNITANY FindMercUnit(LPUNITANY pOwner);
void WorldToScreen(POINT *pPos);
void ScreenToWorld(POINT *pPos);
BOOL IsVendor(LPUNITANY pUnit);
void CreateCleanItemName(const char* input, char* output, size_t outSize);