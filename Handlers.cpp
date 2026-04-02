#include "Hack.h"
#include "Offset.h"
#include "PatchStubs.h"
#include "Settings.h"

LONG STDCALL WindowProc(HWND HWnd, UINT MSG, WPARAM WParam, LPARAM LParam)
{
    if (V_IsHotkeyInputMode)
    {
        if (MSG == WM_LBUTTONDOWN || MSG == WM_RBUTTONDOWN)
        {
            V_IsHotkeyInputMode = FALSE;
            V_InputModeType = MODE_NONE;
        }
        else if (MSG == WM_KEYDOWN)
        {
            if (WParam == VK_BACK)
            {
                switch (V_InputModeType)
                {
                case MODE_MENU_KEY:
                    V_MainMenuKey = 0;
                    break;
                case MODE_REFILL_POTIONS_KEY:
                    V_RefillPotionsKey = 0;
                    break;
                case MODE_PICKIT_KEY:
                    V_PickitKey = 0;
                    break;
                case MODE_ANYA_BOT_KEY:
                    V_AnyaBotKey = 0;
                    break;
                }
                V_IsHotkeyInputMode = FALSE;
                V_InputModeType = MODE_NONE;
                return CallWindowProcA(V_OldWndProc, HWnd, MSG, WParam, LParam);
            }

            switch (V_InputModeType)
            {
            case MODE_MENU_KEY:
                V_MainMenuKey = WParam;
                V_IsHotkeyInputMode = FALSE;
                V_InputModeType = MODE_NONE;
                break;
            case MODE_REFILL_POTIONS_KEY:
                V_RefillPotionsKey = WParam;
                V_IsHotkeyInputMode = FALSE;
                V_InputModeType = MODE_NONE;
                break;
            case MODE_PICKIT_KEY:
                V_PickitKey = WParam;
                V_IsHotkeyInputMode = FALSE;
                V_InputModeType = MODE_NONE;
                break;
            case MODE_ANYA_BOT_KEY:
                V_AnyaBotKey = WParam;
                V_IsHotkeyInputMode = FALSE;
                V_InputModeType = MODE_NONE;
                break;
            }
        }
    }

    else
    {
        if (MSG == WM_KEYDOWN)
        {
            if (WParam == V_MainMenuKey && V_MainMenuKey != 0)
            {
                V_MainMenuOpen = !V_MainMenuOpen;

                if (!V_MainMenuOpen)
                {
                    SaveSettings();
                }
            }
            else if (WParam == V_RefillPotionsKey && !V_MainMenuOpen && V_RefillPotionsKey != 0)
            {
                V_IsRefillingPotions = TRUE;

                V_InitialCursorPos.x = *p_D2CLIENT_MouseX;
                V_InitialCursorPos.y = *p_D2CLIENT_MouseY;
            }
            else if (WParam == V_PickitKey && !V_MainMenuOpen && V_PickitKey != 0)
            {
                V_IsPickingUpItems = TRUE;

                V_InitialCursorPos.x = *p_D2CLIENT_MouseX;
                V_InitialCursorPos.y = *p_D2CLIENT_MouseY;
            }
            else if (WParam == V_AnyaBotKey && !V_MainMenuOpen && V_AnyaBotKey != 0)
            {
                if (V_AnyaBotRunning)
                {
                    ExitAnyaBot();
                }
                else
                {
                    V_AnyaBotRunning = TRUE;
                    PrintText(FONTCOLOR_LIGHTGREEN, "Anya Bot started");
                }
            }
        }
        else if (MSG == WM_LBUTTONDOWN && V_MainMenuOpen)
        {
            POINT mousePos = { *p_D2CLIENT_MouseX, *p_D2CLIENT_MouseY };
            HandleMenuClick(mousePos.x, mousePos.y);
        }
    }

    return CallWindowProcA(V_OldWndProc, HWnd, MSG, WParam, LParam);
}

VOID MainLoop()
{
    InitializeHack();

    if (V_Reveal)
        V_Reveal->RevealAutomap();

    if (!GameReady())
    {
        SetDefaultMenuVars();
    }

    // DrawMouseCoordinates();
    // DrawNearestItem();
    // DrawPlayerInventoryItems();
    // DrawPlayerBeltItems();
    // DrawClosestMonsterStats();
    // DrawCurrentRoomInfo();

    IncreaseTickCount();

    AutoPotExitMain();
    RefillPotions();

    PickUpItems();

    AnyaBot();
    VendorShortcut();

    DrawVendorPreview();

    DrawNearbyEntities();
    DrawTrackedEntitiesLabels();

    DrawMenu();
}

VOID InitializeHack()
{
    if (!V_OldWndProc && D2GFX_GetHwnd())
    {
        V_OldWndProc = (WNDPROC)SetWindowLongPtr(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)WindowProc);
    }

    if (V_Initialized)
        return;

    LoadSettings();
    if (V_MainMenuKey == 0) V_MainMenuKey = VK_F7;
    SetDefaultMenuVars();
    InitMenu();

    if (GameReady())
    {
        V_Reveal = new Reveal;

        PrintText(8, "EZPD2 Initialized");
        V_Initialized = TRUE;
    }
}

VOID ShutdownHack()
{
    if (V_OldWndProc)
    {
        SetWindowLongPtr(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)V_OldWndProc);
        V_OldWndProc = NULL;
    }

    if (!V_Initialized)
        return;

    SaveSettings();

    if (V_Reveal)
    {
        delete V_Reveal;
        V_Reveal = NULL;
    }

    PrintText(8, "EZPD2 Shutdown");
    V_Initialized = FALSE;
}

VOID SetDefaultMenuVars()
{
    V_IsHotkeyInputMode = FALSE;
    V_InputModeType = MODE_NONE;
    V_MainMenuOpen = FALSE;
    V_TickCount = 0;

    V_IsRefillingPotions = FALSE;
    V_ItemSlotLocationToFill.x = -1;
    V_ItemSlotLocationToFill.y = -1;
    V_IsColumn1Empty = FALSE;
    V_IsColumn2Empty = FALSE;
    V_IsColumn3Empty = FALSE;
    V_IsColumn4Empty = FALSE;

    V_IsPickingUpItems = FALSE;

    ResetAnyaBot();
}