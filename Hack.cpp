#define PTRS
#define VARS

#include "Hack.h"

BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        V_DLL = hDll;
        RemovePEHeader(hDll);
        DefineOffsets();

        V_Initialized = FALSE;


        V_OldWndProc = NULL;

        InstallPatches();
        InitializeHack();
    }

    if (dwReason == DLL_PROCESS_DETACH)
    {
        RemovePatches();
        ShutdownHack();
    }

    return true;
}
