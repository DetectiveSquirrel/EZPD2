#include "Hack.h"
#include "Offset.h"
#include "PatchStubs.h"
#include "Settings.h"

#ifndef GET_XBUTTON_WPARAM
#define GET_XBUTTON_WPARAM(wp) ((WORD)HIWORD(wp))
#endif
#ifndef XBUTTON1
#define XBUTTON1 1
#define XBUTTON2 2
#endif

static INT XButtonWParamToVk(WPARAM wParam)
{
    UINT xb = (UINT)GET_XBUTTON_WPARAM(wParam);
    if (xb == XBUTTON1)
        return VK_XBUTTON1;
    if (xb == XBUTTON2)
        return VK_XBUTTON2;
    return 0;
}

static VOID FinishHotkeyBind(INT vk)
{
    switch (V_InputModeType)
    {
    case MODE_MENU_KEY:
        V_MainMenuKey = vk;
        break;
    case MODE_REFILL_POTIONS_KEY:
        V_RefillPotionsKey = vk;
        break;
    case MODE_PICKIT_KEY:
        V_PickitKey = vk;
        break;
    case MODE_ANYA_BOT_KEY:
        V_AnyaBotKey = vk;
        break;
    case MODE_TRADE_INVITE_REPLY_KEY:
        V_TradeInviteReplyKey = vk;
        break;
    case MODE_BUILD_MA_CHARGES_KEY:
        V_BuildMAChargesKey = vk;
        break;
    case MODE_MA_AUTOMATE_KEY:
        V_MAAutomateKey = vk;
        break;
    case MODE_MARTIAL_ART_SKILL_BUTTON:
        V_MartialArtSkillButton = vk;
        break;
    case MODE_FINISHER_SKILL_BUTTON:
        V_FinisherSkillButton = vk;
        break;
    default:
        return;
    }
    V_IsHotkeyInputMode = FALSE;
    V_InputModeType = MODE_NONE;
}

static VOID HandleGameplayHotkeys(INT vk)
{
    if (vk == 0)
        return;

    if (vk == V_MainMenuKey && V_MainMenuKey != 0)
    {
        V_MainMenuOpen = !V_MainMenuOpen;

        if (!V_MainMenuOpen)
        {
            SaveSettings();
        }
    }
    else if (vk == V_RefillPotionsKey && !V_MainMenuOpen && V_RefillPotionsKey != 0)
    {
        V_IsRefillingPotions = TRUE;

        V_InitialCursorPos.x = *p_D2CLIENT_MouseX;
        V_InitialCursorPos.y = *p_D2CLIENT_MouseY;
    }
    else if (vk == V_PickitKey && !V_MainMenuOpen && V_PickitKey != 0)
    {
        V_IsPickingUpItems = TRUE;

        V_InitialCursorPos.x = *p_D2CLIENT_MouseX;
        V_InitialCursorPos.y = *p_D2CLIENT_MouseY;
    }
    else if (vk == V_AnyaBotKey && !V_MainMenuOpen && V_AnyaBotKey != 0)
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

LONG STDCALL WindowProc(HWND HWnd, UINT MSG, WPARAM WParam, LPARAM LParam)
{
    if (V_IsHotkeyInputMode)
    {
        if (MSG == WM_LBUTTONDOWN)
        {
            FinishHotkeyBind(VK_LBUTTON);
            return CallWindowProcA(V_OldWndProc, HWnd, MSG, WParam, LParam);
        }
        if (MSG == WM_RBUTTONDOWN)
        {
            FinishHotkeyBind(VK_RBUTTON);
            return CallWindowProcA(V_OldWndProc, HWnd, MSG, WParam, LParam);
        }
        if (MSG == WM_XBUTTONDOWN)
        {
            INT vk = XButtonWParamToVk(WParam);
            if (vk != 0)
            {
                FinishHotkeyBind(vk);
                return CallWindowProcA(V_OldWndProc, HWnd, MSG, WParam, LParam);
            }
        }
        if (MSG == WM_KEYDOWN)
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
                case MODE_TRADE_INVITE_REPLY_KEY:
                    V_TradeInviteReplyKey = 0;
                    break;
                case MODE_BUILD_MA_CHARGES_KEY:
                    V_BuildMAChargesKey = 0;
                    break;
                case MODE_MA_AUTOMATE_KEY:
                    V_MAAutomateKey = 0;
                    break;
                case MODE_MARTIAL_ART_SKILL_BUTTON:
                    V_MartialArtSkillButton = 0;
                    break;
                case MODE_FINISHER_SKILL_BUTTON:
                    V_FinisherSkillButton = 0;
                    break;
                }
                V_IsHotkeyInputMode = FALSE;
                V_InputModeType = MODE_NONE;
                return CallWindowProcA(V_OldWndProc, HWnd, MSG, WParam, LParam);
            }

            FinishHotkeyBind((INT)WParam);
        }
    }

    else
    {
        if (MSG == WM_KEYDOWN)
        {
            HandleGameplayHotkeys((INT)WParam);
        }
        else if (MSG == WM_XBUTTONDOWN)
        {
            HandleGameplayHotkeys(XButtonWParamToVk(WParam));
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

    AutoPotExitMain();
    RefillPotions();

    PickUpItems();

    TradeInviteReplyTick();
    MartialArtsAutomateTick();

    AnyaBot();
    VendorShortcut();

    DrawVendorPreview();
    DrawMartialArtsCharges();

    DrawNearbyEntities();
    DrawTownPortalOwnerLabels();
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
    MartialArtsAutomateShutdown();

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