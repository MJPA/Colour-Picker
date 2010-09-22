#include "ColourSquare.h"
#include "Colours.h"
#include "ColourPicker.h"

static ATOM ColourSquareClass = 0;

void ColourSquare_RegisterClass(HINSTANCE hInstance)
{
	WNDCLASS cls;
	
	if (ColourSquareClass != 0)
		return;

	cls.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	cls.lpfnWndProc = ColourSquare_WndProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.hInstance = hInstance;
	cls.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	cls.hCursor = LoadCursor (NULL, IDC_ARROW);
	cls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
	cls.lpszMenuName = NULL;
	cls.lpszClassName = COLOURSQUARE_CLASSNAME;
	ColourSquareClass = RegisterClass (&cls);
}

HWND ColourSquare_Create(HINSTANCE hInstance, int x, int y, HWND hParent, ColourSquare_Data *csd)
{
	HWND hWnd = NULL;
	RECT rect;
	ColourSquare_RegisterClass(hInstance);

	hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE, COLOURSQUARE_CLASSNAME, "",
												WS_VISIBLE | WS_CHILD, x, y, 200, 200, hParent, NULL, hInstance, NULL);
	if (!hWnd) return NULL;

	// Ensure the inner size is correct
	GetClientRect(hWnd, &rect);
	MoveWindow(hWnd, x, y, 200 + (200 - rect.right), 200 + (200 - rect.bottom), TRUE);

	SetWindowLong(hWnd, GWL_USERDATA, (long)csd);

	return hWnd;
}

LRESULT CALLBACK ColourSquare_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
			ColourSquare_OnPaint(hWnd);
			return 0;
		case WM_ERASEBKGND:
			return 0;
		case WM_MOUSEMOVE:
			if ((wParam & MK_LBUTTON) && (GetCapture() == hWnd))
				ColourSquare_SetValue(hWnd, (short)LOWORD(lParam), (short)HIWORD(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			SetCapture(hWnd);
			ColourSquare_SetValue(hWnd, LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return 0;
		default:	
			return DefWindowProc (hWnd, message, wParam, lParam);
	}
}

void ColourSquare_OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hDC;
	HBITMAP hBM, hBMOld;
	UINT * ptPixels;
	BITMAPINFO bmi;
	int height, width, x, y;
	ColourSquare_Data *csd;

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

	csd = (ColourSquare_Data*)GetWindowLong(hWnd, GWL_USERDATA);

	// Draw square
	for (x = 0; x < width; x++)
	{
		for (y = 0; y < height; y++)
		{
			switch (csd->mode)
			{
				case SQUARE_MODE_H:
					ptPixels[x + y*width] = HSVtoRGB(csd->c->h, y * 100 / (height-1), x * 100 / (width-1));
					break;
				case SQUARE_MODE_S:
					ptPixels[x + y*width] = HSVtoRGB(y * 359 / (height-1), csd->c->s, x * 100 / (width-1));
					break;
				case SQUARE_MODE_V:
					ptPixels[x + y*width] = HSVtoRGB(y * 359 / (height-1), x * 100 / (width-1), csd->c->v);
					break;
				case SQUARE_MODE_R:
					ptPixels[x + y*width] = RealRGB(csd->c->r, y * 255 / (height-1), x * 255 / (width-1));
					break;
				case SQUARE_MODE_G:
					ptPixels[x + y*width] = RealRGB(y * 255 / (height-1), csd->c->g, x * 255 / (width-1));
					break;
				case SQUARE_MODE_B:
					ptPixels[x + y*width] = RealRGB(y * 255 / (height-1), x * 255 / (width-1), csd->c->b);
					break;
			}
		}
	}

	// Draw the grid lines
	switch (csd->mode)
	{
		case SQUARE_MODE_H:
		{
			for (x = 0; x < width; x++)
				Invert(ptPixels[x + (csd->c->s * (height-1) / 100) * width]);
			for (y = 0; y < height; y++)
				Invert(ptPixels[(csd->c->v * (width-1) / 100) + y * width]);
			break;
		}
		case SQUARE_MODE_S:
		{
			for (x = 0; x < width; x++)
				Invert(ptPixels[x + (csd->c->h * (height-1) / 359) * width]);
			for (y = 0; y < height; y++)
				Invert(ptPixels[(csd->c->v * (width-1) / 100) + y * width]);
			break;
		}
		case SQUARE_MODE_V:
		{
			for (x = 0; x < width; x++)
				Invert(ptPixels[x + (csd->c->h * (height-1) / 359) * width]);
			for (y = 0; y < height; y++)
				Invert(ptPixels[(csd->c->s * (width-1) / 100) + y * width]);
			break;
		}
		case SQUARE_MODE_R:
		{
			for (x = 0; x < width; x++)
				Invert(ptPixels[x + (csd->c->g * (height-1) / 255) * width]);
			for (y = 0; y < height; y++)
				Invert(ptPixels[(csd->c->b * (width-1) / 255) + y * width]);
			break;
		}
		case SQUARE_MODE_G:
		{
			for (x = 0; x < width; x++)
				Invert(ptPixels[x + (csd->c->r * (height-1) / 255) * width]);
			for (y = 0; y < height; y++)
				Invert(ptPixels[(csd->c->b * (width-1) / 255) + y * width]);
			break;
		}
		case SQUARE_MODE_B:
		{
			for (x = 0; x < width; x++)
				Invert(ptPixels[x + (csd->c->r * (height-1) / 255) * width]);
			for (y = 0; y < height; y++)
				Invert(ptPixels[(csd->c->g * (width-1) / 255) + y * width]);
			break;
		}
	}

	// Copy data over to real DC
	BitBlt(ps.hdc, rect.left, rect.top, (rect.right - rect.left),
				 (rect.bottom - rect.top), hDC, rect.left, rect.top, SRCCOPY);
	SelectObject(hDC, hBMOld);
	DeleteObject(hBM);
	DeleteDC(hDC);

	// End painting
	EndPaint(hWnd, &ps);
}

void ColourSquare_SetValue(HWND hWnd, int x, int y)
{
	ColourSquare_Data *csd;
	RECT rect;
	int width, height, changed;
		
	// Find our area to paint
	GetClientRect(hWnd, &rect);
	width = rect.right;
	height = rect.bottom;

	// Out of x range?
	if (x < 0)
		x = 0;
	if (x > width)
		x = width;
	
	// Out of y range?
	if (y < 0)
		y = 0;
	if (y > height)
		y = height;

	csd = (ColourSquare_Data*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (csd->mode)
	{
		case SQUARE_MODE_H:
			changed = SetColourS(csd->c, (height - y) * 100 / (height-1));
			changed |= SetColourV(csd->c, x * 100 / (width-1));
			break;
		case SQUARE_MODE_S:
			changed = SetColourH(csd->c, (height - y) * 359 / (height-1));
			changed |= SetColourV(csd->c, x * 100 / (width-1));
			break;
		case SQUARE_MODE_V:
			changed = SetColourH(csd->c, (height - y) * 359 / (height-1));
			changed |= SetColourS(csd->c, x * 100 / (width-1));
			break;
		case SQUARE_MODE_R:
			changed = SetColourG(csd->c, (height - y) * 255 / (height-1));
			changed |= SetColourB(csd->c, x * 255 / (width-1));
			break;
		case SQUARE_MODE_G:
			changed = SetColourR(csd->c, (height - y) * 255 / (height-1));
			changed |= SetColourB(csd->c, x * 255 / (width-1));
			break;
		case SQUARE_MODE_B:
			changed = SetColourR(csd->c, (height - y) * 255 / (height-1));
			changed |= SetColourG(csd->c, x * 255 / (width-1));
			break;
	}

	if (changed == 1)
		RedrawColourBlocks();
}

