//--------------------------------------------------------------------
// �ļ���:		skeleton_data.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��7��13��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef	_SKELETON_DATA_H
#define	_SKELETON_DATA_H

#include "../visual/i_model_system.h"
#include "location.h"

const unsigned int SKELETON_SCALEINFO_FLAG	= 0x00000001;
const unsigned int SKELETON_TRANSLATEINFO_FLAG = 0x00000002;
const unsigned int SKELETON_HELPERANI_FLAG = 0x00000004;

// �����ڵ�
struct skt_node_t
{
	FXNODE_TYPE nType;
	unsigned int nNodeInfo;
	unsigned int nNameLen;
	char* pszName;
	char szShortName[16]; // ����϶̵����֣����⶯̬�����ڴ�
	unsigned int nHash;
	skt_node_t* pBucketNext;
	unsigned int bActive;
	unsigned int nLocationCount;
	unsigned int nLocationSize;
	location_t* pLocation; // Local coordinate
	location_t LocationZero; // ֻ�г�ʼλ��ʱ���⶯̬�����ڴ�
	FmMat4 mtxOriginal; // Object coordinate
	FmMat4 mtxCurrent; // Transposed object coordinate
	FmMat4 mtxInitInverse; // Init pose inverse matrix
	unsigned int nChildNodeCount;
	skt_node_t* ChildNodes;
	int	nBoneId; // Specified the index of boneNodeList.
	int	nIndex;	 // Specified the index of nodeList.
	skt_node_t* pParent;
	bool bGetSuper;
	skt_node_t* pSuperNode;		// �ϲ㸸�Ǽ��϶�Ӧ�Ĺ����ڵ�
	bool bKineticsClothPiece;
};

// ��������
struct skeleton_t
{
	unsigned int nFlag;
	unsigned int nVersion;
	unsigned int nFileType;
	unsigned int nNameLen;
	char* pszName;
	unsigned int nStartFrame;
	unsigned int nEndFrame;
	float fInternalFPS;
	FmVec3	vRootMin;
	FmVec3	vRootMax;
	unsigned int nRootNodeCount;
	skt_node_t* RootNodes;
	bool bLoaded;
	unsigned int nBoneNodeListSize;
	unsigned int nTotalBoneNodeCount;
	skt_node_t** pBoneNodeList;
	unsigned int nHelperNodeListSize;
	unsigned int nTotalHelperNodeCount;
	skt_node_t** pHelperNodeList;
	unsigned int nNodeListSize;
	unsigned int nTotalNodeCount;
	skt_node_t** pNodeList;
	skt_node_t* AllNodeList; // ���д洢���й����ڵ㣬�����η����ڴ�
	skt_node_t** pHashBuckets; // �����ڵ�Ĺ�ϣ��
};

struct skt_pose_t
{
	skeleton_t* pSkeleton;
	FmMat4* pCurrentNodeTMList;
	FmMat4* pChangeNodeTMList;
    bool* pCurrentNodeUserResetList;    // �жϹ����ڵ��Ƿ����ⲿ������
//	int nState;
//	float fCurrentTime;
//	unsigned int nCurrentFrame;
//	float fCurrentFrameOffset;
//	float fSpeed;
//	bool bLoop;
//	bool bTransformMode;
	FmMat4 mtxCurrentTM;
	skt_pose_t* pParentPose;
	float fScale;
};

// ������������ʵ��
skeleton_t* load_skeleton(const char* pszSkeletonName);
// ж�ع�������ʵ��
bool unload_skeleton(skeleton_t* pSkeleton);

// ���ƹǼܽṹ
void copy_skeleton_structure(skeleton_t* pDstInst, skeleton_t* pSrcInst);

// ��ʼ�����Ǽ�λ����Ϣ
void init_skeleton_pose_inverse_matrix(skt_node_t* node, 
	FmMat4* mtxCurrentTM);

// ���ƶ�����Ϣ
bool copy_skeleton_action(skeleton_t* pDstInst, 
	skeleton_t* pSrcInst, bool bLoop, float fSpeed, 
	float fCurrentTime, skeleton_t* pBaseSkeleton, int nTranslate, 
	float fScale, int* pBoneDisable);

// ��϶���
bool action_blend(skeleton_t* pDstInst, 
	skeleton_t* pSrcInst, float fBlendWeight, bool bLoop2, 
	float fTime2, float fSpeed2, skeleton_t* pBaseSkeleton, 
	int nTranslate, float fScale, int* pBoneDisable);

// ��ö����Ŀ���֡��
int get_skeleton_frame_count(skeleton_t* pInst);

// �������ֻ�ýڵ�
skt_node_t* get_skeleton_node_from_name(skeleton_t* pInst, 
	const char* pszName);
// �������ֻ�ù����ڵ�
skt_node_t* get_skeleton_bone_node_from_name(skeleton_t* pInst, 
	const char* pszName);

// ��ø��ڵ㵱ǰ�ľ���
void get_current_root_node_tm(skt_pose_t* ref, FmMat4* mtxTM);

// ���ָ��֡��������λ�Ƶľ���
void get_root_node_tm_no_transform(skeleton_t* pInst, unsigned int nFrame, 
	int nTranslate, FmMat4* mtxTM);

// ��õ�ǰ��������λ�Ƶľ���
void get_current_root_node_tm_no_transform(skeleton_t* pInst, 
	float fCurrentTime, float fSpeed, bool bLoop, int nTranslate, 
	FmMat4* mtxTM);

// ���¹�������
void update_skeleton(skeleton_t* pInst, FmMat4* pCurrentNodeTMList, 
	FmMat4* pChangeNodeTMList, skt_pose_t* pParentRef);

// ���¹�������
void update_ragdoll_skeleton(skeleton_t* pInst, FmMat4* pCurrentNodeTMList, 
	bool* pCurrentNodeUserResetList, skt_pose_t* pParentRef);

// �ȽϹ����ṹ�Ƿ�һ��
bool compare_skeleton_bone_tree(skeleton_t* dst, skeleton_t* src);

// ���γ�ָ���ڵ㼰���ӽڵ�������������ڵ�Ĺ�������
bool open_skeleton_bone_node(skeleton_t* pInst, skt_node_t* pOpenNode,
   int* pBoneDisable);
// ���γ�ָ���ڵ㼰���ӽڵ�Ĺ�������
bool close_skeleton_bone_node(skeleton_t* pInst, skt_node_t* pCloseNode,
	int* pBoneDisable);

#endif // _SKELETON_DATA_H
