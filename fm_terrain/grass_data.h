//--------------------------------------------------------------------
// �ļ���:		grass_data.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��12��4��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GRASS_DATA_H
#define _GRASS_DATA_H

// ��������

struct grass_data_t
{
	float fPosiX;				// λ��
	float fPosiZ;
	unsigned int nGrassIndex;	// ��������
	unsigned int nScale;		// ��Χ(1-10)
	unsigned int nAmount;		// ����(1-15)
	unsigned int nSeed;			// �������
};
	
#endif // _GRASS_DATA_H
