#pragma once

#include "Matrix.h"

#define MAP_DATA_NULL -1
#define MAP_DATA_AVOID 11115
#define MAP_DATA_FILL 11111

typedef struct PresetUnitInfo_t
{
	DWORD dwClassId;  // e.g., pUnit->dwTxtFileNo
	DWORD dwUnitType; // e.g., pUnit->dwType
	POINT ptPos;	  // World coordinates
	DWORD dwLevelNo;  // Level ID where it was found
	CHAR szName[64];  // Descriptive name for specific NPCs
} PRESETUNITINFO, *LPPRESETUNITINFO;

typedef struct CaveDescriptor_t
{
	CHAR szName[0x40];
	POINT ptPos;
	DWORD dwLevelNo;
	DWORD dwTargetLevelNo;
	DWORD dwAct;
} CAVEDESC, *LPCAVEDESC;

typedef struct LevelExit_t
{
	POINT ptPos;
	DWORD dwTargetLevel;
} LEVELEXIT, *LPLEVELEXIT;

typedef struct UnitClassIdMapping
{
	DWORD dwClassId;
	const char *szName;
} UnitClassIdMapping, *LPUnitClassIdMapping;

const UnitClassIdMapping trackedMapMonsters[] = {
	{738, "Boss"}, // Lesser Hydra that might be used to spawn bosses in-game
	// {739, "Boss"}, // Lesser Hydra that might be used to spawn bosses in-game
	{740, "Boss"}, // Lesser Hydra that might be used to spawn bosses in-game
	{883, "Boss"}, // Battlefield Map
	{893, "Boss"}, // Fall of Calldeum
	{915, "Boss"}, // Lost Temple
	{998, "Boss"}, // Royal Crypts
	{800, "Boss"}, // Horazon's Memory
	{884, "Boss"}, // Sewers of Harogath
	{879, "Boss"}, // Torajan Jungle
	{996, "Boss"}, // Shadow of Westmarch
	{861, "Boss"}, // Ancestral Trial
	{750, "Boss"}, // Ruins of Viz-Jun
	{870, "Boss"}, // Tomb of Zoltun Kulle
	{826, "Boss"}, // Blood Moon
	{1121, "Boss"}, // Durance of Strife
	{964, "Boss"}, // Kehjistan Marketplace
	{746, "Boss"}, // Phlegeton
	{1025, "Boss"}, // Ashen Plains
	{963, "Boss"}, // Canyon of Sescheron
	{894, "Boss"}, // Pandemonium Citadel
	{1026, "Boss"}, // Ruined Cistern
	{1044, "Boss"}, // Ruined Cistern
	{997, "Boss"}, // Sanatorium
	{991, "Boss"}, // Shadow of the Worldstone
	{882, "Boss"}, // Throne of Insanity
	{1104, "Boss"}, // Sanctuary of Sin Dungeon
	{743, "Boss"}, // Plains of Torment Dungeon
	{1105, "Boss"}, // Steppes of Daken-Shar Dungeon
	{1060, "Boss"}, // Stronghold of Acheron (Warlord of Blood unique map)
	{1092, "Boss"}, // Stygian Caverns (Warlord of Night unique map)
	{1093, "Boss"}, // Stronghold of Acheron (Warlord of Blood unique map)
	{1096, "Boss"}, // Fallen Gardens (unique map)
	{1063, "Boss"}, // Collonades of Madness (zhar's sanctum unique map)
	{1062, "Boss"}, // Hidden Waterways (zhar's sanctum unique map)
	{1061, "Boss"}, // Counselor's Causeway (zhar's sanctum unique map)
};

const UnitClassIdMapping trackedCampaignMonsters[] = {
	{1243, "Blood Raven"},
	{1177, "The Countess"},
	{156, "Andariel"},
	{250, "Summoner"},
	{242, "Mephisto"},
	{256, "Izual"},
	{1230, "Anya"},	 // Monster next to Anya
	{543, "Throne"}, // Baal that spawns during waves
};

const UnitClassIdMapping trackedCampaignObjects[] = {
	{17, "Tristram"},			// First of 5 stones = Tristram
	{30, "Tree"},				// Tree in Blood Moor
	{108, "Horadric Malus"},	// Tree in Blood Moor
	{149, "Amulet"},			// Sun Altar that drops Amulet
	{356, "Staff"},				// Chest that drops Staff
	{298, "Portal"},			// Portal to Arcane Sanctuary
	{152, "Duriel"},			// Orifice = Duriel - not quite sure yet
	{354, "Bloodwitch / Cube"}, // Cube chest = Bloodwitch
	{355, "Radament"},			// Horadric scroll chest = Radament
	{404, "Council"},			// Hammer thing = Council
	{405, "Khalim's Heart"},	// Chest that drops Heart
	{406, "Khalim's Brain"},	// Chest that drops Brain
	{407, "Khalim's Eye"},		// Chest that drops Eye
	{376, "Hellforge"},			// Chest that drops Eye
	{392, "Seal"},				// Seal right
	{393, "Seal"},				// Seal right
	{394, "Seal"},				// Seal top middle
	{395, "Seal"},				// Seal left
	{396, "Seal"},				// Seal left
	{473, "Prison"},			// 3 Prisons in A5
	{462, "Nihlathak"},			// Tile that Nihlathak is on

	{60, "Red Portal"}, // Red Portals in A5

	// Waypoints
	{119, "Waypoint"},
	{157, "Waypoint"},
	{156, "Waypoint"},
	{323, "Waypoint"},
	{288, "Waypoint"},
	{402, "Waypoint"},
	{324, "Waypoint"},
	{237, "Waypoint"},
	{238, "Waypoint"},
	{398, "Waypoint"},
	{429, "Waypoint"},
	{496, "Waypoint"},
	{511, "Waypoint"},
	{494, "Waypoint"}};

class Reveal
{
public:
	Reveal();
	virtual ~Reveal();

	VOID RevealAutomap();
	BOOL CreateCollisionMap();
	LPLEVEL GetLevel(LPACTMISC misc, DWORD levelno);
	BOOL GetCaveExits(LPCAVEDESC *lpLevel, INT nMaxExits);
	BOOL GetLevelExits(LPLEVELEXIT *lpLevel, INT nMaxExits);
	VOID DestroyLevelExits();

	WORD GetCollisionInfo(INT nX, INT nY);
	BOOL ExportCollisionMap(CMatrix<WORD, WORD> &cMatrix);

	DWORD GetTileLevelNo(LPROOM2 lpRoom2, DWORD dwTileNo);

	CArrayEx<LPCAVEDESC, LPCAVEDESC> m_LevelExits;
	CArrayEx<PRESETUNITINFO, PRESETUNITINFO &> m_TrackedPresetUnits;
	VOID ClearTrackedPresetUnits();
	BOOL GetTrackedPresetUnits(CArrayEx<PRESETUNITINFO, PRESETUNITINFO &> &outArray);

	UCHAR m_ActLevels[6];
	UCHAR m_RevealedActs[5];

protected:
	VOID AddBoundaryLevelExits();
	VOID AddAutomapRoom(LPROOM2 pRoom2);
	VOID RemoveAutomapRoom(LPROOM2 pRoom2);

	VOID RevealLevel(LPLEVEL pLevel);
	VOID RevealRoom(LPROOM2 pRoom);
	VOID RevealRoom1(LPROOM2 pRoom);
	VOID AddRoomCell(INT xPos, INT yPos, INT nCell, LPROOM2 pRoom);

	LPAUTOMAPLAYER2 InitAutomapLayer(DWORD dwLayer);

	BOOL AddCollisionData(LPROOM2 pRoom2, CArrayEx<DWORD, DWORD> &aSkip);
	BOOL AddCollisionData(LPCOLLMAP pColl);
	BOOL FillGaps();
	BOOL IsGap(INT nX, INT nY);

	INT m_LastLevel;
	INT m_SizeX;
	INT m_SizeY;
	POINT m_LevelOrigin;
	CMatrix<WORD, WORD> m_Map;
};