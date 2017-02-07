//--------------------------------------------------------------------
// �ļ���:		i_block_writer.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��6��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _I_BLOCK_WRITER_H
#define _I_BLOCK_WRITER_H

// ���ļ�д�ӿ�

class IBlockWriter
{
public:
	virtual ~IBlockWriter() = 0;

	// ��ߴ�
	virtual int GetSize() const = 0;
	// д��
	virtual int Write(void* handle) = 0;
};

inline IBlockWriter::~IBlockWriter() {}

#endif // _I_BLOCK_WRITER_H
