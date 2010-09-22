#include <stdio.h>
#include "Colours.h"

#define APPLY_SAT(x,s) x+=(100-s)*(255-x)/100

#define HSV(h,s,v) (h << 16 | s << 8 | v)
#define GetHValue(x) (x >> 16)
#define GetSValue(x) ((x >> 8) & 0xff)
#define GetVValue(x) (x & 0xff)

#define RealGetRValue(x) GetBValue(x)
#define RealGetGValue(x) GetGValue(x)
#define RealGetBValue(x) GetRValue(x)

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

int SetColourH(Colour *c, int h)
{
	COLORREF col;

	if (c->h == h)
		return 0;

	if (h < 0)
		h = 0;

	if (h > 359)
		h = 359;

	c->h = h;

	col = HSVtoRGB(c->h, c->s, c->v);
	c->r = RealGetRValue(col);
	c->g = RealGetGValue(col);
	c->b = RealGetBValue(col);

	return 1;
}

int SetColourS(Colour *c, int s)
{
	COLORREF col;

	if (c->s == s)
		return 0;

	if (s < 0)
		s = 0;

	if (s > 100)
		s = 100;

	c->s = s;

	col = HSVtoRGB(c->h, c->s, c->v);
	c->r = RealGetRValue(col);
	c->g = RealGetGValue(col);
	c->b = RealGetBValue(col);

	return 1;
}

int SetColourV(Colour *c, int v)
{
	COLORREF col;

	if (c->v == v)
		return 0;

	if (v < 0)
		v = 0;

	if (v > 100)
		v = 100;

	c->v = v;

	col = HSVtoRGB(c->h, c->s, c->v);
	c->r = RealGetRValue(col);
	c->g = RealGetGValue(col);
	c->b = RealGetBValue(col);

	return 1;
}

int SetColourR(Colour *c, int r)
{
	COLORREF col;

	if (c->r == r)
		return 0;

	if (r < 0)
		r = 0;

	if (r > 255)
		r = 255;

	c->r = r;

	col = RGBtoHSV(c->r, c->g, c->b);
	c->h = GetHValue(col);
	c->s = GetSValue(col);
	c->v = GetVValue(col);

	return 1;
}

int SetColourG(Colour *c, int g)
{
	COLORREF col;

	if (c->g == g)
		return 0;

	if (g < 0)
		g = 0;

	if (g > 255)
		g = 255;

	c->g = g;

	col = RGBtoHSV(c->r, c->g, c->b);
	c->h = GetHValue(col);
	c->s = GetSValue(col);
	c->v = GetVValue(col);

	return 1;
}

int SetColourB(Colour *c, int b)
{
	COLORREF col;

	if (c->b == b)
		return 0;

	if (b < 0)
		b = 0;

	if (b > 255)
		b = 255;

	c->b = b;

	col = RGBtoHSV(c->r, c->g, c->b);
	c->h = GetHValue(col);
	c->s = GetSValue(col);
	c->v = GetVValue(col);

	return 1;
}

COLORREF HSVtoRGB(int h, int s, int v)
{
	int r, g, b;
	int i, f;
	
	// Find section / remainder
	i = h / 60;
	f = h - (i * 60);

	// H Calc
	switch(i)
	{
		case 0:
			r = 255;
			g = (f * 255 / 60);
			b = 0;
			break;
		case 1:
			r = (60 - f) * 255 / 60;
			g = 255;
			b = 0;
			break;
		case 2:
			r = 0;
			g = 255;
			b = f * 255 / 60;
			break;
		case 3:
			r = 0;
			g = (60 - f) * 255 / 60;
			b = 255;
			break;
		case 4:
			r = f * 255 / 60;
			g = 0;
			b = 255;
			break;
		case 5:
			r = 255;
			g = 0;
			b = (60 - f) * 255 / 60;
			break;
	}

	// S Calc
	switch(i)
	{
		case 0:
		case 5:
			APPLY_SAT(g, s);
			APPLY_SAT(b, s);
			break;
		case 1:
		case 2:
			APPLY_SAT(r, s);
			APPLY_SAT(b, s);
			break;
		case 3:
		case 4:
			APPLY_SAT(r, s);
			APPLY_SAT(g, s);
			break;
	}

	// V Effect
	r = v * r / 100;
	g = v * g / 100;
	b = v * b / 100;

	return RealRGB(r,g,b);
}

COLORREF RGBtoHSV(int r, int g, int b)
{
	int h, s, v;
	int min, max;

	max = MAX(r, MAX(g, b));
	min = MIN(r, MIN(g, b));

	// V
	v = max * 100 / 255;

	// S
	if (max != 0)
		s = (max - min) * 100 / max;
	else
		s = 0;

	// H
	if ((max - min) != 0)
	{
		if (r == max)
			h = 60 * (g - b) / (max - min);
		else if (g == max)
			h = 120 + 60 * (b - r) / (max - min);
		else
			h = 240 + 60 * (r - g) / (max - min);

		while (h >= 360) h -= 360;
		while (h < 0) h+= 360;
	}
	else
		h = 0;
	
	return HSV(h,s,v);
}

