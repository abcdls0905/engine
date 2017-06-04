//--------------------------------------------------------------------
// �ļ���:		WinFreeCamera.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��1��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

	// ������̿����˶�
	CHelper::SetCustomBool(pEntity, "allow_wasd", true);
	// �˶��ٶ�
	CHelper::SetCustomFloat(pEntity, "move_speed", 40.0f);
	// ��ק�ٶ�
	CHelper::SetCustomFloat(pEntity, "drag_speed", 0.5f);
	// ��ת�ٶ�
	CHelper::SetCustomFloat(pEntity, "yaw_speed", FM_PI / 4);
    // add by hutian 2012-1-4
    CHelper::SetCustomFloat(pEntity, "pitch_speed", FM_PI / 4);
	// ��������ľ���
	CHelper::SetCustomFloat(pEntity, "bind_distance", 10.0f);
	// ������ĸ߶�
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

// ������Ϣ����
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
		// ����
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
		// ����
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
		// ����
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
        // ����
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
	// ��ת�Ƕ�
	float angle_x = CHelper::QueryPropertyFloat(this, "AngleX");
	float angle_y = CHelper::QueryPropertyFloat(this, "AngleY");
	float angle_z = CHelper::QueryPropertyFloat(this, "AngleZ");
	
	angle_y += angle;
	
	GetCamera()->SetAngle(angle_x,angle_y,angle_z);
	
	// ������λ��
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

	// ���Ƹ����ĽǶ�
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

	// ������λ��
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
	
	// ��ֱ����
	float v_delta = bind_distance * sinf(angle_x);
	
	float pos_y = bind_height + v_delta;
	
	// ˮƽ����
	float h_delta = bind_distance * cosf(angle_x);
	
	float pos_x = -h_delta * sinf(angle_y);
	float pos_z = -h_delta * cosf(angle_y);
	
	GetCamera()->SetBindPosition(pos_x,pos_y,pos_z);

	return true;
}

// ������������������
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
	
	// �Ƿ�󶨶���
	PERSISTID bind_id = CHelper::QueryPropertyObject(pEntity, "Bind");

	bool is_bind = (pEntity->GetCore()->GetEntity(bind_id) != NULL);
	
	if (is_bind)
	{
		// ��ת
		
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

		// ǰ���ƶ� 
		float len = seconds * GetCurMoveSpeed() * m_fDirection;
		
		// �ı�󶨾���
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
		
		// ǰ���ƶ� 
		float len = seconds * move_speed * m_fDirection;
		
		if (len != 0)
		{
			MoveLength(len);
		}

		// �����ƶ�
		float len1 = seconds * move_speed * m_fUpDirection;

		if (len1 != 0)
		{
			MoveUp(len1);
		}
		
		// ����ƽ�� 
		float delta = seconds * move_speed * m_fShiftDirection;
		
		if (delta != 0)
		{
			MoveShift(delta);
		}
		
		// ��ת
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

		// Ԥ���ƶ�������ٶ�
		float vx = 0.0f;
		float vy = 0.0f;
		float vz = 0.0f;

		float angle_x = CHelper::QueryPropertyFloat(pEntity, "AngleX");
		float angle_y = CHelper::QueryPropertyFloat(pEntity, "AngleY");

		// ǰ���ƶ�
		float v_delta = move_speed * m_fDirection * sinf(angle_x);
		float h_delta = move_speed * m_fDirection * cosf(angle_x);

		vx += h_delta * sinf(angle_y);
		vy += -v_delta;
		vz += h_delta * cosf(angle_y);

		// �����ƶ�
		vy += move_speed * m_fUpDirection;

		// �����ƶ�
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
		// ��ת
		m_fYawDirection = -1.0f;
		break;
	case VK_RIGHT:
		// ��ת
		m_fYawDirection = 1.0f;
		break;
	case VK_UP:
		// ����
		//m_fDirection = 1.0f;
        m_fPitchDirection = -1.0f;
		break;
	case VK_DOWN:
		// ����
		//m_fDirection = -1.0f;
        m_fPitchDirection = 1.0f;
		break;
	case VK_HOME:
		// �����ƶ�
		m_fUpDirection = 1.0f;
		//MoveUp(1.0f);
		break;
	case VK_END:
		// �����ƶ�
		m_fUpDirection = -1.0f;
		//MoveUp(-1.0f);
		break;
	case VK_INSERT:
		// ����
		//PitchAngle(-0.01f);
		break;
	case VK_DELETE:
		// ����
		//PitchAngle(0.01f);
		break;
	case VK_PRIOR:
		// ����
		//RollAngle(-0.01f);
		break;
	case VK_NEXT:
		// ����
		//RollAngle(0.01f);
		break;
	case 'A':
		if (!GetAllowWASD())
		{
			return false;
		}
		// ��ת
		//m_fYawDirection = -1.0f;
		m_fShiftDirection = -1.0f;
		break;
	case 'D':
		if (!GetAllowWASD())
		{
			return false;
		}
		// ��ת
		//m_fYawDirection = 1.0f;
		m_fShiftDirection = 1.0f;
		break;
	case 'W':
		if (!GetAllowWASD())
		{
			return false;
		}
		// ǰ��
		m_fDirection = 1.0f;
		break;
	case 'S':
		if (!GetAllowWASD())
		{
			return false;
		}
		// ����
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
		// �Ƿ�󶨶���
		PERSISTID bind_id = CHelper::QueryPropertyObject(pEntity, "Bind");
		
		bool is_bind = (pEntity->GetCore()->GetEntity(bind_id) != NULL);
		
		if (is_bind)
		{
			size_t win_width = GetSystemMetrics(SM_CXSCREEN);
			size_t win_height = GetSystemMetrics(SM_CYSCREEN);
			
			// ����ת
			float angle = float(delta_x) / win_width * FM_PI;
			
			BindYawAngle(angle);
			
			// ����
			angle = float(delta_y) / win_height* FM_PI;
			
			BindPitchAngle(angle);
		}
		else
		{
			size_t win_width = GetSystemMetrics(SM_CXSCREEN);
			size_t win_height = GetSystemMetrics(SM_CYSCREEN);
			
			// ����ת
			float angle = float(delta_x) / win_width * FM_PI;
			
			YawAngle(angle);
			
			// ����
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

	// �Ƿ�󶨶���
	PERSISTID bind_id = CHelper::QueryPropertyObject(pEntity, "Bind");
	
	bool is_bind = (pEntity->GetCore()->GetEntity(bind_id) != NULL);
	
	if (is_bind)
	{
		// ǰ�������
		int delta = short(HIWORD(param1));
		#define WHEEL_DELTA      120      // Default value for rolling one notch
		delta = delta / WHEEL_DELTA;
		
		float len = float(delta) * 0.1f * 5.0f; 

		// �ı�󶨾���
		float bind_distance = CHelper::QueryCustomFloat(pEntity, 
			"bind_distance");

		bind_distance -= len;

		CHelper::SetCustomFloat(pEntity, "bind_distance", bind_distance);
			
		ChangeBindPosition();
	}
	else
	{
		// ǰ�������
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