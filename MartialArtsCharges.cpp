#include "Hack.h"

static INT GetMartialArtChargeCount(DWORD statId)
{
    INT count = (INT)GetUnitStat(Me, statId);
    if (count < 0)
        return 0;
    if (count > 3)
        return 3;
    return count;
}

static VOID DrawChargePips(INT x, INT y, INT charges, DWORD color)
{
    const INT pipSize = 12;
    const INT spacing = 6;

    for (INT i = 0; i < 3; ++i)
    {
        INT pipX = x + (i * (pipSize + spacing));
        DrawBox(pipX, y, pipX + pipSize, y + pipSize, color);

        if (i < charges)
        {
            D2GFX_DrawLine(pipX + 1, y + 1, pipX + pipSize - 1, y + pipSize - 1, color, -1);
            D2GFX_DrawLine(pipX + pipSize - 1, y + 1, pipX + 1, y + pipSize - 1, color, -1);
        }
    }
}

VOID DrawMartialArtsCharges()
{
    if (!V_MartialArtsChargesEnabled || !GameReady() || !Me || V_MainMenuOpen || IsFullScreenPanelOpen())
        return;

    // Assassin-only helper.
    if (Me->dwTxtFileNo != CLASS_ASN)
        return;

    const INT fofCharges = GetMartialArtChargeCount(STAT_PROGRESSIVEFIRE);
    const INT cotCharges = GetMartialArtChargeCount(STAT_PROGRESSIVELIGHTNING);
    const INT boiCharges = GetMartialArtChargeCount(STAT_PROGRESSIVECOLD);
    const INT cobraCharges = GetMartialArtChargeCount(STAT_PROGRESSIVESTEAL);

    INT activeCharges[4];
    INT activeCount = 0;

    if (fofCharges > 0)
        activeCharges[activeCount++] = fofCharges;
    if (cotCharges > 0)
        activeCharges[activeCount++] = cotCharges;
    if (boiCharges > 0)
        activeCharges[activeCount++] = boiCharges;
    if (cobraCharges > 0)
        activeCharges[activeCount++] = cobraCharges;

    // Do not draw anything if no Martial Arts charges are active.
    if (activeCount == 0)
        return;

    const INT screenCenterX = (*p_D2CLIENT_ScreenSizeX) / 2;
    const INT screenCenterY = (*p_D2CLIENT_ScreenSizeY) / 2;

    const INT pipSize = 12;
    const INT spacing = 6;
    const INT rowHeight = 16;
    const INT rowSpacing = 6;
    const INT rowWidth = (pipSize * 3) + (spacing * 2);
    const INT totalHeight = (activeCount * rowHeight) + ((activeCount - 1) * rowSpacing);
    const INT startX = screenCenterX - (rowWidth / 2);
    const INT startY = screenCenterY - (totalHeight / 2);

    for (INT i = 0; i < activeCount; ++i)
    {
        const INT charges = activeCharges[i];
        const DWORD color = (charges == 3) ? CROSSCOLOR_IMPORTANT : COLOR_WHITE;
        const INT rowY = startY + (i * (rowHeight + rowSpacing));
        DrawChargePips(startX, rowY, charges, color);
    }
}
