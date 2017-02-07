//--------------------------------------------------------------------
// �ļ���:		outer_err.h
// ��  ��:		ͨ�÷��������棬�ͻ��˺ͷ�����ͨѶ�Ĵ������
// ˵  ��:		
// ��������:	2008��9��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SERVER_OUTER_ERR_H
#define _SERVER_OUTER_ERR_H

// ��������Ϣ��ʽ [5λ������]
// ��1λ����ϵͳ����
// ��2λ����ģ�����
// ��3λ������(0-�洢���̡�1-�������2-����ͨ��)
// ����λΪ˳��ű���
// 1000Ϊ����Ԥ֪�Ĵ���

#define OUTER_RESULT_SUCCEED		1		// ����ɹ�
#define OUTER_RESULT_FAILED			0		// ���ʧ��

// 1000Ϊ����Ԥ֪�Ĵ���
#define OUTER_ERR_UNEXPECT			1000	// ��Ԥ�ڴ���

// ��Ϸ��������ش���������[2xxxx]

// ����ӿ���ش���[20xxx]

#define MGS_ERR_VERSION				20101	// ����ͨѶЭ��汾����
#define MGS_ERR_REPEAT_LOGIN		20102	// ͬһ���ʺŲ����ظ���½
#define MGS_ERR_LAND_SCENE			20103	// Ҫ����ĳ���δ���Ż��ѹر�
#define MGS_ERR_NO_CONNECT			20104	// ��������
#define MGS_ERR_BLOCK_MOMENT		20105	// ��һ��ʱ���ڽ�ֹ��½
#define MGS_ERR_TIME_OUT			20106	// ��¼��ʱ
#define MGS_ERR_QUEUE_FULL			20107	// ��¼������
#define MGS_ERR_STORAGE_FAILED		20108	// ���������ɫ���ݿ�����ӹ���
#define MGS_ERR_CHAREGE_FAILED		20109	// ��������Ʒѷ����������ӹ���
#define MGS_ERR_SWITCH_SCENE		20110	// Ҫ�л��ĳ���������
#define MGS_ERR_ACCOUNT_LOGGED		20111	// ���ʺ��Ѿ���½����Ϸ������
#define MGS_ERR_ACCOUNT_INVALID		20112	// �ʺ������а����Ƿ��ַ�
#define MGS_ERR_CHARGE_BREAK		20113	// ����ò����Ѿ���ϵͳ�߳�
#define MGS_ERR_PLAYER_FULL			20114	// ���������Ѿ��ﵽ����������
#define MGS_ERR_NO_ANY_SCENE		20115	// û�п�ʹ�õĳ���
#define MGS_ERR_KICK_OUT			20116	// ���������������
#define MGS_ERR_IN_PROCESSING		20117	// ���ڽ��д�����
#define MGS_ERR_CANT_PROCESS		20118	// �޷���������
#define MGS_ERR_MESSAGE_ILLEGAL		20119	// ����ʹ�����׳�������������ͷǷ���Ϣ
#define MGS_ERR_ACCOUNT_IN_GAME		20120	// ���ʺ��Ѿ�������Ϸ
#define MGS_ERR_CANT_REPLACE		20121	// ʹ���ܱ�����¼�Ĳ��ܱ�������ͬIP����
#define MGS_ERR_SUPERVISOR_KICK_OUT	20122	// ������Ա������
#define MGS_ERR_SUPERVISOR_LOGGED	20123	// ��¼����Ϊ����Ա�����ܱ�������
#define MGS_ERR_DENY_ENTRY_SCENE	20124	// ��ǰ��������������볡��

// �洢�ӿ���ش���[21xxx]

#define MGS_ERR_ROLE_DUPLICATE		21101	// ��������
#define MGS_ERR_ROLE_PROTECT		21102	// �����ڱ������ڣ�����ɾ��
#define MGS_ERR_ROLE_SAVE_FAIL		21103	// �������ʧ��
#define MGS_ERR_ROLE_DATA			21104	// �������ݴ���
#define MGS_ERR_ROLE_TOO_MANY		21105	// �����ܵ�������������
#define MGS_ERR_ROLE_INDEX			21106	// ����λ�ô���
#define MGS_ERR_ROLE_NOT_FOUND		21107	// ����û�ҵ�
#define MGS_ERR_NAME_REPEAT			21108	// �����ظ�
#define MGS_ERR_NAME_INVALID		21109	// ���ְ����Ƿ��ַ�
#define MGS_ERR_NAME_EXISTS			21110	// �������Ѵ���
#define MGS_ERR_STORAGE_BUSY		21111	// ���ݿ�æ���޷���������
#define MGS_ERR_CHARGE_BUSY			21112	// �Ʒ�ϵͳæ���޷���������
#define MGS_ERR_LETTER_TOO_MANY		21113	// �ż�������������
#define MGS_ERR_NO_ROLE_UID			21114	// ��������ʱ�����Դ����
#define MGS_ERR_SWITCH_NO_SERVER	21115	// ����ת��ʱ������������
#define MGS_ERR_SWITCH_CAN_NOT		21116	// �˷���������ִ������ת��
#define MGS_ERR_ROLE_TOO_BIG		21117	// ����������ݳ������������
#define MGS_ERR_STATUS_CHANGED		21118	// ת��ʱĿ���������״̬�����ı�
#define MGS_ERR_SET_LAND_SCENE		21119	// �½�����ʱδ���õ�½����
#define MGS_ERR_NEW_ROLE_FAILED		21120	// �½�����ʱ��������ʧ��
#define MGS_ERR_ROLE_BLOCKED		21121	// ����������һ��ʱ���ڱ����
#define MGS_ERR_LOGON_FAILED		21122	// �Ʒѵ�½ʧ��
#define MGS_ERR_LOAD_ROLE_FAILED	21123	// ������������ʧ��
#define MGS_ERR_GET_ROLES_FAILED	21124	// ��ȡ��ɫ�б�ʧ��
#define MGS_ERR_CREATE_ROLE_FAILED	21125	// ������������ʧ��
#define MGS_ERR_DELETE_ROLE_FAILED	21126	// ɾ����������ʧ��
#define MGS_ERR_RECEIVER_NOT_FOUND	21127	// �����˲�����
#define MGS_ERR_ROLE_IN_USE			21128	// ��ɫ����ʹ����
#define MGS_ERR_VALIDATE_FAILED		21129	// ��֤�ʺ�ʧ��
#define MGS_ERR_CONTINUE_FAILED		21130	// �����¼ʧ��
#define MGS_ERR_ROLE_LOCKED			21131	// ��ɫ����ʱ����
#define MGS_ERR_DENY_CREATE_ROLE	21132	// ��ֹ����������

// �Ʒ�Ӧ����ش���������[5xxxx]

// ������ע�ᡢע�� 50xxx
#define CAS_ERR_SERVER_ID			50001	// ��Ϸ������ID����ʶ��
#define CAS_ERR_SERVER_FORBID		50002	// ��Ϸ������״̬Ϊ�ѽ���
#define CAS_ERR_SERVER_IP			50003	// ��Ϸ������ע��IP����ȷ
#define CAS_ERR_GAME_ID				50004	// ��Ϸ����ID����ȷ

// ����ʺŵ�¼���۵㡢�˳� 51xxx
#define CAS_ERR_NO_ACCOUNT			51001	// �ʺŲ�����
#define CAS_ERR_ACCOUNT_PSWD		51002	// �ʺŻ��������
#define CAS_ERR_ACCOUNT_BLOCK		51003	// ���ʺ��ѱ�����
#define CAS_ERR_ACCOUNT_ONLY		51004	// ʵ���ʺ�ֻ�ܵ�¼��ɫ��
#define CAS_ERR_ACCOUNT_DENIED		51005	// ��ͨ�ʺŲ��ܵ�¼��ɫ��
#define	CAS_ERR_ILLEGAL_GM			51006	// �Ƿ�GM�ʺŵ�¼
#define CAS_ERR_UNACTIVE			51007	// �˷����ʻ���δ����
#define	CAS_ERR_EXPIRED				51008	// ������Ϸ�ʻ�����
#define CAS_ERR_SAVE_LOG			51009	// ������¼��־ʧ��
#define CAS_ERR_GET_ITEM			51010	// ��ȡ��Ʒʧ�ܣ���Ʒ�����ڻ���֮ǰ��ȡ��
#define CAS_ERR_ACCOUNT_LOGGED		51011	// ���ʺ����ڷ�����������������¼
#define CAS_ERR_ACCOUNT_LOCKED		51012	// �ʺ�Ϊ����״̬����ͨ���ֻ�����������
#define CAS_ERR_ACCOUNT_FORBID		51013	// �����ʺŲ����Ե�¼����Ϸ������(�ʺ�����Ӫ�̲�ƥ��),�뵽��Ա���Ĳ鿴�Լ��ܵ�¼��Щ��Ϸ������.

// �㿨���ۡ����򡢵��߹��� 52xxx
#define CAS_ERR_CARD_ACCOUNT		52000	// δ֪�ʺ�����
#define CAS_ERR_CARD_PSWD			52001	// ���Ż����벻��ȷ
#define CAS_ERR_CARD_INVALID		52002	// ��Ч�㿨���ѱ�ʹ��
#define CAS_ERR_CARD_BLOCK			52003	// �㿨Ŀǰ���ڼ�����
#define CAS_ERR_CARD_DISCARD		52004	// ��Ч�㿨���ѱ�����
#define CAS_ERR_NO_POINTS			52005	// �����ʻ�����
#define CAS_ERR_CARD_FILL			52006	// ����㿨��ֵʱ����
#define CAS_ERR_CARD_TYPE			52007	// �����Ͳ���ʶ�𣬲��������
#define CAS_ERR_CARD_SELLED			52008	// ����ʧ��,�㿨֮ǰ�Ѿ��۳�
#define CAS_ERR_CARD_CANCEL			52009	// ����ʧ��,�㿨�����ѳ�������
#define CAS_ERR_CARD_SELL_CANCEL	52010	// �㿨����֮ǰ���۳����ѳ���
#define CAS_ERR_CARD_CANT_CANCEL	52011	// �㿨������Сʱ�ڲ��ɳ���
#define CAS_ERR_ITEM_NOT_FOUND		52012	// ������߱��벻����
#define CAS_ERR_ITEM_AMOUNT_PRICE	52013	// ���������򵥼�����
#define CAS_ERR_ITEM_PRICE_CHANGE	52014	// ��Ʒ�������۷����仯,��ˢ��

#endif // _SERVER_OUTER_ERR_H

