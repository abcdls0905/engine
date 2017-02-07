#ifndef __LIB_PVR_H__
#define __LIB_PVR_H__


#include "render_header.h"

/****************************************************************************
** Integer types
****************************************************************************/

typedef signed char        PVRTint8;
typedef signed short       PVRTint16;
typedef signed int         PVRTint32;
typedef unsigned char      PVRTuint8;
typedef unsigned short     PVRTuint16;
typedef unsigned int       PVRTuint32;

#if !defined(__BADA__) && (defined(__int64) || defined(_WIN32))
typedef signed __int64     PVRTint64;
typedef unsigned __int64   PVRTuint64;
#elif defined(TInt64)
typedef TInt64             PVRTint64;
typedef TUInt64            PVRTuint64;
#else
typedef signed long long   PVRTint64;
typedef unsigned long long PVRTuint64;
#endif



enum PixelType
{
	MGLPT_ARGB_4444 = 0x00,
	MGLPT_ARGB_1555,
	MGLPT_RGB_565,
	MGLPT_RGB_555,
	MGLPT_RGB_888,
	MGLPT_ARGB_8888,
	MGLPT_ARGB_8332,
	MGLPT_I_8,
	MGLPT_AI_88,
	MGLPT_1_BPP,
	MGLPT_VY1UY0,
	MGLPT_Y1VY0U,
	MGLPT_PVRTC2,
	MGLPT_PVRTC4,

	// OpenGL version of pixel types
	OGL_RGBA_4444= 0x10,
	OGL_RGBA_5551,
	OGL_RGBA_8888,
	OGL_RGB_565,
	OGL_RGB_555,
	OGL_RGB_888,
	OGL_I_8,
	OGL_AI_88,
	OGL_PVRTC2,
	OGL_PVRTC4,
	OGL_BGRA_8888,
	OGL_A_8,
	OGL_PVRTCII4,	//Not in use
	OGL_PVRTCII2,	//Not in use

#ifdef _WIN32
	// S3TC Encoding
	D3D_DXT1 = 0x20,
	D3D_DXT2,
	D3D_DXT3,
	D3D_DXT4,
	D3D_DXT5,
#endif

	//RGB Formats
	D3D_RGB_332 
#ifndef _WIN32
	= 0x25
#endif
	,
	D3D_AL_44,
	D3D_LVU_655,
	D3D_XLVU_8888,
	D3D_QWVU_8888,
	
	//10 bit integer - 2 bit alpha
	D3D_ABGR_2101010,
	D3D_ARGB_2101010,
	D3D_AWVU_2101010,

	//16 bit integers
	D3D_GR_1616,
	D3D_VU_1616,
	D3D_ABGR_16161616,

	//Float Formats
	D3D_R16F,
	D3D_GR_1616F,
	D3D_ABGR_16161616F,

	//32 bits per channel
	D3D_R32F,
	D3D_GR_3232F,
	D3D_ABGR_32323232F,
	
	// Ericsson
	ETC_RGB_4BPP,
	ETC_RGBA_EXPLICIT,				// unimplemented
	ETC_RGBA_INTERPOLATED,			// unimplemented
	
	D3D_A8 = 0x40,
	D3D_V8U8,
	D3D_L16,
			
	D3D_L8,
	D3D_AL_88,

	//Y'UV Colourspace
	D3D_UYVY,
	D3D_YUY2,
	
	// DX10
	DX10_R32G32B32A32_FLOAT= 0x50,
	DX10_R32G32B32A32_UINT , 
	DX10_R32G32B32A32_SINT,

	DX10_R32G32B32_FLOAT,
	DX10_R32G32B32_UINT,
	DX10_R32G32B32_SINT,

	DX10_R16G16B16A16_FLOAT ,
	DX10_R16G16B16A16_UNORM,
	DX10_R16G16B16A16_UINT ,
	DX10_R16G16B16A16_SNORM ,
	DX10_R16G16B16A16_SINT ,

	DX10_R32G32_FLOAT ,
	DX10_R32G32_UINT ,
	DX10_R32G32_SINT ,

	DX10_R10G10B10A2_UNORM ,
	DX10_R10G10B10A2_UINT ,

	DX10_R11G11B10_FLOAT ,				// unimplemented

	DX10_R8G8B8A8_UNORM , 
	DX10_R8G8B8A8_UNORM_SRGB ,
	DX10_R8G8B8A8_UINT ,
	DX10_R8G8B8A8_SNORM ,
	DX10_R8G8B8A8_SINT ,

	DX10_R16G16_FLOAT , 
	DX10_R16G16_UNORM , 
	DX10_R16G16_UINT , 
	DX10_R16G16_SNORM ,
	DX10_R16G16_SINT ,

	DX10_R32_FLOAT ,
	DX10_R32_UINT ,
	DX10_R32_SINT ,

	DX10_R8G8_UNORM ,
	DX10_R8G8_UINT ,
	DX10_R8G8_SNORM , 
	DX10_R8G8_SINT ,

	DX10_R16_FLOAT ,
	DX10_R16_UNORM ,
	DX10_R16_UINT ,
	DX10_R16_SNORM ,
	DX10_R16_SINT ,

	DX10_R8_UNORM, 
	DX10_R8_UINT,
	DX10_R8_SNORM,
	DX10_R8_SINT,

	DX10_A8_UNORM, 
	DX10_R1_UNORM, 
	DX10_R9G9B9E5_SHAREDEXP,	// unimplemented
	DX10_R8G8_B8G8_UNORM,		// unimplemented
	DX10_G8R8_G8B8_UNORM,		// unimplemented

#ifdef _WIN32
	DX10_BC1_UNORM,	
	DX10_BC1_UNORM_SRGB,

	DX10_BC2_UNORM,	
	DX10_BC2_UNORM_SRGB,

	DX10_BC3_UNORM,	
	DX10_BC3_UNORM_SRGB,

	DX10_BC4_UNORM,				// unimplemented
	DX10_BC4_SNORM,				// unimplemented

	DX10_BC5_UNORM,				// unimplemented
	DX10_BC5_SNORM,				// unimplemented
#endif
	// OpenVG

	/* RGB{A,X} channel ordering */
	ePT_VG_sRGBX_8888  = 0x90,
	ePT_VG_sRGBA_8888,
	ePT_VG_sRGBA_8888_PRE,
	ePT_VG_sRGB_565,
	ePT_VG_sRGBA_5551,
	ePT_VG_sRGBA_4444,
	ePT_VG_sL_8,
	ePT_VG_lRGBX_8888,
	ePT_VG_lRGBA_8888,
	ePT_VG_lRGBA_8888_PRE,
	ePT_VG_lL_8,
	ePT_VG_A_8,
	ePT_VG_BW_1,

	/* {A,X}RGB channel ordering */
	ePT_VG_sXRGB_8888,
	ePT_VG_sARGB_8888,
	ePT_VG_sARGB_8888_PRE,
	ePT_VG_sARGB_1555,
	ePT_VG_sARGB_4444,
	ePT_VG_lXRGB_8888,
	ePT_VG_lARGB_8888,
	ePT_VG_lARGB_8888_PRE,

	/* BGR{A,X} channel ordering */
	ePT_VG_sBGRX_8888,
	ePT_VG_sBGRA_8888,
	ePT_VG_sBGRA_8888_PRE,
	ePT_VG_sBGR_565,
	ePT_VG_sBGRA_5551,
	ePT_VG_sBGRA_4444,
	ePT_VG_lBGRX_8888,
	ePT_VG_lBGRA_8888,
	ePT_VG_lBGRA_8888_PRE,

	/* {A,X}BGR channel ordering */
	ePT_VG_sXBGR_8888,
	ePT_VG_sABGR_8888 ,
	ePT_VG_sABGR_8888_PRE,
	ePT_VG_sABGR_1555,
	ePT_VG_sABGR_4444,
	ePT_VG_lXBGR_8888,
	ePT_VG_lABGR_8888,
	ePT_VG_lABGR_8888_PRE,

	// max cap for iterating
	END_OF_PIXEL_TYPES,

	MGLPT_NOTYPE = 0xffffffff

};


/*! Enum error codes */
enum EPVRTError
{
	PVR_SUCCESS = 0,
	PVR_FAIL = 1,
	PVR_OVERFLOW = 2
};

/*!***************************************************************************
Describes the header of a PVR header-texture
*****************************************************************************/
struct PVR_Texture_Header
{
	PVRTuint32 dwHeaderSize;			/*!< size of the structure */
	PVRTuint32 dwHeight;				/*!< height of surface to be created */
	PVRTuint32 dwWidth;				/*!< width of input surface */
	PVRTuint32 dwMipMapCount;			/*!< number of mip-map levels requested */
	PVRTuint32 dwpfFlags;				/*!< pixel format flags */
	PVRTuint32 dwTextureDataSize;		/*!< Total size in bytes */
	PVRTuint32 dwBitCount;			/*!< number of bits per pixel  */
	PVRTuint32 dwRBitMask;			/*!< mask for red bit */
	PVRTuint32 dwGBitMask;			/*!< mask for green bits */
	PVRTuint32 dwBBitMask;			/*!< mask for blue bits */
	PVRTuint32 dwAlphaBitMask;		/*!< mask for alpha channel */
	PVRTuint32 dwPVR;					/*!< magic number identifying pvr file */
	PVRTuint32 dwNumSurfs;			/*!< the number of surfaces present in the pvr */
} ;

/*****************************************************************************
* constants
*****************************************************************************/

const PVRTuint32 PVRTEX_MIPMAP		= (1<<8);		// has mip map levels
const PVRTuint32 PVRTEX_TWIDDLE		= (1<<9);		// is twiddled
const PVRTuint32 PVRTEX_BUMPMAP		= (1<<10);		// has normals encoded for a bump map
const PVRTuint32 PVRTEX_TILING		= (1<<11);		// is bordered for tiled pvr
const PVRTuint32 PVRTEX_CUBEMAP		= (1<<12);		// is a cubemap/skybox
const PVRTuint32 PVRTEX_FALSEMIPCOL	= (1<<13);		// are there false coloured MIP levels
const PVRTuint32 PVRTEX_VOLUME		= (1<<14);		// is this a volume texture
const PVRTuint32 PVRTEX_ALPHA			= (1<<15);		// v2.1 is there transparency info in the texture
const PVRTuint32 PVRTEX_VERTICAL_FLIP	= (1<<16);		// v2.1 is the texture vertically flipped

const PVRTuint32 PVRTEX_PIXELTYPE		= 0xff;			// pixel type is always in the last 16bits of the flags
const PVRTuint32 PVRTEX_IDENTIFIER	= 0x21525650;	// the pvr identifier is the characters 'P','V','R'

const PVRTuint32 PVRTEX_V1_HEADER_SIZE = 44;			// old header size was 44 for identification purposes

const PVRTuint32 PVRTC2_MIN_TEXWIDTH		= 16;
const PVRTuint32 PVRTC2_MIN_TEXHEIGHT		= 8;
const PVRTuint32 PVRTC4_MIN_TEXWIDTH		= 8;
const PVRTuint32 PVRTC4_MIN_TEXHEIGHT		= 8;
const PVRTuint32 ETC_MIN_TEXWIDTH			= 4;
const PVRTuint32 ETC_MIN_TEXHEIGHT		= 4;
const PVRTuint32 DXT_MIN_TEXWIDTH			= 4;
const PVRTuint32 DXT_MIN_TEXHEIGHT		= 4;


//platform is endian
inline bool platformIsEndian()
{
	static bool bLittleEndian;
	static bool bIsInit = false;

	if(!bIsInit)
	{
		short int word = 0x0001;
		char *byte = (char*) &word;
		bLittleEndian = byte[0] ? true : false;
		bIsInit = true;
	}

	return bLittleEndian;
};

bool pvr_load_rgba(const void* point, int& width, int& height, unsigned char*& pixels, bool& flip_y);

bool pvr_load_rgba_file(const char* filename,int& width, int& height, unsigned char*& pixels,bool& flip_y);

/*!***********************************************************************
 @Function		PVRTDecompressPVRTC
 @Input			pCompressedData The PVRTC texture data to decompress
 @Input			Do2bitMode Signifies whether the data is PVRTC2 or PVRTC4
 @Input			XDim X dimension of the texture
 @Input			YDim Y dimension of the texture
 @Modified		pResultImage The decompressed texture data
 @Description	Decompresses PVRTC to RGBA 8888
*************************************************************************/
void PVRTDecompressPVRTC(const void *pCompressedData,const int Do2bitMode,const int XDim,const int YDim,unsigned char* pResultImage);

/*
*@Function		PVRTDecompressETC
*@Input			pSrcData The ETC texture data to decompress
*@Input			x X dimension of the texture
*@Input			y Y dimension of the texture
*@Modified		pDestData The decompressed texture data
*@Input			nMode The format of the data
*@Returns		The number of bytes of ETC data decompressed
*@Description	Decompresses ETC to RGBA 8888
*/
int PVRTDecompressETC(const void * const pSrcData,const unsigned int &x,const unsigned int &y,void *pDestData,const int &nMode);


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
										const void *psTextureHeader=NULL,
										bool bAllowDecompress = true,
										const unsigned int nLoadFromLevel=0,
										const void * const texPtr=0);

#endif //__LIB_PVR_H__