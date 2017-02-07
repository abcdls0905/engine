//--------------------------------------------------------------------
// �ļ���:        Sprite.h
// ��  ��:        
// ˵  ��:        
// ��������:    2010��3��14��
// ������:        lixj
// ��Ȩ����:    ������ţ�������޹�˾
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

// ���ֱ�ǩ
struct SingleType
{
    //ʹ�õ�����ͼƬ
    std::string StrFontImage;                
    //ʹ�õĹ켣
    std::string StrTrackMode;                
    //����ͼƬ�Ŀ��
    unsigned int iBackImageW;
    unsigned int iBackImageH;
    //�����뱳��֮���ƫ��ֵ
    int FontOffsetX;
    int FontOffsetY;
    //����������֮���ƫ��ֵ��������ŵı���
    int OffsetX;
    //����ͼƬ
    std::string StrBackImage;
    //�Ǵ�������ϻ��Ǵ���Ļ�ϳ���
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

//�˶��켣��Ϣ
struct Tracklogic
{
    const float GetPostionX(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;
    const float GetPostionY(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;
    const float GetScaleW(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;
    const float GetScaleH(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;
    const unsigned int GetBlendColor(double dtime,float dElapseTime,float fmovetime, float fstatictime) const;

    float m_fStaticTime; //ͼƬ��ֹʱ��
    float m_fMoveTime; //ͼ�ƶ���ʱ��
    float m_fBackX; //��ʼ����ʱX�᷽���ϵ�λ��
    float m_fBackY; //��ʼ����ʱX�᷽���ϵ�λ��
    int m_fBackW; //��ʼ����ʱ�ĸ�
    int m_fBackH; //��ʼ����ʱ�Ŀ�
    unsigned int m_dStartColor; //��ʼʱ�����ɫ
    unsigned int m_dEndColor; //��ʼʱ�����ɫ
    int m_fMoveSpeedX; //ͼƬ��X�����ϵ��ƶ��ٶ�
    int m_fMoveSpeedY; //ͼƬ��Y�����ϵ��ƶ��ٶ�
    int m_fMoveSpeedAX; //ͼƬ��X�����ϵļ��ٶ�
    int m_fMoveSpeedAY; //ͼƬ��Y�����ϵļ��ٶ�
    int m_fStaticScaleSpeedW; //ͼƬ��������ٶ�
    int m_fStaticScaleSpeedH; //ͼƬ�ߵ������ٶ�
    int m_fMoveScaleSpeedW; //ͼƬ��������ٶ�
    int m_fMoveScaleSpeedH; //ͼƬ�ߵ������ٶ�
    float m_fOffsetY; //ͼƬ����ŵ׵ľ���
    float m_fColorTime;
    bool m_bIsStop;            
    float m_fROffsetY; //ͼƬ����ŵ׵ľ���
    float m_fROffsetX;
    std::string m_StrFont; //ʹ�õ���������

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

    // �Ƿ�͸��
    void SetTransparent(bool value);
    bool GetTransparent() const;

    //������еĳ�
    void ClearAllPool();

    //bool InitParam(SingleType * pParam1,Tracklogic * pParam2,IMAGEINFO * pParam3,
      //  const char *pImageText,const char *pText,long posx,long posy);
	bool InitParam(SingleType* pParam1, Tracklogic* pParam2, 
		CImageInfo* pParam3, const char* pImageText, const char* pText,
		long posx, long posy);

    // ���ø�����Ϣ
    bool SetParentInfo(SpriteManager* pMng, int sprite_index);

    void SetDTime(float dTime);

    //�����Ƿ���� true ���� false ������
    bool GetPaint();
    void SetPaint(bool );

    void RefreshSprite(float );
    bool CreateFontImage();

    bool SetSpriteLeft(int val);
    bool SetSpriteTop(int val);

protected:
    // �ж�ָ��λ���Ƿ�͸��
    bool TestTransparent(int x, int y);

    CImagePlayer * GetImagePlayer(unsigned int index);

private:
    bool m_bTransparent;
    // ֻ������,���ⲿ����
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
    bool m_bIsPaint;                            //true ����
    //��ǰ���ƵĿ��
    int m_lNowWidth;
    int m_lNowHeight;
    long m_lChangePosx;
    long m_lChangePosy;
};

#endif // _Sprite_H
