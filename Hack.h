#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE
#define _WIN32_WINNT 0x600

#undef UNICODE

#include <algorithm>
#include <windows.h>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iosfwd>
#include <list>
#include <math.h>
#include <process.h>
#include <psapi.h>
#include <shlwapi.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <time.h>
#include <tlhelp32.h>
#include <valarray>
#include <vector>
#include <winbase.h>
#include <windef.h>
#include <winnt.h>
#include <winuser.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi.lib")

#define EXPORT __declspec(dllexport)
#define FASTCALL __fastcall
#define FUNCCALL __cdecl
#define IMPORT __declspec(dllimport)
#define NAKED __declspec(naked)
#define STDCALL __stdcall

#pragma warning(disable : 4311)
#pragma warning(disable : 4312)
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4154)
#pragma warning(disable : 4018)

#include "ArrayEx.h"
#include "AutoExitPotion.h"
#include "Constants.h"
#include "D2Pointers.h"
#include "D2Structs.h"
#include "D2Stubs.h"
#include "DebugDrawing.h"
#include "Drawing.h"
#include "Handlers.h"
#include "Helpers.h"
#include "Matrix.h"
#include "Menu.h"
#include "Offset.h"
#include "PatchStubs.h"
#include "Pickit.h"
#include "Reveal.h"
#include "Settings.h"
#include "Vars.h"
#include "Vendor.h"