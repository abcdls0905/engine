
#ifndef _PUBLIC_VAR_TYPE_H
#define _PUBLIC_VAR_TYPE_H

// ��������

enum VARIANT_TYPE_ENUM
{
	VTYPE_UNKNOWN,	// δ֪
	VTYPE_BOOL,		// ����
	VTYPE_INT,		// 32λ����
	VTYPE_INT64,	// 64λ����
	VTYPE_FLOAT,	// �����ȸ�����
	VTYPE_DOUBLE,	// ˫���ȸ�����
	VTYPE_STRING,	// �ַ���
	VTYPE_WIDESTR,	// ���ַ���
	VTYPE_OBJECT,	// �����
	VTYPE_POINTER,	// ָ��
	VTYPE_USERDATA,	// �û�����
    VTYPE_TABLE,	// ��
	VTYPE_MAX,
};

#endif // _PUBLIC_VAR_TYPE_H
