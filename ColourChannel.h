#ifndef _COLOURCHANNEL_H_
#define _COLOURCHANNEL_H_

#include <windows.h>
#include "Colours.h"

#define COLOURCHANNEL_CLASSNAME "ColourChannelCtrl"

#define CHANNEL_MODE_H 0
#define CHANNEL_MODE_S 1
#define CHANNEL_MODE_V 2
#define CHANNEL_MODE_R 3
#define CHANNEL_MODE_G 4
#define CHANNEL_MODE_B 5

typedef struct _ColourChannel_Data 
{
	int mode;
	Colour *c;
} ColourChannel_Data;

void ColourChannel_RegisterClass(HINSTANCE hInstance);
HWND ColourChannel_Create(HINSTANCE hInstance, int x, int y, int width, HWND hParent, ColourChannel_Data *ccd);
LRESULT CALLBACK ColourChannel_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ColourChannel_OnPaint(HWND hWnd);
void ColourChannel_SetValue(HWND hWnd, int y);

#endif
