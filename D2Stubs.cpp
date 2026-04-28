#include "Hack.h"

DWORD NAKED FASTCALL InitAutomapLayerSTUB(DWORD dwLayer)
{
	__asm
	{
		PUSH EAX;
		MOV EAX, ECX;
		CALL D2CLIENT_InitAutomapLayer_I;
		POP EAX;
		RET;
	}
}

DWORD NAKED FASTCALL GetUnitNameSTUB(DWORD Unit)
{
    __asm
    {
		MOV EAX, ECX;
		JMP D2CLIENT_GetUnitName_I;
    }
}

DWORD NAKED FASTCALL GetUIVarSTUB(DWORD UI)
{
    __asm
    {
		MOV EAX, ECX;
		JMP D2CLIENT_GetUiVar_I;
    }
}

DWORD NAKED FASTCALL TestPvpFlag_STUB(DWORD playerId1, DWORD playerId2, DWORD flagmask)
{
	__asm
	{
		push esi;
		push dword ptr [esp + 8];
		mov esi, edx;
		mov edx, ecx;
		call D2CLIENT_TestPvPFlag_I;
		pop esi;
		ret 4;
	}
}