#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#define FONT_OUTLINE_BEGIN namespace FontOutline {
#define FONT_OUTLINE_END }
#define USE_FONT_OUTLINE using namespace FontOutline;

FONT_OUTLINE_BEGIN	

// Define some fixed size types.
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

union Pixel32
{
	Pixel32()
	: integer(0) { }
	Pixel32(uint8 bi, uint8 gi, uint8 ri, uint8 ai = 255)
	{
		b = bi;
		g = gi;
		r = ri;
		a = ai;
	}

	uint32 integer;

	struct
	{
		#ifdef BIG_ENDIAN
		uint8 a, r, g, b;
		#else // BIG_ENDIAN
		uint8 b, g, r, a;
		#endif // BIG_ENDIAN
	};
};

struct FontRect
{
	 float x, y;
	 float w, h;
};

extern 
bool WriteOutline(const FT_Library &library,
                wchar_t ch,
                FT_Face &face,
                int size,
                const Pixel32 &fontCol,
                const Pixel32 outlineCol,
                float outlineWidth,
				char*& alpha_map,
				int& imgWidth, int& imgHeight,
				int & color_top, int& color_bottom,
				FontRect& r,
				int &xAdvance);

FONT_OUTLINE_END