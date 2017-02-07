//--------------------------------------------------------------------
// �ļ���:		painter.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PAINTER_H
#define _PAINTER_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/module.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "paint_batch.h"
#include "../visual/i_painter.h"

// �������0.5������ƫ��,��Щ�Կ���Ҫ���
//#define SETTEXX(x) ((float(x) - 0.5F))
//#define SETTEXY(y) (-(float(y) - 0.5F))

#define SETTEXX(x) ((float(x) - 0.5f))
#define SETTEXY(y) (-(float(y) -0.5f))
#define SETX(x)	((x-0.5f))
#define SETY(y)	(-(y-0.5f))

class TPainterAlloc
{
public:
	TPainterAlloc() {}
	~TPainterAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TPainterAlloc& src) {}
};

// ƽ����Ⱦ

class IFont;
class IShaderTex;
class Render;
class CFontManager;
class CPaintBatchs;

class Painter: public IPainter
{
	int m_paintmode;
private:
	typedef TStringPod<char, IFont*, TStringTraits<char>,
		TPainterAlloc> font_container_t;

public:
	Painter();
	virtual ~Painter();

	// ��ʼ��
	virtual bool Init();
	// �ر�
	virtual bool Shut();
	
	// ���µ�ǰ���ڵĳߴ�
	bool UpdateWindowSize();

	// ��ʼ������Ⱦ
	bool BeginPaint();
	// ����������Ⱦ
	bool EndPaint();

	// ��õ�ǰ��ȾĿ��Ŀ��
	int GetRenderWidth();
	// ��õ�ǰ��ȾĿ��ĸ߶�
	int GetRenderHeight();

	// ����豸���
	int GetDeviceWidth();
	// ����豸�߶�
	int GetDeviceHeight();

	// ��ǰ��ʾ���ڿ��
	int GetWinWidth();
	// ��ǰ��ʾ���ڸ߶�
	int GetWinHeight();
	
	// ��ʾ�豸�ߴ��뵱ǰ���ڳߴ�ı���
	bool GetNeedScaleX();
	bool GetNeedScaleY();
	float GetScaleFactorX();
	float GetScaleFactorY();
	
	// ȱʡ��Դ·��
	void SetDefaultPath(const char* value);
	const char* GetDefaultPath();
	
	// ����������ڷ�Χ
	bool SetViewport(int x1, int y1, int x2, int y2,
		bool is_in_parent = true);
	// �ָ�ǰ���������ڷ�Χ
	bool ResetViewport();
	
	// ������ת����
	void SetRotateTransform(const FmMat4* mat);

	// ��Ƕ�����ĳ���
	bool DrawScene(IScene* pScene);
	
	// ������ģʽ������ͼƬ
	bool DrawGlyph(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id);
	// ������ģʽ����ɫת�ڰ׼ӻ�ɫ�Ľ���ͼƬ
	bool DrawBWGlyph(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		unsigned int blendcolor = 0xFFFFFFFF);
	// ������ģʽ����ɫת�ڰ׼ӻ�ɫ�Ľ���ͼƬ
	bool DrawBWGlyphRound(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num, unsigned int blendcolor = 0xFFFFFFFF);
	// �����ӻ��Ч���Ľ���ͼƬblendmode 0��, 1ǿ, 2��
	bool DrawAddGlyph(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int blendmode, unsigned int blendcolor = 0xFFFFFFFF);
	// ������ģʽ��Բ�ν���ͼƬ
	bool DrawGlyphRound(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num);
	// ������ģʽ��Բ�ν���ͼƬ, ��ʾ���֣�ʵ�����ι���
	bool DrawGlyphFan(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num, float begin_angle, float end_angle);
	// ���Զ���չģʽ������ͼƬ
	bool DrawGlyphExpand(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id);
	// ���Զ�������չģʽ������ͼƬ
	bool DrawGlyphExpandH(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id);
	// ���Զ�������չģʽ������ͼƬ
	bool DrawGlyphExpandV(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id);
	// ������ģʽ������ͼƬ�б�
	bool DrawGlyphList(float* x, float* y, float* orient, 
		float* scale, float* alpha, int count, const PERSISTID& glyph_id, 
		int blend_mode, unsigned int blend_color);
	// ������ģʽ������ν���ͼƬ�б�
	bool DrawGlyphConvexPolygon(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id, 
		float* point, int* index, int count, int blend_mode, 
		unsigned int blend_color);
	// ������ν���߽���
	bool DrawGlyphConvexPolygonBorder(int x1, int y1, int x2, int y2, 
		 const PERSISTID& glyph_id, float points[], float normal[], 
		 int point_num);

	// ���ı���
	bool DrawRect(int x1, int y1, int x2, int y2, unsigned int color);
	// �������ɫ���ı���
	bool FillRect(int x1, int y1, int x2, int y2, unsigned int color);
	// �������ɫ����Բ��
	bool FillRound(int x1, int y1, int x2, int y2, 
		unsigned int color, int fan_num);
	// �������ɫ����Բ��
	bool FillRoundPart(int x1, int y1, int x2, int y2, 
		unsigned int color, int show_fan_num, int total_fan_num);
	// ����
	bool DrawLine(int x1, int y1, int x2, int y2, unsigned int color);
	// ��������
	bool DrawLinkedLines(int points[], int point_num, 
		unsigned int color);
	// ��������
	bool DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, 
		unsigned int color);
	// ������
	bool DrawGrids(int x1, int y1, int x2, int y2, int grid_size, 
		unsigned int color);
	// �������
	bool DrawPoints(void* pdata, int point_num);
	
	// �������
	bool WriteText(int x, int y, unsigned int color, 
		const wchar_t* text);
	// ���ָ����������
	bool WriteTextLen(int x, int y, unsigned int color, 
		const wchar_t* text, size_t len);
	// ��ָ����Χ�ھ����������
	bool WriteTextCenter(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text);
	// ��ָ����Χ�ڿ����������
	bool WriteTextLeft(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text);
	// ��ָ����Χ�ڿ����������
	bool WriteTextRight(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text);
	// ������ָ߶�
	int GetTextHeight(const wchar_t* text);
	// ������ֿ��
	int GetTextWidth(const wchar_t* text);
	// ���ָ���������ֿ��
	int GetTextLenWidth(const wchar_t* text, size_t len);
	// ��ȡ��һ�������ַ��Ŀ��
	int GetNextLanguageCharacter(const wchar_t * text, const size_t begin);
	// ������ּ��
	int GetTextInterval();
	// ���ָ������ڿ�����ʾ����������
	int GetTextVisible(const wchar_t* text, int width);
	
	// ���ҽ���ͼƬ
	PERSISTID FindGlyph(const char* name);
	// ��������ͼƬ
	PERSISTID CreateGlyph(const char* name, bool async);
	// �½�����ͼƬ
	PERSISTID CreateNewGlyph(const char* name, size_t width, 
		size_t height);
	// �ͷŽ���ͼƬ
	bool ReleaseGlyph(const PERSISTID& id);
	// ��ý���ͼƬ���
	int GetGlyphWidth(const PERSISTID& id);
	// ��ý���ͼƬ�߶�
	int GetGlyphHeight(const PERSISTID& id);
	// ��ý���ͼƬ��ʽ
	unsigned int GetGlyphFormat(const PERSISTID& id);
	// ����ͼƬ�Ƿ���Ч
	bool GetGlyphValid(const PERSISTID& id);
	// ����ͼƬ�Ƿ����
	bool GetGlyphReady(const PERSISTID& id);
	// ����ͼƬ�Ƿ������ɣ��ɹ���ʧ�ܣ�
	bool GetGlyphLoadComplete(const PERSISTID& id);
	// ��ý���ͼƬָ��λ�õ�ALPHAֵ
	bool GetGlyphAlpha(const PERSISTID& id, int x, int y, int& alpha);
	
	// ���ϵͳ����
	bool AddSysFont(const char* name, const char* font_file, 
		int tex_size, int height, int interval, bool bold, bool smooth, 
		bool italic, bool underline, bool strike_out, bool var_extent, 
		bool outline, int outline_num, const FmVec3* outline_color, 
		int fadetype, 
		int shadow_color, int shadow_offset_x, 
		int shadow_offset_y);
	// �����ͼ����
	bool AddTexFont(const char* name, const char* font_tex,
		int tex_width, int tex_height, int height, int interval,
		const int* char_info, int char_num);
	// ��õ�ǰ����
	const char* GetFont();
	IFont* GetFontPtr();
	// ���õ�ǰ����
	bool SetFont(const char* name);
	// �������ֲ�������
	IFont* GetFont(const char* name);
	// ���õ�ǰ�������Ӱ/���ɫ�������/��Ӱ������Ч
	bool SetFontShadowColor(const char* name, unsigned int color);
	// �������������ͼ
	bool DumpAllFontTex();

	// �������ͳ����Ϣ
	performance_info_t* GetPerformance();

	// �뱳���Ļ�϶�(0��255)
	bool SetBlendAlpha(int value);
	int GetBlendAlpha();

	// ��ǰ�����ֵ��0��1��
	bool SetDepthValue(float depth_z, float depth_w);

	// �Ƿ�����������
	void SetScaleEnable(bool value);
	bool GetScaleEnable();

	// �������ű���
	void SetScaleRatio(float value);
	float GetScaleRatio();

	// ��ȡGUI������
	virtual size_t GetGuiBatchSize();

	// ����Gui�Ŀ��Ի��Ƶ�������������������, ���ó�(0xffffffff)��ʾ�ر�
	virtual void SetDebugBatchNum(size_t num);

	// �½�ƽ����Ⱦ����
	CPaintBatchs::vertex_pict_t* NewPictBatch(IRenderDrawOp::DRAW_MODE primitive_type, 
		int primitive_count, IShaderTex* pTexture);
	CPaintBatchs::vertex_pict_t* NewPictA8Batch(IRenderDrawOp::DRAW_MODE primitive_type, 
		int primitive_count, IShaderTex* pTexture);
	CPaintBatchs::vertex_pict_t* NewPictOutlineBatch(IRenderDrawOp::DRAW_MODE primitive_type, 
		int primitive_count, IShaderTex* pTexture, const FmVec3* pOutColor, bool use_big_texutre, bool fadeing = false);
	CPaintBatchs::vertex_pict_t* NewBWPictBatch(IRenderDrawOp::DRAW_MODE primitive_type, 
		int primitive_count, IShaderTex* pTexture);
	CPaintBatchs::vertex_pict_t* NewAddPictBatch(IRenderDrawOp::DRAW_MODE primitive_type, 
		int primitive_count, IShaderTex* pTexture, int blend_mode);
	CPaintBatchs::vertex_line_t* NewLineBatch(IRenderDrawOp::DRAW_MODE primitive_type,
		int primitive_count, unsigned int alpha);
	CPaintBatchs::vertex_point_t* NewPointBatch(int primitive_count, 
		unsigned int alpha);

private:
	// �������ź��λ��
	int ScaledValue(int value);
	
	// ��ͼƬ
	void DrawVertexPict(CPaintBatchs::vertex_pict_t*& pv, int x1, int y1, 
		int x2, int y2, float u1, float v1, float u2, float v2);
	bool DrawPict(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, 
		int tex_height, int orign_width, int orign_height);
	bool DrawBWPict(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, 
		int tex_height, unsigned int blendcolor);
	bool DrawBWPictRound(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, 
		int tex_height, int fan_num, unsigned int blendcolor);
	bool DrawAddPict(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, 
		int tex_height, int blend_mode, unsigned int blendcolor);
	bool DrawPictRound(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, 
		int tex_height, int fan_num);
	bool DrawColorRound(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, 
		int tex_height, int total_fan_num, int show_fan_num, 
		unsigned int color);

	bool DrawPictExpandTile(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, 
		int tex_height, int orign_width, int orign_height);

	bool DrawPictExpand(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, 
		int tex_height, int orign_width, int orign_height);
	bool DrawPictExpandH(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height,
		int orign_width, int orign_height);
	bool DrawPictExpandV(int x1, int y1, int x2, int y2, int left, int top, 
		int right, int bottom, IShaderTex* pTex, int tex_width, int tex_height,
		int orign_width, int orign_height);

	// ��ָ����������
	bool DrawTextLen(int x, int y, unsigned int color, const wchar_t* text, 
		size_t len);

	//���õ�ǰ��FrameRT
	void SetFrameRT(IFrameRT* pFrameRT);

	IFrameRT* GetFrameRT();

	//���û���ģʽ
	void SetPaintMode(int mode);

	//ȡ�û���ģʽ
	int GetPaintMode()const;

private:
	Render* m_pRender;
	CPaintBatchs* m_pPaintBatchs;
	core_string m_strDefaultPath;
	font_container_t m_Fonts;
	IFont* m_pCurFont;
	IFont* m_pDefFont;
	FmMat4* m_pRotateMatrix;
	FmMat4 m_mtxRotate;
	int m_nBlendAlpha;
	float m_fDepthZ;
	float m_fDepthW;
	bool m_bScaleEnable;
	float m_fScaleRatio;
	int m_nRenderWidth;
	int m_nRenderHeight;
	PERSISTID m_CursorTexId;
	int m_nCursorHotSpotX;
	int m_nCursorHotSpotY;
    CFontManager* m_pFontManager;
};

#endif // _PAINTER_H
