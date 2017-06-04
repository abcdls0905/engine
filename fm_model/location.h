
#ifndef	_LOCATION_H
#define	_LOCATION_H



// ģ��λ�÷��������
struct location_t
{
	FmVec3 position;
	FmQuat rotate;
	FmVec3 scale;
};

#pragma pack(push, 1) 

// ѹ����ŷ���Ƿ���
struct location_cr_t
{
	unsigned short pitch;
	unsigned short yaw;
	unsigned short roll;
};

// ѹ����ŷ���Ƿ����λ��
struct location_ctr_t
{
	unsigned short pitch;
	unsigned short yaw;
	unsigned short roll;
	float px;
	float py;
	float pz;
};

// ѹ����ŷ���Ƿ��������
struct location_csr_t
{
	unsigned short pitch;
	unsigned short yaw;
	unsigned short roll;
	float sx;
	float sy;
	float sz;
};

#pragma pack(pop)

#endif // _LOCATION_H
