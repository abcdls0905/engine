//--------------------------------------------------------------------
// �ļ���:		render_cmd_op.h
// ��  ��:		
// ˵  ��:		ץ֡��Ⱦ����һ�׷�����
// ��������:	2014��5��5��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------


#ifndef _RENDER_CMD_OP_H_
#define _RENDER_CMD_OP_H_

#include "render_draw_op.h"
#include "render_state_op.h"
#include "shader_program.h"
#include "../public/portable.h"
 

#define TAG(x) (unsigned int)(  (((unsigned int)x&0x0000ff00)<<8)+(((unsigned int)x&0x000000ff)<<24)+(((unsigned int)x&0x00ff0000)>>8)+(((unsigned int)x&0xff000000)>>24) )

// ��ʽ
class fmxVersion
{
public:
	unsigned int mVersion;//�ļ��汾����
	unsigned int mSize;//��Ч���ݳ���

	fmxVersion():mVersion(0),mSize(0){};

	void Read(TypePointer p,int inSize)
	{
		if(mVersion)
		{
			return;
		}

		if(inSize != sizeof(*this))
			return;
        mVersion = *p.dw;
		*p.dw++;

		mSize = *p.dw;
	}

	void Write(TypePointer p,unsigned int mSize)
	{
		*p.dw = '.fmx';
		*p.dw ++;
		*p.dw = mSize;
	}

	//
	bool IsValid()
	{
		if( mVersion == '.fmx')
			return true;
		return false;
	}

	void Print()
	{
		CORE_TRACE_EX("fmxVersion %d %d", mVersion,mSize);
	}
};




//��Ⱦ�������
enum RENDER_CMD
{
	R_CMD_NONE,//������(�����Ժ�����һЩ��չʹ�ã�

	R_CMD_BEGIN,//��ʼ֡

	R_CMD_CLEAR,//��������
	R_CMD_FBO,//�л�FBO
	R_CMD_RES_RBO,//RBO�������

	R_CMD_RES_IB,//IB����
	R_CMD_RES_VB,//VB����
	R_CMD_RES_TEX,//��ͼ����
	R_CMD_RES_DRAW_OP,//��Ⱦ����
	R_CMD_RES_STATE_OP,//��Ⱦ״̬����
	R_CMD_RES_SHADER_OP,//shader�������

	R_CMD_DRAW,//һ����Ⱦ���εı��

	R_CMD_END,//����֡
};

// ��������ģʽ
class CRenderCmd
{
public:
	// ��ȡ��ǰ��Ⱦģʽ
	virtual RENDER_CMD GetType()
	{
       return m_CmdType;
	}
	// ��������ݳ���
	virtual size_t GetDataSize() = 0;
	// ���������
	virtual void* GetData() = 0;

	// ��Ҫʵ�ֵĲ���
	virtual void Write(TypePointer p) = 0;

	virtual void Read(TypePointer p) = 0;

	// ��ӡ�����ݣ��������)
	virtual void Print() = 0;
protected:
    RENDER_CMD m_CmdType;
};

class CRenderCmdNone:public CRenderCmd
{
public:
	//��ȡ��ǰ����
	CRenderCmdNone(size_t size,void* pdata)
	{
		m_CmdType = R_CMD_NONE;
	};

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return m_pData;
	}
	// 

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdNone");
	}

private:
	void*  m_pData;
};

class CRenderCmdBegin:public CRenderCmd
{
public:
	//��ȡ��ǰ����
	CRenderCmdBegin(size_t frameid)
	{
		m_CmdType = R_CMD_BEGIN;
		m_FrameId = frameid;
	};

	//��ȡ��ǰ֡��
	virtual size_t GetFrameId()
	{
		return m_FrameId;
	}

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return m_pData;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE_EX("CRenderCmdBegin FrameID:%d",m_FrameId);
	}
private:
	size_t m_FrameId;
	void*  m_pData;
};


class CRenderCmdResIB:public CRenderCmd
{
public:
	//��ȡ��ǰ����
	CRenderCmdResIB(size_t size,void* pdata):m_size(size),m_pData(pdata)
	{
		m_CmdType = R_CMD_RES_IB;
	};

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return m_size;
	}

	// ���������
	virtual void* GetData()
	{
		return m_pData;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdResIB");
	}
private:
	size_t m_size;
	void*  m_pData;
};


class CRenderCmdResVB:public CRenderCmd
{
public:
	//��ȡ��ǰ����
	CRenderCmdResVB(size_t size,void* pdata)
	{
		m_CmdType = R_CMD_RES_VB;
	};

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return m_pData;
	}

	virtual void Write(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdResVB");
	}
private:
	void*  m_pData;
};

class CRenderCmdResTex:public CRenderCmd
{
public:
	//��ȡ��ǰ����
	CRenderCmdResTex(size_t size,void* pdata)
	{
		m_CmdType = R_CMD_RES_TEX;
	};

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return m_pData;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdResTex");
	}
private:
	void*  m_pData;
};


class CRenderCmdResDrawOp:public CRenderCmd
{
public:
	CRenderCmdResDrawOp(CRenderDrawOp& draw_op)
	{
		m_CmdType = R_CMD_RES_DRAW_OP;
		m_draw_op = draw_op;
	}

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return &m_draw_op;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdResDrawOp");
	}
private:
	CRenderDrawOp m_draw_op;
};

class CRenderCmdResStateOp:public CRenderCmd
{
public:
	CRenderCmdResStateOp(CRenderStateOp& state_op)
	{
		m_CmdType = R_CMD_RES_STATE_OP;
		m_state_op = state_op;
	}

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return &m_state_op;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdResStateOp");
	}
private:
	CRenderStateOp m_state_op;
};

class CRenderCmdResShaderOp:public CRenderCmd
{
public:
	//��ȡ��ǰ����
	CRenderCmdResShaderOp(IShaderProgram* pShader)
	{
		m_CmdType = R_CMD_RES_SHADER_OP;
		m_pShader = pShader;

		CShaderProgram* pShaderPro = (CShaderProgram*)pShader;
		pShaderPro->Print();
	};

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return m_pData;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdResShaderOp");
	}
private:
	void*  m_pData;
	IShaderProgram* m_pShader;
};

class CRenderCmdDraw:public CRenderCmd
{
public:
	//��ȡ��ǰ����
	CRenderCmdDraw(CDrawOp& drawop)
	{
		m_CmdType = R_CMD_DRAW;
        m_drawop = drawop;
	};

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return &m_drawop;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,this,sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
		memcpy(this,p.p,sizeof(*this));
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdDraw");
	}
private:
	CDrawOp m_drawop;
};


class CRenderCmdClear:public CRenderCmd
{
public:
	CRenderCmdClear(CClearBufferOp& cls)
	{
		m_CmdType = R_CMD_CLEAR;
        m_cls = cls;
	}

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return &m_cls;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE("CRenderCmdClear");
	}
private:
	CClearBufferOp m_cls;
};

class CRenderCmdEnd:public CRenderCmd
{
public:
	//��ȡ��ǰ����
	CRenderCmdEnd(size_t frameid)
	{
		m_CmdType = R_CMD_END;
		m_FrameId = frameid;
		m_pData = NULL;
	};

	//��ȡ��ǰ֡��
	virtual size_t GetFrameId()
	{
		return m_FrameId;
	}

	// ��������ݳ���
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// ���������
	virtual void* GetData()
	{
		return m_pData;
	}

	virtual void Write(TypePointer p)
	{
		memcpy(p.p,&(*this),sizeof(*this));
	}

	virtual void Read(TypePointer p)
	{
	}

	// ��ӡ�����ݣ��������)
	virtual void Print()
	{
		CORE_TRACE_EX("CRenderCmdEnd FrameID:%d",m_FrameId);
	}
private:
	size_t m_FrameId;
	void*  m_pData;
};

//fmx��ʽ����
class fmxStreamFile
{
private:

	fmxStreamFile():m_usedsize(0),m_size(0),m_readsize(0),m_pdata(NULL)
	{
	}

public:
	~fmxStreamFile()
	{
		if(m_pdata)
		{
		   delete [] m_pdata;
		}
	}

	static fmxStreamFile* NewInstance()
	{
		fmxStreamFile* p = (fmxStreamFile*)CORE_ALLOC(sizeof(fmxStreamFile));

		new (p) fmxStreamFile;

		return p;
	}

	// �ͷ�
	void  Release()
	{
		CORE_DELETE(this);
	}

	// ��������
	void ClearData()
	{
		if(m_pdata)
		{
		   delete [] m_pdata;
		}

		m_readsize = 0;
		m_usedsize = 0;
		m_size = 0;
		m_pdata = NULL;
	}

	// ��ʼ��������
	bool ReceiveData(void* pData)
	{
		TypePointer p(pData);

		//�Ƚ�����ʽͷ����
		fmxVersion ver;
		ver.Read(p,sizeof(fmxVersion));
		ver.Print();
		p.c += sizeof(fmxVersion);

		size_t nPos = 0 ;
		while(nPos < (ver.mSize - sizeof(fmxVersion)) )
		{
			//����������ݸ���
			CRenderCmd* pCmd =(CRenderCmd*) p.p;

			pCmd->Print();
            /*
			switch(pCmd->GetType())
			{
			case R_CMD_BEGIN:
				{
					CORE_TRACE("R_CMD_BEGIN");
				}
				break;

			case R_CMD_CLEAR:
				{
					CORE_TRACE("R_CMD_CLEAR");
				}
				break;

			case R_CMD_FBO:
				{
					CORE_TRACE("R_CMD_FBO");
				}
				break;

			case R_CMD_RES_RBO:
				{
					CORE_TRACE("R_CMD_RES_RBO");
				}
				break;

			case R_CMD_RES_IB:
				{
					CORE_TRACE("R_CMD_RES_IB");
				}
				break;

			case R_CMD_RES_VB:
				{
					CORE_TRACE("R_CMD_RES_VB");
				}
				break;

			case R_CMD_RES_TEX:
				{
					CORE_TRACE("R_CMD_RES_TEX");
				}
				break;

			case R_CMD_RES_DRAW_OP:
				{
					CORE_TRACE("R_CMD_RES_DRAW_OP");
				}
				break;

			case R_CMD_RES_STATE_OP:
				{
					CORE_TRACE("R_CMD_RES_STATE_OP");
				}
				break;

			case R_CMD_RES_SHADER_OP:
				{
					CORE_TRACE("R_CMD_RES_SHADER_OP");
				}
				break;

			case R_CMD_DRAW:
				{
					CORE_TRACE("R_CMD_DRAW");
				}
				break;

			case R_CMD_END:
				{
					CORE_TRACE("R_CMD_END");
				}
				break;
			};
			*/
			nPos += pCmd->GetDataSize();
			p.c += pCmd->GetDataSize();
		}

		return true;
	}

	// ��ʼ��������
	void SendData()
	{
		//��������
		//fm_tracelog �������ݷ���
        //���������з�
		//1024�����ݷ���
		const size_t block_data = 1024;

		//����ͳ���ܵ���Ҫ���͵����ݴ�С
		size_t send_data = m_usedsize + sizeof(fmxVersion);

		fmxVersion fmx_head;

		char* pSendData = new char[send_data];

		TypePointer p(pSendData);
		fmx_head.Write(p,send_data);

		/*
		TypePointer p_r(pSendData);
		fmx_head.Read( p_r,sizeof(fmxVersion));
		*/
		void* pData = p.c + sizeof(fmxVersion);

		memcpy(pData,GetData(),m_usedsize);

		size_t trace_num = (size_t)((float)send_data / (float)block_data);

		const double dTimeOver = 5.0f;//5����
		unsigned int dWaitTime = 100;//�ȴ�һ�ε�ʱ��

		double dTimeLast = Port_GetPerformanceTime();
		double dTimePass = 0;
		bool bSendOk = true;

		for(size_t i = 0; i < trace_num; i++ )
		{
			bool bTraceOK = fm_tracelog( p.c + i*block_data,LOG_FRAME);
			while( !bTraceOK && (dTimePass< dTimeOver) )
			{
				Port_Sleep(dWaitTime);
				bTraceOK = fm_tracelog( p.c + i*block_data,LOG_FRAME);
				double dCurTime = Port_GetPerformanceTime();
				dTimePass += dCurTime - dTimeLast;
				dTimeLast = dCurTime;
			}
			if( !bTraceOK )
			{
				bSendOk = false;
			}
		}

  		ReceiveData( pSendData );

		delete [] pSendData;

		if( !bSendOk )
		{
			CORE_TRACE("fmxStreamFile Send Data Fail!");
		}

		if( dTimePass < dTimeOver)
		{
			CORE_TRACE("fmxStreamFile Send Data OK!");
		}
		else
		{
			CORE_TRACE("fmxStreamFile Send Data TimeOut!");
		}
	}

	// ����
	fmxStreamFile& operator>>(CRenderCmd* value)
	{ 
		RENDER_CMD cmd = value->GetType();

		switch(cmd)
		{
		case R_CMD_DRAW:
			{
				CRenderCmdDraw* pCmd = (CRenderCmdDraw*)value;
				Read(pCmd);
			}
			break;
		}
		return *this;
	}

	// д��
	fmxStreamFile& operator<<(CRenderCmd* value)
	{ 
		RENDER_CMD cmd = value->GetType();

		switch(cmd)
		{
		case R_CMD_NONE:
			{
				CRenderCmdNone* pCmd = (CRenderCmdNone*)value;
				Write(pCmd);
			}
			break;

		case R_CMD_CLEAR:
			{
				CRenderCmdClear* pCmd = (CRenderCmdClear*)value;
				Write(pCmd);
			}
			break;

		case R_CMD_BEGIN:
			{
				CRenderCmdBegin* pCmd = (CRenderCmdBegin*)value;
				Write(pCmd);
			}
			break;

		case R_CMD_RES_DRAW_OP:
			{
				CRenderCmdResDrawOp* pCmd = (CRenderCmdResDrawOp*)value;
				Write(pCmd);
			}
			break;
		case R_CMD_RES_STATE_OP:
			{
				CRenderCmdResStateOp* pCmd = (CRenderCmdResStateOp*)value;
				Write(pCmd);
			}
			break;
		case R_CMD_RES_SHADER_OP:
			{
				CRenderCmdResShaderOp* pCmd = (CRenderCmdResShaderOp*)value;
				Write(pCmd);
			}
			break;
		case R_CMD_RES_IB:
			{
				CRenderCmdResIB* pCmd = (CRenderCmdResIB*)value;
				Write(pCmd);
			}
			break;
		case R_CMD_RES_VB:
			{
				CRenderCmdResVB* pCmd = (CRenderCmdResVB*)value;
				Write(pCmd);
			}
			break;
		case R_CMD_RES_TEX:
			{
				CRenderCmdResTex* pCmd = (CRenderCmdResTex*)value;
				Write(pCmd);
			}
			break;

		case R_CMD_DRAW:
			{
				CRenderCmdDraw* pCmd = (CRenderCmdDraw*)value;
				Write(pCmd);
			}
			break;

		case R_CMD_END:
			{
				CRenderCmdEnd* pCmd = (CRenderCmdEnd*)value;
				Write(pCmd);
			}
			break;		
		};
		
     
		return *this; 
	}

	 void* GetData()
	 {
		 return m_pdata;
	 }

	 void* GetCanUsedData()
	 {
		 return (void*)&m_pdata[m_usedsize];
	 }

	 size_t GetDataSize()
	 {
		 return m_usedsize;
	 }

	 size_t GetCaptionSize()
	 {
		 return m_size;
	 }

private:
	size_t m_usedsize;//�Ѿ�ʹ�õĴ�С
	size_t m_readsize;//��ȡλ��
	size_t m_size;//�ܴ�С
	char*  m_pdata;//��ʵλ��
private:
	// д��
	void Write(CRenderCmdNone* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Write(CRenderCmdBegin* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Write(CRenderCmdResDrawOp* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Write(CRenderCmdClear* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Write(CRenderCmdResStateOp* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Write(CRenderCmdEnd* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Write(CRenderCmdResShaderOp* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Write(CRenderCmdResIB* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Write(CRenderCmdResVB* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}	

	void Write(CRenderCmdResTex* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}	

	void Write(CRenderCmdDraw* pCmd)
	{
		size_t nsize = pCmd->GetDataSize();
		ExpendMem(nsize);
		void* w_ptr = GetCanUsedData();
		TypePointer ptr(w_ptr);
	 	pCmd->Write(ptr);
		m_usedsize += nsize;
	}

	void Read(CRenderCmdDraw* pCmd)
	{   
		void* w_ptr =(void*)&m_pdata[m_readsize];
		TypePointer ptr(w_ptr);
		pCmd->Read(ptr);
		m_readsize+= pCmd->GetDataSize();
	}


	// ��Ҫ�Ĵ�С
	bool ExpendMem(size_t size)
	{
		char* pData = NULL;

		// �����ǰ��Ҫ�Ĵ�С�����ܴ�С
		if(size > m_size )
		{
			size_t n_size = size*2+m_usedsize;
			pData = new char[n_size];

			memset(pData,0x00,n_size);

			if( m_usedsize != 0 )
				memcpy(pData,m_pdata,m_usedsize);

			if(m_pdata)
				delete [] m_pdata;

			m_size = n_size;
		}
		else//����������ܴ�С
		{
			//ʣ��ռ��Ƿ񹻴�
			size_t lsize = m_size - m_usedsize;
			if( lsize < size )
			{
				size_t n_size = size*2+m_usedsize;
				pData = new char[n_size];
			
				memset(pData,0x00,n_size);
				
				memcpy(pData,m_pdata,m_usedsize);

				if(m_pdata)
					delete [] m_pdata;

				m_size = n_size;

				m_pdata = pData;
			}


			pData = m_pdata;
		}

		m_pdata = pData;

		return true;
	}


};




#endif