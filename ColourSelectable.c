#include "ColourSelectable.h"
#include "Colours.h"
#include "ColourPicker.h"

#include <windows.h>
#include <stdio.h>

static ATOM ColourSelectableClass = 0;

void ColourSelectable_RegisterClass(HINSTANCE hInstance)
{
	WNDCLASS cls;
	
	if (ColourSelectableClass != 0)
		return;

	cls.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	cls.lpfnWndProc = ColourSelectable_WndProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.hInstance = hInstance;
	cls.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	cls.hCursor = LoadCursor (NULL, IDC_HAND);
	cls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
	cls.lpszMenuName = NULL;
	cls.lpszClassName = COLOURSELECTABLE_CLASSNAME;
	ColourSelectableClass = RegisterClass (&cls);
}

HWND ColourSelectable_Create(HINSTANCE hInstance, int x, int y, int width, int height, HWND hParent, ColourSelectable_Data *csd)
{
	HWND hWnd = NULL;
	RECT rect;
	ColourSelectable_RegisterClass(hInstance);

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, COLOURSELECTABLE_CLASSNAME, "",	WS_VISIBLE | WS_CHILD,
													x, y, width, height,	hParent, NULL, hInstance, NULL);
	if (!hWnd) return NULL;

	// Ensure the inner size is correct
	GetClientRect(hWnd, &rect);
	MoveWindow(hWnd, x, y, width + (width - rect.right), height + (height - rect.bottom), TRUE);

	SetWindowLong(hWnd, GWL_USERDATA, (long)csd);

	return hWnd;
}

LRESULT CALLBACK ColourSelectable_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			ColourSelectable_OnPaint(hWnd);
			return 0;
		case WM_ERASEBKGND:
			return 0;
		case WM_LBUTTONDOWN:
		{
			SetCapture(hWnd);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			ColourSelectable_Data *csd = (ColourSelectable_Data*)GetWindowLong(hWnd, GWL_USERDATA);
			RECT me;
			POINT pt;

			if (GetCapture() != hWnd)
				return 0;

			ReleaseCapture();

			GetClientRect(hWnd, &me);
			pt.x = (short)LOWORD(lParam);
			pt.y = (short)HIWORD(lParam);
			if (!PtInRect(&me, pt))
				return 0;

			*csd->current = *csd->mine;
			RedrawColourBlocks();
			return 0;
		}
		default:
			return DefWindowProc (hWnd, message, wParam, lParam);
	}
}

void ColourSelectable_OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hDC;
	HBITMAP hBM, hBMOld;
	BITMAPINFO bmi;
	int x, y, height, width;
	UINT * ptPixels;
	Colour *c;
	ColourSelectable_Data *csd;
	
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

	csd = (ColourSelectable_Data*)GetWindowLong(hWnd, GWL_USERDATA);
	c = csd->mine;

	// Fill colour
	for (x = 0; x < width; x++)
		for (y = 0; y < height; y++)
			ptPixels[x + y * width] = RealRGB(c->r, c->g, c->b);

	// Copy data over to real DC
	BitBlt(ps.hdc, rect.left, rect.top, (rect.right - rect.left),
				 (rect.bottom - rect.top), hDC, rect.left, rect.top, SRCCOPY);
	SelectObject(hDC, hBMOld);
	DeleteObject(hBM);
	DeleteDC(hDC);

	// End painting
	EndPaint(hWnd, &ps);
}

