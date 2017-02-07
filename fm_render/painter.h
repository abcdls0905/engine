//--------------------------------------------------------------------
// 文件名:		painter.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

// 纹理需加0.5个像素偏移,有些显卡需要这个
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TPainterAlloc& src) {}
};

// 平面渲染

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

	// 初始化
	virtual bool Init();
	// 关闭
	virtual bool Shut();
	
	// 更新当前窗口的尺寸
	bool UpdateWindowSize();

	// 开始界面渲染
	bool BeginPaint();
	// 结束界面渲染
	bool EndPaint();

	// 获得当前渲染目标的宽度
	int GetRenderWidth();
	// 获得当前渲染目标的高度
	int GetRenderHeight();

	// 获得设备宽度
	int GetDeviceWidth();
	// 获得设备高度
	int GetDeviceHeight();

	// 当前显示窗口宽度
	int GetWinWidth();
	// 当前显示窗口高度
	int GetWinHeight();
	
	// 显示设备尺寸与当前窗口尺寸的比例
	bool GetNeedScaleX();
	bool GetNeedScaleY();
	float GetScaleFactorX();
	float GetScaleFactorY();
	
	// 缺省资源路径
	void SetDefaultPath(const char* value);
	const char* GetDefaultPath();
	
	// 设置输出窗口范围
	bool SetViewport(int x1, int y1, int x2, int y2,
		bool is_in_parent = true);
	// 恢复前面的输出窗口范围
	bool ResetViewport();
	
	// 设置旋转矩阵
	void SetRotateTransform(const FmMat4* mat);

	// 画嵌入界面的场景
	bool DrawScene(IScene* pScene);
	
	// 用缩放模式画界面图片
	bool DrawGlyph(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id);
	// 用缩放模式画彩色转黑白加混色的界面图片
	bool DrawBWGlyph(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		unsigned int blendcolor = 0xFFFFFFFF);
	// 用缩放模式画彩色转黑白加混色的界面图片
	bool DrawBWGlyphRound(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num, unsigned int blendcolor = 0xFFFFFFFF);
	// 画叠加混合效果的界面图片blendmode 0中, 1强, 2弱
	bool DrawAddGlyph(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int blendmode, unsigned int blendcolor = 0xFFFFFFFF);
	// 用缩放模式画圆形界面图片
	bool DrawGlyphRound(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num);
	// 用缩放模式画圆形界面图片, 显示部分，实现扇形功能
	bool DrawGlyphFan(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num, float begin_angle, float end_angle);
	// 以自动扩展模式画界面图片
	bool DrawGlyphExpand(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id);
	// 以自动横向扩展模式画界面图片
	bool DrawGlyphExpandH(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id);
	// 以自动纵向扩展模式画界面图片
	bool DrawGlyphExpandV(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id);
	// 用缩放模式画界面图片列表
	bool DrawGlyphList(float* x, float* y, float* orient, 
		float* scale, float* alpha, int count, const PERSISTID& glyph_id, 
		int blend_mode, unsigned int blend_color);
	// 以缩放模式画多边形界面图片列表
	bool DrawGlyphConvexPolygon(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id, 
		float* point, int* index, int count, int blend_mode, 
		unsigned int blend_color);
	// 画多边形界面边界线
	bool DrawGlyphConvexPolygonBorder(int x1, int y1, int x2, int y2, 
		 const PERSISTID& glyph_id, float points[], float normal[], 
		 int point_num);

	// 画四边形
	bool DrawRect(int x1, int y1, int x2, int y2, unsigned int color);
	// 画填充颜色的四边形
	bool FillRect(int x1, int y1, int x2, int y2, unsigned int color);
	// 画填充颜色的椭圆形
	bool FillRound(int x1, int y1, int x2, int y2, 
		unsigned int color, int fan_num);
	// 画填充颜色的椭圆形
	bool FillRoundPart(int x1, int y1, int x2, int y2, 
		unsigned int color, int show_fan_num, int total_fan_num);
	// 画线
	bool DrawLine(int x1, int y1, int x2, int y2, unsigned int color);
	// 画连接线
	bool DrawLinkedLines(int points[], int point_num, 
		unsigned int color);
	// 画三角形
	bool DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, 
		unsigned int color);
	// 画网格
	bool DrawGrids(int x1, int y1, int x2, int y2, int grid_size, 
		unsigned int color);
	// 画多个点
	bool DrawPoints(void* pdata, int point_num);
	
	// 输出文字
	bool WriteText(int x, int y, unsigned int color, 
		const wchar_t* text);
	// 输出指定长度文字
	bool WriteTextLen(int x, int y, unsigned int color, 
		const wchar_t* text, size_t len);
	// 在指定范围内居中输出文字
	bool WriteTextCenter(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text);
	// 在指定范围内靠左输出文字
	bool WriteTextLeft(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text);
	// 在指定范围内靠右输出文字
	bool WriteTextRight(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text);
	// 获得文字高度
	int GetTextHeight(const wchar_t* text);
	// 获得文字宽度
	int GetTextWidth(const wchar_t* text);
	// 获得指定长度文字宽度
	int GetTextLenWidth(const wchar_t* text, size_t len);
	// 获取下一个语言字符的宽度
	int GetNextLanguageCharacter(const wchar_t * text, const size_t begin);
	// 获得文字间距
	int GetTextInterval();
	// 获得指定宽度内可以显示的文字数量
	int GetTextVisible(const wchar_t* text, int width);
	
	// 查找界面图片
	PERSISTID FindGlyph(const char* name);
	// 创建界面图片
	PERSISTID CreateGlyph(const char* name, bool async);
	// 新建界面图片
	PERSISTID CreateNewGlyph(const char* name, size_t width, 
		size_t height);
	// 释放界面图片
	bool ReleaseGlyph(const PERSISTID& id);
	// 获得界面图片宽度
	int GetGlyphWidth(const PERSISTID& id);
	// 获得界面图片高度
	int GetGlyphHeight(const PERSISTID& id);
	// 获得界面图片格式
	unsigned int GetGlyphFormat(const PERSISTID& id);
	// 界面图片是否有效
	bool GetGlyphValid(const PERSISTID& id);
	// 界面图片是否就绪
	bool GetGlyphReady(const PERSISTID& id);
	// 界面图片是否加载完成（成功或失败）
	bool GetGlyphLoadComplete(const PERSISTID& id);
	// 获得界面图片指定位置的ALPHA值
	bool GetGlyphAlpha(const PERSISTID& id, int x, int y, int& alpha);
	
	// 添加系统字体
	bool AddSysFont(const char* name, const char* font_file, 
		int tex_size, int height, int interval, bool bold, bool smooth, 
		bool italic, bool underline, bool strike_out, bool var_extent, 
		bool outline, int outline_num, const FmVec3* outline_color, 
		int fadetype, 
		int shadow_color, int shadow_offset_x, 
		int shadow_offset_y);
	// 添加贴图字体
	bool AddTexFont(const char* name, const char* font_tex,
		int tex_width, int tex_height, int height, int interval,
		const int* char_info, int char_num);
	// 获得当前字体
	const char* GetFont();
	IFont* GetFontPtr();
	// 设置当前字体
	bool SetFont(const char* name);
	// 根据名字查找字体
	IFont* GetFont(const char* name);
	// 设置当前字体的阴影/描边色，仅描边/阴影字体有效
	bool SetFontShadowColor(const char* name, unsigned int color);
	// 输出所有字体贴图
	bool DumpAllFontTex();

	// 获得性能统计信息
	performance_info_t* GetPerformance();

	// 与背景的混合度(0到255)
	bool SetBlendAlpha(int value);
	int GetBlendAlpha();

	// 当前的深度值（0到1）
	bool SetDepthValue(float depth_z, float depth_w);

	// 是否开启界面缩放
	void SetScaleEnable(bool value);
	bool GetScaleEnable();

	// 界面缩放比例
	void SetScaleRatio(float value);
	float GetScaleRatio();

	// 获取GUI批次数
	virtual size_t GetGuiBatchSize();

	// 设置Gui的可以绘制的批次数量，用来调试, 设置成(0xffffffff)表示关闭
	virtual void SetDebugBatchNum(size_t num);

	// 新建平面渲染批次
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
	// 经过缩放后的位置
	int ScaledValue(int value);
	
	// 画图片
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

	// 画指定长度文字
	bool DrawTextLen(int x, int y, unsigned int color, const wchar_t* text, 
		size_t len);

	//设置当前的FrameRT
	void SetFrameRT(IFrameRT* pFrameRT);

	IFrameRT* GetFrameRT();

	//设置绘制模式
	void SetPaintMode(int mode);

	//取得绘制模式
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
