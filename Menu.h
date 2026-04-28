#pragma once

#include <vector>

struct MenuItem;

typedef void (*MenuCallback)();

enum MenuItemType {
    Checkbox,
    IntSlider,
    Button,
    Label,
    Category
};

struct MenuItem {
    MenuItem() : label(nullptr), type(Label), x(0), y(0), width(0), height(0), boolValue(nullptr), intValue(nullptr), intMin(0), intMax(0), callback(nullptr), indent(0), parentIndex(-1) {}
    const char* label;
    MenuItemType type;
    int x;
    int y;
    int width;
    int height;
    BOOL* boolValue;
    DWORD* intValue;
    int intMin;
    int intMax;
    MenuCallback callback;
    int indent;
    int parentIndex; // -1 for no parent
};

void InitMenu();
void DrawMenu();
bool HandleMenuClick(int mouseX, int mouseY);
void HandleMenuScroll(int delta);