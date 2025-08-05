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