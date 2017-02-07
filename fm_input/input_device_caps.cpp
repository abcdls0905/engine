#include "input_device_caps.h"

DECLARE_ENTITY(CInputDeviceCaps,0 , IEntity);


CInputDeviceCaps::CInputDeviceCaps()
{
}


CInputDeviceCaps::~CInputDeviceCaps()
{
}

bool CInputDeviceCaps::Init(const IVarList& args)
{
	return true;
}

bool CInputDeviceCaps::Shut()
{
	return true;
}

bool CInputDeviceCaps::IsSupported(INPUT_DEVICE_TYPE device)
{
	return m_bDeviceSupported[device];
}

void CInputDeviceCaps::ScanDevice()
{
}