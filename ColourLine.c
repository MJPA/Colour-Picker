#include "ColourLine.h"
#include "Colours.h"
#include "ColourPicker.h"

#include <windows.h>

static ATOM ColourLineClass = 0;

void ColourLine_RegisterClass(HINSTANCE hInstance)
{
	WNDCLASS cls;
	
	if (ColourLineClass != 0)
		return;

	cls.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	cls.lpfnWndProc = ColourLine_WndProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.hInstance = hInstance;
	cls.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	cls.hCursor = LoadCursor (NULL, IDC_ARROW);
	cls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
	cls.lpszMenuName = NULL;
	cls.lpszClassName = COLOURLINE_CLASSNAME;
	ColourLineClass = RegisterClass (&cls);
}

HWND ColourLine_Create(HINSTANCE hInstance, int x, int y, int height, HWND hParent, ColourLine_Data *cld)
{
	HWND hWnd = NULL;
	RECT rect;
	ColourLine_RegisterClass(hInstance);

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, COLOURLINE_CLASSNAME, "",	WS_VISIBLE | WS_CHILD, x, y, 150, height,
												hParent, NULL, hInstance, NULL);
	if (!hWnd) return NULL;

	// Ensure the inner size is correct
	GetClientRect(hWnd, &rect);
	MoveWindow(hWnd, x, y, 150 + (150 - rect.right), height + (height - rect.bottom), TRUE);

	SetWindowLong(hWnd, GWL_USERDATA, (long)cld);

	return hWnd;
}

LRESULT CALLBACK ColourLine_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			ColourLine_OnPaint(hWnd);
			return 0;
		case WM_ERASEBKGND:
			return 0;
		case WM_MOUSEMOVE:
			if ((wParam & MK_LBUTTON) && (GetCapture() == hWnd))
				ColourLine_SetValue(hWnd, (short)LOWORD(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			SetCapture(hWnd);
			ColourLine_SetValue(hWnd, LOWORD(lParam));
			return 0;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return 0;
		default:	
			return DefWindowProc (hWnd, message, wParam, lParam);
	}
}

void ColourLine_OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hDC;
	HBITMAP hBM, hBMOld;
	BITMAPINFO bmi;
	int height, width, x, y, slider_pos;
	ColourLine_Data *cld;
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

	cld = (ColourLine_Data*)GetWindowLong(hWnd, GWL_USERDATA);

	for (x = 0; x < width; x++)
	{
		COLORREF col;
		switch (cld->mode)
		{
			case LINE_MODE_H:
				col = HSVtoRGB(x * 359 / (width-1), cld->c->s, cld->c->v);
				slider_pos = cld->c->h * (width-1) / 359;
				break;
			case LINE_MODE_S:
				col = HSVtoRGB(cld->c->h, x * 100 / (width-1), cld->c->v);
				slider_pos = cld->c->s * (width-1) / 100;
				break;
			case LINE_MODE_V:
				col = HSVtoRGB(cld->c->h, cld->c->s, x * 100 / (width-1));
				slider_pos = cld->c->v * (width-1) / 100;
				break;
			case LINE_MODE_R:
				col = RealRGB(x * 255 / (width-1), cld->c->g, cld->c->b);
				slider_pos = cld->c->r * (width-1) / 255;
				break;
			case LINE_MODE_G:
				col = RealRGB(cld->c->r, x * 255 / (width-1), cld->c->b);
				slider_pos = cld->c->g * (width-1) / 255;
				break;
			case LINE_MODE_B:
				col = RealRGB(cld->c->r, cld->c->g, x * 255 / (width-1));
				slider_pos = cld->c->b * (width-1) / 255;
				break;
		}

		for (y = 0; y < height; y++)
			ptPixels[x + width * y] = col;
	}

	// Top slider bit
	for (x = slider_pos - 2; x <= slider_pos + 2; x++)
	{
		if (x < 0 || x >= width) continue;
		ptPixels[x + (height-1) * width] = 0x00000000;
	}
	for (x = slider_pos - 1; x <= slider_pos + 1; x++)
	{
		if (x < 0 || x >= width) continue;
		ptPixels[x + (height-2) * width] = 0x00000000;
	}
	ptPixels[slider_pos + (height-3) * width] = 0x00000000;
	ptPixels[slider_pos + (height-4) * width] = 0x00000000;

	// Bottom slider bit
	for (x = slider_pos - 2; x <= slider_pos + 2; x++)
	{
		if (x < 0 || x >= width) continue;
		ptPixels[x] = 0x00FFFFFF;
	}
	for (x = slider_pos - 1; x <= slider_pos + 1; x++)
	{
		if (x < 0 || x >= width) continue;
		ptPixels[x + width] = 0x00FFFFFF;
	}
	ptPixels[slider_pos + 2 * width] = 0x00FFFFFF;
	ptPixels[slider_pos + 3 * width] = 0x00FFFFFF;

	// Copy data over to real DC
	BitBlt(ps.hdc, rect.left, rect.top, (rect.right - rect.left),
				 (rect.bottom - rect.top), hDC, rect.left, rect.top, SRCCOPY);
	SelectObject(hDC, hBMOld);
	DeleteObject(hBM);
	DeleteDC(hDC);

	// End painting
	EndPaint(hWnd, &ps);
}

void ColourLine_SetValue(HWND hWnd, int x)
{
	ColourLine_Data *cld;
	RECT rect;
	int width, changed;
		
	// Find our area to paint
	GetClientRect(hWnd, &rect);
	width = rect.right - 1;
	
	// Out of range?
	if (x < 0)
		x = 0;
	if (x > width)
		x = width;
	
	cld = (ColourLine_Data*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (cld->mode)
	{
		case LINE_MODE_H:
			changed = SetColourH(cld->c, x * 359 / width);
			break;
		case LINE_MODE_S:
			changed = SetColourS(cld->c, x * 100 / width);
			break;
		case LINE_MODE_V:
			changed = SetColourV(cld->c, x * 100 / width);
			break;
		case LINE_MODE_R:
			changed = SetColourR(cld->c, x * 255 / width);
			break;
		case LINE_MODE_G:
			changed = SetColourG(cld->c, x * 255 / width);
			break;
		case LINE_MODE_B:
			changed = SetColourB(cld->c, x * 255 / width);
			break;
	}

	if (changed == 1)
		RedrawColourBlocks();
}

