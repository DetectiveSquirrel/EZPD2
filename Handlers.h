#pragma once

LONG STDCALL WindowProc(HWND HWnd, UINT MSG, WPARAM WParam, LPARAM LParam);
VOID MainLoop();
VOID InitializeHack();
VOID ShutdownHack();
VOID SetDefaultMenuVars();
