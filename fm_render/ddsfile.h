#pragma once

#include <assert.h>


struct ImageData
{
    int height;
    int width;
	int depth;
    int size;
    int num_mipmaps;
    int flags;
	bool iscubmap;
	char* data;
	int format;
};

class DDSFile
{
	#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
	#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
	#define min(a, b)  (((a) < (b)) ? (a) : (b)) 

	typedef unsigned char BYTE;
	typedef unsigned long DWORD;
	typedef unsigned short WORD;
public:
	enum DDSFORMAT
	{
		DDS_DXT1,
		DDS_DXT2,
		DDS_DXT3,
		DDS_DXT4,
		DDS_DXT5,
		DDS_BGRA8,
		DDS_BGR8,
		DDS_BGR5A1,
		DDS_BGR565,
		DDS_MAX
	};
private:
	inline int __LoadFile(const char* filename, char* & buf)
	{
		FILE *fp = fopen(filename, "rb");
		if(fp == NULL)
		{
	#ifdef _OUTPUT_LOG
			printf("loadfile error ==> can't fopen %s, last error %d\n", filename, GetLastError());
	#endif
			return 0;
		}

		fseek(fp, 0L, SEEK_END);
		unsigned long filesize = ftell(fp);
		buf = (char *)malloc(sizeof(char)*filesize + 1);
		fseek(fp, 0L, SEEK_SET);
		fread((char*)buf, sizeof(char), filesize, fp);
		buf[filesize] = 0;
		return filesize;
	}

	struct DdsLoadInfo 
	{
		bool compressed;
		bool swap;
		bool palette;
		unsigned int divSize;
		unsigned int blockBytes;
	};

	const DdsLoadInfo& GetDDSInfo(DDSFORMAT format)
	{
		static const DdsLoadInfo ddsinfo[] = {
			{true, false, false, 4, 8},	//DXT1
			{true, false, false, 4, 16}, //DXT2
			{true, false, false, 4, 16,}, //DXT3
			{true, false, false, 4, 16}, //DXT4
			{true, false, false, 4, 16}, //DXT5
			{false, true, false, 1, 2},
			{false, true, false, 1, 2},
			{false, false, true, 1, 1},
		};
		return ddsinfo[format];
	}	

	class PICT_COLOR
	{
	public:
		union
		{
			struct
			{
				BYTE b;
				BYTE g;
				BYTE r;
				BYTE a;	
			};
			DWORD c;
		};

		inline void operator += (PICT_COLOR & c)
		{
			int _b = b + (int)((char)c.b);
			int _g = g + (int)((char)c.g);
			int _r = r + (int)((char)c.r);
			if(_b > 255) _b = 255; if(_b < 0) _b = 0;
			if(_g > 255) _g = 255; if(_g < 0) _g = 0;
			if(_r > 255) _r = 255; if(_r < 0) _r = 0;
			b = _b; g = _g; r = _r;
		}

		inline void operator = (BYTE n)
		{
			b = g = r = a = n;
		}

		inline void Min(PICT_COLOR & c)
		{
			if(b > c.b) b = c.b;
			if(g > c.g) g = c.g;
			if(r > c.r) r = c.r;
			if(a > c.a) a = c.a;
		}

		inline void Max(PICT_COLOR & c)
		{
			if(b < c.b) b = c.b;
			if(g < c.g) g = c.g;
			if(r < c.r) r = c.r;
			if(a < c.a) a = c.a;
		}

		inline void Blend(float alpha, PICT_COLOR & c1, PICT_COLOR & c2)
		{
			if(alpha < 0.0f) alpha = 0.0f;
			if(alpha > 1.0f) alpha = 1.0f;
			b = BYTE((1.0f - alpha)*c1.b + alpha*c2.b);
			g = BYTE((1.0f - alpha)*c1.g + alpha*c2.g);
			r = BYTE((1.0f - alpha)*c1.r + alpha*c2.r);
			a = BYTE((1.0f - alpha)*c1.a + alpha*c2.a);
		}

		inline void ColorMulAlpha(float alpha)
		{
			if(alpha < 0.0f) alpha = 0.0f;
			if(alpha > 1.0f) alpha = 1.0f;
			b = BYTE(alpha*b);
			g = BYTE(alpha*g);
			r = BYTE(alpha*r);
		}

		inline float Dist2(PICT_COLOR & c)
		{
			return (float(r) - c.r)*(r - c.r) + (float(g) - c.g)*(g - c.g) + (float(b) - c.b)*(b - c.b);
		}

		inline WORD Get565()
		{
			WORD rc = WORD(r*(31.0/255.0f));
			WORD gc = WORD(g*(63.0/255.0f));
			WORD bc = WORD(b*(31.0/255.0f));
			return (rc << 11) | (gc << 5) | bc;
		}

		inline WORD Get555()
		{
			WORD rc = WORD(r*(31.0/255.0f));
			WORD gc = WORD(g*(31.0/255.0f));
			WORD bc = WORD(b*(31.0/255.0f));
			return (rc << 10) | (gc << 5) | bc;
		}

		inline WORD Get4444()
		{
			WORD ac = WORD(a*(15.0/255.0f));
			WORD rc = WORD(r*(15.0/255.0f));
			WORD gc = WORD(g*(15.0/255.0f));
			WORD bc = WORD(b*(15.0/255.0f));
			return (ac << 12) | (rc << 8) | (gc << 4) | bc;
		}

		inline BYTE GetAlpha4()
		{
			return BYTE(a*(15.0f/255.0f));
		}

		inline void Set4444(WORD c4444)
		{
			a = BYTE(((c4444 >> 12) & 0xf)*(255.0f/15.0));
			r = BYTE(((c4444 >> 8) & 0xf)*(255.0f/15.0));
			g = BYTE(((c4444 >> 4) & 0xf)*(255.0f/15.0));
			b = BYTE(((c4444 >> 0) & 0xf)*(255.0f/15.0));
		}

		inline void Set1555(WORD c1555)
		{
			a = (c1555 & 0x8000) ? 0xff : 0x00;
			r = BYTE(((c1555 >> 10) & 0x1f)*(255.0f/31.0));
			g = BYTE(((c1555 >> 5) & 0x1f)*(255.0f/31.0));
			b = BYTE(((c1555 >> 0) & 0x1f)*(255.0f/31.0));
		}

		inline void Set565(WORD c565)
		{
			a = 0xff;
			r = BYTE(((c565 >> 11) & 0x1f)*(255.0f/31.0));
			g = BYTE(((c565 >> 5) & 0x3f)*(255.0f/63.0));
			b = BYTE(((c565 >> 0) & 0x1f)*(255.0f/31.0));
		}

		inline void Set888(DWORD c888)
		{
			a = 0xff;
			r = BYTE(c888 >> 16);
			g = BYTE(c888 >> 8);
			b = BYTE(c888 >> 0);
		}

		inline void Set8888(DWORD c8888)
		{
			a = BYTE(c8888 >> 24);
			r = BYTE(c8888 >> 16);
			g = BYTE(c8888 >> 8);
			b = BYTE(c8888 >> 0);
		}

		inline void Set(BYTE ac, BYTE rc, BYTE gc, BYTE bc)
		{
			a = ac; r = rc; g = gc; b = bc;
		}

		inline void Set(BYTE rc, BYTE gc, BYTE bc)
		{
			a = 0xff; r = rc; g = gc; b = bc;
		}
	};

	typedef unsigned int uint32;

	// Nested structure
	struct DDSPixelFormat
	{
		uint32 size;
		uint32 flags;
		uint32 fourCC;
		uint32 rgbBits;
		uint32 redMask;
		uint32 greenMask;
		uint32 blueMask;
		uint32 alphaMask;
	};

	// Nested structure
	struct DDSCaps
	{
		uint32 caps1;
		uint32 caps2;
		uint32 reserved[2];
	};

	struct DDSHeader
	{
		uint32 size;		
		uint32 flags;
		uint32 height;
		uint32 width;
		uint32 sizeOrPitch;
		uint32 depth;
		uint32 mipMapCount;
		uint32 reserved1[11];
		DDSPixelFormat pixelFormat;
		DDSCaps caps;
		uint32 reserved2;
	};
	static const uint32 DDS_MAGIC = FOURCC('D', 'D', 'S', ' ');
	static const uint32 DDS_PIXELFORMAT_SIZE = 8 * sizeof(uint32);
	static const uint32 DDS_CAPS_SIZE = 4 * sizeof(uint32);
	static const uint32 DDS_HEADER_SIZE = 19 * sizeof(uint32) + DDS_PIXELFORMAT_SIZE + DDS_CAPS_SIZE;

	static const uint32 DDSD_CAPS = 0x00000001;
	static const uint32 DDSD_HEIGHT = 0x00000002;
	static const uint32 DDSD_WIDTH = 0x00000004;
	static const uint32 DDSD_PITCH = 0x00000008;
	static const uint32 DDSD_PIXELFORMAT = 0x00001000;
	static const uint32 DDSD_MIPMAPCOUNT = 0x00020000;
	static const uint32 DDSD_LINEARSIZE = 0x00080000;
	static const uint32 DDSD_DEPTH = 0x00800000;
	static const uint32 DDPF_ALPHAPIXELS = 0x00000001;
	static const uint32 DDPF_FOURCC = 0x00000004;
	static const uint32 DDPF_RGB = 0x00000040;
	static const uint32 DDSCAPS_COMPLEX = 0x00000008;
	static const uint32 DDSCAPS_TEXTURE = 0x00001000;
	static const uint32 DDSCAPS_MIPMAP = 0x00400000;
	static const uint32 DDSCAPS2_CUBEMAP = 0x00000200;

	static const uint32 DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400;
	static const uint32 DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800;
	static const uint32 DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000;
	static const uint32 DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000;
	static const uint32 DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000;
	static const uint32 DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000;
	static const uint32 DDSCAPS2_VOLUME = 0x00200000;

	// Special FourCC codes
	static const uint32 D3DFMT_R16F			= 111;
	static const uint32 D3DFMT_G16R16F			= 112;
	static const uint32 D3DFMT_A16B16G16R16F	= 113;
	static const uint32 D3DFMT_R32F            = 114;
	static const uint32 D3DFMT_G32R32F         = 115;
	static const uint32 D3DFMT_A32B32G32R32F   = 116;
private:
	//根据文件头计算出解压出来的数据有多大(软解压，一律解成32位的)
	inline size_t calc_data_decode_size(const DDSHeader& header)
	{
		int face = 1;
		int mipcount = 1;
		if (header.caps.caps2 & DDSCAPS2_CUBEMAP)
		{
			face = 6;
		}

		if (header.caps.caps1 & DDSCAPS_MIPMAP)
		{
			mipcount = header.mipMapCount;
		}
		
		int size = 0;
		int w = header.width;
		int h = header.height;
		int mip = 1;
		if(header.mipMapCount > 0)
			mip = header.mipMapCount;
		for(int i = 0; i < mip; i++)
		{
			size += (w*h*sizeof(long));
			w>>=1;
			h>>=1;
			if(h < 1)
				h = 1;
			if(w < 1)
				w = 1;
		}
		size*=face;
		return size;
	}

	inline bool GetFormat(const DDSHeader& header, int& format)
	{
		#define PF_IS_BGRA8(pf) \
		  ((pf.flags & DDPF_RGB) && \
		   (pf.flags & DDPF_ALPHAPIXELS) && \
		   (pf.rgbBits == 32) && \
		   (pf.redMask == 0xff0000) && \
		   (pf.greenMask == 0xff00) && \
		   (pf.blueMask == 0xff) && \
		   (pf.alphaMask == 0xff000000U))

		#define PF_IS_BGR8(pf) \
		  ((pf.flags & DDPF_ALPHAPIXELS) && \
		  !(pf.flags & DDPF_ALPHAPIXELS) && \
		   (pf.rgbBits == 24) && \
		   (pf.redMask == 0xff0000) && \
		   (pf.greenMask == 0xff00) && \
		   (pf.blueMask == 0xff))

		#define PF_IS_BGR5A1(pf) \
		  ((pf.flags & DDPF_RGB) && \
		   (pf.flags & DDPF_ALPHAPIXELS) && \
		   (pf.rgbBits == 16) && \
		   (pf.redMask == 0x00007c00) && \
		   (pf.greenMask == 0x000003e0) && \
		   (pf.blueMask == 0x0000001f) && \
		   (pf.alphaMask == 0x00008000))


		#define PF_IS_BGR565(pf) \
		  ((pf.flags & DDPF_RGB) && \
		  !(pf.flags & DDPF_ALPHAPIXELS) && \
		   (pf.rgbBits == 16) && \
		   (pf.redMask == 0x0000f800) && \
		   (pf.greenMask == 0x000007e0) && \
		   (pf.blueMask == 0x0000001f))

		switch(header.pixelFormat.fourCC)
		{
			case FOURCC('D','X','T','1'):
				format =  DDS_DXT1;
				return true;
			case FOURCC('D','X','T','2'):
				format =  DDS_DXT2;
				return true;
			case FOURCC('D','X','T','3'):
				format =  DDS_DXT3;
				return true;
			case FOURCC('D','X','T','4'):
				format =  DDS_DXT4;
				return true;
			case FOURCC('D','X','T','5'):
				format =  DDS_DXT5;
				return true;
			default:
				{
					return false;

					if(PF_IS_BGRA8(header.pixelFormat))
						format =  DDS_BGRA8;
					else if(PF_IS_BGR8(header.pixelFormat))
						format =  DDS_BGR8;
					else if(PF_IS_BGR5A1(header.pixelFormat))
						format =  DDS_BGR5A1;
					else if(PF_IS_BGR565(header.pixelFormat))
						format =  DDS_BGR5A1;
					else if(PF_IS_BGR565(header.pixelFormat))
						format =  DDS_BGR565;
					else
						return false;
				}
				break;
		}
		return true;
	}

	inline int DecompressDXT1(const BYTE * buf, unsigned long * dst, int width, int height)
	{
		const BYTE* src = buf;
		for(DWORD by = 0; by < height; by += 4)
		{
			for(DWORD bx = 0; bx < width; bx += 4)
			{
				PICT_COLOR min, max;
				WORD cmin = ((WORD *)buf)[0];
				WORD cmax = ((WORD *)buf)[1];
				min.Set565(cmin);
				max.Set565(cmax);
				buf += 4;
				int x, y;

				DWORD color = *(DWORD *)buf;
				buf += 4;

				PICT_COLOR pal[4];
				if(cmin > cmax)
				{
					pal[0] = min;
					pal[1] = max;
					pal[2].Blend(1.0f/3.0f, min, max);
					pal[3].Blend(2.0f/3.0f, min, max);
				}else
				{
					pal[0] = min;
					pal[1] = max;
					pal[2].Blend(0.5f, min, max);
					pal[3] = 0;
				}
				
				int w = min(width, 4);
				int h = min(height, 4);
				for(y = 0; y < h; y++)
				{
					for(x = 0; x < w; x++, color >>= 2)
					{
						dst[(by + y)*width +bx + x] = pal[color & 3].c;
					}
				}	
			}
		}
		const BYTE* dstt = buf;
		int len = dstt - src;
		return len;
	}

	inline void DecompressDXT23(const BYTE * buf, unsigned long * dst, int width, int height, bool isPremul)
	{
		int x, y;

		for(DWORD by = 0; by < height; by += 4)
		for(DWORD bx = 0; bx < width; bx += 4)
		{
			BYTE ic[4][4];
			DWORD alpha = *(DWORD *)buf;
			buf += 4;
			for(y = 0; y < 2; y++)
				for(x = 0; x < 4; x++, alpha >>= 4)
									ic[x][y] = BYTE((alpha & 15)*(255.0f/15.0f));
			alpha = *(DWORD *)buf;
			buf += 4;
			for(; y < 4; y++)
				for(x = 0; x < 4; x++, alpha >>= 4)
									ic[x][y] = BYTE((alpha & 15)*(255.0f/15.0f));

			PICT_COLOR min, max;
			WORD cmin = ((WORD *)buf)[0];
			WORD cmax = ((WORD *)buf)[1];
			min.Set565(cmin);
			max.Set565(cmax);
			buf += 4;

			DWORD color = *(DWORD *)buf;
			buf += 4;

			PICT_COLOR pal[4];

			pal[0] = min;
			pal[1] = max;
			pal[2].Blend(1.0f/3.0f, min, max);
			pal[3].Blend(2.0f/3.0f, min, max);

			int w = min(width, 4);
			int h = min(height, 4);

			for(y = 0; y < h; y++)
			{
				for(x = 0; x < w; x++, color >>= 2)
				{
					PICT_COLOR clr;
					clr = pal[color & 3];
					clr.a = ic[x][y];
					if(isPremul & false)
					{						
						if(clr.a)
						{
							float a = clr.a/255.0f;
							clr.b = BYTE(clr.b/a);
							clr.g = BYTE(clr.g/a);
							clr.r = BYTE(clr.r/a);
						}
					}
					dst[(by + y)*width +bx + x] = clr.c;
					//dst->SetPixel(bx + x, by + y, clr);
				}
			}
		}
	}

	inline void DecompressDXT45(const BYTE * buf, unsigned long * dst, int width, int height, bool isPremul)
	{
		int x, y;
		for(DWORD by = 0; by < height; by += 4)
		{
			for(DWORD bx = 0; bx < width; bx += 4)
			{
				BYTE ap[8];
				ap[0] = buf[0];
				ap[1] = buf[1];
				buf += 2;
				if(ap[0] > ap[1])
				{
					ap[2] = (6*ap[0] + 1*ap[1] + 3)/7;
					ap[3] = (5*ap[0] + 2*ap[1] + 3)/7;
					ap[4] = (4*ap[0] + 3*ap[1] + 3)/7;
					ap[5] = (3*ap[0] + 4*ap[1] + 3)/7;
					ap[6] = (2*ap[0] + 5*ap[1] + 3)/7;
					ap[7] = (1*ap[0] + 6*ap[1] + 3)/7;
				}else{
					ap[2] = (4*ap[0] + 1*ap[1] + 2)/5;
					ap[3] = (3*ap[0] + 2*ap[1] + 2)/5;
					ap[4] = (2*ap[0] + 3*ap[1] + 2)/5;
					ap[5] = (1*ap[0] + 4*ap[1] + 2)/5;
					ap[6] = 0;
					ap[7] = 255;
				}

				BYTE ic[4][4];
				DWORD alpha = *(DWORD *)buf;
				BYTE bp = 0;
				for(y = 0; y < 4; y++)
					for(x = 0; x < 4; x++)
					{
						BYTE ba = 0;
						for(BYTE i = 0; i < 3; i++, bp++, ba >>= 1)
						{
							ba |= ((buf[bp >> 3] >> (bp & 7)) & 1) << 3;
						}
						ic[x][y] = ba;
					}
				buf += 6;

				PICT_COLOR min, max;
				WORD cmin = ((WORD *)buf)[0];
				WORD cmax = ((WORD *)buf)[1];
				min.Set565(cmin);
				max.Set565(cmax);
				buf += 4;

				DWORD color = *(DWORD *)buf;
				buf += 4;

				PICT_COLOR pal[4];

				pal[0] = min;
				pal[1] = max;
				pal[2].Blend(1.0f/3.0f, min, max);
				pal[3].Blend(2.0f/3.0f, min, max);

				int w = min(width, 4);
				int h = min(height, 4);

				for(y = 0; y < h; y++)
				{
					for(x = 0; x < w; x++, color >>= 2)
					{
						PICT_COLOR clr;
						clr = pal[color & 3];
						clr.a = ap[ic[x][y]];
						if(isPremul)
						{						
							if(clr.a)
							{
								float a = clr.a/255.0f;
								clr.b = BYTE(clr.b/a);
								clr.g = BYTE(clr.g/a);
								clr.r = BYTE(clr.r/a);
							}
						}
						int dy = by+y;
						dst[(by + y)*width +bx + x] = clr.c;
					}
				}
			}
		}
	}

public:
	inline bool Parse(ImageData* image, const char* memPtr, unsigned long memSize)
	{
		image->iscubmap = false;
		uint32 fileType;
		memcpy(&fileType, memPtr, sizeof(uint32));
		memPtr += sizeof(uint32);
		if (FOURCC('D', 'D', 'S', ' ') != fileType) 
		{
			//This is not a DDS file!
			return false;
		}

		const DDSHeader& header = *(DDSHeader*)memPtr;
		if (header.size != DDS_HEADER_SIZE)
		{
			//DDS header size mismatch!,
			return false;
		}

		if (header.pixelFormat.size != DDS_PIXELFORMAT_SIZE)
		{
			//"DDS header size mismatch!", "DDSCodec::decode");
			return false;
		}

		memPtr += sizeof(DDSHeader);

		if (header.caps.caps1 & DDSCAPS_MIPMAP)
		{
			image->num_mipmaps = header.mipMapCount;
		}
		else
		{
			image->num_mipmaps = 1;
		}

		int face = 1;
		if (header.caps.caps2 & DDSCAPS2_CUBEMAP)
		{
	       image->iscubmap = true;
		   face = 6;
		}

		if(!GetFormat(header, image->format))
			return false;
		int size = calc_data_decode_size(header);
		image->data = (char*)malloc(size);
		assert(image->data);
		char * dstdata = image->data;
		const DdsLoadInfo& li = GetDDSInfo((DDSFile::DDSFORMAT)image->format);
		int w = header.width;
		int h = header.height;
		image->width = w;
		image->height = h;
		if(li.compressed)
		{
			for(int f = 0; f < face; f++)
			{
				w = image->width;
				h = image->height;
				for(int m = 0; m < image->num_mipmaps; m++)
				{
					const BYTE* bf = (const BYTE*)memPtr;
					unsigned long* out = (unsigned long*)dstdata;
					switch(image->format)
					{
					case DDS_DXT1:
						{
							DecompressDXT1(bf, out, w, h);
						}break;
					case DDS_DXT2:
						{
							DecompressDXT23(bf, out, w, h, true);
						}break;
					case DDS_DXT3:
						{
							DecompressDXT23(bf, out, w, h, false);
						}break;	
					case DDS_DXT4:
						{
							DecompressDXT45(bf, out, w, h, true);
						}break;	
					case DDS_DXT5:
						{
							DecompressDXT45(bf, out, w, h, false);
						}break;	
					}
					dstdata += w*h*sizeof(long);
					int size = max( li.divSize, w )/li.divSize * max( li.divSize, h )/li.divSize * li.blockBytes;
					memPtr += size;
					w>>=1;
					h>>=1;
					if(w < 1)
						w = 1;
					if(h < 1)
						h = 1;
				}
			}
			return true;
		}
		return false;
	}

	inline void Load(ImageData* image, const char* filename)
	{
		
		char* buf;
		int len = __LoadFile(filename, buf);
		Parse(image, buf, len);
		free(buf);
	}
};
