//--------------------------------------------------------------------
// 文件名:		WinFreeCamera.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月1日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "../fm_gui/gui_input.h"

#include "free_camera.h"
#include "helper.h"
 
DECLARE_ENTITY(WinFreeCamera, 0, IEntity);

WinFreeCamera::WinFreeCamera()
{
	m_nClickX = 0;
	m_nClickY = 0;
	m_fDirection = 0.0f;
	m_fUpDirection = 0.0f;
	m_fYawDirection = 0.0f;
    m_fPitchDirection = 0.0f;
	m_fShiftDirection = 0.0f;
	m_bAllowControl = true;
}

WinFreeCamera::~WinFreeCamera()
{
}

bool WinFreeCamera::Init(const IVarList& args)
{
	IEntity* pEntity = this;

//	m_pCamera = 

	// 允许键盘控制运动
	CHelper::SetCustomBool(pEntity, "allow_wasd", true);
	// 运动速度
	CHelper::SetCustomFloat(pEntity, "move_speed", 40.0f);
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
	
	return true;
}


bool WinFreeCamera::EnableControl()
{
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
	
	return true;
}

bool WinFreeCamera::DisableControl()
{
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
	
	return true;
}

void WinFreeCamera::SetAllowControl(bool value)
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

// 窗口消息处理
bool WinFreeCamera::MsgProc(unsigned int msg, size_t param1, size_t param2, 
						 int& result)
{
	
	if (!m_bAllowControl)
	{
		return false;
	}

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
    

	return false;
}

bool WinFreeCamera::Shut()
{
	GetCore()->RemoveExecute(this);
	return true;
}

static inline int get_x_lparam(size_t lparam)
{
	return (short)(LOWORD(lparam));
}

static inline int get_y_lparam(size_t lparam)
{
	return (short)(HIWORD(lparam));
}

float WinFreeCamera::GetCurMoveSpeed()
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

float WinFreeCamera::GetCurDragSpeed()
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

float WinFreeCamera::GetCurYawSpeed()
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

float WinFreeCamera::GetCurPitchSpeed()
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

bool WinFreeCamera::GetAllowWASD()
{
	return CHelper::QueryCustomBool(this, "allow_wasd");
}

bool WinFreeCamera::MoveLength(float len)
{
	return GetCamera()->MoveLength(len);
}

bool WinFreeCamera::MoveShift(float delta)
{
	return GetCamera()->MoveShift(delta);
}

bool WinFreeCamera::MoveUp(float delta)
{
	return GetCamera()->MoveUp(delta);
}

bool WinFreeCamera::YawAngle(float angle)
{
	return GetCamera()->YawAngle(angle);
}

bool WinFreeCamera::PitchAngle(float angle)
{
	return GetCamera()->PitchAngle(angle);
}

bool WinFreeCamera::RollAngle(float angle)
{
	return GetCamera()->RollAngle(angle);
}

bool WinFreeCamera::SetVelocity(float x, float y, float z)
{
	return GetCamera()->SetVelocity(x,y,z);
}

bool WinFreeCamera::SetAngleSpeed(float x, float y, float z)
{
	return GetCamera()->SetAngleSpeed(x,y,z); 
}

bool WinFreeCamera::BindYawAngle(float angle)
{
	// 旋转角度
	float angle_x = CHelper::QueryPropertyFloat(this, "AngleX");
	float angle_y = CHelper::QueryPropertyFloat(this, "AngleY");
	float angle_z = CHelper::QueryPropertyFloat(this, "AngleZ");
	
	angle_y += angle;
	
	GetCamera()->SetAngle(angle_x,angle_y,angle_z);
	
	// 调整绑定位置
	ChangeBindPosition();

	return true;
}

bool WinFreeCamera::BindPitchAngle(float angle)
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
	
    GetCamera()->SetAngle(angle_x,angle_y,angle_z);

	// 调整绑定位置
	ChangeBindPosition();
	
	return true;
}

bool WinFreeCamera::ChangeBindPosition()
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
	
	GetCamera()->SetBindPosition(pos_x,pos_y,pos_z);

	return true;
}

// 创建后必须设置摄像机
void WinFreeCamera::SetBindCamera(PERSISTID pid)
{
//	GetCamera() = (ICamera*)GetCore()->GetEntity(pid);
}

void WinFreeCamera::Execute(float seconds)
{
    if(NULL == GetCamera())
		return;
#ifdef PERFORMANCE_TEST
    __int64 time_count = usTimeCounter();
#endif

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

#ifdef PERFORMANCE_TEST

    time_count = usTimeCounter() -time_count;

    if (time_count > 10)
    {
        PERFORMANCE_LOG("WinFreeCamera::Execute", time_count);
    }

#endif

}

bool WinFreeCamera::OnKeyDown(size_t vk)
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

bool WinFreeCamera::OnKeyUp(size_t vk)
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

bool WinFreeCamera::OnChar(size_t vk)
{
	return false;	
}

bool WinFreeCamera::OnMouseMove(size_t param1, size_t param2)
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

bool WinFreeCamera::OnMouseWheel(size_t param1, size_t param2)
{
	IEntity* pEntity = this;

	// 是否绑定对象
	PERSISTID bind_id = CHelper::QueryPropertyObject(pEntity, "Bind");
	
	bool is_bind = (pEntity->GetCore()->GetEntity(bind_id) != NULL);
	
	if (is_bind)
	{
		// 前进或后退
		int delta = short(HIWORD(param1));
		#define WHEEL_DELTA      120      // Default value for rolling one notch
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

bool WinFreeCamera::OnLButtonDown(size_t param1, size_t param2)
{
	m_nClickX = get_x_lparam(param2);
	m_nClickY = get_y_lparam(param2);
	
	return false;
}

bool WinFreeCamera::OnLButtonUp(size_t param1, size_t param2)
{
	return false;
}

bool WinFreeCamera::OnMButtonDown(size_t param1, size_t param2)
{
	m_nClickX = get_x_lparam(param2);
	m_nClickY = get_y_lparam(param2);
	
	return false;
}

bool WinFreeCamera::OnMButtonUp(size_t param1, size_t param2)
{
	return false;
}

bool WinFreeCamera::OnRButtonDown(size_t param1, size_t param2)
{
	return false;
}

bool WinFreeCamera::OnRButtonUp(size_t param1, size_t param2)
{
	return false;
}


void WinFreeCamera::OnEnable()
{
	EnableControl();
	GetCore()->AddExecute(this);
}

void WinFreeCamera::OnDisable()
{
	DisableControl();
	GetCore()->RemoveExecute(this);
}