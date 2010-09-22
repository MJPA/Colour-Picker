#ifndef _COLOURSQUARE_H_
#define _COLOURSQUARE_H_

#include <windows.h>
#include "Colours.h"

#define COLOURSQUARE_CLASSNAME "ColourSquareCtrl"

#define SQUARE_MODE_H 0
#define SQUARE_MODE_S 1
#define SQUARE_MODE_V 2
#define SQUARE_MODE_R 3
#define SQUARE_MODE_G 4
#define SQUARE_MODE_B 5

typedef struct _ColourSquare_Data 
{
	int mode;
	Colour *c;
} ColourSquare_Data;

void ColourSquare_RegisterClass(HINSTANCE hInstance);
HWND ColourSquare_Create(HINSTANCE hInstance, int x, int y, HWND hParent, ColourSquare_Data *csd);
LRESULT CALLBACK ColourSquare_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ColourSquare_OnPaint(HWND hWnd);
void ColourSquare_SetValue(HWND hWnd, int x, int y);

#endif
