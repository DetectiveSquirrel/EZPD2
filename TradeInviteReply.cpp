#include "Hack.h"
#include <string.h>

enum TradeInvitePhase
{
	TIR_IDLE = 0,
	TIR_AFTER_ESC,
	TIR_AFTER_ENTER_OPEN,
	TIR_TYPING,
	TIR_BEFORE_SEND_ENTER,
};

static TradeInvitePhase s_tirPhase = TIR_IDLE;
static DWORD s_tirNextTick = 0;
static WCHAR s_tirWideLine[256];
static int s_tirWideLen = 0;
static int s_tirCharPos = 0;
static BOOL s_tirKeyWasDown = FALSE;

static void TrimTrailingSpacesInPlace(char *s)
{
	if (!s)
		return;
	size_t n = strlen(s);
	while (n > 0 && (unsigned char)s[n - 1] <= ' ')
	{
		s[n - 1] = '\0';
		n--;
	}
}

static void CopyGameField(char *dst, size_t dstChars, const char *src, size_t srcMax)
{
	if (!dst || dstChars == 0)
		return;
	size_t i;
	for (i = 0; i + 1 < dstChars && i < srcMax; i++)
	{
		char c = src[i];
		if (c == '\0')
			break;
		dst[i] = c;
	}
	dst[i] = '\0';
	TrimTrailingSpacesInPlace(dst);
}

static BOOL BuildTradeInviteWideLine(void)
{
	LPGAMESTRUCTINFO gi = D2CLIENT_GetGameInfo();
	if (!gi || !VALIDPTR(gi))
		return FALSE;

	char gameName[sizeof(gi->szGameName) + 1];
	char gamePass[sizeof(gi->szGamePassword) + 1];

	CopyGameField(gameName, sizeof(gameName), gi->szGameName, sizeof(gi->szGameName));
	CopyGameField(gamePass, sizeof(gamePass), gi->szGamePassword, sizeof(gi->szGamePassword));

	if (gameName[0] == '\0')
		return FALSE;

	const char *passOut = gamePass[0] != '\0' ? gamePass : "no password";

	char line[256];
	strcpy_s(line, sizeof(line), "/r ");
	if (strcat_s(line, sizeof(line), gameName) != 0)
		return FALSE;
	if (strcat_s(line, sizeof(line), " / ") != 0)
		return FALSE;
	if (strcat_s(line, sizeof(line), passOut) != 0)
		return FALSE;

	WCHAR wbuf[256];
	if (MultiByteToWideChar(CP_ACP, 0, line, -1, wbuf, 256) <= 0)
		return FALSE;

	int len = (int)wcslen(wbuf);
	if (len <= 0 || len >= (int)ArraySize(s_tirWideLine))
		return FALSE;

	memcpy(s_tirWideLine, wbuf, (len + 1) * sizeof(WCHAR));
	s_tirWideLen = len;
	s_tirCharPos = 0;
	return TRUE;
}

VOID TradeInviteReplyTick()
{
	if (!V_TradeInviteReplyEnabled || V_TradeInviteReplyKey == 0)
	{
		s_tirPhase = TIR_IDLE;
		return;
	}

	if (V_MainMenuOpen || V_IsHotkeyInputMode || !GameReady())
	{
		s_tirPhase = TIR_IDLE;
		return;
	}

	if (V_IsRefillingPotions || V_IsPickingUpItems)
		return;

	DWORD now = GetTickCount();

	BOOL keyDown = KeyDown(V_TradeInviteReplyKey);
	if (keyDown && !s_tirKeyWasDown && s_tirPhase == TIR_IDLE)
	{
		if (!BuildTradeInviteWideLine())
		{
			PrintText(FONTCOLOR_RED, "Trade reply: no game name (need multiplayer game info)");
			s_tirKeyWasDown = keyDown;
			return;
		}

		if (GetUIVar(UI_CHAT) == 1)
		{
			SimulateKeyPress(VK_ESCAPE);
			s_tirPhase = TIR_AFTER_ESC;
			s_tirNextTick = now + 80;
		}
		else
		{
			SimulateKeyPress(VK_RETURN);
			s_tirPhase = TIR_AFTER_ENTER_OPEN;
			s_tirNextTick = now + 65;
		}
	}
	s_tirKeyWasDown = keyDown;

	if (s_tirPhase == TIR_IDLE)
		return;

	if ((LONG)(now - s_tirNextTick) < 0)
		return;

	switch (s_tirPhase)
	{
	case TIR_AFTER_ESC:
		SimulateKeyPress(VK_RETURN);
		s_tirPhase = TIR_AFTER_ENTER_OPEN;
		s_tirNextTick = now + 65;
		break;

	case TIR_AFTER_ENTER_OPEN:
		s_tirPhase = TIR_TYPING;
		s_tirNextTick = now + 40;
		break;

	case TIR_TYPING:
		if (s_tirCharPos < s_tirWideLen)
		{
			SimulateUnicodeChar(s_tirWideLine[s_tirCharPos]);
			s_tirCharPos++;
			s_tirNextTick = now + 20;
		}
		else
		{
			s_tirPhase = TIR_BEFORE_SEND_ENTER;
			s_tirNextTick = now + 30;
		}
		break;

	case TIR_BEFORE_SEND_ENTER:
		SimulateKeyPress(VK_RETURN);
		s_tirPhase = TIR_IDLE;
		break;

	default:
		s_tirPhase = TIR_IDLE;
		break;
	}
}
