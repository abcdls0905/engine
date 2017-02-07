//--------------------------------------------------------------------
// �ļ���:		walk_height_compress.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��1��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _WALK_HEIGHT_COMPRESS_H
#define _WALK_HEIGHT_COMPRESS_H

#include "../public/macros.h"

// ��������֮��ĸ߶�
#define OUT_SCENE_HEIGHT 1e6

// ���߸߶���Ϣѹ��

class CWalkHeightCompress
{
private:
	// �ڵ�����
	enum 
	{
		NODE_BRANCH,	// �����ӽڵ�
		NODE_ORIGINAL,	// δѹ����ֵ
		NODE_COMPRESS,	// ѹ�����ֵ
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
	CWalkHeightCompress();
	~CWalkHeightCompress();

	// ѹ��
	bool Compress(size_t scale, const float* pdata, size_t size);
	// ��ѹ��
	bool Decompress(size_t scale, const unsigned char* pdata, size_t size);

	// ��ý������
	const unsigned char* GetResultData() const { return m_pResultData; }
	size_t GetResultSize() const { return m_nResultSize; }

	// ��ѹ�������л�ø߶�ֵ
	static bool GetHeight(size_t scale, size_t row, size_t col,
		const unsigned char* pdata, size_t size, float& value);

private:
	CWalkHeightCompress(const CWalkHeightCompress&);
	CWalkHeightCompress& operator=(const CWalkHeightCompress&);

	void ReleaseAll();

	// ɾ���ڵ�
	void DeleteNode(node_data_t* node);
	// ���ýڵ�
	node_data_t* CreateNode(size_t scale, size_t row, size_t col,
		const int* pdata, size_t size);
	// ѹ���ڵ�
	size_t CompressNode(node_data_t* node, const int* pdata, size_t size);
	// ���ݽ�ѹ��
	bool DecompressData(size_t scale, size_t row, size_t col,
		const unsigned char* pdata, size_t size, size_t& pos);

	// ��ѹ�������л�ø߶�
	static bool GetHeightData(size_t scale, size_t row, size_t col, 
		const unsigned char* pdata, size_t size, int min_height, 
		int mul_factor, size_t& pos, float& value);

private:
	node_data_t* m_pRoot;
	size_t m_nScale;
	unsigned char* m_pResultData;
	size_t m_nResultMemSize;
	size_t m_nResultSize;
	int* m_pMiddleData;
	size_t m_nMiddleDataSize;
	int m_nMinHeight;
	int m_nMulFactor;
};

#endif // _WALK_HEIGHT_COMPRESS_H
