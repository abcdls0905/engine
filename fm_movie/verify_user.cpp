//--------------------------------------------------------------------
// 文件名:		VerifyUser.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年9月30日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "verify_user.h"
#include <windows.h>
#include <winldap.h>
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "base_64_helper.h"
#include <string>

#pragma comment(lib, "Wldap32.Lib")

/// \file VerifyUser.cpp
/// \brief 用户验证

/// entity: VerifyUser
/// \brief 用户验证类
DECLARE_ENTITY(VerifyUser, 0, IEntity);

VerifyUser::VerifyUser()
{
}

VerifyUser::~VerifyUser()
{
}

bool VerifyUser::Init(const IVarList& args)
{
	return true;
}

bool VerifyUser::Shut()
{
	return true;
}

result_string VerifyUser::Encode(const char* in_str)
{
	return Base64Helper::Encode(in_str);
}

result_string VerifyUser::Decode(const char* in_str)
{
	return Base64Helper::Decode(in_str);
}

int VerifyUser::Search(ldap* mpConnection, char* FindDN, char* filter)
{

	LDAPMessage* pSearchResult = NULL,*pEntry = NULL;

	ULONG errorCode = ldap_search_s(
		mpConnection,    // Session handle
		FindDN,              // DN to start search
		LDAP_SCOPE_SUBTREE, // Scope
		filter,          // Filter
		NULL,      // Retrieve list of attributes
		0,                  // Get both attributes and values
		&pSearchResult);    // [out] Search results

	ULONG numberOfEntries = 0;
	if(errorCode == LDAP_SUCCESS)
	{
		numberOfEntries = 1;
		//numberOfEntries = ldap_count_entries(
		//	mpConnection,    // Session handle
		//	pSearchResult);     // Search result

	}
	return numberOfEntries;
}

bool VerifyUser::Verify(const char* user_name, const char* password)
{
	ldap* pLDAP = NULL;
	bool  bRet = false;

	pLDAP = ldap_open("snail.com", LDAP_PORT);
	if (NULL == pLDAP)
	{
		ldap_unbind(pLDAP);
		return bRet;
	}

	result_string str_user = Decode(user_name);
	result_string str_password = Decode(password);
	result_string str_filter = result_string() + "(&(objectClass=dominoPerson)(|(cn=" + user_name \
			+ ")(uid=" + user_name + ")(mail=" + user_name + ")))";

	ULONG ret = ldap_simple_bind_s(pLDAP, (char*)str_user.c_str(), (char*)str_password.c_str());
	if(ret == LDAP_SUCCESS) 	
	{
		if (Search(pLDAP, "ou=snail,dc=snail,dc=com", (char*)str_filter.c_str()) > 0)
		{
			bRet = true;
		}
	}

	ldap_unbind(pLDAP); 
	return bRet;
}
