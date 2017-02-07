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
		PAINT_VERTICAL_FLIP = 1<<0, //上下颠倒
		PAINT_HORIZONTAL_FLIP = 1 << 1,  //水平颠倒
	};

	// 更新当前窗口的尺寸
	virtual bool UpdateWindowSize() = 0;

	// 开始界面渲染
	virtual bool BeginPaint() = 0;
	// 结束界面渲染
	virtual bool EndPaint() = 0;

	// 获得当前渲染目标的宽度
	virtual int GetRenderWidth() = 0;
	// 获得当前渲染目标的高度
	virtual int GetRenderHeight() = 0;

	// 获得设备宽度
	virtual int GetDeviceWidth() = 0;
	// 获得设备高度
	virtual int GetDeviceHeight() = 0;

	// 当前显示窗口宽度
	virtual int GetWinWidth() = 0;
	// 当前显示窗口高度
	virtual int GetWinHeight() = 0;
	
	// 显示设备尺寸与当前窗口尺寸的比例
	virtual bool GetNeedScaleX() = 0;
	virtual bool GetNeedScaleY() = 0;
	virtual float GetScaleFactorX() = 0;
	virtual float GetScaleFactorY() = 0;
	
	// 缺省资源路径
	virtual void SetDefaultPath(const char* value) = 0;
	virtual const char* GetDefaultPath() = 0;
	
	// 设置输出窗口范围
	virtual bool SetViewport(int x1, int y1, int x2, int y2,
		bool is_in_parent = true) = 0;
	// 恢复前面的输出窗口范围
	virtual bool ResetViewport() = 0;
	
	// 设置旋转矩阵
	virtual void SetRotateTransform(const FmMat4* mat) = 0;

	// 画嵌入界面的场景
	virtual bool DrawScene(IScene* pScene) = 0;
	
	// 用缩放模式画界面图片
	virtual bool DrawGlyph(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id) = 0;
	// 用缩放模式画彩色转黑白加混色的界面图片
	virtual bool DrawBWGlyph(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		unsigned int blendcolor = 0xFFFFFFFF) = 0;
	// 用缩放模式画彩色转黑白加混色的界面图片
	virtual bool DrawBWGlyphRound(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num, unsigned int blendcolor = 0xFFFFFFFF) = 0;
	// 画叠加混合效果的界面图片blendmode 0中, 1强, 2弱
	virtual bool DrawAddGlyph(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int blendmode, unsigned int blendcolor = 0xFFFFFFFF) = 0;
	// 用缩放模式画圆形界面图片
	virtual bool DrawGlyphRound(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num) = 0;
	// 用缩放模式画圆形界面图片, 显示部分，实现扇形功能
	virtual bool DrawGlyphFan(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id, 
		int fan_num, float begin_angle, float end_angle) = 0;
	// 以自动扩展模式画界面图片
	virtual bool DrawGlyphExpand(int x1, int y1, int x2, int y2, int left, 
		int top, int right, int bottom, const PERSISTID& glyph_id) = 0;
	// 以自动横向扩展模式画界面图片
	virtual bool DrawGlyphExpandH(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id) = 0;
	// 以自动纵向扩展模式画界面图片
	virtual bool DrawGlyphExpandV(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id) = 0;
	// 用缩放模式画界面图片列表
	virtual bool DrawGlyphList(float* x, float* y, float* orient, 
		float* scale, float* alpha, int count, const PERSISTID& glyph_id, 
		int blend_mode, unsigned int blend_color) = 0;
	// 以缩放模式画多边形界面图片列表
	virtual bool DrawGlyphConvexPolygon(int x1, int y1, int x2, int y2, 
		int left, int top, int right, int bottom, const PERSISTID& glyph_id, 
		float* point, int* index, int count, int blend_mode, 
		unsigned int blend_color) = 0;
	// 画多边形界面边界线
	virtual bool DrawGlyphConvexPolygonBorder(int x1, int y1, int x2, int y2, 
		 const PERSISTID& glyph_id, float points[], float normal[], 
		 int point_num) = 0;

	// 画四边形
	virtual bool DrawRect(int x1, int y1, int x2, int y2, unsigned int color) = 0;
	// 画填充颜色的四边形
	virtual bool FillRect(int x1, int y1, int x2, int y2, unsigned int color) = 0;
	// 画填充颜色的椭圆形
	virtual bool FillRound(int x1, int y1, int x2, int y2, 
		unsigned int color, int fan_num) = 0;
	// 画填充颜色的椭圆形
	virtual bool FillRoundPart(int x1, int y1, int x2, int y2, 
		unsigned int color, int show_fan_num, int total_fan_num) = 0;
	// 画线
	virtual bool DrawLine(int x1, int y1, int x2, int y2, unsigned int color) = 0;
	// 画连接线
	virtual bool DrawLinkedLines(int points[], int point_num, 
		unsigned int color) = 0;
	// 画三角形
	virtual bool DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, 
		unsigned int color) = 0;
	// 画网格
	virtual bool DrawGrids(int x1, int y1, int x2, int y2, int grid_size, 
		unsigned int color) = 0;
	// 画多个点
	virtual bool DrawPoints(void* pdata, int point_num) = 0;
	
	// 输出文字
	virtual bool WriteText(int x, int y, unsigned int color, 
		const wchar_t* text) = 0;
	// 输出指定长度文字
	virtual bool WriteTextLen(int x, int y, unsigned int color, 
		const wchar_t* text, size_t len) = 0;
	// 在指定范围内居中输出文字
	virtual bool WriteTextCenter(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text) = 0;
	// 在指定范围内靠左输出文字
	virtual bool WriteTextLeft(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text) = 0;
	// 在指定范围内靠右输出文字
	virtual bool WriteTextRight(int x1, int y1, int x2, int y2,
		unsigned int color, const wchar_t* text) = 0;
	// 获得文字高度
	virtual int GetTextHeight(const wchar_t* text) = 0;
	// 获得文字宽度
	virtual int GetTextWidth(const wchar_t* text) = 0;
	// 获得指定长度文字宽度
	virtual int GetTextLenWidth(const wchar_t* text, size_t len) = 0;
	// 获取下一个语言字符的宽度
	virtual int GetNextLanguageCharacter(const wchar_t * text, const size_t begin) { return 1; }
	// 获得文字间距
	virtual int GetTextInterval() = 0;
	// 获得指定宽度内可以显示的文字数量
	virtual int GetTextVisible(const wchar_t* text, int width) = 0;
	
	// 查找界面图片
	virtual PERSISTID FindGlyph(const char* name) = 0;
	// 创建界面图片
	virtual PERSISTID CreateGlyph(const char* name, bool async) = 0;
	// 新建界面图片
	virtual PERSISTID CreateNewGlyph(const char* name, size_t width, 
		size_t height) = 0;
	// 释放界面图片
	virtual bool ReleaseGlyph(const PERSISTID& id) = 0;
	// 获得界面图片宽度
	virtual int GetGlyphWidth(const PERSISTID& id) = 0;
	// 获得界面图片高度
	virtual int GetGlyphHeight(const PERSISTID& id) = 0;
	// 获得界面图片格式
	virtual unsigned int GetGlyphFormat(const PERSISTID& id) = 0;
	// 界面图片是否有效
	virtual bool GetGlyphValid(const PERSISTID& id) = 0;
	// 界面图片是否就绪
	virtual bool GetGlyphReady(const PERSISTID& id) = 0;
	// 界面图片是否加载完成（成功或失败）
	virtual bool GetGlyphLoadComplete(const PERSISTID& id) = 0;
	// 获得界面图片指定位置的ALPHA值
	virtual bool GetGlyphAlpha(const PERSISTID& id, int x, int y, int& alpha) = 0;
	
	// 添加系统字体
	virtual bool AddSysFont(const char* name, const char* font_file, 
		int tex_size, int height, int interval, bool bold, bool smooth, 
		bool italic, bool underline, bool strike_out, bool var_extent, 
		bool outline, int outline_num, const FmVec3* outline_color, 
		int fadetype, 
		int shadow_color, int shadow_offset_x, int shadow_offset_y) = 0;
	// 添加贴图字体
	virtual bool AddTexFont(const char* name, const char* font_tex,
		int tex_width, int tex_height, int height, int interval,
		const int* char_info, int char_num) = 0;
	// 获得当前字体
	virtual const char* GetFont() = 0;
	virtual IFont* GetFontPtr() = 0;
	// 设置当前字体
	virtual bool SetFont(const char* name) = 0;
	// 根据名字查找字体
	virtual IFont* GetFont(const char* name) = 0;
	// 设置当前字体的阴影/描边色，仅描边/阴影字体有效
	virtual bool SetFontShadowColor(const char* name, unsigned int color) = 0;
	// 输出所有字体贴图
	virtual bool DumpAllFontTex() = 0;

	// 获得性能统计信息
	virtual performance_info_t* GetPerformance() = 0;

	// 与背景的混合度(0到255)
	virtual bool SetBlendAlpha(int value) = 0;
	virtual int GetBlendAlpha() = 0;

	// 当前的深度值（0到1）
	virtual bool SetDepthValue(float depth_z, float depth_w) = 0;

	// 是否开启界面缩放
	virtual void SetScaleEnable(bool value) = 0;
	virtual bool GetScaleEnable() = 0;

	// 界面缩放比例
	virtual void SetScaleRatio(float value) = 0;
	virtual float GetScaleRatio() = 0;

	//设置当前的FrameRT
	virtual void SetFrameRT(IFrameRT* pFrameRT) = 0;

	virtual IFrameRT* GetFrameRT() = 0;

	// 获取GUI批次数
	virtual size_t GetGuiBatchSize() = 0;

	// 设置Gui的可以绘制的批次数量，用来调试, 设置成(0xffffffff)表示关闭
	virtual void SetDebugBatchNum(size_t num) = 0;

	//设置绘制模式
	virtual void SetPaintMode(int mode) = 0;

	//取得绘制模式
	virtual int GetPaintMode()const = 0;
};

#endif