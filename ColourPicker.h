#ifndef _COLOURPICKER_H_
#define _COLOURPICKER_H_

#include <windows.h>
#include "Colours.h"

typedef struct _Session
{
	RECT window;
	int square_mode;
	Colour current_colour;
	Colour prev_colours[8];
} Session;

BOOL CALLBACK MainDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void SetOverrideEditCtrlWndProc(HWND hWnd);
BOOL CALLBACK OverrideEditCtrlWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void FixWorkingDirectory(HINSTANCE hInstance);
void LoadSession(HWND hWnd);
void EndSession(HWND hWnd);
void RedrawColourBlocks();
void HandleUpDownScroll(HWND hWnd, int pos);
void AddCurrentColourToPrevious();
void LoadPreviousColours(HWND hWnd);
void SavePreviousColours(HWND hWnd);

#endif
