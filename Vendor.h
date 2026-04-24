#pragma once

#define ITEM_NAME_SEARCH "buy"

struct VendorShortcut
{
    int textfileno;
    int y_offset;
};

static const VendorShortcut vendorShortcuts[] = {
    {NPCID_Akara, 125},
    {NPCID_Charsi, 125},
    {NPCID_Gheed, 125},
    {NPCID_Cain1, 125},
    {NPCID_Kashya, 125},
    {NPCID_Elzix, 125},
    {NPCID_Greiz, 125},
    {NPCID_Drognan, 125},
    {NPCID_Fara, 125},
    {NPCID_Lysander, 125},
    {NPCID_Cain2, 125},
    {NPCID_Cain3, 125},
    {NPCID_Cain4, 125},
    {NPCID_Cain5, 125},
    {NPCID_Ormus, 125},
    {NPCID_Asheara, 125},
    {NPCID_Alkor, 125},
    {NPCID_Halbu, 125},
    {NPCID_Jamella, 125},
    {NPCID_Tyrael, 110},
    {NPCID_Larzuk, 125},
    {NPCID_Malah, 125},
    {NPCID_Anya, 125},
    {NPCID_QualKehk, 125},
};
static const int numVendorShortcuts = sizeof(vendorShortcuts) / sizeof(vendorShortcuts[0]);

VOID DrawVendorPreview();
VOID AnyaBot();
VOID ExitAnyaBot();
VOID ResetAnyaBot();
VOID VendorShortcut();

