PATCH Patches[] =
    {
        {PatchCall, DLLOFFSET(D2CLIENT, 0x6264D), (DWORD)AntiMaphackPatch, 5},
        {PatchCall, DLLOFFSET(D2CLIENT, 0x2929E), (DWORD)DrawManaOrbSTUB, 5},
};