#ifndef _TEX_TOOLS_H
#define _TEX_TOOLS_H

#include "../visual/fm_define.h"

typedef struct _IMAGE_INFO
{
    unsigned int            Width;
    unsigned int            Height;
	unsigned int			OriginWidth;
	unsigned int			OriginHeight;
    unsigned int            MipLevels;
    TEX_FORMAT              Format;
	unsigned int			DataSize;
} IMAGE_INFO;

typedef struct _ALPHA_INFO
{
    unsigned int            Width;
    unsigned int            Height;
	unsigned char*			Data;
	unsigned int			DataSize;
} ALPHA_INFO;

class Render;
extern void TexTools_Init(Render* pRender);
extern bool CreateCubeTextureFromFile(const char* file_name, fm_uint& texture, _IMAGE_INFO* info = NULL);
extern bool CreateCubeTextureFromFile(const char* file_name, const void* data, unsigned int data_size, fm_uint& texture, _IMAGE_INFO* info = NULL);
extern bool CreateTextureFromFile(const char* file_name, fm_uint& texture, _IMAGE_INFO* info = NULL);
extern bool CreateTextureFromData(const char* file_name, const void* data, unsigned int data_size, fm_uint& texture, _IMAGE_INFO* info);
extern bool CreateTextureFromData(const char* file_name, 
								  const void* data, unsigned int data_size, 
								  const void* alpha_mask_data, unsigned int alpha_mask_size, 
								  fm_uint& texture, _IMAGE_INFO* info);
extern bool CreateTextureAlphaFromData(const char* file_name, const void* data, unsigned int data_size, _ALPHA_INFO* info);
extern bool SaveScreenToFile(const char* file_name, int x, int y, int w, int h);

#endif