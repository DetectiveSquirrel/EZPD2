#include "Hack.h"

enum MAState
{
    MA_STATE_IDLE = 0,
    MA_STATE_BUILDING,
    MA_STATE_FINISHING,
};

struct MAContext
{
    MAState state;
    BOOL chargeInputDown;
    BOOL finisherInputDown;
    DWORD nextTick;
};

static MAContext g_maCtx = {MA_STATE_IDLE, FALSE, FALSE, 0};

static BOOL IsAnyMAChargeFull()
{
    return GetUnitStat(Me, STAT_PROGRESSIVEFIRE) >= 3 ||
           GetUnitStat(Me, STAT_PROGRESSIVELIGHTNING) >= 3 ||
           GetUnitStat(Me, STAT_PROGRESSIVECOLD) >= 3 ||
           GetUnitStat(Me, STAT_PROGRESSIVESTEAL) >= 3;
}

static BOOL HasAnyMACharge()
{
    return GetUnitStat(Me, STAT_PROGRESSIVEFIRE) > 0 ||
           GetUnitStat(Me, STAT_PROGRESSIVELIGHTNING) > 0 ||
           GetUnitStat(Me, STAT_PROGRESSIVECOLD) > 0 ||
           GetUnitStat(Me, STAT_PROGRESSIVESTEAL) > 0;
}

static VOID ReleaseConfiguredInput(INT vkCode, BOOL &isHeld)
{
    if (!isHeld)
        return;

    POINT mousePos = {*p_D2CLIENT_MouseX, *p_D2CLIENT_MouseY};
    if (vkCode == VK_LBUTTON)
        SimulateLeftUp(mousePos);
    else if (vkCode == VK_RBUTTON)
        SimulateRightUp(mousePos);
    else
        SimulateKeyUp(vkCode);

    isHeld = FALSE;
}

static VOID HoldConfiguredInput(INT vkCode, BOOL &isHeld)
{
    if (isHeld || vkCode == 0)
        return;

    POINT mousePos = {*p_D2CLIENT_MouseX, *p_D2CLIENT_MouseY};
    if (vkCode == VK_LBUTTON)
        SimulateLeftDown(mousePos);
    else if (vkCode == VK_RBUTTON)
        SimulateRightDown(mousePos);
    else
        SimulateKeyDown(vkCode);

    isHeld = TRUE;
}

static VOID ReleaseAllAutomationInput()
{
    ReleaseConfiguredInput(V_MartialArtSkillButton, g_maCtx.chargeInputDown);
    ReleaseConfiguredInput(V_FinisherSkillButton, g_maCtx.finisherInputDown);
    g_maCtx.state = MA_STATE_IDLE;
}

static BOOL IsAutomationAllowed()
{
    if (!GameReady() || !Me || V_MainMenuOpen)
        return FALSE;
    if (IsPlayerInTown())
        return FALSE;
    if (GetUIVar(UI_CHAT))
        return FALSE;
    if (Me->dwTxtFileNo != CLASS_ASN)
        return FALSE;
    return TRUE;
}

VOID MartialArtsAutomateTick()
{
    if (GetTickCount() < g_maCtx.nextTick)
        return;

    g_maCtx.nextTick = GetTickCount() + 30;

    if (!IsAutomationAllowed())
    {
        ReleaseAllAutomationInput();
        return;
    }

    const BOOL holdBuildKey = V_BuildMAChargesEnabled && V_BuildMAChargesKey != 0 && KeyDown(V_BuildMAChargesKey);
    const BOOL holdRotateKey = V_MAAutomateEnabled && V_MAAutomateKey != 0 && KeyDown(V_MAAutomateKey);

    if (!holdBuildKey && !holdRotateKey)
    {
        ReleaseAllAutomationInput();
        return;
    }

    if (V_MartialArtSkillButton == 0 || V_FinisherSkillButton == 0)
    {
        ReleaseAllAutomationInput();
        return;
    }

    // Build MA Charges mode: hold MA skill until any charge reaches 3.
    if (holdBuildKey)
    {
        g_maCtx.state = MA_STATE_BUILDING;

        if (IsAnyMAChargeFull())
        {
            ReleaseConfiguredInput(V_MartialArtSkillButton, g_maCtx.chargeInputDown);
        }
        else
        {
            HoldConfiguredInput(V_MartialArtSkillButton, g_maCtx.chargeInputDown);
        }

        // Ensure finisher input is not pressed in this mode.
        ReleaseConfiguredInput(V_FinisherSkillButton, g_maCtx.finisherInputDown);
        return;
    }

    // MA Automate mode: build to 3, then use finisher, repeat.
    if (g_maCtx.state == MA_STATE_IDLE)
        g_maCtx.state = MA_STATE_BUILDING;

    if (g_maCtx.state == MA_STATE_BUILDING)
    {
        if (IsAnyMAChargeFull())
        {
            ReleaseConfiguredInput(V_MartialArtSkillButton, g_maCtx.chargeInputDown);
            g_maCtx.state = MA_STATE_FINISHING;
        }
        else
        {
            ReleaseConfiguredInput(V_FinisherSkillButton, g_maCtx.finisherInputDown);
            HoldConfiguredInput(V_MartialArtSkillButton, g_maCtx.chargeInputDown);
        }
    }
    else if (g_maCtx.state == MA_STATE_FINISHING)
    {
        if (!HasAnyMACharge())
        {
            ReleaseConfiguredInput(V_FinisherSkillButton, g_maCtx.finisherInputDown);
            g_maCtx.state = MA_STATE_BUILDING;
        }
        else
        {
            ReleaseConfiguredInput(V_MartialArtSkillButton, g_maCtx.chargeInputDown);
            HoldConfiguredInput(V_FinisherSkillButton, g_maCtx.finisherInputDown);
        }
    }

    // Optional mode hints requested by user:
    // MA skill usage commonly shows PLAYER_MODE_ATTACK1 (7).
    // Finisher usage commonly shows PLAYER_MODE_SEQUENCE (18).
    // We intentionally keep hold logic stat-driven to stay robust.
}

VOID MartialArtsAutomateShutdown()
{
    ReleaseAllAutomationInput();
}
