//--------------------------------------------------------------------
// �ļ���:		height_compress.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��1��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _HEIGHT_COMPRESS_H
#define _HEIGHT_COMPRESS_H

#include "../public/macros.h"

// �߶���Ϣѹ��

class CHeightCompress
{
private:
	// �ڵ�����
	enum 
	{
		NODE_BRANCH,			// �����ӽڵ�
		NODE_ORIGINAL,			// δѹ����ֵ
		NODE_COMPRESS_EQUAL,	// ��ֵѹ��
		NODE_COMPRESS_DELTA8,	// 8λ��ֵѹ��
		NODE_COMPRESS_DELTA16,	// 16λ��ֵѹ��
	};

	// �ڵ�����
	struct node_data_t
	{
		int nType;
		size_t nScale;
		size_t nRow;
		size_t nCol;
		node_data_t* pChildren[4];
	};

public:
	CHeightCompress();
	~CHeightCompress();

	// ѹ��
	bool Compress(size_t scale, const float* pdata, size_t size);
	// ��ѹ��
	bool Decompress(size_t scale, const unsigned char* pdata, size_t size);

	// ��ý������
	const unsigned char* GetResultData() const { return m_pResultData; }
	size_t GetResultSize() const { return m_nResultSize; }

private:
	CHeightCompress(const CHeightCompress&);
	CHeightCompress& operator=(const CHeightCompress&);

	void ReleaseAll();

	// ɾ���ڵ�
	void DeleteNode(node_data_t* node);
	// ���ýڵ�
	node_data_t* CreateNode(size_t scale, size_t row, size_t col,
		const float* pdata, size_t size);
	// ѹ���ڵ�
	size_t CompressNode(node_data_t* node, const float* pdata, size_t size);
	// ���ݽ�ѹ��
	bool DecompressData(size_t scale, size_t row, size_t col,
		const unsigned char* pdata, size_t size, size_t& pos);

private:
	node_data_t* m_pRoot;
	size_t m_nScale;
	unsigned char* m_pResultData;
	size_t m_nResultMemSize;
	size_t m_nResultSize;
};

#endif // _HEIGHT_COMPRESS_H
