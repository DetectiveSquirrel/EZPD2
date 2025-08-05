#ifdef VARS

#define ARRAY(Type, Name) CArrayEx<Type, Type> V_##Name;
#define KEY(Name) INT K_##Name;
#define TOGGLE(Name) \
    BOOL V_##Name;   \
    INT K_##Name;
#define VAR(Type, Name) Type V##_##Name;

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
};

VAR(HINSTANCE, DLL)
VAR(WNDPROC, OldWndProc)
VAR(Reveal *, Reveal)
VAR(BOOL, Initialized)

VAR(BOOL, IsHotkeyInputMode)
VAR(INT, InputModeType)
VAR(BOOL, MainMenuOpen)
VAR(INT, TickCount)


// Settings

//Hotkeys
VAR(INT, MainMenuKey)
VAR(INT, RefillPotionsKey)
VAR(INT, PickitKey)
VAR(INT, AnyaBotKey)
// Maphack / Drawing
VAR(BOOL, MapHackEnabled)
VAR(BOOL, ShowZoneTransitions)
VAR(BOOL, ShowPreloads)

VAR(BOOL, NearbyEntitiesEnabled)
VAR(BOOL, DrawChampBossMonsters)
VAR(BOOL, DrawNormalMonsters)

VAR(BOOL, DrawShrines)
VAR(BOOL, DrawGoodShrines)
VAR(BOOL, DrawOtherShrines)

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

// Anya Bot, not for Settings
VAR(BOOL, AnyaBotRunning)
VAR(BOOL, AnyaBotCheckedItems)
