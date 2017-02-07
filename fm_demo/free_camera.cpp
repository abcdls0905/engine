//--------------------------------------------------------------------
// 文件名:		FreeCamera.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月1日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------


#include "free_camera.h"
#include "helper.h"
#include "../public/core_log.h"

#ifdef FX_SYSTEM_WINDOWS
#define _WIN32_WINNT 0x0400
#include <windows.h>
#endif

DECLARE_ENTITY(FreeCamera, 0, IEntity);

FreeCamera::FreeCamera()
{
	m_nClickX = 0;
	m_nClickY = 0;
	m_fDirection = 0.0f;
	m_fUpDirection = 0.0f;
	m_fYawDirection = 0.0f;
    m_fPitchDirection = 0.0f;
	m_fShiftDirection = 0.0f;
	m_touchonnum = 0;

	m_nCamraPos = 0;
}

FreeCamera::~FreeCamera()
{
}

bool FreeCamera::Init(const IVarList& args)
{
	IEntity* pEntity = this;
    m_pPainter = (IPainter*)GetCore()->GetInterface("Painter");

	// 允许键盘控制运动
	CHelper::SetCustomBool(pEntity, "allow_wasd", true);
	// 运动速度
	CHelper::SetCustomFloat(pEntity, "move_speed", 20.0f);
	// 拖拽速度
	CHelper::SetCustomFloat(pEntity, "drag_speed", 0.5f);
	// 旋转速度
	CHelper::SetCustomFloat(pEntity, "yaw_speed", FM_PI / 4);
    // add by hutian 2012-1-4
    CHelper::SetCustomFloat(pEntity, "pitch_speed", FM_PI / 4);
	// 到绑定人物的距离
	CHelper::SetCustomFloat(pEntity, "bind_distance", 10.0f);
	// 绑定人物的高度
	CHelper::SetCustomFloat(pEntity, "bind_height", 1.8f);
	
	GetCore()->AddExecute(this);

    m_InputManager = (IInputManager*)GetCore()->GetInterface("CInputManager");
 	m_InputManager->RegisterInputEventCallBack(this);

	return true;
}

//切回尽头坐标
void FreeCamera::ChangeCarmraPos()
{
	if(m_nCamraPos == 0)
	{
		m_pCamera->SetPosition(354.813f,33.192f,-835.174f);    
		m_pCamera->SetAngle(0.148f,-8.570f,0.0f);
	}

	if(m_nCamraPos == 1)
	{
		m_pCamera->SetPosition(192.426f,60.473f,-774.070f);    
		m_pCamera->SetAngle(0.911f,-20.313f,0.f);
	}

	if(m_nCamraPos == 2)
	{
		m_pCamera->SetPosition(266.989,36.359f,-641.289f); 
	}

	if(m_nCamraPos>2)
	{
		m_nCamraPos = 0;
	}
	else
	{
		m_nCamraPos++;
	}
}


void FreeCamera::WriteLogTouchAction(ITouchEvent* Event)
{
	return;
	//这里打印出
	switch(Event->GetTouchEventType())
	{
	case Touch_Began:
		CORE_TRACE("Touch_Began:");
		break;
	case Touch_Moved:
		CORE_TRACE("Touch_Moved:");
		break;
	case Touch_Ended:
		CORE_TRACE("Touch_Ended:");
		break;
	case Touch_Cancel:
        CORE_TRACE("Touch_Cancel:");
		return;
		break;
	}

	char mbzTemp[128];
	for(unsigned int i = 0 ;i < Event->GetTouchNum();i++ )
	{
		Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"index:%d: [x:%f,y:%f]",i,Event->GetTouchPointPos(i).x,Event->GetTouchPointPos(i).y);
		CORE_TRACE(mbzTemp);
	}

}

bool FreeCamera::EnableControl()
{
#ifdef FX_SYSTEM_WINDOWS
#define WM_MOUSEWHEEL (WM_MOUSELAST+1)

	GetCore()->AddMsgProc(this, WM_KEYDOWN);
	GetCore()->AddMsgProc(this, WM_KEYUP);
	GetCore()->AddMsgProc(this, WM_CHAR);
	GetCore()->AddMsgProc(this, WM_MOUSEMOVE);
	GetCore()->AddMsgProc(this, WM_MOUSEWHEEL);
	GetCore()->AddMsgProc(this, WM_LBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_LBUTTONUP);
	GetCore()->AddMsgProc(this, WM_MBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_MBUTTONUP);
	GetCore()->AddMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->AddMsgProc(this, WM_RBUTTONUP);
#endif	
	return true;
}

bool FreeCamera::DisableControl()
{
#ifdef FX_SYSTEM_WINDOWS
	GetCore()->RemoveMsgProc(this, WM_KEYDOWN);
	GetCore()->RemoveMsgProc(this, WM_KEYUP);
	GetCore()->RemoveMsgProc(this, WM_CHAR);
	GetCore()->RemoveMsgProc(this, WM_MOUSEMOVE);
	GetCore()->RemoveMsgProc(this, WM_MOUSEWHEEL);
	GetCore()->RemoveMsgProc(this, WM_LBUTTONDOWN);
	GetCore()->RemoveMsgProc(this, WM_LBUTTONUP);
	GetCore()->RemoveMsgProc(this, WM_MBUTTONDOWN);
	GetCore()->RemoveMsgProc(this, WM_MBUTTONUP);
	GetCore()->RemoveMsgProc(this, WM_RBUTTONDOWN);
	GetCore()->RemoveMsgProc(this, WM_RBUTTONUP);
#endif	
	return true;
}

void FreeCamera::SetAllowControl(bool value)
{
	if (value == m_bAllowControl)
	{
		return;
	}

	m_bAllowControl = value;

	if (m_bAllowControl)
	{
		EnableControl();
	}
	else
	{
		DisableControl();
	}
}

extern ICamera* g_test_camera;

#include "../visual/i_scene.h"
extern IScene* g_test_scene;

//开始按下的时候响应
void FreeCamera::TouchesBegan(ITouchEvent* Event)
{
	WriteLogTouchAction(Event);
	//如果屏幕上大于2个手指以上着直接忽略掉
	if(m_touchonnum>2)
	{
		m_touchonnum+=Event->GetTouchNum();
		return;
	}
	if(Event->GetTouchNum() == 1)
	{
		if(m_touchonnum == 0)
		{
			m_vec2TouchBegin = Event->GetTouchPointPos(0);
		}
		else
		{
			m_vec2TouchBeginTwo = Event->GetTouchPointPos(0);
		}
	}

	//如同同时2个手指按上着
	if(Event->GetTouchNum() == 2)
	{
		m_vec2TouchBegin = Event->GetTouchPointPos(0);
		m_vec2TouchBeginTwo = Event->GetTouchPointPos(1);
	}

	m_touchonnum+=Event->GetTouchNum();
}

//移动的时候响应
void FreeCamera::TouchesMoved(ITouchEvent* Event)
{	
	WriteLogTouchAction(Event);
	//如果超过2个手指在屏幕上着直接忽视该手势
	if(m_touchonnum>2)
	{
		return;
	}

	//如果只有1个手指在屏幕上着表示摄像机进行镜头旋转行为
	if(m_touchonnum == 1)
	{
		if(Event->GetTouchNum() == 1)//只有一个手指在移动
		{
			FmVec2 movepos =  Event->GetTouchPointPos(0);

			FmVec2 abspos =movepos - m_vec2TouchBegin;

			FmVec2 delta = abspos;

			abspos *= FM_PI;

			YawAngle(abspos.x);
			PitchAngle(abspos.y);


			{
// 				g_test_camera->YawAngle(delta.x / 200.f * FM_PIf);
// 				g_test_camera->PitchAngle(delta.y / 200.f * FM_PIf);
			}


			m_vec2TouchBegin = movepos;
			m_vec2TouchBeginTwo = movepos;
		}
	}

	//如果有2个手指在屏幕上，这表示进行摄像机前进后退操作
	if(m_touchonnum == 2)
	{
		float fMove = 0.0;
		if(Event->GetTouchNum() == 1)//如果同时只有1个手指在运动
		{ 
			FmVec2 movepos =  Event->GetTouchPointPos(0);//获取这个坐标
			FmVec2 pre_movepos = Event->GetTouchPrePointPos(0);//获取这点的上一个点坐标位置

			if(pre_movepos == m_vec2TouchBegin) //如果是第一次触摸点,则说明第2次触摸点没有发生移动
			{
                //这次触摸2点之间距离
                FmVec2 vec_abs = m_vec2TouchBeginTwo - movepos;
				//上次触摸2点之间距离
				FmVec2 vec_pre_abs = m_vec2TouchBeginTwo - pre_movepos;

				fMove = vec_abs.length() - vec_pre_abs.length();
                
                m_vec2TouchBegin = movepos;
			}
			else//否则为第2个触摸点的移动事件
			{
                //这次触摸2点之间距离
                FmVec2 vec_abs = m_vec2TouchBegin - movepos;
				//上次触摸2点之间距离
				FmVec2 vec_pre_abs = m_vec2TouchBegin - pre_movepos;

				fMove = vec_abs.length() - vec_pre_abs.length();

				m_vec2TouchBeginTwo = movepos;
			}
		}
		if(Event->GetTouchNum() == 2)//如果同时有2个手指在运动
		{
			FmVec2 vec_abs = Event->GetTouchPointPos(0)-Event->GetTouchPointPos(1);
			//上次触摸2点之间距离
			FmVec2 vec_pre_abs = Event->GetTouchPrePointPos(0)-Event->GetTouchPrePointPos(1);;

			fMove = vec_abs.length() - vec_pre_abs.length(); 

			m_vec2TouchBegin = Event->GetTouchPointPos(0);
            m_vec2TouchBeginTwo = Event->GetTouchPointPos(1);
		} 

		fMove*=20.0;
		MoveLength(fMove);
	}

}


//松手的时候响应
void FreeCamera::TouchesEnded(ITouchEvent* Event)
{
	WriteLogTouchAction(Event);

	// 已经有2个手指进行操作的时候,这个时候有一个手指离开了
	if(m_touchonnum ==2 )
	{
		if(Event->GetTouchNum() == 1)
		{
			//如果是第2个手指离开着需要处理
			if(Event->GetTouchPrePointPos(0) == m_vec2TouchBeginTwo)
				m_vec2TouchBegin = Event->GetTouchPointPos(0);
		}

		if(Event->GetTouchNum() == 2)
		{
            m_vec2TouchBegin = Event->GetTouchPointPos(0);
			m_vec2TouchBeginTwo = Event->GetTouchPointPos(1);
		}
	}

	if(m_touchonnum == 1)
	{
		m_vec2TouchBegin = Event->GetTouchPointPos(0);
	}

	m_touchonnum-=Event->GetTouchNum();

	if( m_touchonnum == 3)
	{
		ChangeCarmraPos();
	}
}

//因为其他原因导致的事件取消，比如说电话事件
void FreeCamera::TouchesCancelled(ITouchEvent* Event)
{
	WriteLogTouchAction(Event);
	m_touchonnum = 0;
}

// 窗口消息处理
bool FreeCamera::MsgProc(unsigned int msg, size_t param1, size_t param2, 
						 int& result)
{
	if (!m_bAllowControl)
	{
		return false;
	}
#ifdef FX_SYSTEM_WINDOWS
	switch(msg)
	{
	case WM_KEYDOWN:
		{
			OnKeyDown(param1);
		}break;
	case WM_KEYUP:
		{
			OnKeyUp(param1);
		}break;
	case WM_MOUSEMOVE:
		{
			OnMouseMove(param1,param2);
		}break;
	case WM_MOUSEWHEEL:
		{
			OnMouseWheel(param1,param2);
		}break;
	case WM_RBUTTONDOWN:
		{
			OnRButtonDown(param1,param2);
		}break;
	case WM_RBUTTONUP:
		{
			OnRButtonUp(param1,param2);
		}break;
	case WM_MBUTTONDOWN:
		{
		}break;
	default:
		break;
	}
    
#endif
	return false;
}

bool FreeCamera::Shut()
{
	GetCore()->RemoveExecute(this);
	return true;
}

#ifdef FX_SYSTEM_WINDOWS

static inline int get_x_lparam(size_t lparam)
{
	return (short)(LOWORD(lparam));
}

static inline int get_y_lparam(size_t lparam)
{
	return (short)(HIWORD(lparam));
}

#endif


#ifdef FX_SYSTEM_WINDOWS
float FreeCamera::GetCurMoveSpeed()
{
	float move_speed = CHelper::QueryCustomFloat(this, "move_speed");
	
	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		// 加速
		return move_speed * 2.0f;
	}
	else
	{
		return move_speed;
	}
}

float FreeCamera::GetCurDragSpeed()
{
	float drag_speed = CHelper::QueryCustomFloat(this, "drag_speed");
	
	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		// 加速
		return drag_speed * 2.0f;
	}
	else
	{
		return drag_speed;
	}
}

float FreeCamera::GetCurYawSpeed()
{
	float yaw_speed = CHelper::QueryCustomFloat(this, "yaw_speed");
	
	if (GetAsyncKeyState(VK_SHIFT) < 0)
	{
		// 加速
		return yaw_speed * 2.0f;
	}
	else
	{
		return yaw_speed;
	}
}

float FreeCamera::GetCurPitchSpeed()
{
    float pitch_speed = CHelper::QueryCustomFloat(this, "pitch_speed");

    if (GetAsyncKeyState(VK_SHIFT) < 0)
    {
        // 加速
        return pitch_speed * 2.0f;
    }
    else
    {
        return pitch_speed;
    }
}
#endif


bool FreeCamera::GetAllowWASD()
{
	return CHelper::QueryCustomBool(this, "allow_wasd");
}

bool FreeCamera::MoveLength(float len)
{
	return m_pCamera->MoveLength(len);
}

bool FreeCamera::MoveShift(float delta)
{
	return m_pCamera->MoveShift(delta);
}

bool FreeCamera::MoveUp(float delta)
{
	return m_pCamera->MoveUp(delta);
}

bool FreeCamera::YawAngle(float angle)
{
	return m_pCamera->YawAngle(angle);
}

bool FreeCamera::PitchAngle(float angle)
{
	return m_pCamera->PitchAngle(angle);
}

bool FreeCamera::RollAngle(float angle)
{
	return m_pCamera->RollAngle(angle);
}

bool FreeCamera::SetVelocity(float x, float y, float z)
{
	return m_pCamera->SetVelocity(x,y,z);
}

bool FreeCamera::SetAngleSpeed(float x, float y, float z)
{
	return m_pCamera->SetAngleSpeed(x,y,z); 
}

bool FreeCamera::BindYawAngle(float angle)
{
	// 旋转角度
	float angle_x = CHelper::QueryPropertyFloat(this, "AngleX");
	float angle_y = CHelper::QueryPropertyFloat(this, "AngleY");
	float angle_z = CHelper::QueryPropertyFloat(this, "AngleZ");
	
	angle_y += angle;
	
	m_pCamera->SetAngle(angle_x,angle_y,angle_z);
	
	// 调整绑定位置
	ChangeBindPosition();

	return true;
}

bool FreeCamera::BindPitchAngle(float angle)
{
	float angle_x = CHelper::QueryPropertyFloat(this, "AngleX");
	float angle_y = CHelper::QueryPropertyFloat(this, "AngleY");
	float angle_z = CHelper::QueryPropertyFloat(this, "AngleZ");
	
	float pos_x = CHelper::QueryPropertyFloat(this, "BindPositionX");
	float pos_y = CHelper::QueryPropertyFloat(this, "BindPositionY");
	float pos_z = CHelper::QueryPropertyFloat(this, "BindPositionZ");

	float dist = pos_y / sinf(angle_x);

	angle_x += angle;

	// 限制俯仰的角度
	float limit_angle = FM_PI * 0.5f;

	if (angle_x > limit_angle)
	{
		angle_x = limit_angle;
	}

	if (angle_x < -limit_angle)
	{
		angle_x = -limit_angle;
	}
	
    m_pCamera->SetAngle(angle_x,angle_y,angle_z);

	// 调整绑定位置
	ChangeBindPosition();
	
	return true;
}

bool FreeCamera::ChangeBindPosition()
{
	float bind_distance = CHelper::QueryCustomFloat(this, 
		"bind_distance");
	float bind_height = CHelper::QueryCustomFloat(this, "bind_height");
	
	float angle_x = CHelper::QueryPropertyFloat(this, "AngleX");
	float angle_y = CHelper::QueryPropertyFloat(this, "AngleY");
	
	// 垂直方向
	float v_delta = bind_distance * sinf(angle_x);
	
	float pos_y = bind_height + v_delta;
	
	// 水平方向
	float h_delta = bind_distance * cosf(angle_x);
	
	float pos_x = -h_delta * sinf(angle_y);
	float pos_z = -h_delta * cosf(angle_y);
	
	m_pCamera->SetBindPosition(pos_x,pos_y,pos_z);

	return true;
}

// 创建后必须设置摄像机
void FreeCamera::SetBindCamera(PERSISTID pid)
{
	m_pCamera = (ICamera*)GetCore()->GetEntity(pid);
}

void FreeCamera::Execute(float seconds)
{
    if(NULL == m_pCamera)
		return;
 
#ifdef FX_SYSTEM_WINDOWS
    //return false;
	IEntity* pEntity = this;
	
	// 是否绑定对象
	PERSISTID bind_id = CHelper::QueryPropertyObject(pEntity, "Bind");

	bool is_bind = (pEntity->GetCore()->GetEntity(bind_id) != NULL);
	
	if (is_bind)
	{
		// 旋转
		
		float delta = seconds * GetCurYawSpeed() * m_fYawDirection;

		if (delta != 0)
		{
			BindYawAngle(delta);
		}
		
        // add by hutian 2012-1-4
        float delta_pitch = seconds * GetCurPitchSpeed() * m_fPitchDirection;

        if (delta_pitch != 0)
        {
            BindPitchAngle(delta_pitch);
        }

		// 前后移动 
		float len = seconds * GetCurMoveSpeed() * m_fDirection;
		
		// 改变绑定距离
		float bind_distance = CHelper::QueryCustomFloat(pEntity, 
			"bind_distance");
		
		bind_distance -= len;

		if (bind_distance < 0)
		{
			bind_distance = 0;
		}
		
		CHelper::SetCustomFloat(pEntity, "bind_distance", bind_distance);
		
		ChangeBindPosition();

		SetVelocity(0.0f, 0.0f, 0.0f);
		SetAngleSpeed(0.0f, 0.0f, 0.0f);
	}
	else
	{
		float move_speed = GetCurMoveSpeed();
		
		// 前后移动 
		float len = seconds * move_speed * m_fDirection;
		
		if (len != 0)
		{
			MoveLength(len);
		}

		// 上下移动
		float len1 = seconds * move_speed * m_fUpDirection;

		if (len1 != 0)
		{
			MoveUp(len1);
		}
		
		// 左右平移 
		float delta = seconds * move_speed * m_fShiftDirection;
		
		if (delta != 0)
		{
			MoveShift(delta);
		}
		
		// 旋转
		float yaw_speed = GetCurYawSpeed();

		float delta1 = seconds * yaw_speed * m_fYawDirection;
		
		if (delta1 != 0)
		{
			YawAngle(delta1);
		}

        // add by hutian 2012-1-4
        float pitch_speed = GetCurPitchSpeed();

        float delta_pitch = seconds * pitch_speed * m_fPitchDirection;

        if (delta_pitch != 0)
        {
            PitchAngle(delta_pitch);
        }

		// 预测移动方向和速度
		float vx = 0.0f;
		float vy = 0.0f;
		float vz = 0.0f;

		float angle_x = CHelper::QueryPropertyFloat(pEntity, "AngleX");
		float angle_y = CHelper::QueryPropertyFloat(pEntity, "AngleY");

		// 前后移动
		float v_delta = move_speed * m_fDirection * sinf(angle_x);
		float h_delta = move_speed * m_fDirection * cosf(angle_x);

		vx += h_delta * sinf(angle_y);
		vy += -v_delta;
		vz += h_delta * cosf(angle_y);

		// 上下移动
		vy += move_speed * m_fUpDirection;

		// 左右移动
		float shift_angle = angle_y + FM_PI * 0.5f;

		vx += move_speed * m_fShiftDirection * sinf(shift_angle);
		vz += move_speed * m_fShiftDirection * cosf(shift_angle);

		SetVelocity(vx, vy, vz);
		//SetAngleSpeed(0.0f, yaw_speed * m_fYawDirection, 0.0f);
        SetAngleSpeed(pitch_speed * m_fPitchDirection, yaw_speed * m_fYawDirection, 0.0f);
	}
#endif
	wchar_t camerainfo[128];
	FmVec3  _pos = m_pCamera->GetPosition();
	FmVec3  _angle = m_pCamera->GetAngle();
	Port_SafeSwprintf(camerainfo, sizeof(camerainfo), L"camera pos x:%.03f ,y:%.03f ,z:%.03f angle x:%.03f y:%.03f z:%.03f",_pos.x,_pos.y,_pos.z ,_angle.x,_angle.y,_angle.z);
	int w = m_pPainter->GetTextWidth(camerainfo);		
	m_pPainter->WriteText(10, 10, 0xFFFF0000, camerainfo);

}

#ifdef FX_SYSTEM_WINDOWS
bool FreeCamera::OnKeyDown(size_t vk)
{
	switch (vk)
	{
	case VK_LEFT:
		// 左转
		m_fYawDirection = -1.0f;
		break;
	case VK_RIGHT:
		// 右转
		m_fYawDirection = 1.0f;
		break;
	case VK_UP:
		// 俯视
		//m_fDirection = 1.0f;
        m_fPitchDirection = -1.0f;
		break;
	case VK_DOWN:
		// 仰视
		//m_fDirection = -1.0f;
        m_fPitchDirection = 1.0f;
		break;
	case VK_HOME:
		// 向上移动
		m_fUpDirection = 1.0f;
		//MoveUp(1.0f);
		break;
	case VK_END:
		// 向下移动
		m_fUpDirection = -1.0f;
		//MoveUp(-1.0f);
		break;
	case VK_INSERT:
		// 仰视
		//PitchAngle(-0.01f);
		break;
	case VK_DELETE:
		// 俯视
		//PitchAngle(0.01f);
		break;
	case VK_PRIOR:
		// 左倾
		//RollAngle(-0.01f);
		break;
	case VK_NEXT:
		// 右倾
		//RollAngle(0.01f);
		break;
	case 'A':
		if (!GetAllowWASD())
		{
			return false;
		}
		// 左转
		//m_fYawDirection = -1.0f;
		m_fShiftDirection = -1.0f;
		break;
	case 'D':
		if (!GetAllowWASD())
		{
			return false;
		}
		// 右转
		//m_fYawDirection = 1.0f;
		m_fShiftDirection = 1.0f;
		break;
	case 'W':
		if (!GetAllowWASD())
		{
			return false;
		}
		// 前进
		m_fDirection = 1.0f;
		break;
	case 'S':
		if (!GetAllowWASD())
		{
			return false;
		}
		// 后退
		m_fDirection = -1.0f;
		break;
	default:
		return false;
	}
	
	return true;
}

bool FreeCamera::OnKeyUp(size_t vk)
{
	switch (vk)
	{
	case VK_LEFT:
		m_fYawDirection = 0.0f;
		break;
	case VK_RIGHT:
		m_fYawDirection = 0.0f;
		break;
	case VK_UP:
		//m_fDirection = 0.0f;
        m_fPitchDirection = 0.0f;
		break;
	case VK_DOWN:
		//m_fDirection = 0.0f;
        m_fPitchDirection = 0.0f;
		break;
	case VK_HOME:
		m_fUpDirection = 0.0f;
		break;
	case VK_END:
		m_fUpDirection = 0.0f;
		break;
	case VK_INSERT:
		break;
	case VK_DELETE:
		break;
	case VK_PRIOR:
		break;
	case VK_NEXT:
		break;
	case 'A':
		if (!GetAllowWASD())
		{
			return false;
		}
		//m_fYawDirection = 0.0f;
		m_fShiftDirection = 0.0f;
		break;
	case 'D':
		if (!GetAllowWASD())
		{
			return false;
		}
		//m_fYawDirection = 0.0f;
		m_fShiftDirection = 0.0f;
		break;
	case 'W':
		if (!GetAllowWASD())
		{
			return false;
		}
		m_fDirection = 0.0f;
		break;
	case 'S':
		if (!GetAllowWASD())
		{
			return false;
		}
		m_fDirection = 0.0f;
		break;
	default:
		return false;
	}
	
	return true;
}

bool FreeCamera::OnChar(size_t vk)
{
	return false;	
}

bool FreeCamera::OnMouseMove(size_t param1, size_t param2)
{
	IEntity* pEntity = this;

	int x = get_x_lparam(param2);
	int y = get_y_lparam(param2);
	
	int delta_x = x - m_nClickX;
	int delta_y = y - m_nClickY;
	
	if ((param1 & MK_RBUTTON) != 0)
	{
		// 是否绑定对象
		PERSISTID bind_id = CHelper::QueryPropertyObject(pEntity, "Bind");
		
		bool is_bind = (pEntity->GetCore()->GetEntity(bind_id) != NULL);
		
		if (is_bind)
		{
			size_t win_width = GetSystemMetrics(SM_CXSCREEN);
			size_t win_height = GetSystemMetrics(SM_CYSCREEN);
			
			// 左右转
			float angle = float(delta_x) / win_width * FM_PI;
			
			BindYawAngle(angle);
			
			// 俯仰
			angle = float(delta_y) / win_height* FM_PI;
			
			BindPitchAngle(angle);
		}
		else
		{
			size_t win_width = GetSystemMetrics(SM_CXSCREEN);
			size_t win_height = GetSystemMetrics(SM_CYSCREEN);
			
			// 左右转
			float angle = float(delta_x) / win_width * FM_PI;
			
			YawAngle(angle);
			
			// 俯仰
			angle = float(delta_y) / win_height * FM_PI;
			
			PitchAngle(angle);
		}
	}
	
	m_nClickX = x;
	m_nClickY = y;
	
	return true;
}

bool FreeCamera::OnMouseWheel(size_t param1, size_t param2)
{
	IEntity* pEntity = this;

	// 是否绑定对象
	PERSISTID bind_id = CHelper::QueryPropertyObject(pEntity, "Bind");
	
	bool is_bind = (pEntity->GetCore()->GetEntity(bind_id) != NULL);
	
	if (is_bind)
	{
		// 前进或后退
		int delta = short(HIWORD(param1));
		#define WHEEL_DELTA      120 
		delta = delta / WHEEL_DELTA;
		
		float len = float(delta) * 0.1f * 5.0f; 

		// 改变绑定距离
		float bind_distance = CHelper::QueryCustomFloat(pEntity, 
			"bind_distance");

		bind_distance -= len;

		CHelper::SetCustomFloat(pEntity, "bind_distance", bind_distance);
			
		ChangeBindPosition();
	}
	else
	{
		// 前进或后退
		int delta = short(HIWORD(param1));
		
		delta = delta / WHEEL_DELTA;
		
		float len = float(delta) * 0.1f * GetCurMoveSpeed(); 
		
		if (len != 0)
		{
			MoveLength(len);
		}
	}

	return false;
}

bool FreeCamera::OnLButtonDown(size_t param1, size_t param2)
{
	m_nClickX = get_x_lparam(param2);
	m_nClickY = get_y_lparam(param2);
	
	return false;
}

bool FreeCamera::OnLButtonUp(size_t param1, size_t param2)
{
	return false;
}

bool FreeCamera::OnMButtonDown(size_t param1, size_t param2)
{
	m_nClickX = get_x_lparam(param2);
	m_nClickY = get_y_lparam(param2);
	
	return false;
}

bool FreeCamera::OnMButtonUp(size_t param1, size_t param2)
{
	return false;
}

bool FreeCamera::OnRButtonDown(size_t param1, size_t param2)
{
	return false;
}

bool FreeCamera::OnRButtonUp(size_t param1, size_t param2)
{
	return false;
}

#endif

