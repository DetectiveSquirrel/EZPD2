#pragma once

#ifdef VARS

#define ARRAY(Type, Name) CArrayEx<Type, Type> V_##Name;
#define KEY(Name) INT K_##Name;
#define TOGGLE(Name) \
    BOOL V_##Name;   \
    INT K_##Name;
/* Must match non-VARS branch: V_##Name only. V##_##Name wrongly becomes V__Name vs extern V_Name. */
#define VAR(Type, Name) Type V_##Name;

#else

#define ARRAY(Type, Name) extern CArrayEx<Type, Type> V_##Name;
#define KEY(Name) extern INT K_##Name;
#define TOGGLE(Name)      \
    extern BOOL V_##Name; \
    extern INT K_##Name;
#define VAR(Type, Name) extern Type V_##Name;

#endif

enum InputModeType
{
	MODE_NONE,
	MODE_MENU_KEY,
	MODE_REFILL_POTIONS_KEY,
	MODE_PICKIT_KEY,
	MODE_ANYA_BOT_KEY,
	MODE_TRADE_INVITE_REPLY_KEY,
	MODE_BUILD_MA_CHARGES_KEY,
	MODE_MA_AUTOMATE_KEY,
	MODE_MARTIAL_ART_SKILL_BUTTON,
	MODE_FINISHER_SKILL_BUTTON,
};

VAR(HINSTANCE, DLL)
VAR(WNDPROC, OldWndProc)
VAR(Reveal *, Reveal)
VAR(BOOL, Initialized)

VAR(BOOL, IsHotkeyInputMode)
VAR(INT, InputModeType)
VAR(BOOL, MainMenuOpen)


// Settings

//Hotkeys
VAR(INT, MainMenuKey)
VAR(INT, RefillPotionsKey)
VAR(INT, PickitKey)
VAR(INT, AnyaBotKey)
VAR(INT, TradeInviteReplyKey)
VAR(INT, BuildMAChargesKey)
VAR(INT, MAAutomateKey)
// Maphack / Drawing
VAR(BOOL, MapHackEnabled)
VAR(BOOL, ShowZoneTransitions)
VAR(BOOL, ShowPreloads)
VAR(BOOL, DrawPOILinesToTarget)
VAR(BOOL, TownPortalOwnerLabelsEnabled)

VAR(BOOL, NearbyEntitiesEnabled)
VAR(BOOL, DrawChampBossMonsters)
VAR(BOOL, DrawNormalMonsters)
VAR(DWORD, MonsterMarkerStyle)
VAR(DWORD, MonsterMarkerFontSize)
VAR(DWORD, MonsterNormalColor)
VAR(DWORD, MonsterMinionColor)
VAR(DWORD, MonsterChampionColor)
VAR(DWORD, MonsterBossColor)
// Hardcoded txt-list bosses (ActualBossTextFileNo / InterestingMonsterTextFileNo) on automap
VAR(BOOL, DrawListedBossMarkers)
VAR(DWORD, ListedBossCrossColor)
VAR(DWORD, ListedBossNameColor)
VAR(BOOL, ListedBossHpInName)
VAR(DWORD, MonsterResistTextOffsetX)
VAR(DWORD, MonsterResistTextOffsetY)
VAR(BOOL, DrawMonsterResistances)
VAR(BOOL, DrawMonsterEnchantments)

VAR(BOOL, DrawShrines)
VAR(BOOL, DrawGoodShrines)
VAR(BOOL, DrawOtherShrines)
VAR(BOOL, DrawMonsterHealthPercent)
VAR(BOOL, DrawMonsterClassIds)
VAR(BOOL, DebugMouseCoordinates)
VAR(BOOL, DebugNearestItem)
VAR(BOOL, DebugPlayerInventory)
VAR(BOOL, DebugPlayerBelt)
VAR(BOOL, DebugClosestMonster)
VAR(BOOL, DebugCurrentRoom)

// AutoPot / AutoExit
VAR(BOOL, AutoExitEnabled)
VAR(DWORD, AutoExitLifeThreshold)

VAR(BOOL, AutoPotEnabled)
VAR(BOOL, AutoPotRejuvEnabled)
VAR(DWORD, AutoPotRejuvThreshold)
VAR(BOOL, AutoPotLifeEnabled)
VAR(DWORD, AutoPotLifeThreshold)
VAR(BOOL, AutoPotManaEnabled)
VAR(DWORD, AutoPotManaThreshold)

VAR(BOOL, MercAutoPotRejuvEnabled)
VAR(DWORD, MercAutoPotRejuvThreshold)
VAR(BOOL, MercAutoPotLifeEnabled)
VAR(DWORD, MercAutoPotLifeThreshold)

VAR(BOOL, AutoRefillEnabled)
VAR(INT, RefillSlot1PotionType)
VAR(INT, RefillSlot2PotionType)
VAR(INT, RefillSlot3PotionType)
VAR(INT, RefillSlot4PotionType)

// Refill Vars, not for Settings
VAR(BOOL, IsRefillingPotions)
VAR(POINT, InitialCursorPos)

VAR(BOOL, IsColumn1Empty)
VAR(BOOL, IsColumn2Empty)
VAR(BOOL, IsColumn3Empty)
VAR(BOOL, IsColumn4Empty)
VAR(POINT, ItemSlotLocationToFill)

// Pickit
VAR(BOOL, PickitEnabled)
// not for Settings
VAR(BOOL, IsPickingUpItems)

// Misc
VAR(BOOL, VendorPreviewEnabled)
VAR(BOOL, VendorShortcutEnabled)
VAR(BOOL, AnyaAutoPurchaseEnabled)
VAR(BOOL, TradeInviteReplyEnabled)
VAR(BOOL, MartialArtsChargesEnabled)
VAR(BOOL, BuildMAChargesEnabled)
VAR(BOOL, MAAutomateEnabled)
VAR(INT, MartialArtSkillButton)
VAR(INT, FinisherSkillButton)

// Anya Bot, not for Settings
VAR(BOOL, AnyaBotRunning)
