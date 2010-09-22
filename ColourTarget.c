#include "ColourTarget.h"
#include "Colours.h"
#include "ColourPicker.h"

#include <windows.h>
#include <stdio.h>

static ATOM ColourTargetClass = 0;

#define R 0x00FF0000
#define W 0x00FFFFFF
#define B 0x00000000

UINT target[] = {	W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
									W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
									W, W, W, W, W, W, W, W, W, W, R, W, W, W, W, W, W, W, W, W,
									W, W, W, W, W, W, W, W, W, W, R, W, W, W, W, W, W, W, W, W,
									W, W, W, W, W, W, W, W, R, R, R, R, R, W, W, W, W, W, W, W,
									W, W, W, W, W, W, R, R, W, W, R, W, W, R, R, W, W, W, W, W,
									W, W, W, W, W, R, W, W, W, W, R, W, W, W, W, R, W, W, W, W,
									W, W, W, W, W, R, W, W, W, W, R, W, W, W, W, R, W, W, W, W,
									W, W, W, W, R, W, W, W, W, W, W, W, W, W, W, W, R, W, W, W,
									W, W, W, W, R, W, W, W, W, W, W, W, W, W, W, W, R, W, W, W,
									W, W, R, R, R, R, R, W, W, W, W, W, W, W, R, R, R, R, R, W,
									W, W, W, W, R, W, W, W, W, W, W, W, W, W, W, W, R, W, W, W,
									W, W, W, W, R, W, W, W, W, W, W, W, W, W, W, W, R, W, W, W,
									W, W, W, W, W, R, W, W, W, W, W, W, W, W, W, R, W, W, W, W,
									W, W, W, W, W, R, W, W, W, W, R, W, W, W, W, R, W, W, W, W,
									W, W, W, W, W, W, R, R, W, W, R, W, W, R, R, W, W, W, W, W,
									W, W, W, W, W, W, W, W, R, R, R, R, R, W, W, W, W, W, W, W,
									W, W, W, W, W, W, W, W, W, W, R, W, W, W, W, W, W, W, W, W,
									W, W, W, W, W, W, W, W, W, W, R, W, W, W, W, W, W, W, W, W,
									W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W };

UINT question[] = { W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, B, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, B, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, B, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, B, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, B, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, B, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, B, W, W, W, B, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, B, B, B, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
										W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W };

void ColourTarget_RegisterClass(HINSTANCE hInstance)
{
	WNDCLASS cls;
	
	if (ColourTargetClass != 0)
		return;

	cls.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	cls.lpfnWndProc = ColourTarget_WndProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.hInstance = hInstance;
	cls.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	cls.hCursor = LoadCursor (NULL, IDC_ARROW);
	cls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
	cls.lpszMenuName = NULL;
	cls.lpszClassName = COLOURTARGET_CLASSNAME;
	ColourTargetClass = RegisterClass (&cls);
}

HWND ColourTarget_Create(HINSTANCE hInstance, int x, int y, HWND hParent, ColourTarget_Data *ctd)
{
	HWND hWnd = NULL;
	RECT rect;
	ColourTarget_RegisterClass(hInstance);

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, COLOURTARGET_CLASSNAME, "",	WS_VISIBLE | WS_CHILD,
													x, y, 20, 20,	hParent, NULL, hInstance, NULL);
	if (!hWnd) return NULL;

	// Ensure the inner size is correct
	GetClientRect(hWnd, &rect);
	MoveWindow(hWnd, x, y, 20 + (20 - rect.right), 20 + (20 - rect.bottom), TRUE);

	SetWindowLong(hWnd, GWL_USERDATA, (long)ctd);

	return hWnd;
}

LRESULT CALLBACK ColourTarget_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			ColourTarget_OnPaint(hWnd);
			return 0;
		case WM_ERASEBKGND:
			return 0;
		case WM_LBUTTONDOWN:
		{
			ColourTarget_Data *ctd = (ColourTarget_Data*)GetWindowLong(hWnd, GWL_USERDATA);
			SetCapture(hWnd);
			SetCursor(LoadCursor(NULL, IDC_CROSS));
			ctd->is_picking = 1;
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			ColourTarget_Data *ctd = (ColourTarget_Data*)GetWindowLong(hWnd, GWL_USERDATA);
			COLORREF col;
			HDC hDC;
			POINT p;

			if (GetCapture() != hWnd) return 0;
			if (ctd->is_picking == 0) return 0;

			ReleaseCapture();
			ctd->is_picking = 0;

			p.x = (short)LOWORD(lParam); 
			p.y = (short)HIWORD(lParam);
			ClientToScreen(hWnd, &p);

			hDC = GetDC(HWND_DESKTOP);
			col = GetPixel(hDC, p.x, p.y);

			SetColourR(ctd->c, GetRValue(col));
			SetColourG(ctd->c, GetGValue(col));
			SetColourB(ctd->c, GetBValue(col));

			RedrawColourBlocks();
			ReleaseDC(HWND_DESKTOP, hDC);
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}
		default:
			return DefWindowProc (hWnd, message, wParam, lParam);
	}
}

void ColourTarget_OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hDC;
	HBITMAP hBM, hBMOld;
	BITMAPINFO bmi;
	int height, width;
	ColourTarget_Data *ctd;
	UINT * ptPixels;
	
	// Start painting
	BeginPaint(hWnd, &ps);
	
	// Find our area to paint
	GetClientRect(hWnd, &rect);

	// Find the height/width
	height = (rect.bottom - rect.top);
	width = (rect.right - rect.left);

	// Prep bitmap info
	ZeroMemory(&bmi,sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;

	// Off-screen DC
	hDC = CreateCompatibleDC(ps.hdc);
	hBM = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void **)&ptPixels, NULL, 0);
	hBMOld = (HBITMAP)SelectObject(hDC, hBM);

	ctd = (ColourTarget_Data*)GetWindowLong(hWnd, GWL_USERDATA);

	// Copy what mode we're in
	if (ctd->is_picking == 0)
		memcpy(ptPixels, target, sizeof(target));
	else
		memcpy(ptPixels, question, sizeof(question));

	// Copy data over to real DC
	BitBlt(ps.hdc, rect.left, rect.top, (rect.right - rect.left),
				 (rect.bottom - rect.top), hDC, rect.left, rect.top, SRCCOPY);
	SelectObject(hDC, hBMOld);
	DeleteObject(hBM);
	DeleteDC(hDC);

	// End painting
	EndPaint(hWnd, &ps);
}

