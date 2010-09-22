#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <Commdlg.h>

#include "ColourPicker.h"
#include "ColourSquare.h"
#include "ColourChannel.h"
#include "ColourLine.h"
#include "ColourTarget.h"
#include "ColourBlock.h"
#include "ColourSelectable.h"
#include "resource.h"

#define LINE_H 0
#define LINE_S 1
#define LINE_V 2
#define LINE_R 3
#define LINE_G 4
#define LINE_B 5

static Session session;

static HWND hColourSquare = NULL;
static HWND hColourChannel = NULL;
static HWND hColourSquareModes[6];
static HWND hColourLines[6];
static HWND hColourLineValues[6];
static HWND hColourLineValueUpDowns[6];
static HWND hCurrentColourLabel;
static HWND hCurrentColourBlock;
static HWND hCurrentColourHexLabel;
static HWND hCurrentColourHexValue;
static HWND hAddToPreviousButton;
static HWND hPreviousColours[8];
static HWND hColourPickLabel;
static HWND hColourPickTarget;
static HWND hLoadPreviousColours;
static HWND hSavePreviousColours;

static ColourSquare_Data csd;
static ColourChannel_Data ccd;
static ColourLine_Data clds[6];
static ColourTarget_Data ctd;
static ColourSelectable_Data csed[8];

static char *ColourSquare_Titles[6] = { "H", "S", "V", "R", "G", "B" };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	int i;
	HWND hWnd;
	MSG msg;
	WNDCLASS cls;
	RECT desktop_area;
	int win_x, win_y;
	HICON load,save;

	InitCommonControls();
	FixWorkingDirectory(hInstance);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &desktop_area, 0);

	// Register class for our window
	cls.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	cls.lpfnWndProc = MainDlgProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.hInstance = hInstance;
	cls.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	cls.hCursor = LoadCursor (NULL, IDC_ARROW);
	cls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
	cls.lpszMenuName = NULL;
	cls.lpszClassName = "ColourPicker";
	RegisterClass(&cls);

	// Create our window
	win_x = desktop_area.left + ((desktop_area.right - desktop_area.left) / 2) - 245;
	win_y = desktop_area.top + ((desktop_area.bottom - desktop_area.top) / 2) - 125;
	hWnd = CreateWindow(cls.lpszClassName, "Colour Picker", DS_CENTER | WS_MINIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU,
												win_x, win_y, 490, 251, NULL, NULL, hInstance, 0);
	GetWindowRect(hWnd, &session.window);

	// Add icon
	SetClassLong(hWnd, GCL_HICONSM, (LONG)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 16, 16, 0));
	SetClassLong(hWnd, GCL_HICON, (LONG)LoadIcon(hInstance, (LPCTSTR)IDI_MAIN));

	// Set modes
	clds[LINE_H].mode = LINE_MODE_H;
	clds[LINE_S].mode = LINE_MODE_S;
	clds[LINE_V].mode = LINE_MODE_V;
	clds[LINE_R].mode = LINE_MODE_R;
	clds[LINE_G].mode = LINE_MODE_G;
	clds[LINE_B].mode = LINE_MODE_B;

	// Add Controls
	hColourSquare = ColourSquare_Create(hInstance, 7, 7, hWnd, &csd);
	csd.c = &session.current_colour;
	hColourChannel = ColourChannel_Create(hInstance, 217, 7, 10, hWnd, &ccd);
	ccd.c = &session.current_colour;
	for (i = 0; i < 6; i++)
	{
		// Radio buttons
		hColourSquareModes[i] = CreateWindow("BUTTON", ColourSquare_Titles[i], 
																					WS_VISIBLE|BS_AUTORADIOBUTTON|WS_CHILD|(i==0?WS_GROUP|WS_TABSTOP:0),
																					243, (i * 22) + 9, 25, 15, hWnd, NULL, hInstance, 0);
		SendMessage(hColourSquareModes[i], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
	}
	for (i = 0; i < 6; i++)
	{
		// Lines
		clds[i].c = &session.current_colour;
		hColourLines[i] = ColourLine_Create(hInstance, 272, (i * 22) + 7, 15, hWnd, &clds[i]);

		// Line values
		hColourLineValues[i] = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "0", WS_VISIBLE|WS_CHILD|ES_NUMBER|WS_TABSTOP,
																						430, (i * 22) + 7, 45, 19, hWnd, NULL, hInstance, 0);
		SendMessage(hColourLineValues[i], WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
		SendMessage(hColourLineValues[i], EM_SETLIMITTEXT, (WPARAM) 3, 0);
		SetOverrideEditCtrlWndProc(hColourLineValues[i]);

		// Line value up/downs
		hColourLineValueUpDowns[i] = CreateWindow(UPDOWN_CLASS, NULL,
																							WS_VISIBLE|WS_CHILD|UDS_ALIGNRIGHT|UDS_SETBUDDYINT|UDS_ARROWKEYS,
																							0, 0, 0, 0, hWnd, NULL, hInstance, 0);
		SendMessage(hColourLineValueUpDowns[i], UDM_SETBUDDY, (WPARAM)hColourLineValues[i], 0);
	}
	// Current colour
	hCurrentColourLabel = CreateWindow("STATIC", "Current colour:", WS_VISIBLE|WS_CHILD, 258, 141, 
																			69, 20, hWnd, NULL, hInstance, 0);
	SendMessage(hCurrentColourLabel, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
	// Current colour block
	hCurrentColourBlock = ColourBlock_Create(hInstance, 271, 158, 43, 36, hWnd, &session.current_colour);

	// Add to previous button
	hAddToPreviousButton = CreateWindow("BUTTON", ">", WS_VISIBLE|WS_CHILD|WS_TABSTOP, 334, 151, 18, 23, hWnd, 
																				NULL, hInstance, 0);
	SendMessage(hAddToPreviousButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

	// Previous colours
	for (i = 0; i < 4; i++)
	{
		csed[i].current = &session.current_colour;
		csed[i].mine = &session.prev_colours[i];
		hPreviousColours[i] = ColourSelectable_Create(hInstance, 357 + (i * 25), 142, 18, 16, hWnd, &csed[i]);
	}
	for (i = 4; i < 8; i++)
	{
		csed[i].current = &session.current_colour;
		csed[i].mine = &session.prev_colours[i];
		hPreviousColours[i] = ColourSelectable_Create(hInstance, 357 + ((i-4) * 25), 165, 18, 16, hWnd, &csed[i]);
	}

	// Open previous colours
	hLoadPreviousColours = CreateWindow("BUTTON", "Open", WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_ICON, 457, 142, 20, 20, 
																				hWnd, NULL, hInstance, 0);
	load = LoadImage(hInstance, MAKEINTRESOURCE(IDI_OPEN), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	SendMessage(hLoadPreviousColours, BM_SETIMAGE, IMAGE_ICON, (LPARAM)load);

	// Save previous colours
	hSavePreviousColours = CreateWindow("BUTTON", "Save", WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_ICON, 457, 165, 20, 20, 
																				hWnd, NULL, hInstance, 0);
	save = LoadImage(hInstance, MAKEINTRESOURCE(IDI_SAVE), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	SendMessage(hSavePreviousColours, BM_SETIMAGE, IMAGE_ICON, (LPARAM)save);

	// Hex Label
	hCurrentColourHexLabel = CreateWindow("STATIC", "Hex:", WS_VISIBLE|WS_CHILD, 257, 202, 32, 20, hWnd, NULL, hInstance, 0);
	SendMessage(hCurrentColourHexLabel, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
	// Hex Value
	hCurrentColourHexValue = CreateWindow("EDIT", "#FF0000", WS_VISIBLE|WS_CHILD|ES_READONLY,
																				282, 202, 65, 19, hWnd, NULL, hInstance, 0);
	SendMessage(hCurrentColourHexValue, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

	// Colour Target
	ctd.c = &session.current_colour;
	hColourPickLabel = CreateWindow("STATIC", "Screen Pick:", WS_VISIBLE|WS_CHILD,
																		372, 199, 65, 23, hWnd, NULL, hInstance, 0);
	SendMessage(hColourPickLabel, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
	hColourPickTarget = ColourTarget_Create(hInstance, 442, 194, hWnd, &ctd);

	// UpDown ranges
	SendMessage(hColourLineValueUpDowns[LINE_H], UDM_SETRANGE, 0, MAKELONG(359, 0));
	SendMessage(hColourLineValueUpDowns[LINE_S], UDM_SETRANGE, 0, MAKELONG(100, 0));
	SendMessage(hColourLineValueUpDowns[LINE_V], UDM_SETRANGE, 0, MAKELONG(100, 0));
	SendMessage(hColourLineValueUpDowns[LINE_R], UDM_SETRANGE, 0, MAKELONG(255, 0));
	SendMessage(hColourLineValueUpDowns[LINE_G], UDM_SETRANGE, 0, MAKELONG(255, 0));
	SendMessage(hColourLineValueUpDowns[LINE_B], UDM_SETRANGE, 0, MAKELONG(255, 0));

	// Load last session
	LoadSession(hWnd);

	// Set some controls to what the session info is
	SendMessage(hColourSquareModes[session.square_mode], BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(hColourLineValueUpDowns[LINE_H], UDM_SETPOS32, 0, (LPARAM)session.current_colour.h);
	SendMessage(hColourLineValueUpDowns[LINE_S], UDM_SETPOS32, 0, (LPARAM)session.current_colour.s);
	SendMessage(hColourLineValueUpDowns[LINE_V], UDM_SETPOS32, 0, (LPARAM)session.current_colour.v);
	SendMessage(hColourLineValueUpDowns[LINE_R], UDM_SETPOS32, 0, (LPARAM)session.current_colour.r);
	SendMessage(hColourLineValueUpDowns[LINE_G], UDM_SETPOS32, 0, (LPARAM)session.current_colour.g);
	SendMessage(hColourLineValueUpDowns[LINE_B], UDM_SETPOS32, 0, (LPARAM)session.current_colour.b);

	// Show the window finally
	ShowWindow(hWnd, SW_SHOW);

	// Wait for messages
	while (GetMessage(&msg, hWnd, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if (!IsWindow(hWnd)) break;
	}

	return 0;
}

BOOL CALLBACK MainDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_CLOSE:
		{
			EndSession(hWnd);
				return 0;
		}

		case WM_WINDOWPOSCHANGED:
		{
			if (!IsIconic(hWnd))
				GetWindowRect(hWnd, &session.window);
			return 0;
		}

		case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED)
			{
				int old_mode = csd.mode;

				if ((HWND)lParam == hColourSquareModes[LINE_H])
					ccd.mode = csd.mode = SQUARE_MODE_H;
				else if ((HWND)lParam == hColourSquareModes[LINE_S])
					ccd.mode = csd.mode = SQUARE_MODE_S;
				else if ((HWND)lParam == hColourSquareModes[LINE_V])
					ccd.mode = csd.mode = SQUARE_MODE_V;
				else if ((HWND)lParam == hColourSquareModes[LINE_R])
					ccd.mode = csd.mode = SQUARE_MODE_R;
				else if ((HWND)lParam == hColourSquareModes[LINE_G])
					ccd.mode = csd.mode = SQUARE_MODE_G;
				else if ((HWND)lParam == hColourSquareModes[LINE_B])
					ccd.mode = csd.mode = SQUARE_MODE_B;
				else if ((HWND)lParam == hAddToPreviousButton)
					AddCurrentColourToPrevious();
				else if ((HWND)lParam == hLoadPreviousColours)
					LoadPreviousColours(hWnd);
				else if ((HWND)lParam == hSavePreviousColours)
					SavePreviousColours(hWnd);

				session.square_mode = ccd.mode;

				if (old_mode != csd.mode)
					RedrawColourBlocks();
			}
			break;
		}

		case WM_VSCROLL:
		{
			if (LOWORD(wParam) != SB_ENDSCROLL)
				HandleUpDownScroll((HWND)lParam, HIWORD(wParam));
			break;
		}
	}
	 
	return DefWindowProc (hWnd, Message, wParam, lParam);
}

void SetOverrideEditCtrlWndProc(HWND hWnd)
{
	SetWindowLong(hWnd, GWL_USERDATA, (long)GetWindowLong(hWnd, GWL_WNDPROC));
	SetWindowLong(hWnd, GWL_WNDPROC, (long)OverrideEditCtrlWndProc);
}

BOOL CALLBACK OverrideEditCtrlWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;

		case WM_SETFOCUS:
			SendMessage(hWnd, EM_SETSEL, 0, -1);
			break;

		case WM_CHAR:
			if (wParam == VK_TAB)
			{
				SetFocus(GetNextDlgTabItem(GetParent(hWnd), hWnd, FALSE));
				return 0;
			}
			else if (wParam == VK_RETURN)
			{
				int val;
				char text[4];
				GetWindowText(hWnd, text, 4);
				val = atoi(text);

				if (hColourLineValues[LINE_H] == hWnd)
					SetColourH(&session.current_colour, val);
				else if (hColourLineValues[LINE_S] == hWnd)
					SetColourS(&session.current_colour, val);
				else if (hColourLineValues[LINE_V] == hWnd)
					SetColourV(&session.current_colour, val);
				else if (hColourLineValues[LINE_R] == hWnd)
					SetColourR(&session.current_colour, val);
				else if (hColourLineValues[LINE_G] == hWnd)
					SetColourG(&session.current_colour, val);
				else if (hColourLineValues[LINE_B] == hWnd)
					SetColourB(&session.current_colour, val);

				RedrawColourBlocks();
				return 0;
			}
			break;
	}

	return CallWindowProc((WNDPROC)GetWindowLong(hWnd, GWL_USERDATA), hWnd, Message, wParam, lParam);
}

void FixWorkingDirectory(HINSTANCE hInstance)
{
	char buffer[_MAX_PATH], *slash = NULL;
	unsigned int characters_copied = 0;
	unsigned int buffer_length = _MAX_PATH;
	
	// Get full filename
	characters_copied = GetModuleFileName(hInstance, buffer, buffer_length - 1);
	buffer[characters_copied] = 0;

	// Remove exe name
	slash = strrchr(buffer, '\\');
	if (slash)
		*(slash+1) = 0;

	SetCurrentDirectory(buffer);
	free(buffer);
}
	
void LoadSession(HWND hWnd)
{
	FILE *f;
	RECT screen;
	int x,y;

	// Defaults
	session.current_colour.h = 0;
	session.current_colour.s = 100;
	session.current_colour.v = 100;
	session.current_colour.r = 255;
	session.current_colour.g = 0;
	session.current_colour.b = 0;
	csd.mode = ccd.mode = SQUARE_MODE_H;

	// Open session file
	f = fopen("ColourPicker.dat", "r");
	if (!f) return;

	// Read contents
	fread(&session, sizeof(session), 1, f);
	fclose(f);
	
	// Find desktop work area
	SystemParametersInfo(SPI_GETWORKAREA, 0, &screen, 0);

	// Work out the co-ords
	x = session.window.left + screen.left;
	y = session.window.top + screen.top;

	// Reset window position
	SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE|SWP_NOREDRAW);

	// Colour mode
	csd.mode = ccd.mode = session.square_mode;

	RedrawColourBlocks();
}

void EndSession(HWND hWnd)
{
	FILE *f;
	RECT screen;

	// Find desktop work area
	SystemParametersInfo(SPI_GETWORKAREA, 0, &screen, 0);

	// Fix the co-ords
	session.window.left -= screen.left;
	session.window.top -= screen.top;
	
	// Save session to file
	f = fopen("ColourPicker.dat", "w");
	if (f)
	{
		fwrite(&session, sizeof(session), 1, f);
		fclose(f);
	}

	// Kill window
	DestroyWindow(hWnd);
}

void RedrawColourBlocks()
{
	int i;
	char hex[8];

	// Main colour square
	InvalidateRect(hColourSquare, NULL, FALSE);
	//ColourSquare_OnPaint(hColourSquare);
	
	// Colour channel
	InvalidateRect(hColourChannel, NULL, FALSE);

	// Colour lines
	for (i = 0; i < 6; i++)
		InvalidateRect(hColourLines[i], NULL, FALSE);

	// Ensure up down positios are correct
	SendMessage(hColourLineValueUpDowns[LINE_H], UDM_SETPOS32, 0, (LPARAM)session.current_colour.h);
	SendMessage(hColourLineValueUpDowns[LINE_S], UDM_SETPOS32, 0, (LPARAM)session.current_colour.s);
	SendMessage(hColourLineValueUpDowns[LINE_V], UDM_SETPOS32, 0, (LPARAM)session.current_colour.v);
	SendMessage(hColourLineValueUpDowns[LINE_R], UDM_SETPOS32, 0, (LPARAM)session.current_colour.r);
	SendMessage(hColourLineValueUpDowns[LINE_G], UDM_SETPOS32, 0, (LPARAM)session.current_colour.g);
	SendMessage(hColourLineValueUpDowns[LINE_B], UDM_SETPOS32, 0, (LPARAM)session.current_colour.b);

	// Hex readout
	_snprintf(hex, 8, "#%.2X%.2X%.2X", session.current_colour.r, session.current_colour.g, session.current_colour.b);
	SetWindowText(hCurrentColourHexValue, hex);

	// Colour block
	InvalidateRect(hCurrentColourBlock, NULL, FALSE);

	// Previous colours
	for (i = 0; i < 8; i++)
		InvalidateRect(hPreviousColours[i], NULL, FALSE);
}

void HandleUpDownScroll(HWND hWnd, int pos)
{
	if (hColourLineValueUpDowns[LINE_H] == hWnd)
		SetColourH(&session.current_colour, pos);
	else if (hColourLineValueUpDowns[LINE_S] == hWnd)
		SetColourS(&session.current_colour, pos);
	else if (hColourLineValueUpDowns[LINE_V] == hWnd)
		SetColourV(&session.current_colour, pos);
	else if (hColourLineValueUpDowns[LINE_R] == hWnd)
		SetColourR(&session.current_colour, pos);
	else if (hColourLineValueUpDowns[LINE_G] == hWnd)
		SetColourG(&session.current_colour, pos);
	else if (hColourLineValueUpDowns[LINE_B] == hWnd)
		SetColourB(&session.current_colour, pos);

	RedrawColourBlocks();
}

void AddCurrentColourToPrevious()
{
	int i;
	for (i = 7; i > 0; i--)
		session.prev_colours[i] = session.prev_colours[i-1];

	session.prev_colours[0] = session.current_colour;

	RedrawColourBlocks();
}

void LoadPreviousColours(HWND hWnd)
{
	OPENFILENAME ofn;
	char filename[_MAX_PATH];
	char init_dir[_MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(&filename, sizeof(filename));
	GetCurrentDirectory(_MAX_PATH, init_dir);
		
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "Colour Picker Files\0*.cst\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename;
	ofn.lpstrInitialDir = init_dir;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = "Open Colour Set";
	ofn.lpstrDefExt = "cst";
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;	
	
	if (GetOpenFileName(&ofn) == TRUE)
	{
		FILE *f = fopen(ofn.lpstrFile, "r");
		if (!f)
		{
			MessageBox(hWnd, "Couldn't open the selected file for reading.", "Error", MB_OK|MB_ICONERROR);
			return;
		}

		fread(&session.prev_colours, sizeof(session.prev_colours), 1, f);
		fclose(f);
		RedrawColourBlocks();
		MessageBox(hWnd, "Loaded the current previous colours successfully.", "Saved", MB_OK);
	}
}

void SavePreviousColours(HWND hWnd)
{
	OPENFILENAME ofn;
	char filename[_MAX_PATH];
	char init_dir[_MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(&filename, sizeof(filename));
	GetCurrentDirectory(_MAX_PATH, init_dir);
		
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "Colour Picker Files\0*.cst\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename;
	ofn.lpstrInitialDir = init_dir;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = "Save Colour Set";
	ofn.lpstrDefExt = "cst";
	ofn.Flags = OFN_HIDEREADONLY;	
	
	if (GetSaveFileName(&ofn) == TRUE)
	{
		FILE *f = fopen(ofn.lpstrFile, "w");
		if (!f)
		{
			MessageBox(hWnd, "Couldn't open the selected file for writing.", "Error", MB_OK|MB_ICONERROR);
			return;
		}

		fwrite(&session.prev_colours, sizeof(session.prev_colours), 1, f);
		fclose(f);
		MessageBox(hWnd, "Saved the current previous colours successfully.", "Saved", MB_OK);
	}
}


