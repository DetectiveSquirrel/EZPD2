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
        if (D2GFX_GetHwnd())
        {
            V_OldWndProc = (WNDPROC)SetWindowLongPtr(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)WindowProc);
        }

        InstallPatches();
        InitializeHack();
    }

    if (dwReason == DLL_PROCESS_DETACH)
    {
        SetWindowLongPtr(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)V_OldWndProc);

        RemovePatches();
        ShutdownHack();
    }

    return true;
}
