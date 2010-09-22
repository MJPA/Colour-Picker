#ifndef _COLOURS_H_
#define _COLOURS_H_

#include <windows.h>

typedef struct _Colour
{
	int h,s,v,r,g,b;
} Colour;

#define RealRGB(r,g,b) (r << 16 | g << 8 | b)
#define Invert(x) x = 0x00FFFFFF - x
	
int SetColourH(Colour *c, int h);
int SetColourS(Colour *c, int s);
int SetColourV(Colour *c, int v);
int SetColourR(Colour *c, int r);
int SetColourG(Colour *c, int g);
int SetColourB(Colour *c, int b);
COLORREF HSVtoRGB(int h, int s, int v);
COLORREF RGBtoHSV(int r, int g, int b);

#endif