//--------------------------------------------------------------------
// �ļ���:		i_block_reader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��6��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _I_BLOCK_READER_H
#define _I_BLOCK_READER_H

// ���ļ����ӿ�

class IBlockReader
{
public:
	virtual ~IBlockReader() = 0;

	// �ͷ�
	virtual void Release() = 0;
	// ��ߴ�
	virtual int GetSize() const = 0;
	// ��ȡ
	virtual int Read(void* handle) = 0;
};

inline IBlockReader::~IBlockReader() {}

#endif // _I_BLOCK_READER_H
