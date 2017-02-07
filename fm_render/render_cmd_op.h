//--------------------------------------------------------------------
// 文件名:		render_cmd_op.h
// 内  容:		
// 说  明:		抓帧渲染数据一套分析器
// 创建日期:	2014年5月5日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------


#ifndef _RENDER_CMD_OP_H_
#define _RENDER_CMD_OP_H_

#include "render_draw_op.h"
#include "render_state_op.h"
#include "shader_program.h"
#include "../public/portable.h"
 

#define TAG(x) (unsigned int)(  (((unsigned int)x&0x0000ff00)<<8)+(((unsigned int)x&0x000000ff)<<24)+(((unsigned int)x&0x00ff0000)>>8)+(((unsigned int)x&0xff000000)>>24) )

// 格式
class fmxVersion
{
public:
	unsigned int mVersion;//文件版本数据
	unsigned int mSize;//有效数据长度

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




//渲染相关命令
enum RENDER_CMD
{
	R_CMD_NONE,//空命令(保留以后用于一些扩展使用）

	R_CMD_BEGIN,//开始帧

	R_CMD_CLEAR,//清屏操作
	R_CMD_FBO,//切换FBO
	R_CMD_RES_RBO,//RBO相关数据

	R_CMD_RES_IB,//IB数据
	R_CMD_RES_VB,//VB数据
	R_CMD_RES_TEX,//贴图数据
	R_CMD_RES_DRAW_OP,//渲染操作
	R_CMD_RES_STATE_OP,//渲染状态操作
	R_CMD_RES_SHADER_OP,//shader相关数据

	R_CMD_DRAW,//一个渲染批次的标记

	R_CMD_END,//结束帧
};

// 基本操作模式
class CRenderCmd
{
public:
	// 获取当前渲染模式
	virtual RENDER_CMD GetType()
	{
       return m_CmdType;
	}
	// 命令的数据长度
	virtual size_t GetDataSize() = 0;
	// 命令的数据
	virtual void* GetData() = 0;

	// 需要实现的部分
	virtual void Write(TypePointer p) = 0;

	virtual void Read(TypePointer p) = 0;

	// 打印出数据（方便调试)
	virtual void Print() = 0;
protected:
    RENDER_CMD m_CmdType;
};

class CRenderCmdNone:public CRenderCmd
{
public:
	//获取当前命令
	CRenderCmdNone(size_t size,void* pdata)
	{
		m_CmdType = R_CMD_NONE;
	};

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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
	//获取当前命令
	CRenderCmdBegin(size_t frameid)
	{
		m_CmdType = R_CMD_BEGIN;
		m_FrameId = frameid;
	};

	//获取当前帧数
	virtual size_t GetFrameId()
	{
		return m_FrameId;
	}

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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
	//获取当前命令
	CRenderCmdResIB(size_t size,void* pdata):m_size(size),m_pData(pdata)
	{
		m_CmdType = R_CMD_RES_IB;
	};

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return m_size;
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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
	//获取当前命令
	CRenderCmdResVB(size_t size,void* pdata)
	{
		m_CmdType = R_CMD_RES_VB;
	};

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
	virtual void* GetData()
	{
		return m_pData;
	}

	virtual void Write(TypePointer p)
	{
	}

	// 打印出数据（方便调试)
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
	//获取当前命令
	CRenderCmdResTex(size_t size,void* pdata)
	{
		m_CmdType = R_CMD_RES_TEX;
	};

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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
	//获取当前命令
	CRenderCmdResShaderOp(IShaderProgram* pShader)
	{
		m_CmdType = R_CMD_RES_SHADER_OP;
		m_pShader = pShader;

		CShaderProgram* pShaderPro = (CShaderProgram*)pShader;
		pShaderPro->Print();
	};

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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
	//获取当前命令
	CRenderCmdDraw(CDrawOp& drawop)
	{
		m_CmdType = R_CMD_DRAW;
        m_drawop = drawop;
	};

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
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
	//获取当前命令
	CRenderCmdEnd(size_t frameid)
	{
		m_CmdType = R_CMD_END;
		m_FrameId = frameid;
		m_pData = NULL;
	};

	//获取当前帧数
	virtual size_t GetFrameId()
	{
		return m_FrameId;
	}

	// 命令的数据长度
	virtual size_t GetDataSize()
	{
		return sizeof(*this);
	}

	// 命令的数据
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

	// 打印出数据（方便调试)
	virtual void Print()
	{
		CORE_TRACE_EX("CRenderCmdEnd FrameID:%d",m_FrameId);
	}
private:
	size_t m_FrameId;
	void*  m_pData;
};

//fmx格式操作
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

	// 释放
	void  Release()
	{
		CORE_DELETE(this);
	}

	// 放弃数据
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

	// 开始接受数据
	bool ReceiveData(void* pData)
	{
		TypePointer p(pData);

		//先解析格式头数据
		fmxVersion ver;
		ver.Read(p,sizeof(fmxVersion));
		ver.Print();
		p.c += sizeof(fmxVersion);

		size_t nPos = 0 ;
		while(nPos < (ver.mSize - sizeof(fmxVersion)) )
		{
			//后面进行数据更新
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

	// 开始发送数据
	void SendData()
	{
		//这里利用
		//fm_tracelog 进行数据发送
        //进行数据切封
		//1024的数据发送
		const size_t block_data = 1024;

		//这里统计总的需要发送的数据大小
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

		const double dTimeOver = 5.0f;//5秒钟
		unsigned int dWaitTime = 100;//等待一次的时间

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

	// 读出
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

	// 写入
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
	size_t m_usedsize;//已经使用的大小
	size_t m_readsize;//读取位置
	size_t m_size;//总大小
	char*  m_pdata;//其实位置
private:
	// 写入
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


	// 需要的大小
	bool ExpendMem(size_t size)
	{
		char* pData = NULL;

		// 如果当前需要的大小超过总大小
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
		else//如果不超过总大小
		{
			//剩余空间是否够大
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