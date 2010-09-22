#include "ColourChannel.h"
#include "Colours.h"
#include "ColourPicker.h"

#include <windows.h>

static ATOM ColourChannelClass = 0;

void ColourChannel_RegisterClass(HINSTANCE hInstance)
{
	WNDCLASS cls;
	
	if (ColourChannelClass != 0)
		return;

	cls.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	cls.lpfnWndProc = ColourChannel_WndProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.hInstance = hInstance;
	cls.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	cls.hCursor = LoadCursor (NULL, IDC_ARROW);
	cls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
	cls.lpszMenuName = NULL;
	cls.lpszClassName = COLOURCHANNEL_CLASSNAME;
	ColourChannelClass = RegisterClass (&cls);
}

HWND ColourChannel_Create(HINSTANCE hInstance, int x, int y, int width, HWND hParent, ColourChannel_Data *ccd)
{
	HWND hWnd = NULL;
	RECT rect;
	ColourChannel_RegisterClass(hInstance);

	hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME|WS_EX_CLIENTEDGE, COLOURCHANNEL_CLASSNAME, "",	WS_VISIBLE | WS_CHILD,
													x, y, width, 200,	hParent, NULL, hInstance, NULL);
	if (!hWnd) return NULL;

	// Ensure the inner size is correct
	GetClientRect(hWnd, &rect);
	MoveWindow(hWnd, x, y, width + (width - rect.right), 200 + (200 - rect.bottom), TRUE);

	SetWindowLong(hWnd, GWL_USERDATA, (long)ccd);

	return hWnd;
}

LRESULT CALLBACK ColourChannel_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			ColourChannel_OnPaint(hWnd);
			return 0;
		case WM_ERASEBKGND:
			return 0;
		case WM_MOUSEMOVE:
			if ((wParam & MK_LBUTTON) && (GetCapture() == hWnd))
				ColourChannel_SetValue(hWnd, (short)HIWORD(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			SetCapture(hWnd);
			ColourChannel_SetValue(hWnd, HIWORD(lParam));
			return 0;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return 0;
		default:	
			return DefWindowProc (hWnd, message, wParam, lParam);
	}
}

void ColourChannel_OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hDC;
	HBITMAP hBM, hBMOld;
	BITMAPINFO bmi;
	int height, width, x, y, slider_pos;
	ColourChannel_Data *ccd;
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

	ccd = (ColourChannel_Data*)GetWindowLong(hWnd, GWL_USERDATA);

	for (y = 0; y < height; y++)
	{
		COLORREF col;
		switch (ccd->mode)
		{
			case CHANNEL_MODE_H:
			{
				col = HSVtoRGB(y * 359 / (height-1), 100, 100);
				slider_pos = ccd->c->h * (height-1) / 359;
				break;
			}
			case CHANNEL_MODE_S:
			{
				col = RealRGB(y * 255 / (height-1), y * 255 / (height-1), y * 255 / (height-1));
				slider_pos = ccd->c->s * (height-1) / 100;
				break;
			}
			case CHANNEL_MODE_V:
			{
				col = RealRGB(y * 255 / (height-1), y * 255 / (height-1), y * 255 / (height-1));
				slider_pos = ccd->c->v * (height-1) / 100;
				break;
			}
			case CHANNEL_MODE_R:
			{
				col = RealRGB(y * 255 / (height-1), 0, 0);
				slider_pos = ccd->c->r * (height-1) / 255;
				break;
			}
			case CHANNEL_MODE_G:
			{
				col = RealRGB(0, y * 255 / (height-1), 0);
				slider_pos = ccd->c->g * (height-1) / 255;
				break;
			}
			case CHANNEL_MODE_B:
			{
				col = RealRGB(0, 0, y * 255 / (height-1));
				slider_pos = ccd->c->b * (height-1) / 255;
				break;
			}
		}

		for (x = 0; x < width; x++)
			ptPixels[x + y * width] = col;
	}

	// Left slider bit
	for (y = slider_pos - 2; y <= slider_pos + 2; y++)
	{
		if (y < 0 || y >= height) continue;
		ptPixels[y * width] = 0x00000000;
	}
	for (y = slider_pos - 1; y <= slider_pos + 1; y++)
	{
		if (y < 0 || y >= height) continue;
		ptPixels[y * width + 1] = 0x00000000;
	}
	ptPixels[slider_pos * width + 2] = 0x00000000;
	ptPixels[slider_pos * width + 3] = 0x00000000;

	// Right slider bit
	for (y = slider_pos - 2; y <= slider_pos + 2; y++)
	{
		if (y < 0 || y >= height) continue;
		ptPixels[(width - 1) + y * width] = 0x00FFFFFF;
	}
	for (y = slider_pos - 1; y <= slider_pos + 1; y++)
	{
		if (y < 0 || y >= height) continue;
		ptPixels[(width - 2) + y * width] = 0x00FFFFFF;
	}
	ptPixels[(width - 3) + slider_pos * width] = 0x00FFFFFF;
	ptPixels[(width - 4) + slider_pos * width] = 0x00FFFFFF;

	// Copy data over to real DC
	BitBlt(ps.hdc, rect.left, rect.top, (rect.right - rect.left),
				 (rect.bottom - rect.top), hDC, rect.left, rect.top, SRCCOPY);
	SelectObject(hDC, hBMOld);
	DeleteObject(hBM);
	DeleteDC(hDC);

	// End painting
	EndPaint(hWnd, &ps);
}

void ColourChannel_SetValue(HWND hWnd, int y)
{
	ColourChannel_Data *ccd;
	RECT rect;
	int height,changed;
		
	// Find our area to paint
	GetClientRect(hWnd, &rect);
	height = rect.bottom - 1;

	// Out of range?
	if (y < 0)
		y = 0;
	if (y > height)
		y = height;
	
	ccd = (ColourChannel_Data*)GetWindowLong(hWnd, GWL_USERDATA);
	y = height - y;

	switch (ccd->mode)
	{
		case CHANNEL_MODE_H:
			changed = SetColourH(ccd->c, y * 359 / height);
			break;
		case CHANNEL_MODE_S:
			changed = SetColourS(ccd->c, y * 100 / height);
			break;
		case CHANNEL_MODE_V:
			changed = SetColourV(ccd->c, y * 100 / height);
			break;
		case CHANNEL_MODE_R:
			changed = SetColourR(ccd->c, y * 255 / height);
			break;
		case CHANNEL_MODE_G:
			changed = SetColourG(ccd->c, y * 255 / height);
			break;
		case CHANNEL_MODE_B:
			changed = SetColourB(ccd->c, y * 255 / height);
			break;
	}

	if (changed == 1)
		RedrawColourBlocks();
}

