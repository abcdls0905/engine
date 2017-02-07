//--------------------------------------------------------------------
// 文件名:        Sprite.h
// 内  容:        
// 说  明:        
// 创建日期:    2010年3月14日
// 创建人:        lixj
// 版权所有:    苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SPRITE_H
#define _SPRITE_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "i_control.h"
//#include <vector>
#include <string>
//#include <map>

class SpriteManager;

// 文字标签
struct SingleType
{
    //使用的字体图片
    std::string StrFontImage;                
    //使用的轨迹
    std::string StrTrackMode;                
    //背景图片的宽高
    unsigned int iBackImageW;
    unsigned int iBackImageH;
    //字体与背景之间的偏移值
    int FontOffsetX;
    int FontOffsetY;
    //字体与字体之间的偏移值结算的缩放的比例
    int OffsetX;
    //背景图片
    std::string StrBackImage;
    //是从玩家身上还是从屏幕上出来
    bool bIsScreenPos;
    bool singleton;

    SingleType()
    {
        StrFontImage = "";
        StrTrackMode = "";
        iBackImageW = 0;
        iBackImageH = 0;
        FontOffsetX = 0;
        FontOffsetY = 0;
        OffsetX = 0;
        StrBackImage = "";
        bIsScreenPos = false;
    }
};

//运动轨迹信息
struct Tracklogic
{
    const float GetPostionX(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;
    const float GetPostionY(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;
    const float GetScaleW(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;
    const float GetScaleH(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;
    const unsigned int GetBlendColor(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;

    float m_fStaticTime; //图片静止时间
    float m_fMoveTime; //图移动的时间
    float m_fBackX; //开始产生时X轴方向上的位置
    float m_fBackY; //开始产生时X轴方向上的位置
    int m_fBackW; //开始产生时的高
    int m_fBackH; //开始产生时的宽
    unsigned int m_dStartColor; //开始时候的颜色
    unsigned int m_dEndColor; //开始时候的颜色
    int m_fMoveSpeedX; //图片在X方向上的移动速度
    int m_fMoveSpeedY; //图片在Y方向上的移动速度
    int m_fMoveSpeedAX; //图片在X方向上的加速度
    int m_fMoveSpeedAY; //图片在Y方向上的加速度
    int m_fStaticScaleSpeedW; //图片宽的缩放速度
    int m_fStaticScaleSpeedH; //图片高的缩放速度
    int m_fMoveScaleSpeedW; //图片宽的缩放速度
    int m_fMoveScaleSpeedH; //图片高的缩放速度
    float m_fOffsetY; //图片距离脚底的距离
    float m_fColorTime;
    bool m_bIsStop;            
    float m_fROffsetY; //图片距离脚底的距离
    float m_fROffsetX;
    std::string m_StrFont; //使用的字体名字

    Tracklogic()
    {
        m_fStaticTime = 0.0f;
        m_fMoveTime = 0.0f;
        m_fBackX = 0.0f;
        m_fBackY = 0.0f;
        m_fBackW = 0;
        m_fBackH = 0;
        m_dStartColor = 0x00000000;
        m_dEndColor = 0x00000000;
        m_fMoveSpeedX = 0;
        m_fMoveSpeedY = 0;
        m_fMoveSpeedAX = 0;
        m_fMoveSpeedAY = 0;
        m_fStaticScaleSpeedW = 0;
        m_fStaticScaleSpeedH = 0;
        m_fMoveScaleSpeedW = 0;
        m_fMoveScaleSpeedH = 0;
        m_fOffsetY = 0.0f;
        m_fColorTime = 0.0f;
        m_fROffsetY = 0;
        m_fROffsetX = 0;
    }
};

//typedef std::map<char,std::string> IMAGEINFO;

class CImageInfo
{
private:
	struct name_t
	{
		char nChar;
		char* pName;
		size_t nSize;
	};

public:
	CImageInfo()
	{
	}

	~CImageInfo()
	{
		Clear();
	}

	size_t GetCount() const
	{
		return m_Names.size();
	}

	const char* GetByIndex(size_t index) const
	{
		Assert(index < m_Names.size());

		return m_Names[index].pName;
	}

	bool AddName(char ch, const char* name)
	{
		Assert(name != NULL);

		size_t name_size = strlen(name) + 1;
		char* pName = (char*)CORE_ALLOC(name_size);
		name_t data;

		memcpy(pName, name, name_size);
		data.pName = pName;
		data.nSize = name_size;
		data.nChar = ch;

		m_Names.push_back(data);

		return true;
	}

	const char* FindName(char ch) const
	{
		for (size_t i = 0; i < m_Names.size(); ++i)
		{
			if (m_Names[i].nChar == ch)
			{
				return m_Names[i].pName;
			}
		}

		return NULL;
	}

	void Clear()
	{
		for (size_t i = 0; i < m_Names.size(); ++i)
		{
			CORE_FREE(m_Names[i].pName, m_Names[i].nSize);
		}

		m_Names.clear();
	}

private:
	CImageInfo(const CImageInfo&);
	CImageInfo& operator=(const CImageInfo&);

private:	
	TArrayPod<name_t, 1, TCoreAlloc> m_Names;
};

class Sprite: public IControl
{
public:
    Sprite();
    virtual ~Sprite();

public:
    virtual bool Init(const IVarList& args);
    virtual bool Shut();

    virtual void PrepareDesignInfo(IDesignInfo* di);
    virtual void Paint(float seconds);

    virtual void SetImageText(const wchar_t* value);
    virtual const wchar_t* GetImageText() const;
    virtual void Execute(float seconds);

    virtual IControl* OnGetInWhat(int x, int y);

    // 是否透明
    void SetTransparent(bool value);
    bool GetTransparent() const;

    //清空所有的池
    void ClearAllPool();

    //bool InitParam(SingleType * pParam1,Tracklogic * pParam2,IMAGEINFO * pParam3,
      //  const char *pImageText,const char *pText,long posx,long posy);
	bool InitParam(SingleType* pParam1, Tracklogic* pParam2, 
		CImageInfo* pParam3, const char* pImageText, const char* pText,
		long posx, long posy);

    // 设置附加信息
    bool SetParentInfo(SpriteManager* pMng, int sprite_index);

    void SetDTime(float dTime);

    //决定是否绘制 true 绘制 false 不绘制
    bool GetPaint();
    void SetPaint(bool );

    void RefreshSprite(float );
    bool CreateFontImage();

    bool SetSpriteLeft(int val);
    bool SetSpriteTop(int val);

protected:
    // 判断指定位置是否透明
    bool TestTransparent(int x, int y);

    CImagePlayer * GetImagePlayer(unsigned int index);

private:
    bool m_bTransparent;
    // 只读属性,从外部传入
    SpriteManager* m_pSpriteManager;
    int m_sprite_index;
    const Tracklogic* m_pTrackMode;
    const SingleType* m_pSingleType;
    //const IMAGEINFO*                        m_pImageInfo;
	const CImageInfo* m_pImageInfo;
    TArrayPod<CImagePlayer*, 1, TCoreAlloc> m_ImageSingle;
    CImagePlayer m_BackImageEx;
    std::wstring m_ImageText;
    float m_dTime;
    bool m_bIsPaint;                            //true 死亡
    //当前绘制的宽高
    int m_lNowWidth;
    int m_lNowHeight;
    long m_lChangePosx;
    long m_lChangePosy;
};

#endif // _Sprite_H
