#ifndef _DRAWING_H
#define _DRAWING_H

#define FONTCOLOR_WHITE 0
#define FONTCOLOR_DARKORANGE 1
#define FONTCOLOR_LIGHTGREEN 2
#define FONTCOLOR_BLUE 3
#define FONTCOLOR_GOLD 4
#define FONTCOLOR_DARKGREY 5
#define FONTCOLOR_TRANSPARENTGREY 6
#define FONTCOLOR_BEIGE 7
#define FONTCOLOR_ORANGE 8
#define FONTCOLOR_YELLOW 9
#define FONTCOLOR_DARKGREEN 10
#define FONTCOLOR_PINK 11
#define FONTCOLOR_LIGHTGREY 12
#define FONTCOLOR_CYAN 13
#define FONTCOLOR_TEAL 14
#define FONTCOLOR_LIGHTLIGHTGREY 15
#define FONTCOLOR_BROWNISH 16
#define FONTCOLOR_RED 17


#define COLOR_WHITE 255

// Automap monster marker palette indices — defaults from slashdiablo BH Maphack.cpp (monsterColors map).
#define BH_AUTOMAP_MONSTER_COLOR_NORMAL 0x5B
#define BH_AUTOMAP_MONSTER_COLOR_MINION 0x60
#define BH_AUTOMAP_MONSTER_COLOR_CHAMPION 0x91
#define BH_AUTOMAP_MONSTER_COLOR_BOSS 0x84

#define CROSSCOLOR_SHRINE 31
#define CROSSCOLOR_IMPORTANT 132

#define MONSTER_MARKER_STYLE_CROSS 0
#define MONSTER_MARKER_STYLE_X 1

const int ActualBossTextFileNo[] = {
    570, 559, 550, 1166, 1164, 546, 544, 526, 704, 705, 706, 707, 708, 709, 743, 744, 746, 747, 748, 1149, 1148,
    753, 754, 755, 789, 799, 800, 809, 365, 826, 333, 861, 870, 879, 882, 883, 884, 893, 894, 1122, 1121,
    1120, 1119, 1118, 1117, 1116, 933, 934, 935, 1112, 938, 939, 229, 211, 963, 964, 1105, 1104, 1103, 990, 991, 996,
    997, 998, 1000, 1096, 156, 1094, 1093, 1092, 1044, 1058, 1060, 1061, 1062, 1063, 1064, 989, 256, 919, 1001, 750,
    936, 915, 250, 547, 1025, 1026, 1027, 242, 966, 749, 243, 267, 1183,
    1184, 1185, 1186, 1187, 1188, 1189};

const int InterestingMonsterTextFileNo[] = {
    945, // Treasure Fallen
    922, // Mendeln Event
    921, // Dark Wanderer
    993, // Shadow of Hate
    994, // Shadow of Terror
    995, // Shadow of Destruction
    1141, // Veiled Portal
    };

const int InterestingObjectTextFileNo[] = {
    593, // Spire of Darkness
    };

VOID DrawTextB(INT X, INT Y, DWORD Color, INT Font, INT Center, LPSTR Text, ...);
VOID DrawCross(INT X, INT Y, DWORD Color);
VOID DrawBox(INT X1, INT Y1, INT X2, INT Y2, INT LineColor);
VOID DrawCheckBox(INT X, INT Y, INT Size, INT FontSize, BOOL Checked, DWORD BoxColor, DWORD TextColor, LPSTR Text, ...);
VOID DrawIncreaseDecrease(INT X, INT Y, BOOL Increase, DWORD BoxColor);
VOID DrawTrackedEntitiesLabels();
VOID DrawNearbyEntities();
VOID DrawMonsterHealthPercent();
VOID DrawMonsterClassIds();
#endif