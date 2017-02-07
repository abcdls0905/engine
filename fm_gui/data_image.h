//--------------------------------------------------------------------
// �ļ���:		DataImage.h
// ��  ��:		����ͼƬ�ؼ�
// ˵  ��:		
// ��������:	2011��9��14��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DataImage_h
#define _DataImage_h

#include "i_control.h"

// ͨ�����ݻ�������
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

    // ���ƿؼ�
    virtual void Paint(float seconds);
    // �����������Ϣ
    virtual void PrepareDesignInfo(IDesignInfo* di);
    virtual bool TestTransparent(int x, int y);

    // ��ӵ�
    bool AddImageData(const char* global_name, int width, int height, const char* data);

    const int GetImageHeight();
    const int GetImageWidth();

    // ���������߶κ͵�
    bool Clear();

    //// ͸��
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

