#include "Hack.h"

VOID NAKED AntiMaphackPatch()
{
    __asm
    {
                ret 0x8;
    }
}

VOID NAKED DrawManaOrbSTUB()
{
    __asm
    {
		CALL D2CLIENT_DrawManaOrb;
        CALL MainLoop;
		RETN;
    }
}