#ifndef _COLOURLINE_H_
#define _COLOURLINE_H_

#include <windows.h>
#include "Colours.h"

#define COLOURLINE_CLASSNAME "ColourLineCtrl"

#define LINE_MODE_H 0
#define LINE_MODE_S 1
#define LINE_MODE_V 2
#define LINE_MODE_R 3
#define LINE_MODE_G 4
#define LINE_MODE_B 5

typedef struct _ColourLine_Data 
{
	int mode;
	Colour *c;
} ColourLine_Data;

void ColourLine_RegisterClass(HINSTANCE hInstance);
HWND ColourLine_Create(HINSTANCE hInstance, int x, int y, int height, HWND hParent, ColourLine_Data *cld);
LRESULT CALLBACK ColourLine_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ColourLine_OnPaint(HWND hWnd);
void ColourLine_SetValue(HWND hWnd, int x);

#endif
