#ifndef _COLOURSELECTABLE_H_
#define _COLOURSELECTABLE_H_

#include <windows.h>
#include "Colours.h"

#define COLOURSELECTABLE_CLASSNAME "ColourSelectableCtrl"

typedef struct _ColourSelectable_Data
{
	Colour *current;
	Colour *mine;
} ColourSelectable_Data;

void ColourSelectable_RegisterClass(HINSTANCE hInstance);
HWND ColourSelectable_Create(HINSTANCE hInstance, int x, int y, int width, int height, HWND hParent, ColourSelectable_Data *csd);
LRESULT CALLBACK ColourSelectable_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ColourSelectable_OnPaint(HWND hWnd);

#endif
