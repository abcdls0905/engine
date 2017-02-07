//--------------------------------------------------------------------
// 文件名:		skeleton_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef	_SKELETON_DATA_H
#define	_SKELETON_DATA_H

#include "../visual/i_model_system.h"
#include "location.h"

const unsigned int SKELETON_SCALEINFO_FLAG	= 0x00000001;
const unsigned int SKELETON_TRANSLATEINFO_FLAG = 0x00000002;
const unsigned int SKELETON_HELPERANI_FLAG = 0x00000004;

// 动作节点
struct skt_node_t
{
	FXNODE_TYPE nType;
	unsigned int nNodeInfo;
	unsigned int nNameLen;
	char* pszName;
	char szShortName[16]; // 保存较短的名字，避免动态分配内存
	unsigned int nHash;
	skt_node_t* pBucketNext;
	unsigned int bActive;
	unsigned int nLocationCount;
	unsigned int nLocationSize;
	location_t* pLocation; // Local coordinate
	location_t LocationZero; // 只有初始位置时避免动态分配内存
	FmMat4 mtxOriginal; // Object coordinate
	FmMat4 mtxCurrent; // Transposed object coordinate
	FmMat4 mtxInitInverse; // Init pose inverse matrix
	unsigned int nChildNodeCount;
	skt_node_t* ChildNodes;
	int	nBoneId; // Specified the index of boneNodeList.
	int	nIndex;	 // Specified the index of nodeList.
	skt_node_t* pParent;
	bool bGetSuper;
	skt_node_t* pSuperNode;		// 上层父骨架上对应的骨骼节点
	bool bKineticsClothPiece;
};

// 动作数据
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
	skt_node_t* AllNodeList; // 集中存储所有骨骼节点，避免多次分配内存
	skt_node_t** pHashBuckets; // 骨骼节点的哈希表
};

struct skt_pose_t
{
	skeleton_t* pSkeleton;
	FmMat4* pCurrentNodeTMList;
	FmMat4* pChangeNodeTMList;
    bool* pCurrentNodeUserResetList;    // 判断骨骼节点是否由外部重设置
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

// 创建骨骼动画实例
skeleton_t* load_skeleton(const char* pszSkeletonName);
// 卸载骨骼动画实例
bool unload_skeleton(skeleton_t* pSkeleton);

// 复制骨架结构
void copy_skeleton_structure(skeleton_t* pDstInst, skeleton_t* pSrcInst);

// 初始化主骨架位置信息
void init_skeleton_pose_inverse_matrix(skt_node_t* node, 
	FmMat4* mtxCurrentTM);

// 复制动作信息
bool copy_skeleton_action(skeleton_t* pDstInst, 
	skeleton_t* pSrcInst, bool bLoop, float fSpeed, 
	float fCurrentTime, skeleton_t* pBaseSkeleton, int nTranslate, 
	float fScale, int* pBoneDisable);

// 混合动作
bool action_blend(skeleton_t* pDstInst, 
	skeleton_t* pSrcInst, float fBlendWeight, bool bLoop2, 
	float fTime2, float fSpeed2, skeleton_t* pBaseSkeleton, 
	int nTranslate, float fScale, int* pBoneDisable);

// 获得动作的可用帧数
int get_skeleton_frame_count(skeleton_t* pInst);

// 根据名字获得节点
skt_node_t* get_skeleton_node_from_name(skeleton_t* pInst, 
	const char* pszName);
// 根据名字获得骨骼节点
skt_node_t* get_skeleton_bone_node_from_name(skeleton_t* pInst, 
	const char* pszName);

// 获得根节点当前的矩阵
void get_current_root_node_tm(skt_pose_t* ref, FmMat4* mtxTM);

// 获得指定帧根骨骼无位移的矩阵
void get_root_node_tm_no_transform(skeleton_t* pInst, unsigned int nFrame, 
	int nTranslate, FmMat4* mtxTM);

// 获得当前根骨骼无位移的矩阵
void get_current_root_node_tm_no_transform(skeleton_t* pInst, 
	float fCurrentTime, float fSpeed, bool bLoop, int nTranslate, 
	FmMat4* mtxTM);

// 更新骨骼矩阵
void update_skeleton(skeleton_t* pInst, FmMat4* pCurrentNodeTMList, 
	FmMat4* pChangeNodeTMList, skt_pose_t* pParentRef);

// 更新骨骼矩阵
void update_ragdoll_skeleton(skeleton_t* pInst, FmMat4* pCurrentNodeTMList, 
	bool* pCurrentNodeUserResetList, skt_pose_t* pParentRef);

// 比较骨骼结构是否一致
bool compare_skeleton_bone_tree(skeleton_t* dst, skeleton_t* src);

// 屏蔽除指定节点及其子节点外的所有其他节点的骨骼动画
bool open_skeleton_bone_node(skeleton_t* pInst, skt_node_t* pOpenNode,
   int* pBoneDisable);
// 屏蔽除指定节点及其子节点的骨骼动画
bool close_skeleton_bone_node(skeleton_t* pInst, skt_node_t* pCloseNode,
	int* pBoneDisable);

#endif // _SKELETON_DATA_H
