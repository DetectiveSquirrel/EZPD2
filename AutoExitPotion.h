#define POTION_TYPE_NONE 0
#define POTION_TYPE_REJUV 1
#define POTION_TYPE_LIFE 2
#define POTION_TYPE_MANA 3

VOID AutoPotExitMain();
VOID RefillPotions();
INT GetPotionTypeFromItemCode(LPSTR lpszItemCode);
INT GetHotkeySlotForPotionType(INT potionType);
VOID UsePotion(INT hotkeyToUse, BOOL isMercPotion);
VOID ExitRefillPotions();
POINT GetFirstPotionPosInInventory(INT potionType);
VOID SetEmptyBeltSlotVars();
LPSTR GetCurrentlyEquippedBeltItemCode();
INT GetAmountOfBeltSlots(LPSTR itemCode);