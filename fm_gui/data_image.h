//--------------------------------------------------------------------
// 文件名:		DataImage.h
// 内  容:		数据图片控件
// 说  明:		
// 创建日期:	2011年9月14日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DataImage_h
#define _DataImage_h

#include "i_control.h"

// 通过数据绘制纹理
class IDxRender;
class IDxTexture;

class DataImage: public IControl
{
private:
	
public:
    DataImage();
    virtual ~DataImage();

    virtual bool Init(const IVarList& args);
    virtual bool Shut();

    // 绘制控件
    virtual void Paint(float seconds);
    // 设置设计器信息
    virtual void PrepareDesignInfo(IDesignInfo* di);
    virtual bool TestTransparent(int x, int y);

    // 添加点
    bool AddImageData(const char* global_name, int width, int height, const char* data);

    const int GetImageHeight();
    const int GetImageWidth();

    // 清理所有线段和点
    bool Clear();

    //// 透明
    //void SetTransparent(bool bTrans);
    //bool GetTransparent();

private:
    IDxRender* m_pRender;
    int m_nImageHeight;   
    int m_nImageWidth;
    char* m_pDataBuffer;

    //bool m_bTransparent;
    //IDxTexture* m_pTexture;
};

#endif //_DataImage_h

