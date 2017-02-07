//--------------------------------------------------------------------
// �ļ���:		ImageAnimationManager.h
// ��  ��:		���������ʹ�õ��Ķ������������
// ˵  ��:		
// ��������:	2008��8��22��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef ImageAnimationManager_h_2008_8_22
#define ImageAnimationManager_h_2008_8_22

#include "../public/i_entity.h"
#include "../visual/i_painter.h"
#include "../utils/string_pod.h"
#include "gui_utils.h"


#include <string>
#include <vector>
#include <map>
//#include <windows.h>

typedef FmMat4 D3DXMATRIX;
// ImageAnimationManager

// ˵��
// ��̬�����Ͷ�̬�����Ķ�����ֱ�ӵ���DrawAnimation,
// ��̬�����Ļ��ڹ�������ʼ����������ʱ�ͱ�������ֱ������������ʱ�ű�����
// ��̬������ֻ���ڱ�����ʱ����δ����������²Żᱻ��̬������ͬʱ��¼����ʱ�䣬
//  �������5�벻��ͻ���Execute�б��Զ�����

class ImageAnimationManager: public IEntity
{
public:
    struct FaceFrame
    {
        FaceFrame()
        {
            lEndTime = 100;
        }

        std::string szFileName;		// �ļ���
        PERSISTID ImageID;			// ������ͼԪID
        long lEndTime;			// ����ʱ��
    };

    struct AlphaItem
	{
        int starttime;
        int alpha;
    };

    struct RotateItem
	{
        int starttime;
        int angle;
    };

    struct ScaleItem
	{
        int starttime;
        float xvalue;
        float yvalue;
    };

    struct MoveItem
    {
        int x;
        int y;
        long lTimeIndex;
    };

    enum ePlayEvent
    {
        EVENT_PLAY_NONE  = 0,
        EVENT_PLAY_START = 1,
        EVENT_PLAY_END   = 2,
        EVENT_PLAY_LOOP  = 3,
        EVENT_PLAY_MAX = 4,
    };

    // ���Ʋ���
    struct ControlParam
    {
        ControlParam()
        {
            lTimeCount = 0;
            lCurIndex = 0;
            x = 0;
            y = 0;

            nCurAlpha = 255;
            fCurXScaleValue = 1.0f;
            fCurYScaleValue = 1.0f;
            nCurAngle = 0;
            loop = true;
            athwart = false;
            cevent = EVENT_PLAY_NONE;
        }
        // �������Ʋ���
        long lTime;        // ʱ������
        long lTimeCount;        // �ؼ�֡�����ܼ�ʱ
        long lCurIndex;		    // ��ǰ���ŵ�ͼԪ���

        int x;                  // ��ǰXλ������
        int y;                  // ��ǰYλ������

        std::vector<ControlParam> vSubControls;
        // �����ı䵼�µĶ�������ֵ
        int nCurAlpha;
        float fCurXScaleValue;
        float fCurYScaleValue;
        int nCurAngle;
        bool loop;              // �Ƿ�ѭ��
        bool athwart;           // �Ƿ������򲥷�
        // ���ز���
        ePlayEvent cevent;
    };

    struct SubAnimation
    {
        int x;
        int y;
        bool bFollowMove;
        bool bFollowAlpha;
        bool bFollowScale;
        bool bFollowRotate;
        bool bFollowPlay;
        int start_time;
        int end_time;
        std::string szSubAniName;   // ����Ч��
    };

    enum ePartMotiveMode
    {
        PART_LTTOALL = 0,
        PART_LTOALL = 1,
        PART_LBTOALL = 2,
        PART_BTOALL = 3,
        PART_BRTOALL = 4,
        PART_RTOALL = 5,
        PART_TRTOALL = 6,
        PART_TTOALL = 7,
        PART_MTOLRSIDE = 8,
        PART_MTOTBSIDE = 9,
        PART_CTOALL = 10,
    };

    struct PartMotive
    {
        PartMotive()
        {
            x = 0;
            y = 0;
            lStartTime = 0;
            lEndTime = 0;
            width = 0;
            height = 0;
        }

        ePartMotiveMode mode;
        int x;
        int y;
        std::string szFileName;
        PERSISTID ImageID;
        // �������ò���
        int width;
        int height;

        long lStartTime;
        long lEndTime;
    };

    struct FaceItem
    {
        FaceItem()
        {
            control.lTimeCount = 0;
            control.lCurIndex = 0;

            control.x = 0;
            control.y = 0;

            lWidth = 32;
            lHeight = 32;
            UseCount = 0;
            IsDynamicCreate = false;
            IsHasRotateMovite = false;
            IsHasScaleMotive = false;
            IsHasAlphaMotive = false;
            lLastDrawTime = 0;
            IsPng = false;
            IsEverCreate = false;
            IsCreated = false;
            nBlendMode = 0;
        }

        // �������ж�ȡ-------------------------------
        // 1 �Ƿ�̬����
        bool IsDynamicCreate;
        // 2 �Ƿ���PNG
        bool IsPng;
        // 3 �ں�ģʽ0Ĭ���޻��,1����, 2,����,3,�䰵
        int nBlendMode;
        // 4 �Ƿ�����ת����(��ת�������ĵ���ת)
        bool IsHasRotateMovite;
        std::vector<RotateItem> vRotateMotive;
        // 5 �Ƿ��з�������(�����������ĵ����)
        bool IsHasScaleMotive;
        std::vector<ScaleItem> vScaleMotive;
        // 6 �Ƿ���ALHPA͸������
        bool IsHasAlphaMotive;
        std::vector<AlphaItem> vAlphaMotive;
        // 4-6�������˶������ڵĶ���
        // 7 ��������
        std::string szFaceName;
        // 8 ������֡
        std::vector<FaceFrame> vFrames;	// ����֡
        // 9 �˶�����
        std::vector<MoveItem> vMoveMotive;
        // 10 �ֲ��仯����
        std::vector<PartMotive> vPartMotive;
        // 11 ����Ч
        std::vector<SubAnimation> vSubAnimation;

        long UseCount;		    // ���ü���

        ControlParam control;

        long lLastDrawTime;     // ��¼���һ�α����Ƶ�ϵͳʱ��
        bool IsEverCreate;      // �Ƿ��������������Ƿ����ٴ�����һ��
        bool IsCreated;         // �Ƿ��ѱ�����

        long lWidth;
        long lHeight;
    };

private:
	struct AniCallBackItem
	{
		PERSISTID id;
		std::string szLuaFile;
		std::string szLuaFunc;
	};

	typedef std::vector<AniCallBackItem> AniCallBacks;

public:
	ImageAnimationManager();
    virtual ~ImageAnimationManager();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ���¶���֡
	virtual void Execute(float seconds);

    // ��ȡ���������ļ�
    void SetFilePath(const char* pFile);
    const char* GetFilePath();

	// �ж��Ƿ��Ƕ���
	bool IsAnimation(const char* pName);
    // �Ƿ��ǻ�϶���
    bool IsBlendAnimation(const char* pName);
	// ����һ�ֶ���
    bool DrawAnimation(const char* pName, long x, long y, rect_t& rect, int blend_color = 0xffffffff);
	bool DrawAnimation(const char* pName, long x, long y, rect_t& rect, 
		ControlParam& control, int blend_color = 0xffffffff);
    bool DrawSubAnimation(const char* pName, long x, long y, rect_t& rect, 
        ControlParam& parentcontrol, const size_t control_index, 
		SubAnimation& subconfig, D3DXMATRIX& matRotate);
	// ��ȡͼƬ�����Ŀ��
	point_t GetAnimationSize(const char* pName);
    // ���ö����Ƿ�ѭ��
    bool SetAnimationLoop(const char* pName, bool value);
    // ��������,Ĭ�ϴ����þ�����
    bool StartAnimation(const char* pName);
    float GetAnimationCurTime(const char* pName);
	bool SetAnimationCurTime(const char* pName, int millionseconds);
    float GetAnimationTotalTime(const char* pName);
    // 
    bool Register(const char* pName, const char* pLuaFile, 
		const char* pLuaFunc, PERSISTID id);
    bool UnRegister(const char* pName, const char* pLuaFile, 
		const char* pLuaFunc, PERSISTID id);

    bool Clear();

    // �������ж���
    bool Load();

    // ��ȡ�����б�
    void GetAnimationList(const IVarList& param, IVarList& result);
    // ������Ч����--------------------------------------------------------
    // ��ʼ��0���Ŷ���
    bool StartAnimation(const char* pName, ControlParam& control);
    // ���¶���֡
    bool UpdateAnimationByName(const char* pName, ControlParam& control, 
		long lDelta_Time);
    // ���¶�����̬�����ͷż�ʱ
    bool UpdateDynamicCount(const char* pName);
    // ��ʼ��������
    bool InitControlParam(const char* pName, ControlParam& control);

private:
    // ֻ�ж�̬�����Ķ����Ż���������
    bool DynamicCreateAnmation(FaceItem &faceitem);
    bool DoCallBack(const char* pName, const char* pMode);
    PERSISTID CreateGlyph(void* Pointer, bool IsPng, const char* pFileName, 
		long& Width, long& Height);

    //bool UpdateAnimation(size_t index, FaceItem& faceItem, long lDelta_Time);
    bool UpdateAnimation(FaceItem& faceitem, ControlParam& control, 
		long lDelta_Time);

    // ���򶯻�
    bool OrderUpdate(FaceItem& faceItem, ControlParam& control, 
		int delt_time);
    // ���򶯻�
    bool AthwardUpdate(FaceItem& faceItem, ControlParam& control, 
		int delt_time);

    // �赽��ʼ״̬
    bool SetToBegin(FaceItem& faceitem, ControlParam& control);
    // �赽����״̬
    bool SetToEnd(FaceItem& faceitem, ControlParam& control);

	// ��ö����ص��б�
	AniCallBacks& GetAniCallBacks(const char* name);

private:
	IPainter* m_pPainter;
    std::string m_szFilePath;
	std::vector<FaceItem> m_vFaces;
	TStringPod<char, size_t, TStringTraits<char>, 
		TCoreAlloc> m_mNameToIndexMap;
	//std::map<std::string, long> m_mNameToIndexMap;

    //struct AniCallBackItem
    //{
	//	PERSISTID id;
    //  std::string szLuaFile;
    //    std::string szLuaFunc;
    //};

	std::vector<AniCallBacks*> m_CallBacks;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_CallBackIndex;
    //std::map<std::string, std::vector<AniCallBackItem>> m_mapCallBack;
    bool m_bDeleteSafe;
    //bool m_bNeedClear;
    std::vector<std::string> m_vNeedClear;
};

#endif //ImageAnimationManager_h_2008_8_22

