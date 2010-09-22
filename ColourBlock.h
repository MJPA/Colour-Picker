#ifndef _COLOURBLOCK_H_
#define _COLOURBLOCK_H_

#include <windows.h>
#include "Colours.h"

#define COLOURBLOCK_CLASSNAME "ColourBlockCtrl"

void ColourBlock_RegisterClass(HINSTANCE hInstance);
HWND ColourBlock_Create(HINSTANCE hInstance, int x, int y, int width, int height, HWND hParent, Colour *c);
LRESULT CALLBACK ColourBlock_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ColourBlock_OnPaint(HWND hWnd);

#endif
