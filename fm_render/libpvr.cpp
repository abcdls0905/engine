
/******************************************************************************

 @File         PVRTDecompress.cpp

 @Title        PVRTDecompress

 @Version      

 @Copyright    Copyright (C)  Imagination Technologies Limited.

 @Platform     ANSI compatible

 @Description  PVRTC Texture Decompression.

******************************************************************************/
#define TARGET_OS_IPHONE1
#define FREE(X)		{ if(X) { free(X); (X) = 0; } }
#define GL_BGRA							0x80E1

#ifndef _ASSERT 
#define _ASSERT (bool)
#endif

#include "../public/core_log.h"

#include <string>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "libpvr.h"


#define PVRT_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define PVRT_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define PVRT_CLAMP(x, l, h)      (PVRT_MIN((h), PVRT_MAX((x), (l))))


#ifdef _ANDROID
	#include "CCCommon.h"
	#define PVRTErrorOutputDebug(msg) CCLOG(msg);
#else
	#define PVRTErrorOutputDebug(msg) CORE_TRACE(msg); //assert(false);
#endif

/****************************************************************************
** Functions
****************************************************************************/

/*!***************************************************************************
@Function		PVRTTextureCreate
@Input			w			Size of the texture
@Input			h			Size of the texture
@Input			wMin		Minimum size of a texture level
@Input			hMin		Minimum size of a texture level
@Input			nBPP		Bits per pixel of the format
@Input			bMIPMap		Create memory for MIP-map levels also?
@Return			Allocated texture memory (must be free()d)
@Description	Creates a PVR_Texture_Header structure, including room for
the specified texture, in memory.
*****************************************************************************/
PVR_Texture_Header *PVRTTextureCreate(unsigned int		w,
									  unsigned int		h,
									  const unsigned int	wMin,
									  const unsigned int	hMin,
									  const unsigned int	nBPP,
									  const bool			bMIPMap)
{
	size_t			len;
	unsigned char	*p;

	{
		unsigned int	wTmp = w, hTmp = h;

		len = 0;
		do
		{
			len += PVRT_MAX(wTmp, wMin) * PVRT_MAX(hTmp, hMin);
			wTmp >>= 1;
			hTmp >>= 1;
		}
		while(bMIPMap && (wTmp || hTmp));
	}

	len = (len * nBPP) / 8;
	len += sizeof(PVR_Texture_Header);

	p = (unsigned char*)malloc(len);
	_ASSERT(p>0);

	if(p)
	{
		PVR_Texture_Header * const psTexHeader = (PVR_Texture_Header*)p;

		psTexHeader->dwHeaderSize		= sizeof(PVR_Texture_Header);
		psTexHeader->dwWidth			= w;
		psTexHeader->dwHeight			= h;
		psTexHeader->dwMipMapCount		= 0;
		psTexHeader->dwpfFlags			= 0;
		psTexHeader->dwTextureDataSize	= (PVRTuint32)(len - sizeof(PVR_Texture_Header));
		psTexHeader->dwBitCount			= nBPP;
		psTexHeader->dwRBitMask			= 0;
		psTexHeader->dwGBitMask			= 0;
		psTexHeader->dwBBitMask			= 0;
		psTexHeader->dwAlphaBitMask		= 0;
		psTexHeader->dwPVR				= 0;
		psTexHeader->dwNumSurfs			= 1;

		return psTexHeader;
	}
	else
	{
		return 0;
	}
};

#if 0
/*!***************************************************************************
 @Function			PVRTTextureFormatGetBPP
 @Input				nFormat
 @Input				nType
 @Description		Returns the bits per pixel (BPP) of the format.
*****************************************************************************/
unsigned int PVRTTextureFormatGetBPP(const GLuint nFormat, const GLuint nType)
{
	switch(nFormat)
	{
	case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
	case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
		return 2;
	case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
	case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
		return 4;
	case GL_UNSIGNED_BYTE:
		switch(nType)
		{
		case GL_RGBA:
		//case GL_BGRA:
			return 32;
		}
	case GL_UNSIGNED_SHORT_5_5_5_1:
		switch(nType)
		{
		case GL_RGBA:
			return 16;
		}
	}

	return 0xFFFFFFFF;
};

EPVRTError PVRTTextureLoadFromPointer(	const void* pointer,
										GLuint *const texName,
                                        unsigned int & width,
                                        unsigned int & height,
										bool & hasAlpha,
										const void *psTextureHeader,
										bool bAllowDecompress,
										const unsigned int nLoadFromLevel,
										const void * const texPtr)
{
	PVR_Texture_Header* psPVRHeader = (PVR_Texture_Header*)pointer;
	PVRTuint32* pData = (PVRTuint32*) pointer;
	unsigned int u32NumSurfs;

	// perform checks for old PVR psPVRHeader
	if(psPVRHeader->dwHeaderSize!=sizeof(PVR_Texture_Header))
	{	// Header V1
		if(psPVRHeader->dwHeaderSize==PVRTEX_V1_HEADER_SIZE)
		{	// react to old psPVRHeader: i.e. fill in numsurfs as this is missing from old header
			PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: this is an old pvr"
				" - you can use PVRTexTool to update its header.\n");
			if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
				u32NumSurfs = 6;
			else
				u32NumSurfs = 1;
		}
		else
		{	// not a pvr at all
			PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: not a valid pvr.\n");
			return PVR_FAIL;
		}
	}
	else
	{	// Header V2
		if(psPVRHeader->dwNumSurfs<1)
		{	// encoded with old version of PVRTexTool before zero numsurfs bug found.
			if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
				u32NumSurfs = 6;
			else
				u32NumSurfs = 1;
		}
		else
		{
			u32NumSurfs = psPVRHeader->dwNumSurfs;
		}
	}

	GLuint textureName;
	GLenum eTextureFormat = 0;
	GLenum eTextureInternalFormat = 0;	// often this is the same as textureFormat, but not for BGRA8888 on the iPhone, for instance
	GLenum eTextureType = 0;

	bool bIsPVRTCSupported = IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc");
#ifndef IOS
	bool bIsBGRA8888Supported  = IsGLExtensionSupported("GL_IMG_texture_format_BGRA8888");
#else
	bool bIsBGRA8888Supported  = IsGLExtensionSupported("GL_APPLE_texture_format_BGRA8888");
#endif

	*texName = 0;	// install warning value
	bool bIsCompressedFormatSupported = false, bIsCompressedFormat = false;
	/* Only accept untwiddled data UNLESS texture format is PVRTC */
	if ( ((psPVRHeader->dwpfFlags & PVRTEX_TWIDDLE) == PVRTEX_TWIDDLE)
		&& ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC2)
		&& ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC4) )
	{
		// We need to load untwiddled textures -- hw will twiddle for us.
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: texture should be untwiddled.\n");
		return PVR_FAIL;
	}

	unsigned int ePixelType = psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE;
    width = psPVRHeader->dwWidth;
    height = psPVRHeader->dwHeight;
	hasAlpha = psPVRHeader->dwAlphaBitMask != 0;
	switch(ePixelType)
	{
	case OGL_RGBA_4444:
		eTextureFormat = eTextureInternalFormat = GL_RGBA;
		eTextureType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	case OGL_RGBA_5551:
		eTextureFormat = eTextureInternalFormat = GL_RGBA ;
		eTextureType = GL_UNSIGNED_SHORT_5_5_5_1;
		break;

	case OGL_RGBA_8888:
		eTextureFormat = eTextureInternalFormat = GL_RGBA;
		eTextureType = GL_UNSIGNED_BYTE ;
		break;

	// New OGL Specific Formats Added

	case OGL_RGB_565:
		eTextureFormat = eTextureInternalFormat = GL_RGB ;
		eTextureType = GL_UNSIGNED_SHORT_5_6_5;
		break;

	case OGL_RGB_555:
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: pixel type OGL_RGB_555 not supported.\n");
		return PVR_FAIL; // Deal with exceptional case

	case OGL_RGB_888:
		eTextureFormat = eTextureInternalFormat = GL_RGB ;
		eTextureType = GL_UNSIGNED_BYTE;
		break;

	case OGL_I_8:
		eTextureFormat = eTextureInternalFormat = GL_LUMINANCE;
		eTextureType = GL_UNSIGNED_BYTE;
		break;

	case OGL_AI_88:
		eTextureFormat = eTextureInternalFormat = GL_LUMINANCE_ALPHA ;
		eTextureType = GL_UNSIGNED_BYTE;
		break;

	case MGLPT_PVRTC2:
	case OGL_PVRTC2:
		if(bIsPVRTCSupported)
		{
			bIsCompressedFormatSupported = bIsCompressedFormat = true;
			eTextureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG ;	// PVRTC2
		}
		else
		{
			if(bAllowDecompress)
			{
				bIsCompressedFormatSupported = false;
				bIsCompressedFormat = true;
				eTextureFormat = eTextureInternalFormat = GL_RGBA;
				eTextureType = GL_UNSIGNED_BYTE;
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: PVRTC2 not supported. Converting to RGBA8888 instead.\n");
			}
			else
			{
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer error: PVRTC2 not supported.\n");
				return PVR_FAIL;
			}
		}
		break;
	case MGLPT_PVRTC4:
	case OGL_PVRTC4:
		if(bIsPVRTCSupported)
		{
			bIsCompressedFormatSupported = bIsCompressedFormat = true;
			eTextureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ;	// PVRTC4
		}
		else
		{
			if(bAllowDecompress)
			{
				bIsCompressedFormatSupported = false;
				bIsCompressedFormat = true;
				eTextureFormat = eTextureInternalFormat = GL_RGBA;
				eTextureType = GL_UNSIGNED_BYTE;
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: PVRTC4 not supported. Converting to RGBA8888 instead.\n");
			}
			else
			{
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer error: PVRTC4 not supported.\n");
				return PVR_FAIL;
			}
		}
		break;
	case OGL_A_8:
		eTextureFormat = eTextureInternalFormat = GL_ALPHA;
		eTextureType = GL_UNSIGNED_BYTE;
		break;
	case OGL_BGRA_8888:
//		if(bIsBGRA8888Supported)
//		{
//			eTextureType = GL_UNSIGNED_BYTE;
//#ifndef __APPLE__
//			eTextureFormat = eTextureInternalFormat = GL_BGRA;
//#else
//			eTextureFormat = GL_BGRA;
//			eTextureInternalFormat = GL_RGBA;
//#endif
//		}
//		else
		{
			PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: Unable to load GL_BGRA texture as extension GL_IMG_texture_format_BGRA8888 is unsupported.\n");
			return PVR_FAIL;
		}
		break;

	default:											// NOT SUPPORTED
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: pixel type not supported.\n");
		return PVR_FAIL;
	}

	// load the texture up
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);				// Never have row-aligned in psPVRHeaders

	glGenTextures(1, &textureName);

	//  check that this data is cube map data or not.
	if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
	{ // not in OGLES you don't
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: cube map textures are not available in OGLES1.x.\n");
		return PVR_FAIL;
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, textureName);
	}

	if(glGetError())
	{
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: glBindTexture() failed.\n");
		return PVR_FAIL;
	}

	for(unsigned int i=0; i<u32NumSurfs; i++)
	{
		PVRTuint8 *theTexturePtr = (texPtr? (PVRTuint8*)texPtr :  (PVRTuint8*)pData + psPVRHeader->dwHeaderSize) + psPVRHeader->dwTextureDataSize * i;
		PVRTuint8 *theTextureToLoad = 0;
		int		nMIPMapLevel;
		int		nTextureLevelsNeeded = (psPVRHeader->dwpfFlags & PVRTEX_MIPMAP)? psPVRHeader->dwMipMapCount : 0;
		unsigned int		nSizeX= psPVRHeader->dwWidth, nSizeY = psPVRHeader->dwHeight;
		unsigned int		CompressedImageSize = 0;

		for(nMIPMapLevel = 0; nMIPMapLevel <= nTextureLevelsNeeded; nSizeX = PVRT_MAX(nSizeX/2, (unsigned int)1), nSizeY = PVRT_MAX(nSizeY/2, (unsigned int)1), nMIPMapLevel++)
		{
			// Do Alpha-swap if needed

			theTextureToLoad = theTexturePtr;

			// Load the Texture
			/* If the texture is PVRTC then use GLCompressedTexImage2D */
			if(bIsCompressedFormat)
			{
				/* Calculate how many bytes this MIP level occupies */
				if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
				{
					CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC2_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC2_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount + 7) / 8;
				}
				else
				{// PVRTC4 case
					CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC4_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC4_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount + 7) / 8;
				}

				if(((signed int)nMIPMapLevel - (signed int)nLoadFromLevel) >= 0)
				{
					if(bIsCompressedFormatSupported)
					{
						/* Load compressed texture data at selected MIP level */
						glCompressedTexImage2D(GL_TEXTURE_2D, nMIPMapLevel-nLoadFromLevel, eTextureFormat, nSizeX, nSizeY, 0,
										CompressedImageSize, theTextureToLoad);

					}
					else
					{
						// Convert PVRTC to 32-bit
						PVRTuint8 *u8TempTexture = (PVRTuint8*)malloc(nSizeX*nSizeY*4);
						if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
						{
							PVRTDecompressPVRTC(theTextureToLoad, 1, nSizeX, nSizeY, u8TempTexture);
						}
						else
						{// PVRTC4 case
							PVRTDecompressPVRTC(theTextureToLoad, 0, nSizeX, nSizeY, u8TempTexture);
						}


						//if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
						//{// Load compressed cubemap data at selected MIP level
						//	// Upload the texture as 32-bits
						//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,nMIPMapLevel-nLoadFromLevel,GL_RGBA,
						//		nSizeX,nSizeY,0, GL_RGBA,GL_UNSIGNED_BYTE,u8TempTexture);
						//	FREE(u8TempTexture);
						//}
						//else
						{// Load compressed 2D data at selected MIP level
							// Upload the texture as 32-bits
							glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel-nLoadFromLevel,GL_RGBA,
								nSizeX,nSizeY,0, GL_RGBA,GL_UNSIGNED_BYTE,u8TempTexture);
							::free(u8TempTexture);
						}
					}
				}
			}
			else
			{
				if(((signed int)nMIPMapLevel - (signed int)nLoadFromLevel) >= 0)
				{
					/* Load uncompressed texture data at selected MIP level */
						glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel-nLoadFromLevel,eTextureInternalFormat,nSizeX,nSizeY,0,eTextureFormat,eTextureType,theTextureToLoad);
				}
			}



			if(glGetError())
			{
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: glTexImage2D() failed.\n");
				return PVR_FAIL;
			}

			// offset the texture pointer by one mip-map level
			/* PVRTC case */
			if ( bIsCompressedFormat )
			{
				theTexturePtr += CompressedImageSize;
			}
			else
			{
				/* New formula that takes into account bit counts inferior to 8 (e.g. 1 bpp) */
				theTexturePtr += (nSizeX * nSizeY * psPVRHeader->dwBitCount + 7) / 8;
			}
		}
	}

	*texName = textureName;

    if(psTextureHeader)
    {
        *(PVR_Texture_Header*)psTextureHeader = *psPVRHeader;
        ((PVR_Texture_Header*)psTextureHeader)->dwPVR = PVRTEX_IDENTIFIER;
        ((PVR_Texture_Header*)psTextureHeader)->dwNumSurfs = u32NumSurfs;
    }

	if(!psPVRHeader->dwMipMapCount)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	return PVR_SUCCESS;
};
#endif

bool pvr_load_rgba_file(const char* filename,int& width, int& height, unsigned char*& pixels,bool& flip_y) {
	 
	if(filename == NULL) return false;
	pixels = NULL;
	FILE *fp = fopen(filename, "rb");
	if (!fp) return false;
	unsigned char *file_raw_data = NULL;
    long filesize = 0;
	fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    file_raw_data = (unsigned char*)::malloc(sizeof(unsigned char) * (filesize+1));
    fread(file_raw_data, sizeof(unsigned char), filesize, fp );
	fclose(fp);
	bool result  = pvr_load_rgba(file_raw_data,width,height,pixels,flip_y);
	::free(file_raw_data);
	return result;
}
bool pvr_load_rgba(const void* point, int& width, int& height, unsigned char*& pixels,bool& flip_y) {
	pixels = NULL;

	PVR_Texture_Header* psPVRHeader = (PVR_Texture_Header*)point;
	PVRTuint32* pData = (PVRTuint32*) point;
	unsigned int u32NumSurfs;

	// perform checks for old PVR psPVRHeader
	if(psPVRHeader->dwHeaderSize!=sizeof(PVR_Texture_Header))
	{	// Header V1
		if(psPVRHeader->dwHeaderSize==PVRTEX_V1_HEADER_SIZE)
		{	// react to old psPVRHeader: i.e. fill in numsurfs as this is missing from old header
			PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: this is an old pvr"
				" - you can use PVRTexTool to update its header.\n");
			if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
				u32NumSurfs = 6;
			else
				u32NumSurfs = 1;
		}
		else
		{	// not a pvr at all
			PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: not a valid pvr.\n");
			return PVR_FAIL;
		}
	}
	else
	{	// Header V2
		if(psPVRHeader->dwNumSurfs<1)
		{	// encoded with old version of PVRTexTool before zero numsurfs bug found.
			if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
				u32NumSurfs = 6;
			else
				u32NumSurfs = 1;
		}
		else
		{
			u32NumSurfs = psPVRHeader->dwNumSurfs;
		}
	}

	if(u32NumSurfs != 1) return false;

	bool bIsCompressedFormatSupported = false, bIsCompressedFormat = false;
	/* Only accept untwiddled data UNLESS texture format is PVRTC */
	if ( ((psPVRHeader->dwpfFlags & PVRTEX_TWIDDLE) == PVRTEX_TWIDDLE)
		&& ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC2)
		&& ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC4) )
	{
		// We need to load untwiddled textures -- hw will twiddle for us.
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: texture should be untwiddled.\n");
		return PVR_FAIL;
	}

	unsigned int ePixelType = psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE;
    width = psPVRHeader->dwWidth;
    height = psPVRHeader->dwHeight;
	
	flip_y = (psPVRHeader->dwpfFlags & PVRTEX_VERTICAL_FLIP) != 0 ? true: false;
	//hasalpha = psPVRHeader->dwAlphaBitMask != 0;
	switch(ePixelType)
	{
	case MGLPT_PVRTC2:
	case OGL_PVRTC2:
		bIsCompressedFormat = true;
		break;
	case MGLPT_PVRTC4:
	case OGL_PVRTC4:
		bIsCompressedFormat = true;
		break;

	case OGL_RGBA_4444:
	case OGL_RGBA_5551:
	case OGL_RGBA_8888:
	// New OGL Specific Formats Added
	case OGL_RGB_565:
	case OGL_RGB_555:
	case OGL_RGB_888:
	case OGL_I_8:
	case OGL_AI_88:
	case OGL_A_8:
	case OGL_BGRA_8888:
	default:											// NOT SUPPORTED
		//PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: pixel type not supported.\n");
		//return false;
		break;
	}

	//  check that this data is cube map data or not.
	if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
	{ // not in OGLES you don't
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: cube map textures are not available in OGLES1.x.\n");
		return false;
	}


	for(unsigned int i=0; i<u32NumSurfs; i++)
	{
		PVRTuint8 *theTexturePtr = ((PVRTuint8*)pData + psPVRHeader->dwHeaderSize) + psPVRHeader->dwTextureDataSize * i;
		PVRTuint8 *theTextureToLoad = 0;
		int		nMIPMapLevel;
		int		nTextureLevelsNeeded = (psPVRHeader->dwpfFlags & PVRTEX_MIPMAP)? psPVRHeader->dwMipMapCount : 0;
		unsigned int		nSizeX= psPVRHeader->dwWidth, nSizeY = psPVRHeader->dwHeight;
		unsigned int		CompressedImageSize = 0;


		// load first level only
		nTextureLevelsNeeded = 0;

		for(nMIPMapLevel = 0; nMIPMapLevel <= nTextureLevelsNeeded; nSizeX = PVRT_MAX(nSizeX/2, (unsigned int)1), nSizeY = PVRT_MAX(nSizeY/2, (unsigned int)1), nMIPMapLevel++)
		{
			// Do Alpha-swap if needed

			theTextureToLoad = theTexturePtr;

			// Load the Texture
			/* If the texture is PVRTC then use GLCompressedTexImage2D */
			if(bIsCompressedFormat)
			{
				/* Calculate how many bytes this MIP level occupies */
				if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
				{
					CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC2_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC2_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount + 7) / 8;
				}
				else
				{// PVRTC4 case
					CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC4_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC4_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount + 7) / 8;
				}

				if(((signed int)nMIPMapLevel - (signed int)0) >= 0)
				{
					//if(bIsCompressedFormatSupported)
					//{
					//	/* Load compressed texture data at selected MIP level */
					//	glCompressedTexImage2D(GL_TEXTURE_2D, nMIPMapLevel-nLoadFromLevel, eTextureFormat, nSizeX, nSizeY, 0,
					//					CompressedImageSize, theTextureToLoad);

					//}
					//else
					//{
						// Convert PVRTC to 32-bit
						PVRTuint8 *u8TempTexture = (PVRTuint8*)::malloc(nSizeX*nSizeY*4);
						if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
						{
							PVRTDecompressPVRTC(theTextureToLoad, 1, nSizeX, nSizeY, u8TempTexture);
						}
						else
						{// PVRTC4 case
							PVRTDecompressPVRTC(theTextureToLoad, 0, nSizeX, nSizeY, u8TempTexture);
						}
						

						pixels = u8TempTexture;
						return true;
						//{// Load compressed 2D data at selected MIP level
						//	// Upload the texture as 32-bits
						//	glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel-nLoadFromLevel,GL_RGBA,
						//		nSizeX,nSizeY,0, GL_RGBA,GL_UNSIGNED_BYTE,u8TempTexture);
						//	::free(u8TempTexture);
						//}
				//	}
				}
			}
			else
			{

				unsigned buffer_size = nSizeX*nSizeY*4;
				pixels = (PVRTuint8*)::malloc(buffer_size);
				switch(ePixelType)
				{

				case OGL_RGBA_8888:
					::memcpy(pixels,theTextureToLoad,buffer_size);
					return true;
				case OGL_RGBA_4444:
					for (size_t i = 0; i <  nSizeX*nSizeY; i++)
					{
						pixels[i * 4 + 0] = (theTextureToLoad[i* 2 + 0]) & 0xF0;
						pixels[i * 4 + 1] = (theTextureToLoad[i* 2 + 0] << 4) & 0xF0 ;
						pixels[i * 4 + 2] = (theTextureToLoad[i* 2 + 1]) & 0xF0;
						pixels[i * 4 + 3] = (theTextureToLoad[i* 2 + 1] << 4) & 0xF0;
					}
					return true;
				case OGL_RGBA_5551:
				case OGL_RGB_565:
				case OGL_RGB_555:
					return false;
				case OGL_RGB_888:
					for (size_t i = 0; i <  nSizeX*nSizeY; i++)
					{
						pixels[i * 4 + 0] = (theTextureToLoad[i* 3 + 0]);
						pixels[i * 4 + 1] = (theTextureToLoad[i* 3 + 1]) ;
						pixels[i * 4 + 2] = (theTextureToLoad[i* 3 + 2]);
						pixels[i * 4 + 3] = 0xFF;
					}
					return true;
				default:
					return false;
				}
				return false;
				//if(((signed int)nMIPMapLevel - (signed int)nLoadFromLevel) >= 0)
				//{
				//	/* Load uncompressed texture data at selected MIP level */
				//		glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel-nLoadFromLevel,eTextureInternalFormat,nSizeX,nSizeY,0,eTextureFormat,eTextureType,theTextureToLoad);
				//}
			}

			// offset the texture pointer by one mip-map level
			/* PVRTC case */
			if ( bIsCompressedFormat )
			{
				theTexturePtr += CompressedImageSize;
			}
			else
			{
				/* New formula that takes into account bit counts inferior to 8 (e.g. 1 bpp) */
				theTexturePtr += (nSizeX * nSizeY * psPVRHeader->dwBitCount + 7) / 8;
			}
		}
	}

	return true;
};

/*****************************************************************************
 * defines and consts
 *****************************************************************************/
#define PT_INDEX (2)	// The Punch-through index

#define BLK_Y_SIZE 	(4) // always 4 for all 2D block types

#define BLK_X_MAX	(8)	// Max X dimension for blocks

#define BLK_X_2BPP	(8) // dimensions for the two formats
#define BLK_X_4BPP	(4)

#define WRAP_COORD(Val, Size) ((Val) & ((Size)-1))

#define POWER_OF_2(X)   util_number_is_power_2(X)

/*
	Define an expression to either wrap or clamp large or small vals to the
	legal coordinate range
*/
#define LIMIT_COORD(Val, Size, AssumeImageTiles) \
      ((AssumeImageTiles)? WRAP_COORD((Val), (Size)): PVRT_CLAMP((Val), 0, (Size)-1))

/*****************************************************************************
 * Useful typedefs
 *****************************************************************************/
typedef PVRTuint32 U32;
typedef PVRTuint8 U8;

/***********************************************************
				DECOMPRESSION ROUTINES
************************************************************/

/*!***********************************************************************
 @Struct	AMTC_BLOCK_STRUCT
 @Brief
*************************************************************************/
typedef struct
{
	// Uses 64 bits pre block
	U32 PackedData[2];
}AMTC_BLOCK_STRUCT;


void Decompress(AMTC_BLOCK_STRUCT *pCompressedData,
					   const int Do2bitMode,
					   const int XDim,
					   const int YDim,
					   const int AssumeImageTiles,
					   unsigned char* pResultImage);
/*!***********************************************************************
 @Function		PVRTDecompressPVRTC
 @Input			pCompressedData The PVRTC texture data to decompress
 @Input			Do2bitMode Signifies whether the data is PVRTC2 or PVRTC4
 @Input			XDim X dimension of the texture
 @Input			YDim Y dimension of the texture
 @Modified		pResultImage The decompressed texture data
 @Description	Decompresses PVRTC to RGBA 8888
*************************************************************************/
void PVRTDecompressPVRTC(const void *pCompressedData,
				const int Do2bitMode,
				const int XDim,
				const int YDim,
				unsigned char* pResultImage)
{
	Decompress((AMTC_BLOCK_STRUCT*)pCompressedData,Do2bitMode,XDim,YDim,1,pResultImage);
}

 /*!***********************************************************************
  @Function		util_number_is_power_2
  @Input		input A number
  @Returns		TRUE if the number is an integer power of two, else FALSE.
  @Description	Check that a number is an integer power of two, i.e.
             	1, 2, 4, 8, ... etc.
				Returns FALSE for zero.
*************************************************************************/
int util_number_is_power_2( unsigned  input )
{
  unsigned minus1;

  if( !input ) return 0;

  minus1 = input - 1;
  return ( (input | minus1) == (input ^ minus1) ) ? 1 : 0;
};


/*!***********************************************************************
 @Function		Unpack5554Colour
 @Input			pBlock
 @Input			ABColours
 @Description	Given a block, extract the colour information and convert
 				to 5554 formats
*************************************************************************/
void Unpack5554Colour(const AMTC_BLOCK_STRUCT *pBlock,
							 int   ABColours[2][4])
{
	U32 RawBits[2];

	int i;

	// Extract A and B
	RawBits[0] = pBlock->PackedData[1] & (0xFFFE); // 15 bits (shifted up by one)
	RawBits[1] = pBlock->PackedData[1] >> 16;	   // 16 bits

	// step through both colours
	for(i = 0; i < 2; i++)
	{
		// If completely opaque
		if(RawBits[i] & (1<<15))
		{
			// Extract R and G (both 5 bit)
			ABColours[i][0] = (RawBits[i] >> 10) & 0x1F;
			ABColours[i][1] = (RawBits[i] >>  5) & 0x1F;

			/*
				The precision of Blue depends on  A or B. If A then we need to
				replicate the top bit to get 5 bits in total
			*/
			ABColours[i][2] = RawBits[i] & 0x1F;
			if(i==0)
			{
				ABColours[0][2] |= ABColours[0][2] >> 4;
			}

			// set 4bit alpha fully on...
			ABColours[i][3] = 0xF;
		}
		else // Else if colour has variable translucency
		{
			/*
				Extract R and G (both 4 bit).
				(Leave a space on the end for the replication of bits
			*/
			ABColours[i][0] = (RawBits[i] >>  (8-1)) & 0x1E;
			ABColours[i][1] = (RawBits[i] >>  (4-1)) & 0x1E;

			// replicate bits to truly expand to 5 bits
			ABColours[i][0] |= ABColours[i][0] >> 4;
			ABColours[i][1] |= ABColours[i][1] >> 4;

			// grab the 3(+padding) or 4 bits of blue and add an extra padding bit
			ABColours[i][2] = (RawBits[i] & 0xF) << 1;

			/*
				expand from 3 to 5 bits if this is from colour A, or 4 to 5 bits if from
				colour B
			*/
			if(i==0)
			{
				ABColours[0][2] |= ABColours[0][2] >> 3;
			}
			else
			{
				ABColours[0][2] |= ABColours[0][2] >> 4;
			}

			// Set the alpha bits to be 3 + a zero on the end
			ABColours[i][3] = (RawBits[i] >> 11) & 0xE;
		}
	}
}

/*!***********************************************************************
 @Function		UnpackModulations
 @Input			pBlock
 @Input			Do2bitMode
 @Input			ModulationVals
 @Input			ModulationModes
 @Input			StartX
 @Input			StartY
 @Description	Given the block and the texture type and it's relative
 				position in the 2x2 group of blocks, extract the bit
 				patterns for the fully defined pixels.
*************************************************************************/
  void	UnpackModulations(const AMTC_BLOCK_STRUCT *pBlock,
							  const int Do2bitMode,
							  int ModulationVals[8][16],
							  int ModulationModes[8][16],
							  int StartX,
							  int StartY)
{
	int BlockModMode;
	U32 ModulationBits;

	int x, y;

	BlockModMode= pBlock->PackedData[1] & 1;
	ModulationBits	= pBlock->PackedData[0];

	// if it's in an interpolated mode
	if(Do2bitMode && BlockModMode)
	{
		/*
			run through all the pixels in the block. Note we can now treat all the
			"stored" values as if they have 2bits (even when they didn't!)
		*/
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_2BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;

				// if this is a stored value...
				if(((x^y)&1) == 0)
				{
					ModulationVals[y+StartY][x+StartX] = ModulationBits & 3;
					ModulationBits >>= 2;
				}
			}
		}
	}
	else if(Do2bitMode) // else if direct encoded 2bit mode - i.e. 1 mode bit per pixel
	{
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_2BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;

				// double the bits so 0=> 00, and 1=>11
				if(ModulationBits & 1)
				{
					ModulationVals[y+StartY][x+StartX] = 0x3;
				}
				else
				{
					ModulationVals[y+StartY][x+StartX] = 0x0;
				}
				ModulationBits >>= 1;
			}
		}
	}
	else // else its the 4bpp mode so each value has 2 bits
	{
		for(y = 0; y < BLK_Y_SIZE; y++)
		{
			for(x = 0; x < BLK_X_4BPP; x++)
			{
				ModulationModes[y+StartY][x+StartX] = BlockModMode;

				ModulationVals[y+StartY][x+StartX] = ModulationBits & 3;
				ModulationBits >>= 2;
			}
		}
	}

	// make sure nothing is left over
	_ASSERT(ModulationBits==0);
}

/*!***********************************************************************
 @Function		InterpolateColours
 @Input			ColourP
 @Input			ColourQ
 @Input			ColourR
 @Input			ColourS
 @Input			Do2bitMode
 @Input			x
 @Input			y
 @Modified		Result
 @Description	This performs a HW bit accurate interpolation of either the
				A or B colours for a particular pixel.

				NOTE: It is assumed that the source colours are in ARGB 5554
				format - This means that some "preparation" of the values will
				be necessary.
*************************************************************************/
  void InterpolateColours(const int ColourP[4],
						  const int ColourQ[4],
						  const int ColourR[4],
						  const int ColourS[4],
						  const int Do2bitMode,
						  const int x,
						  const int y,
						  int Result[4])
{
	int u, v, uscale;
	int k;

	int tmp1, tmp2;

	int P[4], Q[4], R[4], S[4];

	// Copy the colours
	for(k = 0; k < 4; k++)
	{
		P[k] = ColourP[k];
		Q[k] = ColourQ[k];
		R[k] = ColourR[k];
		S[k] = ColourS[k];
	}

	// put the x and y values into the right range
	v = (y & 0x3) | ((~y & 0x2) << 1);

	if(Do2bitMode)
		u = (x & 0x7) | ((~x & 0x4) << 1);
	else
		u = (x & 0x3) | ((~x & 0x2) << 1);

	// get the u and v scale amounts
	v  = v - BLK_Y_SIZE/2;

	if(Do2bitMode)
	{
		u = u - BLK_X_2BPP/2;
		uscale = 8;
	}
	else
	{
		u = u - BLK_X_4BPP/2;
		uscale = 4;
	}

	for(k = 0; k < 4; k++)
	{
		tmp1 = P[k] * uscale + u * (Q[k] - P[k]);
		tmp2 = R[k] * uscale + u * (S[k] - R[k]);

		tmp1 = tmp1 * 4 + v * (tmp2 - tmp1);

		Result[k] = tmp1;
	}

	// Lop off the appropriate number of bits to get us to 8 bit precision
	if(Do2bitMode)
	{
		// do RGB
		for(k = 0; k < 3; k++)
		{
			Result[k] >>= 2;
		}

		Result[3] >>= 1;
	}
	else
	{
		// do RGB  (A is ok)
		for(k = 0; k < 3; k++)
		{
			Result[k] >>= 1;
		}
	}

	// sanity check
	for(k = 0; k < 4; k++)
	{
		_ASSERT(Result[k] < 256);
	}


	/*
		Convert from 5554 to 8888

		do RGB 5.3 => 8
	*/
	for(k = 0; k < 3; k++)
	{
		Result[k] += Result[k] >> 5;
	}

	Result[3] += Result[3] >> 4;

	// 2nd sanity check
	for(k = 0; k < 4; k++)
	{
		_ASSERT(Result[k] < 256);
	}

}

/*!***********************************************************************
 @Function		GetModulationValue
 @Input			x
 @Input			y
 @Input			Do2bitMode
 @Input			ModulationVals
 @Input			ModulationModes
 @Input			Mod
 @Input			DoPT
 @Description	Get the modulation value as a numerator of a fraction of 8ths
*************************************************************************/
  void GetModulationValue(int x,
							   int y,
							   const int Do2bitMode,
							   const int ModulationVals[8][16],
							   const int ModulationModes[8][16],
							   int *Mod,
							   int *DoPT)
{
	  const int RepVals0[4] = {0, 3, 5, 8};
	  const int RepVals1[4] = {0, 4, 4, 8};

	int ModVal;

	// Map X and Y into the local 2x2 block
	y = (y & 0x3) | ((~y & 0x2) << 1);

	if(Do2bitMode)
		x = (x & 0x7) | ((~x & 0x4) << 1);
	else
		x = (x & 0x3) | ((~x & 0x2) << 1);

	// assume no PT for now
	*DoPT = 0;

	// extract the modulation value. If a simple encoding
	if(ModulationModes[y][x]==0)
	{
		ModVal = RepVals0[ModulationVals[y][x]];
	}
	else if(Do2bitMode)
	{
		// if this is a stored value
		if(((x^y)&1)==0)
			ModVal = RepVals0[ModulationVals[y][x]];
		else if(ModulationModes[y][x] == 1) // else average from the neighbours if H&V interpolation..
		{
			ModVal = (RepVals0[ModulationVals[y-1][x]] +
					  RepVals0[ModulationVals[y+1][x]] +
					  RepVals0[ModulationVals[y][x-1]] +
					  RepVals0[ModulationVals[y][x+1]] + 2) / 4;
		}
		else if(ModulationModes[y][x] == 2) // else if H-Only
		{
			ModVal = (RepVals0[ModulationVals[y][x-1]] +
					  RepVals0[ModulationVals[y][x+1]] + 1) / 2;
		}
		else // else it's V-Only
		{
			ModVal = (RepVals0[ModulationVals[y-1][x]] +
					  RepVals0[ModulationVals[y+1][x]] + 1) / 2;
		}
	}
	else // else it's 4BPP and PT encoding
	{
		ModVal = RepVals1[ModulationVals[y][x]];

		*DoPT = ModulationVals[y][x] == PT_INDEX;
	}

	*Mod =ModVal;
}

/*!***********************************************************************
 @Function		TwiddleUV
 @Input			YSize	Y dimension of the texture in pixels
 @Input			XSize	X dimension of the texture in pixels
 @Input			YPos	Pixel Y position
 @Input			XPos	Pixel X position
 @Returns		The twiddled offset of the pixel
 @Description	Given the Block (or pixel) coordinates and the dimension of
 				the texture in blocks (or pixels) this returns the twiddled
 				offset of the block (or pixel) from the start of the map.

				NOTE the dimensions of the texture must be a power of 2
*************************************************************************/
  int DisableTwiddlingRoutine = 0;

  U32 TwiddleUV(U32 YSize, U32 XSize, U32 YPos, U32 XPos)
{
	U32 Twiddled;

	U32 MinDimension;
	U32 MaxValue;

	U32 SrcBitPos;
	U32 DstBitPos;

	int ShiftCount;

	_ASSERT(YPos < YSize);
	_ASSERT(XPos < XSize);

	_ASSERT(POWER_OF_2(YSize));
	_ASSERT(POWER_OF_2(XSize));

	if(YSize < XSize)
	{
		MinDimension = YSize;
		MaxValue	 = XPos;
	}
	else
	{
		MinDimension = XSize;
		MaxValue	 = YPos;
	}

	// Nasty hack to disable twiddling
	if(DisableTwiddlingRoutine)
		return (YPos* XSize + XPos);

	// Step through all the bits in the "minimum" dimension
	SrcBitPos = 1;
	DstBitPos = 1;
	Twiddled  = 0;
	ShiftCount = 0;

	while(SrcBitPos < MinDimension)
	{
		if(YPos & SrcBitPos)
		{
			Twiddled |= DstBitPos;
		}

		if(XPos & SrcBitPos)
		{
			Twiddled |= (DstBitPos << 1);
		}


		SrcBitPos <<= 1;
		DstBitPos <<= 2;
		ShiftCount += 1;

	}

	// prepend any unused bits
	MaxValue >>= ShiftCount;

	Twiddled |=  (MaxValue << (2*ShiftCount));

	return Twiddled;
}

/*!***********************************************************************
 @Function		Decompress
 @Input			pCompressedData The PVRTC texture data to decompress
 @Input			Do2BitMode Signifies whether the data is PVRTC2 or PVRTC4
 @Input			XDim X dimension of the texture
 @Input			YDim Y dimension of the texture
 @Input			AssumeImageTiles Assume the texture data tiles
 @Modified		pResultImage The decompressed texture data
 @Description	Decompresses PVRTC to RGBA 8888
*************************************************************************/
void Decompress(AMTC_BLOCK_STRUCT *pCompressedData,
				const int Do2bitMode,
				const int XDim,
				const int YDim,
				const int AssumeImageTiles,
				unsigned char* pResultImage)
{
	int x, y;
	int i, j;

	int BlkX, BlkY;
	int BlkXp1, BlkYp1;
	int XBlockSize;
	int BlkXDim, BlkYDim;

	int StartX, StartY;

	int ModulationVals[8][16];
	int ModulationModes[8][16];

	int Mod, DoPT;

	unsigned int uPosition;

	// local neighbourhood of blocks
	AMTC_BLOCK_STRUCT *pBlocks[2][2];

	AMTC_BLOCK_STRUCT *pPrevious[2][2] = {{NULL, NULL}, {NULL, NULL}};

	// Low precision colours extracted from the blocks
	struct
	{
		int Reps[2][4];
	}Colours5554[2][2];

	// Interpolated A and B colours for the pixel
	int ASig[4], BSig[4];

	int Result[4];

	if(Do2bitMode)
		XBlockSize = BLK_X_2BPP;
	else
		XBlockSize = BLK_X_4BPP;

	// For MBX don't allow the sizes to get too small
	BlkXDim = PVRT_MAX(2, XDim / XBlockSize);
	BlkYDim = PVRT_MAX(2, YDim / BLK_Y_SIZE);

	/*
		Step through the pixels of the image decompressing each one in turn

		Note that this is a hideously inefficient way to do this!
	*/
	for(y = 0; y < YDim; y++)
	{
		for(x = 0; x < XDim; x++)
		{
			// map this pixel to the top left neighbourhood of blocks
			BlkX = (x - XBlockSize/2);
			BlkY = (y - BLK_Y_SIZE/2);

			BlkX = LIMIT_COORD(BlkX, XDim, AssumeImageTiles);
			BlkY = LIMIT_COORD(BlkY, YDim, AssumeImageTiles);


			BlkX /= XBlockSize;
			BlkY /= BLK_Y_SIZE;

			// compute the positions of the other 3 blocks
			BlkXp1 = LIMIT_COORD(BlkX+1, BlkXDim, AssumeImageTiles);
			BlkYp1 = LIMIT_COORD(BlkY+1, BlkYDim, AssumeImageTiles);

			// Map to block memory locations
			pBlocks[0][0] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkY, BlkX);
			pBlocks[0][1] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkY, BlkXp1);
			pBlocks[1][0] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkYp1, BlkX);
			pBlocks[1][1] = pCompressedData +TwiddleUV(BlkYDim, BlkXDim, BlkYp1, BlkXp1);


			/*
				extract the colours and the modulation information IF the previous values
				have changed.
			*/
			if(memcmp(pPrevious, pBlocks, 4*sizeof(void*)) != 0)
			{
				StartY = 0;
				for(i = 0; i < 2; i++)
				{
					StartX = 0;
					for(j = 0; j < 2; j++)
					{
						Unpack5554Colour(pBlocks[i][j], Colours5554[i][j].Reps);

						UnpackModulations(pBlocks[i][j],
										  Do2bitMode,
										  ModulationVals,
										  ModulationModes,
										  StartX, StartY);

						StartX += XBlockSize;
					}

					StartY += BLK_Y_SIZE;
				}

				// make a copy of the new pointers
				memcpy(pPrevious, pBlocks, 4*sizeof(void*));
			}

			// decompress the pixel.  First compute the interpolated A and B signals
			InterpolateColours(Colours5554[0][0].Reps[0],
							   Colours5554[0][1].Reps[0],
							   Colours5554[1][0].Reps[0],
							   Colours5554[1][1].Reps[0],
							   Do2bitMode, x, y,
							   ASig);

			InterpolateColours(Colours5554[0][0].Reps[1],
							   Colours5554[0][1].Reps[1],
							   Colours5554[1][0].Reps[1],
							   Colours5554[1][1].Reps[1],
							   Do2bitMode, x, y,
							   BSig);

			GetModulationValue(x,y, Do2bitMode, (const int (*)[16])ModulationVals, (const int (*)[16])ModulationModes,
							   &Mod, &DoPT);

			// compute the modulated colour
			for(i = 0; i < 4; i++)
			{
				Result[i] = ASig[i] * 8 + Mod * (BSig[i] - ASig[i]);
				Result[i] >>= 3;
			}

			if(DoPT)
				Result[3] = 0;

			// Store the result in the output image
			uPosition = (x+y*XDim)<<2;
			pResultImage[uPosition+0] = (U8)Result[0];
			pResultImage[uPosition+1] = (U8)Result[1];
			pResultImage[uPosition+2] = (U8)Result[2];
			pResultImage[uPosition+3] = (U8)Result[3];
		}
	}
}

/****************************
**	ETC Compression
****************************/

/*****************************************************************************
Macros
*****************************************************************************/
#define _CLAMP_(X,Xmin,Xmax) (  (X)<(Xmax) ?  (  (X)<(Xmin)?(Xmin):(X)  )  : (Xmax)    )

/*****************************************************************************
Constants
******************************************************************************/
unsigned int ETC_FLIP =  0x01000000;
unsigned int ETC_DIFF = 0x02000000;
const int mod[8][4]={{2, 8,-2,-8},
					{5, 17, -5, -17},
					{9, 29, -9, -29},
					{13, 42, -13, -42},
					{18, 60, -18, -60},
					{24, 80, -24, -80},
					{33, 106, -33, -106},
					{47, 183, -47, -183}};

 /*!***********************************************************************
 @Function		modifyPixel
 @Input			red		Red value of pixel
 @Input			green	Green value of pixel
 @Input			blue	Blue value of pixel
 @Input			x	Pixel x position in block
 @Input			y	Pixel y position in block
 @Input			modBlock	Values for the current block
 @Input			modTable	Modulation values
 @Returns		Returns actual pixel colour
 @Description	Used by ETCTextureDecompress
*************************************************************************/
unsigned long modifyPixel(int red, int green, int blue, int x, int y, unsigned long modBlock, int modTable)
{
	int index = x*4+y, pixelMod;
	unsigned long mostSig = modBlock<<1;

	if (index<8)
		pixelMod = mod[modTable][((modBlock>>(index+24))&0x1)+((mostSig>>(index+8))&0x2)];
	else
		pixelMod = mod[modTable][((modBlock>>(index+8))&0x1)+((mostSig>>(index-8))&0x2)];

	red = _CLAMP_(red+pixelMod,0,255);
	green = _CLAMP_(green+pixelMod,0,255);
	blue = _CLAMP_(blue+pixelMod,0,255);

	return ((red<<16) + (green<<8) + blue)|0xff000000;
}

 /*!***********************************************************************
 @Function		ETCTextureDecompress
 @Input			pSrcData The ETC texture data to decompress
 @Input			x X dimension of the texture
 @Input			y Y dimension of the texture
 @Modified		pDestData The decompressed texture data
 @Input			nMode The format of the data
 @Returns		The number of bytes of ETC data decompressed
 @Description	Decompresses ETC to RGBA 8888
*************************************************************************/
int ETCTextureDecompress(const void * const pSrcData, const int &x, const int &y, const void *pDestData,const int &/*nMode*/)
{
	unsigned long blockTop, blockBot, *input = (unsigned long*)pSrcData, *output;
	unsigned char red1, green1, blue1, red2, green2, blue2;
	bool bFlip, bDiff;
	int modtable1,modtable2;

	for(int i=0;i<y;i+=4)
	{
		for(int m=0;m<x;m+=4)
		{
				blockTop = *(input++);
				blockBot = *(input++);

			output = (unsigned long*)pDestData + i*x +m;

			// check flipbit
			bFlip = (blockTop & ETC_FLIP) != 0;
			bDiff = (blockTop & ETC_DIFF) != 0;

			if(bDiff)
			{	// differential mode 5 colour bits + 3 difference bits
				// get base colour for subblock 1
				blue1 = (unsigned char)((blockTop&0xf80000)>>16);
				green1 = (unsigned char)((blockTop&0xf800)>>8);
				red1 = (unsigned char)(blockTop&0xf8);

				// get differential colour for subblock 2
				signed char blues = (signed char)(blue1>>3) + ((signed char) ((blockTop & 0x70000) >> 11)>>5);
				signed char greens = (signed char)(green1>>3) + ((signed char)((blockTop & 0x700) >>3)>>5);
				signed char reds = (signed char)(red1>>3) + ((signed char)((blockTop & 0x7)<<5)>>5);

				blue2 = (unsigned char)blues;
				green2 = (unsigned char)greens;
				red2 = (unsigned char)reds;

				red1 = red1 +(red1>>5);	// copy bits to lower sig
				green1 = green1 + (green1>>5);	// copy bits to lower sig
				blue1 = blue1 + (blue1>>5);	// copy bits to lower sig

				red2 = (red2<<3) +(red2>>2);	// copy bits to lower sig
				green2 = (green2<<3) + (green2>>2);	// copy bits to lower sig
				blue2 = (blue2<<3) + (blue2>>2);	// copy bits to lower sig
			}
			else
			{	// individual mode 4 + 4 colour bits
				// get base colour for subblock 1
				blue1 = (unsigned char)((blockTop&0xf00000)>>16);
				blue1 = blue1 +(blue1>>4);	// copy bits to lower sig
				green1 = (unsigned char)((blockTop&0xf000)>>8);
				green1 = green1 + (green1>>4);	// copy bits to lower sig
				red1 = (unsigned char)(blockTop&0xf0);
				red1 = red1 + (red1>>4);	// copy bits to lower sig

				// get base colour for subblock 2
				blue2 = (unsigned char)((blockTop&0xf0000)>>12);
				blue2 = blue2 +(blue2>>4);	// copy bits to lower sig
				green2 = (unsigned char)((blockTop&0xf00)>>4);
				green2 = green2 + (green2>>4);	// copy bits to lower sig
				red2 = (unsigned char)((blockTop&0xf)<<4);
				red2 = red2 + (red2>>4);	// copy bits to lower sig
			}
			// get the modtables for each subblock
			modtable1 = (blockTop>>29)&0x7;
			modtable2 = (blockTop>>26)&0x7;

			if(!bFlip)
			{	// 2 2x4 blocks side by side

				for(int j=0;j<4;j++)	// vertical
				{
					for(int k=0;k<2;k++)	// horizontal
					{
						*(output+j*x+k) = modifyPixel(red1,green1,blue1,k,j,blockBot,modtable1);
						*(output+j*x+k+2) = modifyPixel(red2,green2,blue2,k+2,j,blockBot,modtable2);
					}
				}

			}
			else
			{	// 2 4x2 blocks on top of each other
				for(int j=0;j<2;j++)
				{
					for(int k=0;k<4;k++)
					{
						*(output+j*x+k) = modifyPixel(red1,green1,blue1,k,j,blockBot,modtable1);
						*(output+(j+2)*x+k) = modifyPixel(red2,green2,blue2,k,j+2,blockBot,modtable2);
					}
				}
			}
		}
	}

	return x*y/2;
}

/*!***********************************************************************
@Function		PVRTDecompressETC
@Input			pSrcData The ETC texture data to decompress
@Input			x X dimension of the texture
@Input			y Y dimension of the texture
@Modified		pDestData The decompressed texture data
@Input			nMode The format of the data
@Returns		The number of bytes of ETC data decompressed
@Description	Decompresses ETC to RGBA 8888
*************************************************************************/
int PVRTDecompressETC(const void * const pSrcData,
						 const unsigned int &x,
						 const unsigned int &y,
						 void *pDestData,
						 const int &nMode)
{
	int i32read;

	if(x<ETC_MIN_TEXWIDTH || y<ETC_MIN_TEXHEIGHT)
	{	// decompress into a buffer big enough to take the minimum size
		char* pTempBuffer =	(char*)malloc(PVRT_MAX(x,ETC_MIN_TEXWIDTH)*PVRT_MAX(y,ETC_MIN_TEXHEIGHT)*4);
		i32read = ETCTextureDecompress(pSrcData,PVRT_MAX(x,ETC_MIN_TEXWIDTH),PVRT_MAX(y,ETC_MIN_TEXHEIGHT),pTempBuffer,nMode);

		for(unsigned int i=0;i<y;i++)
		{	// copy from larger temp buffer to output data
			memcpy((char*)(pDestData)+i*x*4,pTempBuffer+PVRT_MAX(x,ETC_MIN_TEXWIDTH)*4*i,x*4);
		}

		if(pTempBuffer) free(pTempBuffer);
	}
	else	// decompress larger MIP levels straight into the output data
		i32read = ETCTextureDecompress(pSrcData,x,y,pDestData,nMode);

	// swap r and b channels
	unsigned char* pSwap = (unsigned char*)pDestData, swap;

	for(unsigned int i=0;i<y;i++)
		for(unsigned int j=0;j<x;j++)
		{
			swap = pSwap[0];
			pSwap[0] = pSwap[2];
			pSwap[2] = swap;
			pSwap+=4;
		}

	return i32read;
}


/*!***********************************************************************
@Function			IsGLExtensionSupported
@Input				extension extension to query for
@Returns			True if the extension is supported
@Description		Queries for support of an extension
*************************************************************************/
bool IsGLExtensionSupported(const char *extension)
{
	// The recommended technique for querying OpenGL extensions;
	// from http://opengl.org/resources/features/OGLextensions/
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	/* Extension names should not have spaces. */
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;

	extensions = glGetString(GL_EXTENSIONS);

	/* It takes a bit of care to be fool-proof about parsing the
	OpenGL extensions string. Don't be fooled by sub-strings, etc. */
	start = extensions;
	for (;;) {
		where = (GLubyte *) strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;
		start = terminator;
	}

	return false;
}

/*!***************************************************************************
 @Function		PVRTTextureLoadFromPointer
 @Input			pointer				Pointer to header-texture's structure
 @Modified		texName				the OpenGL ES texture name as returned by glBindTexture
 @Modified		psTextureHeader		Pointer to a PVR_Texture_Header struct. Modified to
									contain the header data of the returned texture Ignored if NULL.
 @Input			bAllowDecompress	Allow decompression if PVRTC is not supported in hardware.
 @Input			nLoadFromLevel		Which mipmap level to start loading from (0=all)
 @Input			texPtr				If null, texture follows header, else texture is here.
 @Return		PVR_SUCCESS on success
 @Description	Allows textures to be stored in C header files and loaded in. Can load parts of a
				mipmaped texture (ie skipping the highest detailed levels).In OpenGL Cube Map, each
				texture's up direction is defined as next (view direction, up direction),
				(+x,-y)(-x,-y)(+y,+z)(-y,-z)(+z,-y)(-z,-y).
				Sets the texture MIN/MAG filter to GL_LINEAR_MIPMAP_NEAREST/GL_LINEAR
				if mipmaps are present, GL_LINEAR/GL_LINEAR otherwise.
*****************************************************************************/
EPVRTError PVRTTextureLoadFromPointer(	const void* pointer,
										GLuint *const texName,
										const void *psTextureHeader,
										bool bAllowDecompress,
										const unsigned int nLoadFromLevel,
										const void * const texPtr)
{
	PVR_Texture_Header* psPVRHeader = (PVR_Texture_Header*)pointer;
	unsigned int u32NumSurfs;

	// perform checks for old PVR psPVRHeader
	if(psPVRHeader->dwHeaderSize!=sizeof(PVR_Texture_Header))
	{	// Header V1
		if(psPVRHeader->dwHeaderSize==PVRTEX_V1_HEADER_SIZE)
		{	// react to old psPVRHeader: i.e. fill in numsurfs as this is missing from old header
			PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: this is an old pvr"
				" - you can use PVRTexTool to update its header.\n");
			if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
				u32NumSurfs = 6;
			else
				u32NumSurfs = 1;
		}
		else
		{	// not a pvr at all
			PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: not a valid pvr.\n");
			return PVR_FAIL;
		}
	}
	else
	{	// Header V2
		if(psPVRHeader->dwNumSurfs<1)
		{	// encoded with old version of PVRTexTool before zero numsurfs bug found.
			if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
				u32NumSurfs = 6;
			else
				u32NumSurfs = 1;
		}
		else
		{
			u32NumSurfs = psPVRHeader->dwNumSurfs;
		}
	}

	GLuint textureName;
	GLenum eTextureFormat = 0;
	GLenum eTextureInternalFormat = 0;	// often this is the same as textureFormat, but not for BGRA8888 on the iPhone, for instance
	GLenum eTextureType = 0;
	GLenum eTarget;

	bool bIsPVRTCSupported = IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc");
#ifndef TARGET_OS_IPHONE
	bool bIsBGRA8888Supported  = IsGLExtensionSupported("GL_IMG_texture_format_BGRA8888");
#else
	bool bIsBGRA8888Supported  = IsGLExtensionSupported("GL_APPLE_texture_format_BGRA8888");
#endif
	bool bIsFloat16Supported = IsGLExtensionSupported("GL_OES_texture_half_float");
	bool bIsFloat32Supported = IsGLExtensionSupported("GL_OES_texture_float");
#ifndef TARGET_OS_IPHONE
	bool bIsETCSupported = IsGLExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");
#endif
	*texName = 0;	// install warning value
	bool bIsCompressedFormatSupported = false, bIsCompressedFormat = false;

	/* Only accept untwiddled data UNLESS texture format is PVRTC */
	if ( ((psPVRHeader->dwpfFlags & PVRTEX_TWIDDLE) == PVRTEX_TWIDDLE)
		&& ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC2)
		&& ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)!=OGL_PVRTC4) )
	{
		// We need to load untwiddled textures -- GL will twiddle for us.
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: texture should be untwiddled.\n");
		//return PVR_FAIL;
	}
	
	unsigned int ePixelType = psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE;

	switch(ePixelType)
	{
	case OGL_RGBA_4444:
		eTextureFormat = eTextureInternalFormat = GL_RGBA;
		eTextureType = GL_UNSIGNED_SHORT_4_4_4_4;
		break;

	case OGL_RGBA_5551:
		eTextureFormat = eTextureInternalFormat = GL_RGBA ;
		eTextureType = GL_UNSIGNED_SHORT_5_5_5_1;
		break;

	case OGL_RGBA_8888:
		eTextureFormat = eTextureInternalFormat = GL_RGBA;
		eTextureType = GL_UNSIGNED_BYTE ;
		break;

	/* New OGL Specific Formats Added */

	case OGL_RGB_565:
		eTextureFormat = eTextureInternalFormat = GL_RGB ;
		eTextureType = GL_UNSIGNED_SHORT_5_6_5;
		break;

	case OGL_RGB_555:
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: pixel type OGL_RGB_555 not supported.\n");
		return PVR_FAIL; // Deal with exceptional case

	case OGL_RGB_888:
		eTextureFormat = eTextureInternalFormat = GL_RGB ;
		eTextureType = GL_UNSIGNED_BYTE;
		break;

	case OGL_I_8:
		eTextureFormat = eTextureInternalFormat = GL_LUMINANCE;
		eTextureType = GL_UNSIGNED_BYTE;
		break;

	case OGL_AI_88:
		eTextureFormat = eTextureInternalFormat = GL_LUMINANCE_ALPHA ;
		eTextureType = GL_UNSIGNED_BYTE;
		break;


	case MGLPT_PVRTC2:
	case OGL_PVRTC2:
		if(bIsPVRTCSupported)
		{
			bIsCompressedFormatSupported = bIsCompressedFormat = true;
#ifdef RENDER_ES_3_0
			return PVR_FAIL;
#else
			eTextureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG ;	// PVRTC2
#endif
		}
		else
		{
			if(bAllowDecompress)
			{
				bIsCompressedFormatSupported = false;
				bIsCompressedFormat = true;
				eTextureFormat = eTextureInternalFormat = GL_RGBA;
				eTextureType = GL_UNSIGNED_BYTE;
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: PVRTC2 not supported. Converting to RGBA8888 instead.\n");
			}
			else
			{
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer error: PVRTC2 not supported.\n");
				return PVR_FAIL;
			}
		}
		break;

	case MGLPT_PVRTC4:
	case OGL_PVRTC4:
		if(bIsPVRTCSupported)
		{
			bIsCompressedFormatSupported = bIsCompressedFormat = true;
#ifdef RENDER_ES_3_0
			return PVR_FAIL;
#else
			eTextureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ;	// PVRTC4
#endif
		}
		else
		{
			if(bAllowDecompress)
			{
				bIsCompressedFormatSupported = false;
				bIsCompressedFormat = true;
				eTextureFormat = eTextureInternalFormat = GL_RGBA;
				eTextureType = GL_UNSIGNED_BYTE;
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: PVRTC4 not supported. Converting to RGBA8888 instead.\n");
			}
			else
			{
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer error: PVRTC4 not supported.\n");
				return PVR_FAIL;
			}
		}
		break;
	case OGL_A_8:
		eTextureFormat = eTextureInternalFormat = GL_ALPHA;
		eTextureType = GL_UNSIGNED_BYTE;
		break;

	case OGL_BGRA_8888:
		if(bIsBGRA8888Supported)
		{
			eTextureType = GL_UNSIGNED_BYTE;
#ifndef __APPLE__
			eTextureFormat = eTextureInternalFormat = GL_BGRA;
#else
			eTextureFormat = GL_BGRA;
			eTextureInternalFormat = GL_RGBA;
#endif
		}
		else
		{
			PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: Unable to load GL_BGRA texture as extension GL_IMG_texture_format_BGRA8888 is unsupported.\n");
			return PVR_FAIL;
		}
		break;
	case D3D_ABGR_16161616F:
		if(bIsFloat16Supported)
		{
			eTextureFormat = eTextureInternalFormat =  GL_RGBA ;
#ifdef RENDER_ES_3_0
			eTextureType = GL_HALF_FLOAT;
#else
			eTextureType = GL_HALF_FLOAT_OES;
#endif
		}
		break;
	case D3D_ABGR_32323232F:
		if(bIsFloat32Supported)
		{
			eTextureFormat = eTextureInternalFormat =  GL_RGBA ;
			eTextureType = GL_FLOAT;
		}
		break;

#ifndef TARGET_OS_IPHONE
	case ETC_RGB_4BPP:
		if(bIsETCSupported)
		{
			bIsCompressedFormatSupported = bIsCompressedFormat = true;
			eTextureFormat = GL_ETC1_RGB8_OES;
		}
		else
		{
			if(bAllowDecompress)
			{
				bIsCompressedFormatSupported = false;
				bIsCompressedFormat = true;
				eTextureFormat = GL_RGBA;
				eTextureType = GL_UNSIGNED_BYTE;
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer warning: ETC not supported. Converting to RGBA8888 instead.\n");
			}
			else
			{
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer error: ETC not supported.\n");
				return PVR_FAIL;
			}
		}
		break;
#endif

// 	case ETC_RGB_4BPP:
// 		{
//       //ls测试
// 			bIsCompressedFormat = true;
// 			break;
// 		}

	default:											// NOT SUPPORTED
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: pixel type not supported.\n");
		return PVR_FAIL;
	}
	
	// load the texture up
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);				// Never have row-aligned in psPVRHeaders

	glGenTextures(1, &textureName);

	//  check that this data is cube map data or not.
	if(psPVRHeader->dwpfFlags & PVRTEX_CUBEMAP)
		eTarget = GL_TEXTURE_CUBE_MAP;
	else
		eTarget = GL_TEXTURE_2D;

	glBindTexture(eTarget, textureName);

	if(glGetError())
	{
		PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: glBindTexture() failed.\n");
		return PVR_FAIL;
	}

	for(unsigned int i=0; i<u32NumSurfs; i++)
	{
		char *theTexturePtr = (texPtr? (char*)texPtr :  (char*)psPVRHeader + psPVRHeader->dwHeaderSize) + psPVRHeader->dwTextureDataSize * i;
		char *theTextureToLoad = 0;
		int		nMIPMapLevel;
		int		nTextureLevelsNeeded = (psPVRHeader->dwpfFlags & PVRTEX_MIPMAP)? psPVRHeader->dwMipMapCount : 0;
		unsigned int		nSizeX= psPVRHeader->dwWidth, nSizeY = psPVRHeader->dwHeight;
		unsigned int		CompressedImageSize = 0;

		for(nMIPMapLevel = 0; nMIPMapLevel <= nTextureLevelsNeeded; nSizeX=PVRT_MAX(nSizeX/2, (unsigned int)1), nSizeY=PVRT_MAX(nSizeY/2, (unsigned int)1), nMIPMapLevel++)
		{
			theTextureToLoad = theTexturePtr;

			// Load the Texture

			// If the texture is PVRTC or ETC then use GLCompressedTexImage2D
			if(bIsCompressedFormat)
			{
				/* Calculate how many bytes this MIP level occupies */
				if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
				{
					CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC2_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC2_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount) / 8;
				}
				else if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC4)
				{
					CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC4_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC4_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount) / 8;
				}
				else
				{// ETC
					CompressedImageSize = ( PVRT_MAX(nSizeX, ETC_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, ETC_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount) / 8;
				}

				if(((signed int)nMIPMapLevel - (signed int)nLoadFromLevel) >= 0)
				{
					if(bIsCompressedFormatSupported)
					{
						if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
						{
							/* Load compressed texture data at selected MIP level */
							glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, nMIPMapLevel-nLoadFromLevel, eTextureFormat, nSizeX, nSizeY, 0,
											CompressedImageSize, theTextureToLoad);
						}
						else
						{
							/* Load compressed texture data at selected MIP level */
							glCompressedTexImage2D(GL_TEXTURE_2D, nMIPMapLevel-nLoadFromLevel, eTextureFormat, nSizeX, nSizeY, 0,
											CompressedImageSize, theTextureToLoad);

						}
					}
					else
					{
						// Convert PVRTC to 32-bit
						PVRTuint8 *u8TempTexture = (PVRTuint8*)malloc(nSizeX*nSizeY*4);
						if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
						{
							PVRTDecompressPVRTC(theTextureToLoad, 1, nSizeX, nSizeY, u8TempTexture);
						}
						else if((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC4)
						{
							PVRTDecompressPVRTC(theTextureToLoad, 0, nSizeX, nSizeY, u8TempTexture);
						}
						else
						{	// ETC
							PVRTDecompressETC(theTextureToLoad, nSizeX, nSizeY, u8TempTexture, 0);
						}


						if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
						{// Load compressed cubemap data at selected MIP level
							// Upload the texture as 32-bits
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,nMIPMapLevel-nLoadFromLevel,GL_RGBA,
								nSizeX,nSizeY,0, GL_RGBA,GL_UNSIGNED_BYTE,u8TempTexture);
							FREE(u8TempTexture);
						}
						else
						{// Load compressed 2D data at selected MIP level
							// Upload the texture as 32-bits
							glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel-nLoadFromLevel,GL_RGBA,
								nSizeX,nSizeY,0, GL_RGBA,GL_UNSIGNED_BYTE,u8TempTexture);
							FREE(u8TempTexture);
						}
					}
				}
			}
			else
			{
				if(((signed int)nMIPMapLevel - (signed int)nLoadFromLevel) >= 0)
				{
					if(psPVRHeader->dwpfFlags&PVRTEX_CUBEMAP)
					{
						/* Load uncompressed texture data at selected MIP level */
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,nMIPMapLevel-nLoadFromLevel,eTextureInternalFormat,nSizeX,nSizeY,
							0, eTextureFormat,eTextureType,theTextureToLoad);
					}
					else
					{
						/* Load uncompressed texture data at selected MIP level */
						glTexImage2D(GL_TEXTURE_2D,nMIPMapLevel-nLoadFromLevel,eTextureInternalFormat,nSizeX,nSizeY,0,eTextureFormat,eTextureType,theTextureToLoad);
					}
				}
			}



			if(glGetError())
			{
				PVRTErrorOutputDebug("PVRTTextureLoadFromPointer failed: glTexImage2D() failed.\n");
				return PVR_FAIL;
			}

			// offset the texture pointer by one mip-map level

			/* PVRTC case */
			if ( bIsCompressedFormat )
			{
				theTexturePtr += CompressedImageSize;
			}
			else
			{
				/* New formula that takes into account bit counts inferior to 8 (e.g. 1 bpp) */
				theTexturePtr += (nSizeX * nSizeY * psPVRHeader->dwBitCount + 7) / 8;
			}
		}
	}

	*texName = textureName;

	if(psTextureHeader)
	{
		*(PVR_Texture_Header*)psTextureHeader = *psPVRHeader;
		((PVR_Texture_Header*)psTextureHeader)->dwPVR = PVRTEX_IDENTIFIER;
		((PVR_Texture_Header*)psTextureHeader)->dwNumSurfs = u32NumSurfs;
	}
#ifdef RENDER_ES_3_0
	if(eTextureType==GL_FLOAT || eTextureType==GL_HALF_FLOAT)
#else
	if(eTextureType==GL_FLOAT || eTextureType==GL_HALF_FLOAT_OES)
#endif
	{
		if(!psPVRHeader->dwMipMapCount)
		{	// Texture filter modes are limited to these for float textures
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
	else
	{
		if(!psPVRHeader->dwMipMapCount)
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	if(
		(psPVRHeader->dwWidth & (psPVRHeader->dwWidth - 1)) |
		(psPVRHeader->dwHeight & (psPVRHeader->dwHeight - 1)))
	{
		/*
			NPOT textures requires the wrap mode to be set explicitly to
			GL_CLAMP_TO_EDGE or the texture will be inconsistent.
		*/
		glTexParameteri(eTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(eTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	return PVR_SUCCESS;
}