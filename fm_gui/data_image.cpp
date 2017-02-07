//--------------------------------------------------------------------
// 文件名:		DataImage.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2011年9月14日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司f
//--------------------------------------------------------------------

#include "data_image.h"
//#include "../visual/idxrender.h"
//#include "../visual/idxtexture.h"
#include "../public/core_mem.h"

/// entity: DataImage
/// \brief 画线控件
DECLARE_ENTITY(DataImage, 0, IControl);

///// property: bool Transparent
///// \brief 是否透明不响应消息
//DECLARE_PROPERTY(bool, DataImage, Transparent, GetTransparent, 
//	SetTransparent);

#if 0	//DECLARE_METHOD
/// method: bool AddImageData(string name, int width, int height, const char* data)
/// \brief 添加图片数据
DECLARE_METHOD_4(bool,DataImage, AddImageData, const char*, int, int, const char*);

/// method: bool Clear()
/// \brief 清理所有线段或点
DECLARE_METHOD_0(bool, DataImage, Clear);
#endif

DataImage::DataImage()
{
    m_nImageWidth = 0;
    m_nImageHeight = 0;   
    m_pDataBuffer = NULL;
    //m_pTexture = NULL;
    //m_pRender = NULL;
    //m_bTransparent = true;
	//SetTransparentDefault(true);
}

DataImage::~DataImage()
{
    Clear();
}

// 添加点
bool DataImage::AddImageData(const char* global_name, int width, int height, const char* data)
{
    Clear();

    //if (m_pRender == NULL)
    //{
    //    return false;
    //}

    m_nImageWidth = width;
    m_nImageHeight = height;

    size_t data_len = width * height * sizeof(unsigned int);

    m_pDataBuffer = (char*)CORE_ALLOC(data_len);
    memcpy(m_pDataBuffer, data, data_len);

#if 0	//m_pTexture并没有使用
	m_pTexture = m_pRender->CreateNewTexture(global_name, width, height, "a8r8g8b8");

    if (m_pTexture == NULL)
    {
        Clear();
        return false;
    }


    D3DLOCKED_RECT lr;
    DxTexture* pTexture = m_pTexture->GetDxTexture();
	HRESULT hr = pTexture->LockRect(0, &lr, NULL, 0);

	if (FAILED(hr))
	{
		return false;
	}

	unsigned int* pDstRow = (unsigned int*)lr.pBits;
    unsigned int * pDst32 = pDstRow;

    // ARGB 4个字节
    int pitch_width = lr.Pitch / sizeof(unsigned int);
    unsigned int* pBits = (unsigned int*)m_pDataBuffer;

    for (int x = 0; x < height; ++x)
    {
        pDst32 = pDstRow;

        for (int y = 0; y < width; ++y)
        {
            pDst32[y] = pBits[width * x + y];
        }

        pDstRow += pitch_width;
    }

	pTexture->UnlockRect(0); 
#endif

    //
    char szImageName[256];

    SafeSprintf(szImageName, sizeof(szImageName), "%s,%d,%d,%d,%d", 
		global_name, 0, 0, width, height);
    SetBackImage(szImageName);

    return true;
}

const int DataImage::GetImageHeight()
{
    return m_nImageHeight;
}

const int DataImage::GetImageWidth()
{
    return m_nImageWidth;
}

// 清理所有线段和点
bool DataImage::Clear()
{
    //if (m_pTexture != NULL)
    //{
    //    SAFE_RELEASE(m_pTexture);
    //}

    if (m_pDataBuffer != NULL)
    {
        CORE_FREE(m_pDataBuffer, 
			sizeof(unsigned int) * m_nImageWidth * m_nImageHeight);
        m_pDataBuffer = NULL;
    }

    return true;
}

bool DataImage::Init(const IVarList& args)
{
    if (!IControl::Init(args))
	{
		return false;
	}

	//m_pRender = (IDxRender*)GetCore()->GetInterface("DxRender");

    unsigned int test[50 * 20];

    for (int i = 0; i < 50; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            if (j % 2 == 0)
            {
                test[j * 50 + i] = 0xff000000;
            }
            else
            {
                test[j * 50 + i] = 0xffffffff;
            }
        }
    }

    AddImageData("dataimage_form_login", 50, 20, (const char*)test);
    return true;
}

bool DataImage::Shut()
{
	//SAFE_RELEASE(m_pRender);

	return IControl::Shut();
}

// 绘制控件
void DataImage::Paint(float seconds)
{
    //if (m_pRender == NULL)
    //{
    //    return;
    //}

    RefreshInnerAbsPos();

	int x1 = InnerGetAbsLeft();
	int y1 = InnerGetAbsTop();
	int x2 = x1 + GetWidth();
	int y2 = y1 + GetHeight();

    if (HasBackImage())
	{
		DrawBackImage(seconds, x1, y1, x2, y2);
	}

    IControl::Paint(seconds);
}

//// 透明
//void DataImage::SetTransparent(bool bTrans)
//{
//    m_bTransparent = bTrans;
//}
//
//bool DataImage::GetTransparent()
//{
//    return m_bTransparent;
//}

bool DataImage::TestTransparent(int x, int y)
{
//     if (GetTransparent())
//     {
//         // 完全透明
//         return true;
//     }

    return IControl::TestTransparent(x, y);
}

// 设置设计器信息
void DataImage::PrepareDesignInfo(IDesignInfo* di)
{
    IControl::PrepareDesignInfo(di);

    di->AddPropInfo("LineStyle", IDesignInfo::TYPE_INT, true, true);
    di->AddPropInfo("UseTexture", IDesignInfo::TYPE_BOOL, true, true);
    di->AddPropInfo("TexWidth", IDesignInfo::TYPE_INT, true, true);
	di->AddPropInfo("LineWidth", IDesignInfo::TYPE_FLOAT, true, true);
    //di->AddPropInfo("Transparent", IDesignInfo::TYPE_BOOL, true, true, 
    //    &CVar(IDesignInfo::TYPE_BOOL, true));
}
