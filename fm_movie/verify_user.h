//--------------------------------------------------------------------
// �ļ���:		VerifyUser.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��9��30��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef __VERIFY_USER_H__
#define __VERIFY_USER_H__

#include "../public/module.h"
struct ldap;
// �û���֤
class VerifyUser: public IEntity
{
public:
	VerifyUser();
	virtual ~VerifyUser();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	bool Verify(const char* user_name, const char* password);
	int Search(ldap* mpConnection, char* FindDN, char* filter);

	// convert from Base64 to ANSI
	result_string Encode(const char* in_str);

	// convert from ANSI to Base64
	result_string Decode(const char* in_str);
};

#endif //__VERIFY_USER_H__
