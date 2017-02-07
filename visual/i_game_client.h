//--------------------------------------------------------------------
// �ļ���:		i_game_client.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_GAME_CLIENT_H
#define _VISUAL_I_GAME_CLIENT_H

#include "../public/module.h"

// �����ʾ�ռ�������������
typedef float len_t;

// ��Ϸ�ͻ��˽ӿ�

class IGameClient: public IEntity
{
public:
	// ������Ҷ����
	virtual const char* GetPlayerIdent() const = 0;
	// �Ƿ���Ҷ���
	virtual bool IsPlayer(const char* ident) const = 0;
	// �����Ҷ���
	virtual PERSISTID GetPlayer() const = 0;
	// ��ó�������
	virtual PERSISTID GetScene() const = 0;
	// ��ó����ڶ���
	virtual PERSISTID GetSceneObj(const char* ident) const = 0;
	// ����Ӵ�����
	virtual PERSISTID GetView(const char* view_ident) const = 0;
	// ����Ӵ��ڶ���
	virtual PERSISTID GetViewObj(const char* view_ident, 
		const char* item_ident) const = 0;
	// ����Ӵ�����
	virtual size_t GetViewCount() const = 0;
	// ����Ӵ��б�
	virtual void GetViewList(const IVarList& args, IVarList& result) = 0;
};

// ��Ϸ�������ӿ�

class IGameObject: public IEntity
{
public:
	// �������ݶ���
	virtual PERSISTID GetDataID() const = 0;
	// �����ʶ
	virtual const char* GetIdent() const = 0;
	// ��������
	virtual bool FindProp(const char* name) = 0;
	// �����������
	virtual int GetPropType(const char* name) = 0;
	// ����������б�
	virtual void GetPropList(const IVarList& args, IVarList& result) = 0;
	// �������ֵ
	virtual void QueryProp(const IVarList& args, IVarList& result) = 0;
	virtual int QueryPropInt(const char* name) = 0;
	virtual int64_t QueryPropInt64(const char* name) = 0;
	virtual float QueryPropFloat(const char* name) = 0;
	virtual double QueryPropDouble(const char* name) = 0;
	virtual const char* QueryPropString(const char* name) = 0;
	virtual const wchar_t* QueryPropWideStr(const char* name) = 0;
	virtual PERSISTID QueryPropObject(const char* name) = 0;
	// ���ұ��
	virtual bool FindRecord(const char* name) = 0;
	// ��ñ������
	virtual int GetRecordRows(const char* name) = 0;
	// ��ñ������
	virtual int GetRecordCols(const char* name) = 0;
	// ��ñ��������
	virtual int GetRecordColType(const char* name, size_t index) = 0;
	// ��ñ�����б�
	virtual void GetRecordList(const IVarList& args, IVarList& result) = 0;
	// ��ñ����ֵ
	virtual void QueryRecord(const IVarList& args, IVarList& result) = 0;
	virtual int QueryRecordInt(const char* name, int row, int col) = 0;
	virtual int64_t QueryRecordInt64(const char* name, int row, int col) = 0;
	virtual float QueryRecordFloat(const char* name, int row, int col) = 0;
	virtual double QueryRecordDouble(const char* name, int row, int col) = 0;
	virtual const char* QueryRecordString(const char* name, int row, int col) = 0;
	virtual const wchar_t* QueryRecordWideStr(const char* name, int row, 
		int col) = 0;
	virtual PERSISTID QueryRecordObject(const char* name, int row, int col) = 0;
	// ���ұ���
	virtual void FindRecordRow(const IVarList& args, IVarList& result) = 0;
	// ��Сд�����в��ұ���
	virtual void FindRecordRowCI(const IVarList& args, IVarList& result) = 0;
	
	// λ��
	virtual len_t GetPosiX() const { return 0.0F; }
	virtual len_t GetPosiY() const { return 0.0F; }
	virtual len_t GetPosiZ() const { return 0.0F; }
	virtual len_t GetOrient() const { return 0.0F; }

	// �ƶ�Ŀ��
	virtual len_t GetDestX() const { return 0.0F; }
	virtual len_t GetDestY() const { return 0.0F; }
	virtual len_t GetDestZ() const { return 0.0F; }
	virtual len_t GetDestOrient() const { return 0.0F; }
	virtual len_t GetMoveSpeed() const { return 0.0F; }
	virtual len_t GetRotateSpeed() const { return 0.0F; }
	virtual len_t GetJumpSpeed() const { return 0.0F; }
	virtual len_t GetGravity() const { return 0.0F; }
	// �ƶ�ģʽ
	virtual int GetMode() const { return 0; }
	virtual int GetDelay() const { return 0; }

	// ���Ӷ���
	virtual const char* GetLinkIdent() const { return ""; }
	// ����λ��
	virtual len_t GetLinkX() const { return 0.0F; }
	virtual len_t GetLinkY() const { return 0.0F; }
	virtual len_t GetLinkZ() const { return 0.0F; }
	virtual len_t GetLinkOrient() const { return 0.0F; }
	
	// ��ó����ж����
	virtual PERSISTID GetSceneObj(const char* object_ident) const { return PERSISTID(); }
	// ��ó����ж�������
	virtual size_t GetSceneObjCount() const { return 0; }
    // ��ó����ж���ͨ�����
    virtual PERSISTID GetSceneObjByIndex(size_t index) const { return PERSISTID();}
	// ��ó����ж����б�
	virtual void GetSceneObjList(const IVarList& args, IVarList& result) {}
	
	// ����Ӵ�����������
	virtual PERSISTID GetViewObj(const char* item_ident) const { return PERSISTID(); }
	// ����Ӵ����������
	virtual size_t GetViewObjCount() const { return 0; }
	// ����Ӵ�����������б�
	virtual void GetViewObjList(const IVarList& args, IVarList& result) {}
};

// ��Ϣ�ص�

class IGameMsgHandler: public IEntity
{
public:
	// �麯����ȱʡʵ����Ϊ�˿��Զ���IGameMsgHandler����
	virtual bool Init(const IVarList& args) { return false; }
	virtual bool Shut() { return false; }

	// �յ���֤��ͼ��
	virtual int OnSetVerify(int width, int height, const char* code,
		const void* pdata) { return 0; }
	// �յ���������
	virtual int OnSetEncode() { return 0; }
	// �յ�������
	virtual int OnErrorCode(int error_code) { return 0; }
	// �յ���¼�ɹ�
	virtual int OnLoginSucceed(int is_free, int points, int year, int month,
		int day, int hour, int minute, int second, int roles) { return 0; }
	// �յ���Ϸ������Ϣ
	virtual int OnWorldInfo(int info_type, const wchar_t* info) { return 0; }
	// �յ�������������
	virtual int OnTerminate(int reason) { return 0; }
	// �յ����������б�
	virtual int OnPropertyTable(int property_num) { return 0; }
	// �յ����ӱ���б�
	virtual int OnRecordTable(int record_num) { return 0; }
	// �յ���Ұ������
	virtual int OnSpeech(const char* ident, 
		const wchar_t* content) { return 0; }
	// �յ�ϵͳ��Ϣ
	virtual int OnSystemInfo(int type, const wchar_t* info) { return 0; }
	// �յ��˵���Ϣ
	virtual int OnMenu(const char* ident, int count) { return 0; }
	// �յ�����˵�
	virtual int OnClearMenu() { return 0; }
	// �յ��Զ�����Ϣ
	virtual int OnCustom(const IVarList& args) { return 0; }
	// �յ�������ʾ
	virtual int OnIdle() { return 0; }
	// �յ�������Ϣ
	virtual int OnWarning(int type, const wchar_t* info) { return 0; }
	// �յ��ͷ���Ϣ
	virtual int OnFromGmcc(int gm_id, const wchar_t* gm_name, 
		const wchar_t* info) { return 0; }
	// �յ��������б���Ϣ
	virtual int OnServerInfo(int server_num) { return 0; }
	// �յ��Ŷ���Ϣ
	virtual int OnQueue(int queue, int position, int prior_count) { return 0; }

	// ���볡��
	virtual int OnEntryScene(const char* player_ident, 
		int scene_property_num) { return 0; }
	virtual int OnEntrySceneEx(const char* player_ident, 
		int scene_property_num, int scene_motion_mode, 
		int frame_time_delta,
		len_t move_line_diff_ratio,
		len_t move_rotate_diff_ratio) { return 0; }
	// �뿪����
	virtual int OnExitScene() { return 0; }
	// ��ӳ����ж���
	virtual int OnAddObject(const char* ident, int property_num) { return 0; }
	// �Ƴ������ڶ���֮ǰ
	virtual int OnBeforeRemoveObject(const char* ident) { return 0; }
	// �Ƴ������ڶ���
	virtual int OnRemoveObject(const char* ident) { return 0; }
	// �������Ը���
	virtual int OnSceneProperty(int property_num) { return 0; }
	// �����ڶ������Ը���
	virtual int OnObjectProperty(const char* ident, 
		int property_num ) { return 0; }
    // �����ڶԶ��񣬵������Ըı�
    virtual int OnObjectPropertyChange(const char* ident, 
        const char * prop_name){return 0;}
	// �Ӵ��ڶ������Ը���
	virtual int OnViewObjProperty(const char* view_ident, 
		const char* item_ident, int property_num) { return 0; }
	// �����Ӵ�
	virtual int OnCreateView(const char* view_ident, int capacity, 
		int view_property_num) { return 0; }
	// ɾ���Ӵ�
	virtual int OnDeleteView(const char* view_ident) { return 0; }
	// �Ӵ����Ը���
	virtual int OnViewProperty(const char* view_ident, 
		int property_num) { return 0; }
	// �Ӵ�����Ӷ��� 
	virtual int OnViewAdd(const char* view_ident, const char* item_ident,
		int property_num) { return 0; }
	// �Ӵ����Ƴ�����
	virtual int OnViewRemove(const char* view_ident, 
		const char* item_ident) { return 0; }
	// �Ӵ��ڶ������ƶ�
	virtual int OnViewChange(const char* view_ident, 
		const char* old_item_ident, const char* new_item_ident) { return 0; }
	// �����ڶ����������
	virtual int OnRecordAddRow(const char* ident, const char* record_name,
		int start_row, int row_num) { return 0; }
	// �Ӵ������������
	virtual int OnViewRecordAddRow(const char* view_ident, 
		const char* record_name, int start_row, int row_num) { return 0; }
	// �Ӵ��ڶ����������
	virtual int OnViewObjRecordAddRow(const char* view_ident, 
		const char* item_ident, const char* record_name, int start_row, 
		int row_num) { return 0; }
	// ������������
	virtual int OnSceneRecordAddRow(const char* record_name, int start_row, 
		int row_num) { return 0; }
	// �����ڶ������Ƴ���֮ǰ
	virtual int OnRecordBeforeRemoveRow(const char* ident, 
		const char* record_name, int row) { return 0; }
	// �����ڶ������Ƴ���
	virtual int OnRecordRemoveRow(const char* ident, 
		const char* record_name, int row) { return 0; }
	// �Ӵ��������Ƴ���֮ǰ
	virtual int OnViewRecordBeforeRemoveRow(const char* view_ident, 
		const char* record_name, int row) { return 0; }
	// �Ӵ��������Ƴ���
	virtual int OnViewRecordRemoveRow(const char* view_ident, 
		const char* record_name, int row) { return 0; }
	// �Ӵ��ڶ������Ƴ���֮ǰ
	virtual int OnViewObjRecordBeforeRemoveRow(const char* view_ident, 
		const char* item_ident, const char* record_name, int row) { return 0; }
	// �Ӵ��ڶ������Ƴ���
	virtual int OnViewObjRecordRemoveRow(const char* view_ident, 
		const char* item_ident, const char* record_name, int row) { return 0; }
	// ��������Ƴ���֮ǰ
	virtual int OnSceneRecordBeforeRemoveRow(const char* record_name, 
		int row) { return 0; }
	// �����ڶ������Ƴ���
	virtual int OnSceneRecordRemoveRow(const char* record_name, 
		int row) { return 0; }
	// �����ڶ��������ݸ���
	virtual int OnRecordGrid(const char* ident, 
		const char* record_name, int grid_num) { return 0; }
	// �Ӵ����������ݸ���
	virtual int OnViewRecordGrid(const char* view_ident, 
		const char* record_name, int grid_num) { return 0; }
	// �Ӵ��ڶ��������ݸ���
	virtual int OnViewObjRecordGrid(const char* view_ident, 
		const char* item_ident, const char* record_name, 
		int grid_num) { return 0; }
	// ����������ݸ���
	virtual int OnSceneRecordGrid(const char* record_name, 
		int grid_num) { return 0; }
	// �����ڶ��������
	virtual int OnRecordClear(const char* ident, 
		const char* record_name) { return 0; }
	// �Ӵ����������
	virtual int OnViewRecordClear(const char* view_ident, 
		const char* record_name) { return 0; }
	// �Ӵ��ڶ��������
	virtual int OnViewObjRecordClear(const char* view_ident, 
		const char* item_ident, const char* record_name) { return 0; }
	// ����������
	virtual int OnSceneRecordClear(const char* record_name) { return 0; }
	// �����ڶ��������ݸ���
	virtual int OnRecordSingleGrid(const char* ident, 
		const char* record_name, int row, int col) { return 0; }
	// �Ӵ����������ݸ���
	virtual int OnViewRecordSingleGrid(const char* view_ident, 
		const char* record_name, int row, int col) { return 0; }
	// �Ӵ��ڶ��������ݸ���
	virtual int OnViewObjRecordSingleGrid(const char* view_ident, 
		const char* item_ident, const char* record_name, int row, 
		int col) { return 0; }
	// ����������ݸ���
	virtual int OnSceneRecordSingleGrid(const char* record_name, int row, 
		int col) { return 0; }
	// ����λ��
	virtual int OnLocation(const char* ident) { return 0; }
	// �����ƶ�
	virtual int OnMoving(const char* ident) { return 0; }
	// ���������ƶ�Ŀ��
	virtual int OnAllDestination(int object_num) { return 0; }
	// ���ӵ�����
	virtual int OnLinkTo(const char* ident, const char* link_ident,
		len_t link_x, len_t link_y, len_t link_z, len_t link_orient) { return 0; }
	// ����λ�øı�
	virtual int OnLinkMove(const char* ident, const char* link_ident,
		len_t link_x, len_t link_y, len_t link_z, len_t link_orient) { return 0; }
	// �������
	virtual int OnUnlink(const char* ident) { return 0; }
	// �����������Ա仯
	virtual int OnAllProperty(int object_num) { return 0; }
	// ���Ӷ������
	virtual int OnAddMoreObject(int object_num) { return 0; }
	// �Ƴ��������
	virtual int OnRemoveMoreObject(int object_num) { return 0; }
	// ���ͻ��˷��ͼƷ���֤��
	virtual int OnServerChargeValidstring(const char* valid_string) {return 0;}
	// �������ӳٶ���
	virtual int OnServerMeasureDelay(size_t index, size_t server_time) { return 0; }
	// �����˶���ֵ--�����߼�֡
	virtual int OnServerFrameMovingDiff(const char* ident, float diff_x, float diff_y,
		float diff_z, float diff_orient) { return 0;}
	// ������֡����
	virtual int OnServerFrameUpdate(const char* ident, unsigned int frame_id){ return 0; }
	
};

class ISockMsgHandler : public IEntity
{
public:
	virtual void OnConnect() = 0;
	virtual void OnConnectFail() = 0;
	virtual void OnClose() = 0;
};

#endif // _VISUAL_I_GAME_CLIENT_H
