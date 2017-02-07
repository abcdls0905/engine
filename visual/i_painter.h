#ifndef _I_PAINTER_H
#define _I_PAINTER_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/module.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../visual/fm_define.h"

class IFrameRT;
class IFont;
class IScene;
class IPainter: public IInterface
{
public:
	enum {
		PAINT_NORMAL = 0,
		PAINT_VERTICAL_FLIP = 1<<0, //���µߵ�
		PAINT_HORIZONTAL_FLIP = 1 << 1,  //ˮƽ�ߵ�
	};

	// ���µ�ǰ���ڵĳߴ�
	virtual bool UpdateWindowSize() = 0;

	// ��ʼ������Ⱦ
	virtual bool BeginPaint() = 0;
	// ����������Ⱦ
	virtual bool EndPaint() = 0;

	// ��õ�ǰ��ȾĿ��Ŀ��
	virtual int GetRenderWidth() = 0;
	// ��õ�ǰ��ȾĿ��ĸ߶�
	virtual int GetRenderHeight() = 0;

	// ����豸���
	virtual int GetDeviceWidth() = 0;
	// ����豸�߶�
	virtual int GetDeviceHeight() = 0;

	// ��ǰ��ʾ���ڿ��
	virtual int GetWinWidth() = 0;
	// ��ǰ��ʾ���ڸ߶�
	virtual int GetWinHeight() = 0;
	
	// ��ʾ�豸�ߴ��뵱ǰ���ڳߴ�ı���
	virtual bool GetNeedScaleX() = 0;
	virtual bool GetNeedScaleY() = 0;
	virtual float GetScaleFactorX() = 0;
	virtual float GetScaleFactorY() = 0;
	
	// ȱʡ��Դ·��
	virtual void SetDefaultPath(const char* value) = 0;
	virtual const char* GetDefaultPath() = 0;
	
	// ����������ڷ�Χ
	virtual bool SetViewport(int x1, int y1, int x2, int y2,
		bool is_in_parent = true) = 0;
	// �ָ�ǰ���������ڷ�Χ
	virtual bool ResetViewport() = 0;
	
	// ������ת����
	virtual void SetRotateTransform(const FmMat4* mat) = 0;

	// ��Ƕ�����ĳ���
	virtual bool DrawScene(IScene* pScene) = 0;
	
	// ������ģʽ������ͼƬ
	virtual bool DrawGlyph(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id) = 0;
	// ������ģʽ����ɫת�ڰ׼ӻ�ɫ�Ľ���ͼƬ
	virtual bool DrawBWGlyph(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		unsigned int blendcolor = 0xFFFFFFFF) = 0;
	// ������ģʽ����ɫת�ڰ׼ӻ�ɫ�Ľ���ͼƬ
	virtual bool DrawBWGlyphRound(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num, unsigned int blendcolor = 0xFFFFFFFF) = 0;
	// �����ӻ��Ч���Ľ���ͼƬblendmode 0��, 1ǿ, 2��
	virtual bool DrawAddGlyph(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int blendmode, unsigned int blendcolor = 0xFFFFFFFF) = 0;
	// ������ģʽ��Բ�ν���ͼƬ
	virtual bool DrawGlyphRound(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num) = 0;
	// ������ģʽ��Բ�ν���ͼƬ, ��ʾ���֣�ʵ�����ι���
	virtual bool DrawGlyphFan(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num, float begin_angle, float end_angle) = 0;
	// ���Զ���չģʽ������ͼƬ
	virtual bool DrawGlyphExpand(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id) = 0;
	// ���Զ�������չģʽ������ͼƬ
	virtual bool DrawGlyphExpandH(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id) = 0;
	// ���Զ�������չģʽ������ͼƬ
	virtual bool DrawGlyphExpandV(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id) = 0;
	// ������ģʽ������ͼƬ�б�
	virtual bool DrawGlyphList(float* x, float* y, float* orient, 
		float* scale, float* alpha, int count, const PERSISTID& glyph_id, 
		int blend_mode, unsigned int blend_color) = 0;
	// ������ģʽ������ν���ͼƬ�б�
	virtual bool DrawGlyphConvexPolygon(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id, 
		float* point, int* index, int count, int blend_mode, 
		unsigned int blend_color) = 0;
	// ������ν���߽���
	virtual bool DrawGlyphConvexPolygonBorder(int x1, int y1, int x2, int y2, 
		 const PERSISTID& glyph_id, float points[], float normal[], 
		 int point_num) = 0;

	// ���ı���
	virtual bool DrawRect(int x1, int y1, int x2, int y2, unsigned int color) = 0;
	// �������ɫ���ı���
	virtual bool FillRect(int x1, int y1, int x2, int y2, unsigned int color) = 0;
	// �������ɫ����Բ��
	virtual bool FillRound(int x1, int y1, int x2, int y2, 
		unsigned int color, int fan_num) = 0;
	// �������ɫ����Բ��
	virtual bool FillRoundPart(int x1, int y1, int x2, int y2, 
		unsigned int color, int show_fan_num, int total_fan_num) = 0;
	// ����
	virtual bool DrawLine(int x1, int y1, int x2, int y2, unsigned int color) = 0;
	// ��������
	virtual bool DrawLinkedLines(int points[], int point_num, 
		unsigned int color) = 0;
	// ��������
	virtual bool DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, 
		unsigned int color) = 0;
	// ������
	virtual bool DrawGrids(int x1, int y1, int x2, int y2, int grid_size, 
		unsigned int color) = 0;
	// �������
	virtual bool DrawPoints(void* pdata, int point_num) = 0;
	
	// �������
	virtual bool WriteText(int x, int y, unsigned int color, 
		const wchar_t* text) = 0;
	// ���ָ����������
	virtual bool WriteTextLen(int x, int y, unsigned int color, 
		const wchar_t* text, size_t len) = 0;
	// ��ָ����Χ�ھ����������
	virtual bool WriteTextCenter(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text) = 0;
	// ��ָ����Χ�ڿ����������
	virtual bool WriteTextLeft(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text) = 0;
	// ��ָ����Χ�ڿ����������
	virtual bool WriteTextRight(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text) = 0;
	// ������ָ߶�
	virtual int GetTextHeight(const wchar_t* text) = 0;
	// ������ֿ��
	virtual int GetTextWidth(const wchar_t* text) = 0;
	// ���ָ���������ֿ��
	virtual int GetTextLenWidth(const wchar_t* text, size_t len) = 0;
	// ��ȡ��һ�������ַ��Ŀ��
	virtual int GetNextLanguageCharacter(const wchar_t * text, const size_t begin) { return 1; }
	// ������ּ��
	virtual int GetTextInterval() = 0;
	// ���ָ������ڿ�����ʾ����������
	virtual int GetTextVisible(const wchar_t* text, int width) = 0;
	
	// ���ҽ���ͼƬ
	virtual PERSISTID FindGlyph(const char* name) = 0;
	// ��������ͼƬ
	virtual PERSISTID CreateGlyph(const char* name, bool async) = 0;
	// �½�����ͼƬ
	virtual PERSISTID CreateNewGlyph(const char* name, size_t width, 
		size_t height) = 0;
	// �ͷŽ���ͼƬ
	virtual bool ReleaseGlyph(const PERSISTID& id) = 0;
	// ��ý���ͼƬ���
	virtual int GetGlyphWidth(const PERSISTID& id) = 0;
	// ��ý���ͼƬ�߶�
	virtual int GetGlyphHeight(const PERSISTID& id) = 0;
	// ��ý���ͼƬ��ʽ
	virtual unsigned int GetGlyphFormat(const PERSISTID& id) = 0;
	// ����ͼƬ�Ƿ���Ч
	virtual bool GetGlyphValid(const PERSISTID& id) = 0;
	// ����ͼƬ�Ƿ����
	virtual bool GetGlyphReady(const PERSISTID& id) = 0;
	// ����ͼƬ�Ƿ������ɣ��ɹ���ʧ�ܣ�
	virtual bool GetGlyphLoadComplete(const PERSISTID& id) = 0;
	// ��ý���ͼƬָ��λ�õ�ALPHAֵ
	virtual bool GetGlyphAlpha(const PERSISTID& id, int x, int y, int& alpha) = 0;
	
	// ���ϵͳ����
	virtual bool AddSysFont(const char* name, const char* font_file, 
		int tex_size, int height, int interval, bool bold, bool smooth, 
		bool italic, bool underline, bool strike_out, bool var_extent, 
		bool outline, int outline_num, const FmVec3* outline_color, 
		int fadetype, 
		int shadow_color, int shadow_offset_x, int shadow_offset_y) = 0;
	// �����ͼ����
	virtual bool AddTexFont(const char* name, const char* font_tex,
		int tex_width, int tex_height, int height, int interval,
		const int* char_info, int char_num) = 0;
	// ��õ�ǰ����
	virtual const char* GetFont() = 0;
	virtual IFont* GetFontPtr() = 0;
	// ���õ�ǰ����
	virtual bool SetFont(const char* name) = 0;
	// �������ֲ�������
	virtual IFont* GetFont(const char* name) = 0;
	// ���õ�ǰ�������Ӱ/���ɫ�������/��Ӱ������Ч
	virtual bool SetFontShadowColor(const char* name, unsigned int color) = 0;
	// �������������ͼ
	virtual bool DumpAllFontTex() = 0;

	// �������ͳ����Ϣ
	virtual performance_info_t* GetPerformance() = 0;

	// �뱳���Ļ�϶�(0��255)
	virtual bool SetBlendAlpha(int value) = 0;
	virtual int GetBlendAlpha() = 0;

	// ��ǰ�����ֵ��0��1��
	virtual bool SetDepthValue(float depth_z, float depth_w) = 0;

	// �Ƿ�����������
	virtual void SetScaleEnable(bool value) = 0;
	virtual bool GetScaleEnable() = 0;

	// �������ű���
	virtual void SetScaleRatio(float value) = 0;
	virtual float GetScaleRatio() = 0;

	//���õ�ǰ��FrameRT
	virtual void SetFrameRT(IFrameRT* pFrameRT) = 0;

	virtual IFrameRT* GetFrameRT() = 0;

	// ��ȡGUI������
	virtual size_t GetGuiBatchSize() = 0;

	// ����Gui�Ŀ��Ի��Ƶ�������������������, ���ó�(0xffffffff)��ʾ�ر�
	virtual void SetDebugBatchNum(size_t num) = 0;

	//���û���ģʽ
	virtual void SetPaintMode(int mode) = 0;

	//ȡ�û���ģʽ
	virtual int GetPaintMode()const = 0;
};

#endif