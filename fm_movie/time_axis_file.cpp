#include "time_axis.h"
#include "time_axis_macro.h"
#include "camera_wrapper.h"
#include "object_loader.h"
#include "utility.h"
#include "helper.h"
#include "rapid_xml_helper.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

using namespace rapidxml;
extern float g_wobble_rand[50];

bool TimeAxis::LoadObjectKeyFrame(const char* object_name, const char* file, bool bAsync)
{
	// ����ö����Ѿ���������ɾ��
	DeleteObject(object_name);

	// ��Ӷ���
	sObjectData* pObjectData = InnerAddObjectDataNoInitCtrl(object_name);
	if (NULL == pObjectData)
	{
		Assert(0);
		return false;
	}

	CObjectLoader *pLoader = CObjectLoader::NewInstance();
	pLoader->SetFileName(pObjectData, file);
	pLoader->SetOwner(this);
	pObjectData->pLoader = pLoader;

	if (bAsync)
	{
		// �����̼߳���
		if (!s_pRender->RequestLoadResource(0, pLoader))
		{
			return NULL;
		}
	}
	else
	{
		if (pLoader->Load(false))
		{
			pLoader->Create(false);
			pLoader->EndCreate(0, true);
		}
	}

	return true;
}

bool TimeAxis::SaveObjectKeyFrame(const char* object_name, const char* file)
{
	sObjectData* pObjectData = InnerGetObjectData(object_name);
	if (NULL == pObjectData)
	{
		CORE_TRACE("(TimeAxis::SaveObjectKeyFrame) Invalid object name.");
		CORE_TRACE(object_name);
		return false;
	}

	return InnerSaveObjectKeyFrame(pObjectData, object_name, file);
}


bool TimeAxis::SaveAllObject(const char* path, bool bForce)
{
	result_string fname;
	result_string fpath(path);
	Objects::const_iterator ite_obj = m_objects.Begin();
	for (; ite_obj != m_objects.End(); ++ite_obj)
	{
		sObjectData* pObjectData = ite_obj.GetData();
		if (pObjectData == NULL)
		{
			continue;
		}

		//  ���û���κ����ݸĶ��������б���
		if (!pObjectData->bDataChanged && !bForce)
		{
			continue;
		}

		fname = fpath + ite_obj.GetKey() + ".xml";
		InnerSaveObjectKeyFrame(pObjectData, ite_obj.GetKey(), 
			fname.c_str());
	}
	return true;
}

bool TimeAxis::InnerSaveObjectKeyFrame(sObjectData* pObjectData, 
									   const char* object_name, 
									   const char* file_path)
{
	Assert(pObjectData != NULL);
	Assert(object_name != NULL);
	Assert(file_path != NULL);

	pObjectData->bDataChanged = false;

	xml_document<> doc;

	xml_node<>* declaration = xml_add_node(&doc, &doc, NULL, 
		node_declaration);
	xml_add_attr(&doc, declaration, "version", "1.0");
	xml_add_attr(&doc, declaration, "encoding", "utf8");

	// ���ڵ�
	xml_node<>* root = xml_add_node(&doc, &doc, XML_NODE_OBJECT);

	xml_add_attr(&doc, root, "version", PROJECT_VERSION);

	xml_add_attr(&doc, root, XML_ATTR_OBJECT_NAME, object_name);

	if (0.f != pObjectData->fRAddY)
	{	// ���Ϊmax�����Ĺ켣
		xml_add_attr(&doc, root, XML_ATTR_CORRECT_Y, pObjectData->fRAddY);
	}

	if (pObjectData->bLocalPosition)
	{
		xml_add_attr(&doc, root, XML_ATTR_LOCAL_POS, 
			(int)pObjectData->bLocalPosition);
		xml_add_attr(&doc, root, XML_ATTR_ORIGIN_X, pObjectData->fOriginX);
		xml_add_attr(&doc, root, XML_ATTR_ORIGIN_Y, pObjectData->fOriginY);
		xml_add_attr(&doc, root, XML_ATTR_ORIGIN_Z, pObjectData->fOriginZ);

		if (pObjectData->bRotation)
		{
			xml_add_attr(&doc, root, XML_ATTR_ROTATION, 
				(int)pObjectData->bRotation);
			xml_add_attr(&doc, root, XML_ATTR_ROTATION_X, 
				pObjectData->fRotationX);
			xml_add_attr(&doc, root, XML_ATTR_ROTATION_Y, 
				pObjectData->fRotationY);
			xml_add_attr(&doc, root, XML_ATTR_ROTATION_Z, 
				pObjectData->fRotationZ);
		}
	}

	if (!pObjectData->strExtProp.empty())
	{
		xml_add_attr(&doc, root, XML_ATTR_EXT_PROP, 
			pObjectData->strExtProp.c_str());
	}

	KeyFrameControllers& controllers = pObjectData->controllers;
	KeyFrameControllers::iterator ite_ctrl = controllers.Begin();
	while (ite_ctrl != controllers.End())
	{
		KeyFrameController& controller = *ite_ctrl.GetData();

		xml_node<>* node = xml_add_node(&doc, root, XML_NODE_CONTROLLER);
		if (!controller.SaveController(node))
		{
			root->remove_node(node);
		}
		++ite_ctrl;
	}

	if (!xml_save_file(&doc, file_path))
	{
		CORE_TRACE("save file failed");
		CORE_TRACE(file_path);
		return false;
	}

	return true;
}

bool TimeAxis::ExportToMaxPath(const char* object_name, const char* file_name,
							   float x, float y, float z, float ry, int scale)
{
	int controllers[] = 
	{
		// ��Ҫ�����Ŀ�����
		CONTROLLER_TYPE_POS_X, 
		CONTROLLER_TYPE_POS_Y,
		CONTROLLER_TYPE_POS_Z,
		CONTROLLER_TYPE_ANGLE_X,
		CONTROLLER_TYPE_ANGLE_Y,
		CONTROLLER_TYPE_ANGLE_Z,

		CONTROLLER_TYPE_FOV_X,
	};

	// ��ȡ����
	sObjectData* pSrcObjectData = InnerGetObjectData(object_name);
	if (NULL == pSrcObjectData)
	{
		CORE_TRACE("(TimeAxis::SaveObjectKeyFrame) Invalid object name.");
		CORE_TRACE(object_name);
		return false;
	}

	// ��ʱ����һ������, ��Ϊ��������.
	sObjectData* pObjectData = CORE_NEW(sObjectData);
	if (NULL == pObjectData)
	{
		Assert(0);
		return false;
	}

	// ���ƶ�������
	pObjectData->bDataChanged = true;
	pObjectData->ObjectID = pSrcObjectData->ObjectID;
	pObjectData->strName = pSrcObjectData->strName;

	pObjectData->bLocalPosition = pSrcObjectData->bLocalPosition;
	pObjectData->fOriginX = pSrcObjectData->fOriginX;
	pObjectData->fOriginY = pSrcObjectData->fOriginY;
	pObjectData->fOriginZ = pSrcObjectData->fOriginZ;
	pObjectData->initPos = pSrcObjectData->initPos;

	pObjectData->bRotation = pSrcObjectData->bRotation;
	pObjectData->fRAddY = pSrcObjectData->fRAddY;
	pObjectData->fRotationX = pSrcObjectData->fRotationX;
	pObjectData->fRotationY = pSrcObjectData->fRotationY;
	pObjectData->fRotationZ = pSrcObjectData->fRotationZ;
	pObjectData->strExtProp = pSrcObjectData->strExtProp;

	// ���ƿ���������
	KeyFrameController *pController = NULL;
	for (int i = 0; i < sizeof(controllers)/sizeof(int); ++i)
	{
		if ( !pSrcObjectData->controllers.GetData(i, pController))
		{
			KeyFrameController *pNewController = CORE_NEW(KeyFrameController);

			*pNewController = *pController;

			pObjectData->controllers.Add(i, pNewController);
		}
	}

	// ִ��֡����ת��
	ChangeToMaxPath(pObjectData, x, y, z, ry, scale);

	// ��������
	bool ret = InnerSaveObjectKeyFrame(pObjectData, object_name, file_name);

	// ������ʱ����
	CORE_DELETE(pObjectData);

	return ret;
}

bool TimeAxis::ExportToMaxPathFrames(const char* object_name, const char* file_name,
									 float x, float y, float z, float ry, int scale)
{
	// ��ȡ����
	sObjectData* pSrcObjectData = InnerGetObjectData(object_name);
	if (NULL == pSrcObjectData)
	{
		CORE_TRACE("(TimeAxis::SaveObjectKeyFrame) Invalid object name.");
		CORE_TRACE(object_name);
		return false;
	}

	// ��ʱ����һ������, ��Ϊ��������.
	sObjectData* pObjectData = CORE_NEW(sObjectData);
	if (NULL == pObjectData)
	{
		Assert(0);
		return false;
	}

	IVisBase* pObject = GetVisBase(pSrcObjectData->ObjectID);
	if (NULL == pObject)
	{
		return false;
	}

	// ���ƶ�������
	pObjectData->bDataChanged = true;
	pObjectData->ObjectID = pSrcObjectData->ObjectID;
	pObjectData->strName = pSrcObjectData->strName;

	pObjectData->bLocalPosition = pSrcObjectData->bLocalPosition;
	pObjectData->fOriginX = pSrcObjectData->fOriginX;
	pObjectData->fOriginY = pSrcObjectData->fOriginY;
	pObjectData->fOriginZ = pSrcObjectData->fOriginZ;
	pObjectData->initPos = pSrcObjectData->initPos;

	pObjectData->bRotation = pSrcObjectData->bRotation;
	pObjectData->fRotationX = pSrcObjectData->fRotationX;
	pObjectData->fRotationY = pSrcObjectData->fRotationY;
	pObjectData->fRotationZ = pSrcObjectData->fRotationZ;
	pObjectData->fRAddY = pSrcObjectData->fRAddY;

	pObjectData->strExtProp = pSrcObjectData->strExtProp;

	float time_s = m_fBeginTime;
	float time_e = m_fBeginTime + 0.1f;
	float step = m_fFrameInterval;
	float info[6] = {0};
	KeyFrameController *pController = NULL;
	KeyFrameController *controllers[6] = {NULL};

	// ������ʱ����Ŀ�����
	for (int i = 0; i <= CONTROLLER_TYPE_ANGLE_Z; ++i)
	{
		controllers[i] = (KeyFrameController*)CORE_ALLOC(sizeof(KeyFrameController));
		new (controllers[i]) KeyFrameController(
			i,
			CONTROLLER_USAGE_MODE_BEZIER,
			pObjectData
			);	
		pObjectData->controllers.Add(i, controllers[i]);

		if (pSrcObjectData->controllers.GetData(i, pController))
		{
			if (pController->GetBegin() < time_s)
			{
				time_s = pController->GetBegin();
			}

			if (pController->GetEnd() > time_e)
			{
				time_e = pController->GetEnd();
			}
		}
	}

	// λ�ƺͽǶȿ��������(����֡)
	while(true)
	{
		if (GetKeyData(*pSrcObjectData, pObject, time_s, info))
		{
			for (int i = 0; i <= CONTROLLER_TYPE_ANGLE_Z; ++i)
			{
				controllers[i]->SetKeyFrameBezierMode(time_s, info[i],
					time_s, info[i], time_s, info[i]);
			}
		}

		time_s += step;

		if (time_s > time_e)
		{
			break;
		}
	}

	// ���fov�ؼ�֡
	if (!pSrcObjectData->controllers.GetData(CONTROLLER_TYPE_FOV_X, pController))
	{
		KeyFrameController *pNewController = CORE_NEW(KeyFrameController);
		*pNewController = *pController;
		pObjectData->controllers.Add(CONTROLLER_TYPE_FOV_X, pNewController);
	}

	pObjectData->bDataChanged = true;
	pObjectData->bLocalPosition = false;
	pObjectData->fOriginX = 0;
	pObjectData->fOriginY = 0;
	pObjectData->fOriginZ = 0;
	pObjectData->bRotation = false;
	pObjectData->fRotationX = 0;
	pObjectData->fRotationY = 0;
	pObjectData->fRotationZ = 0;
	pObjectData->fRAddY = 0;

	// ִ��֡����ת��
	ChangeToMaxPath(pObjectData, x, y, z, ry, scale);

	// ��������
	bool ret = InnerSaveObjectKeyFrame(pObjectData, object_name, file_name);

	// ������ʱ����
	CORE_DELETE(pObjectData);

	return ret;
}

bool TimeAxis::ChangeToMaxPath(sObjectData* pObjectData, float x, float y, float z, float angle_y, int scale)
{
	if (pObjectData->bRotation)
	{
		return true;
	}

	// ��¼��ʼλ��
	IVisBase* pObject = GetVisBase(pObjectData->ObjectID);
	if (NULL == pObject)
	{
		return false;
	}

	// ������ж�
	bool b_camera = pObject->GetEntInfo()->IsKindOf("CameraWrapper");
	bool b_target_camera = (b_camera && ((CameraWrapper *)pObject)->IsTarget());
	bool b_actor2 = pObject->GetEntInfo()->IsKindOf("Actor2");

	// ��λ�µ�����ϵ.
	float delat_x = x - pObjectData->fOriginX;
	float delat_y = y - pObjectData->fOriginY;
	float delat_z = z - pObjectData->fOriginZ;

	KeyFrameController *pControllerX = GetController(*pObjectData, CONTROLLER_TYPE_POS_X);
	KeyFrameController *pControllerY = GetController(*pObjectData, CONTROLLER_TYPE_POS_Y);
	KeyFrameController* pControllerZ = GetController(*pObjectData, CONTROLLER_TYPE_POS_Z);
	if (pControllerX)
	{
		pControllerX->MoveAllKeyValue(-delat_x);
	}
	if (pControllerY)
	{
		pControllerY->MoveAllKeyValue(-delat_y);
	}
	if (pControllerZ)
	{
		pControllerZ->MoveAllKeyValue(-delat_z);
	}
	pObjectData->fOriginX = x;
	pObjectData->fOriginY = y;
	pObjectData->fOriginZ = z;
	pObjectData->bLocalPosition = true;

	// �����ת�������
	FmMat4 mat;
	FmMat4	mat_Inv;	
	FmMatrixIdentity(&mat);
	FmMatrixRotationY(&mat, angle_y);	
	FmMatrixInverse(&mat_Inv, &mat);
	pObjectData->fRotationX = 0;
	pObjectData->fRotationZ = 0;

	// ��¼��ʼλ��
	FmVec3 pos = pObject->GetPosition();
	pos.x -= x; 
	pos.y -= y;
	pos.z -= z;
	FmVec3TransformCoord(&pos, &pos, &mat_Inv);
	pObjectData->initPos = pos;

	// �洢���Ӳ���
	char param[128] = "";
	if (b_camera)
	{
		if(b_target_camera)
		{
			SafeSprintf(param, sizeof(param), "target,%f,%f,%f", pos.x, pos.z, pos.y);
		}
		else
		{
			SafeSprintf(param, sizeof(param), "free,%f,%f,%f", pos.x, pos.z, pos.y);
		}
	}
	else
	{
		SafeSprintf(param, sizeof(param), "object,%f,%f,%f", pos.x, pos.z, pos.y);
	}
	pObjectData->strExtProp = param;

	// ����֡����ת�� x,z
	if (NULL != pControllerX && NULL != pControllerZ)
	{

		sKeyBezier key_x;
		sKeyBezier key_z;
		pos.y = 0;
		size_t count_x = pControllerX->GetKeyCount();
		size_t count_z = pControllerZ->GetKeyCount();
		if (count_x != count_z)
		{
			CORE_TRACE("(TimeAxis::ChangeToMaxPath) The position key's time no same.");
			return false;
		}

		for (size_t i = 0; i< count_x; ++i)
		{
			key_x = pControllerX->GetBezierKey(i);
			key_z = pControllerZ->GetBezierKey(i);

			if (!fm_movie::FloatEqualLocal(key_x.time, key_z.time))
			{
				CORE_TRACE("(TimeAxis::ChangeToMaxPath) The position key's time no same.");
				return false;
			}

			// ���¹ؼ�֡��ֵ
			pos.x = key_x.value;
			pos.z = key_z.value; 
			FmVec3TransformCoord(&pos, &pos, &mat_Inv);
			key_x.value = pos.x;
			key_z.value = pos.z;

			// ��������Ƶ��ֵ
			pos.x = key_x.left_value;
			pos.z = key_z.left_value; 
			FmVec3TransformCoord(&pos, &pos, &mat_Inv);
			key_x.left_value = pos.x;
			key_z.left_value = pos.z;

			// �����ҿ��Ƶ��ֵ
			pos.x = key_x.right_value;
			pos.z = key_z.right_value; 
			FmVec3TransformCoord(&pos, &pos, &mat_Inv);
			key_x.right_value = pos.x;
			key_z.right_value = pos.z;

			pControllerX->SetBezierKey(i, key_x);
			pControllerZ->SetBezierKey(i, key_z);
		}
	}

	// ����֡����ת�� z
	KeyFrameController *pControllerRX = 
		GetController(*pObjectData, CONTROLLER_TYPE_ANGLE_X);
	KeyFrameController *pControllerRY = 
		GetController(*pObjectData, CONTROLLER_TYPE_ANGLE_Y);
	KeyFrameController *pControllerRZ = 
		GetController(*pObjectData, CONTROLLER_TYPE_ANGLE_Z);

	if (NULL != pControllerRX && NULL != pControllerRY && NULL != pControllerRZ)
	{
		size_t count_x = pControllerRX->GetKeyCount();
		size_t count_y = pControllerRY->GetKeyCount();
		size_t count_z = pControllerRZ->GetKeyCount();
		if (count_x != count_y || count_x != count_z)
		{
			CORE_TRACE("(TimeAxis::ChangeToMaxPath) The position key's time no same.");
			return false;
		}

		sKeyBezier key_rx;
		sKeyBezier key_ry;

		sKeyBezier key_rz;
		FmMat4 mat_ref;

		for (size_t i = 0; i< count_x; ++i)
		{
			key_rx = pControllerRX->GetBezierKey(i);
			key_ry = pControllerRY->GetBezierKey(i);
			key_rz = pControllerRZ->GetBezierKey(i);

			if (!fm_movie::FloatEqualLocal(key_rx.time, key_ry.time))
			{
				CORE_TRACE("(TimeAxis::ChangeToMaxPath) The position key's time no same.");
				return false;
			}
			if (!fm_movie::FloatEqualLocal(key_rx.time, key_rz.time))
			{
				CORE_TRACE("(TimeAxis::ChangeToMaxPath) The position key's time no same.");
				return false;
			}

			// ������ת����
			FmMatrixIdentity(&mat_ref);
			FmMat4RotationYawPitchRoll(&mat_ref, key_ry.value, key_rx.value, key_rz.value);


			// ����Լ�����ת����
			FmMatrixIdentity(&mat);
			mat = mat_ref * mat_Inv;

			FmVec3 angle(0, 0, 0);
			VisUtil_GetMatrixAngle(&angle, &mat);

			key_rx.value = angle.x;
			key_ry.value = angle.y;
			key_rz.value = angle.z;

			if(b_actor2)
			{
				// �����actor2�Ķ���, ��ģ������180��ת����.
				key_rx.value = -angle.x;
				key_ry.value = angle.y - FM_PIf;
				key_rz.value = -angle.z;
			}

			key_rx.mode = SHOW_CURVE1;
			key_ry.mode = SHOW_CURVE1;
			key_rz.mode = SHOW_CURVE1;

			pControllerRX->SetBezierKey(i, key_rx);
			pControllerRY->SetBezierKey(i, key_ry);
			pControllerRZ->SetBezierKey(i, key_rz);
		}

		pControllerRX->CreateAllControlPoints();
		pControllerRY->CreateAllControlPoints();
		pControllerRZ->CreateAllControlPoints();
	}

	// ���¶�������
	pObjectData->bRotation = true;
	pObjectData->fRotationY = angle_y;

	return true;
}

bool TimeAxis::GetKeyData(sObjectData &object_data, IVisBase *pObject, float time,
						  float *fPosInfo, size_t nInfoLen)				
{
	if (NULL == pObject || NULL == fPosInfo || 6 > nInfoLen)
	{
		return false;
	}

	// ͳ������ͽǶȹؼ�֡
	FmVec3*	pPosition = (FmVec3*)fPosInfo;
	FmVec3*	pAngle = (FmVec3*)(fPosInfo+3);
	bool			bPosKey;

	// ͳ������صĹؼ�֡
	float	fWobbleInfo[7];
	bool	bWobbleKey;
	float	fMinKey = RAND_MAX, fMaxKey = -RAND_MAX;
	bool bActor = pObject->GetEntInfo()->IsKindOf("Actor2");

	// ͳ������ͽǶȹؼ�֡׼��
	memset(fPosInfo, 0, sizeof(fPosInfo));
	bPosKey = false;

	// ͳ������صĹؼ�֡׼��
	memset(fWobbleInfo, 0, sizeof(fWobbleInfo));
	bWobbleKey = false;

	CVarList temp;
	KeyFrameControllers& controllers = object_data.controllers;
	KeyFrameControllers::iterator ite_ctrl = controllers.Begin();
	while (ite_ctrl != controllers.End())
	{
		KeyFrameController& controller = *ite_ctrl.GetData();
		int controller_type = controller.GetControllerType();

		if (controller_type >= CONTROLLER_TYPE_POS_X &&
			controller_type <= CONTROLLER_TYPE_ANGLE_Z)
		{
			// ͳ������ͽǶȹؼ�֡
			temp.Clear();
			if(controller.GetKeyInfo(pObject, time, temp))
			{
				fPosInfo[controller_type - CONTROLLER_TYPE_POS_X] = temp.FloatVal(1);
				bPosKey = true;
			}

		}
		else if(controller_type >= CONTROLLER_TYPE_WOBBLE_X &&
			controller_type <= CONTROLLER_TYPE_WOBBLE_RZ)
		{
			// ͳ������صĹؼ�֡
			temp.Clear();
			if(controller.GetKeyInfo(pObject, time, temp))
			{
				fWobbleInfo[controller_type - CONTROLLER_TYPE_WOBBLE_X] = temp.FloatVal(1);
				bWobbleKey = true;
			}

			if (controller.GetBegin() < fMinKey)
			{
				fMinKey = controller.GetBegin();
			}
			if (controller.GetEnd() > fMaxKey)
			{
				fMaxKey = controller.GetEnd();
			}
		}

		++ite_ctrl;
	}

	if (!bPosKey && object_data.bRotation)
	{
		// ֡��max����û�йؼ�֡,Ҳˢ��λ��.
		*pPosition = object_data.initPos;
		bPosKey = true;
	}

	if (time <= fMinKey || time >= fMaxKey)
	{
		bWobbleKey = false;
	}

	if (bPosKey)
	{

		bool b_need_rotation = 
			(object_data.bLocalPosition && object_data.bRotation);
		bool b_target_camera = (pObject->GetEntInfo()->IsKindOf("CameraWrapper") && 
			((CameraWrapper *)pObject)->IsTarget());

		if (b_target_camera)
		{
			((CameraWrapper *)pObject)->UpdateTargetCameraKey(pPosition,
				pAngle, object_data, b_need_rotation);
		}			
		else if (b_need_rotation)
		{
			if (bActor)
			{
				// ��ģ�ͷ�ת180
				pAngle->y += FM_PIf;
				pAngle->x = -pAngle->x;
				pAngle->z = -pAngle->z;
			}

			RotationKey(pPosition, pAngle, object_data);
		}

		if (object_data.bLocalPosition)
		{
			pPosition->x += object_data.fOriginX;
			pPosition->y += object_data.fOriginY;
			pPosition->z += object_data.fOriginZ;
		}

		if (bWobbleKey)
		{
			// �𶯹ؼ�֡�ĵ��Ӵ���
			FmVec3 vPos, vAngle;

			GetWobbleInfo(vPos, vAngle, time, fWobbleInfo);

			FmMat4 mat;
			FmMat4RotationYawPitchRoll(&mat, pAngle->y, pAngle->x, pAngle->z);
			FmVec3TransformCoord(&vPos, &vPos, &mat);

			*pPosition += vPos;
			*pAngle += vAngle;

		}

		return true;
	}

	return false;
}

bool TimeAxis::LoadRandomData(const char *file_name)
{
	if (strlen(file_name) < 1)
	{
		return false;
	}

	FILE* fp = core_file::fopen(file_name, "rb");
	if (NULL == fp)
	{
		CORE_TRACE("(TimeAxis::LoadRandomData)open file failed");
		CORE_TRACE(file_name);
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);
	size_t file_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);

	if (file_size < sizeof(g_wobble_rand))
	{
		CORE_TRACE("(TimeAxis::LoadRandomData)open file failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}

	//TAutoMem<char, 256> auto_buf(file_size + 1);
	//char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(g_wobble_rand, sizeof(g_wobble_rand), 1, fp) != file_size)
	{
		core_file::fclose(fp);
		return true;
	}

	core_file::fclose(fp);
	return false;
}

bool TimeAxis::SaveRandomData(const char *file_name)
{
	if (strlen(file_name) < 1)
	{
		return false;
	}

	FILE* fp = core_file::fopen(file_name, "wb");
	if (NULL == fp)
	{
		CORE_TRACE("(TimeAxis::LoadRandomData)save file failed");
		CORE_TRACE(file_name);
		return false;
	}

	if (core_file::fwrite(g_wobble_rand, sizeof(g_wobble_rand), 1, fp))
	{
		core_file::fclose(fp);
		return true;
	}

	core_file::fclose(fp);
	return false;
}
