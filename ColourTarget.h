#ifndef _COLOURTARGET_H_
#define _COLOURTARGET_H_

#include <windows.h>
#include "Colours.h"

#define COLOURTARGET_CLASSNAME "ColourTargetCtrl"

typedef struct _ColourTarget_Data
{
	int is_picking;
	Colour *c;
} ColourTarget_Data;

void ColourTarget_RegisterClass(HINSTANCE hInstance);
HWND ColourTarget_Create(HINSTANCE hInstance, int x, int y, HWND hParent, ColourTarget_Data *ctd);
LRESULT CALLBACK ColourTarget_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ColourTarget_OnPaint(HWND hWnd);

#endif
