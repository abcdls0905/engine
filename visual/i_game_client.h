//--------------------------------------------------------------------
// 文件名:		i_game_client.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_GAME_CLIENT_H
#define _VISUAL_I_GAME_CLIENT_H

#include "../public/module.h"

// 定义表示空间距离的数据类型
typedef float len_t;

// 游戏客户端接口

class IGameClient: public IEntity
{
public:
	// 主角玩家对象号
	virtual const char* GetPlayerIdent() const = 0;
	// 是否玩家对象
	virtual bool IsPlayer(const char* ident) const = 0;
	// 获得玩家对象
	virtual PERSISTID GetPlayer() const = 0;
	// 获得场景对象
	virtual PERSISTID GetScene() const = 0;
	// 获得场景内对象
	virtual PERSISTID GetSceneObj(const char* ident) const = 0;
	// 获得视窗对象
	virtual PERSISTID GetView(const char* view_ident) const = 0;
	// 获得视窗内对象
	virtual PERSISTID GetViewObj(const char* view_ident, 
		const char* item_ident) const = 0;
	// 获得视窗数量
	virtual size_t GetViewCount() const = 0;
	// 获得视窗列表
	virtual void GetViewList(const IVarList& args, IVarList& result) = 0;
};

// 游戏对象基类接口

class IGameObject: public IEntity
{
public:
	// 关联数据对象
	virtual PERSISTID GetDataID() const = 0;
	// 对象标识
	virtual const char* GetIdent() const = 0;
	// 查找属性
	virtual bool FindProp(const char* name) = 0;
	// 获得属性类型
	virtual int GetPropType(const char* name) = 0;
	// 获得属性名列表
	virtual void GetPropList(const IVarList& args, IVarList& result) = 0;
	// 获得属性值
	virtual void QueryProp(const IVarList& args, IVarList& result) = 0;
	virtual int QueryPropInt(const char* name) = 0;
	virtual int64_t QueryPropInt64(const char* name) = 0;
	virtual float QueryPropFloat(const char* name) = 0;
	virtual double QueryPropDouble(const char* name) = 0;
	virtual const char* QueryPropString(const char* name) = 0;
	virtual const wchar_t* QueryPropWideStr(const char* name) = 0;
	virtual PERSISTID QueryPropObject(const char* name) = 0;
	// 查找表格
	virtual bool FindRecord(const char* name) = 0;
	// 获得表格行数
	virtual int GetRecordRows(const char* name) = 0;
	// 获得表格列数
	virtual int GetRecordCols(const char* name) = 0;
	// 获得表格列类型
	virtual int GetRecordColType(const char* name, size_t index) = 0;
	// 获得表格名列表
	virtual void GetRecordList(const IVarList& args, IVarList& result) = 0;
	// 获得表格数值
	virtual void QueryRecord(const IVarList& args, IVarList& result) = 0;
	virtual int QueryRecordInt(const char* name, int row, int col) = 0;
	virtual int64_t QueryRecordInt64(const char* name, int row, int col) = 0;
	virtual float QueryRecordFloat(const char* name, int row, int col) = 0;
	virtual double QueryRecordDouble(const char* name, int row, int col) = 0;
	virtual const char* QueryRecordString(const char* name, int row, int col) = 0;
	virtual const wchar_t* QueryRecordWideStr(const char* name, int row, 
		int col) = 0;
	virtual PERSISTID QueryRecordObject(const char* name, int row, int col) = 0;
	// 查找表行
	virtual void FindRecordRow(const IVarList& args, IVarList& result) = 0;
	// 大小写不敏感查找表行
	virtual void FindRecordRowCI(const IVarList& args, IVarList& result) = 0;
	
	// 位置
	virtual len_t GetPosiX() const { return 0.0F; }
	virtual len_t GetPosiY() const { return 0.0F; }
	virtual len_t GetPosiZ() const { return 0.0F; }
	virtual len_t GetOrient() const { return 0.0F; }

	// 移动目标
	virtual len_t GetDestX() const { return 0.0F; }
	virtual len_t GetDestY() const { return 0.0F; }
	virtual len_t GetDestZ() const { return 0.0F; }
	virtual len_t GetDestOrient() const { return 0.0F; }
	virtual len_t GetMoveSpeed() const { return 0.0F; }
	virtual len_t GetRotateSpeed() const { return 0.0F; }
	virtual len_t GetJumpSpeed() const { return 0.0F; }
	virtual len_t GetGravity() const { return 0.0F; }
	// 移动模式
	virtual int GetMode() const { return 0; }
	virtual int GetDelay() const { return 0; }

	// 连接对象
	virtual const char* GetLinkIdent() const { return ""; }
	// 连接位置
	virtual len_t GetLinkX() const { return 0.0F; }
	virtual len_t GetLinkY() const { return 0.0F; }
	virtual len_t GetLinkZ() const { return 0.0F; }
	virtual len_t GetLinkOrient() const { return 0.0F; }
	
	// 获得场景中对象号
	virtual PERSISTID GetSceneObj(const char* object_ident) const { return PERSISTID(); }
	// 获得场景中对象数量
	virtual size_t GetSceneObjCount() const { return 0; }
    // 获得场景中对像通过序号
    virtual PERSISTID GetSceneObjByIndex(size_t index) const { return PERSISTID();}
	// 获得场景中对象列表
	virtual void GetSceneObjList(const IVarList& args, IVarList& result) {}
	
	// 获得视窗中物件对象号
	virtual PERSISTID GetViewObj(const char* item_ident) const { return PERSISTID(); }
	// 获得视窗中物件数量
	virtual size_t GetViewObjCount() const { return 0; }
	// 获得视窗中物体对象列表
	virtual void GetViewObjList(const IVarList& args, IVarList& result) {}
};

// 消息回调

class IGameMsgHandler: public IEntity
{
public:
	// 虚函数的缺省实现是为了可以定义IGameMsgHandler基类
	virtual bool Init(const IVarList& args) { return false; }
	virtual bool Shut() { return false; }

	// 收到验证码图像
	virtual int OnSetVerify(int width, int height, const char* code,
		const void* pdata) { return 0; }
	// 收到加密数据
	virtual int OnSetEncode() { return 0; }
	// 收到错误码
	virtual int OnErrorCode(int error_code) { return 0; }
	// 收到登录成功
	virtual int OnLoginSucceed(int is_free, int points, int year, int month,
		int day, int hour, int minute, int second, int roles) { return 0; }
	// 收到游戏世界信息
	virtual int OnWorldInfo(int info_type, const wchar_t* info) { return 0; }
	// 收到服务主动断线
	virtual int OnTerminate(int reason) { return 0; }
	// 收到可视属性列表
	virtual int OnPropertyTable(int property_num) { return 0; }
	// 收到可视表格列表
	virtual int OnRecordTable(int record_num) { return 0; }
	// 收到视野内聊天
	virtual int OnSpeech(const char* ident, 
		const wchar_t* content) { return 0; }
	// 收到系统信息
	virtual int OnSystemInfo(int type, const wchar_t* info) { return 0; }
	// 收到菜单消息
	virtual int OnMenu(const char* ident, int count) { return 0; }
	// 收到清除菜单
	virtual int OnClearMenu() { return 0; }
	// 收到自定义消息
	virtual int OnCustom(const IVarList& args) { return 0; }
	// 收到空闲提示
	virtual int OnIdle() { return 0; }
	// 收到警告消息
	virtual int OnWarning(int type, const wchar_t* info) { return 0; }
	// 收到客服信息
	virtual int OnFromGmcc(int gm_id, const wchar_t* gm_name, 
		const wchar_t* info) { return 0; }
	// 收到服务器列表信息
	virtual int OnServerInfo(int server_num) { return 0; }
	// 收到排队信息
	virtual int OnQueue(int queue, int position, int prior_count) { return 0; }

	// 进入场景
	virtual int OnEntryScene(const char* player_ident, 
		int scene_property_num) { return 0; }
	virtual int OnEntrySceneEx(const char* player_ident, 
		int scene_property_num, int scene_motion_mode, 
		int frame_time_delta,
		len_t move_line_diff_ratio,
		len_t move_rotate_diff_ratio) { return 0; }
	// 离开场景
	virtual int OnExitScene() { return 0; }
	// 添加场景中对象
	virtual int OnAddObject(const char* ident, int property_num) { return 0; }
	// 移除场景内对象之前
	virtual int OnBeforeRemoveObject(const char* ident) { return 0; }
	// 移除场景内对象
	virtual int OnRemoveObject(const char* ident) { return 0; }
	// 场景属性更新
	virtual int OnSceneProperty(int property_num) { return 0; }
	// 场景内对象属性更新
	virtual int OnObjectProperty(const char* ident, 
		int property_num ) { return 0; }
    // 场景内对对像，单个属性改变
    virtual int OnObjectPropertyChange(const char* ident, 
        const char * prop_name){return 0;}
	// 视窗内对象属性更新
	virtual int OnViewObjProperty(const char* view_ident, 
		const char* item_ident, int property_num) { return 0; }
	// 创建视窗
	virtual int OnCreateView(const char* view_ident, int capacity, 
		int view_property_num) { return 0; }
	// 删除视窗
	virtual int OnDeleteView(const char* view_ident) { return 0; }
	// 视窗属性更新
	virtual int OnViewProperty(const char* view_ident, 
		int property_num) { return 0; }
	// 视窗内添加对象 
	virtual int OnViewAdd(const char* view_ident, const char* item_ident,
		int property_num) { return 0; }
	// 视窗内移除对象
	virtual int OnViewRemove(const char* view_ident, 
		const char* item_ident) { return 0; }
	// 视窗内对象发生移动
	virtual int OnViewChange(const char* view_ident, 
		const char* old_item_ident, const char* new_item_ident) { return 0; }
	// 场景内对象表格添加行
	virtual int OnRecordAddRow(const char* ident, const char* record_name,
		int start_row, int row_num) { return 0; }
	// 视窗对象表格添加行
	virtual int OnViewRecordAddRow(const char* view_ident, 
		const char* record_name, int start_row, int row_num) { return 0; }
	// 视窗内对象表格添加行
	virtual int OnViewObjRecordAddRow(const char* view_ident, 
		const char* item_ident, const char* record_name, int start_row, 
		int row_num) { return 0; }
	// 场景表格添加行
	virtual int OnSceneRecordAddRow(const char* record_name, int start_row, 
		int row_num) { return 0; }
	// 场景内对象表格移除行之前
	virtual int OnRecordBeforeRemoveRow(const char* ident, 
		const char* record_name, int row) { return 0; }
	// 场景内对象表格移除行
	virtual int OnRecordRemoveRow(const char* ident, 
		const char* record_name, int row) { return 0; }
	// 视窗对象表格移除行之前
	virtual int OnViewRecordBeforeRemoveRow(const char* view_ident, 
		const char* record_name, int row) { return 0; }
	// 视窗对象表格移除行
	virtual int OnViewRecordRemoveRow(const char* view_ident, 
		const char* record_name, int row) { return 0; }
	// 视窗内对象表格移除行之前
	virtual int OnViewObjRecordBeforeRemoveRow(const char* view_ident, 
		const char* item_ident, const char* record_name, int row) { return 0; }
	// 视窗内对象表格移除行
	virtual int OnViewObjRecordRemoveRow(const char* view_ident, 
		const char* item_ident, const char* record_name, int row) { return 0; }
	// 场景表格移除行之前
	virtual int OnSceneRecordBeforeRemoveRow(const char* record_name, 
		int row) { return 0; }
	// 场景内对象表格移除行
	virtual int OnSceneRecordRemoveRow(const char* record_name, 
		int row) { return 0; }
	// 场景内对象表格数据更新
	virtual int OnRecordGrid(const char* ident, 
		const char* record_name, int grid_num) { return 0; }
	// 视窗对象表格数据更新
	virtual int OnViewRecordGrid(const char* view_ident, 
		const char* record_name, int grid_num) { return 0; }
	// 视窗内对象表格数据更新
	virtual int OnViewObjRecordGrid(const char* view_ident, 
		const char* item_ident, const char* record_name, 
		int grid_num) { return 0; }
	// 场景表格数据更新
	virtual int OnSceneRecordGrid(const char* record_name, 
		int grid_num) { return 0; }
	// 场景内对象表格清空
	virtual int OnRecordClear(const char* ident, 
		const char* record_name) { return 0; }
	// 视窗对象表格清空
	virtual int OnViewRecordClear(const char* view_ident, 
		const char* record_name) { return 0; }
	// 视窗内对象表格清空
	virtual int OnViewObjRecordClear(const char* view_ident, 
		const char* item_ident, const char* record_name) { return 0; }
	// 场景表格清空
	virtual int OnSceneRecordClear(const char* record_name) { return 0; }
	// 场景内对象表格数据更新
	virtual int OnRecordSingleGrid(const char* ident, 
		const char* record_name, int row, int col) { return 0; }
	// 视窗对象表格数据更新
	virtual int OnViewRecordSingleGrid(const char* view_ident, 
		const char* record_name, int row, int col) { return 0; }
	// 视窗内对象表格数据更新
	virtual int OnViewObjRecordSingleGrid(const char* view_ident, 
		const char* item_ident, const char* record_name, int row, 
		int col) { return 0; }
	// 场景表格数据更新
	virtual int OnSceneRecordSingleGrid(const char* record_name, int row, 
		int col) { return 0; }
	// 对象位置
	virtual int OnLocation(const char* ident) { return 0; }
	// 对象移动
	virtual int OnMoving(const char* ident) { return 0; }
	// 多个对象的移动目标
	virtual int OnAllDestination(int object_num) { return 0; }
	// 连接到对象
	virtual int OnLinkTo(const char* ident, const char* link_ident,
		len_t link_x, len_t link_y, len_t link_z, len_t link_orient) { return 0; }
	// 连接位置改变
	virtual int OnLinkMove(const char* ident, const char* link_ident,
		len_t link_x, len_t link_y, len_t link_z, len_t link_orient) { return 0; }
	// 解除连接
	virtual int OnUnlink(const char* ident) { return 0; }
	// 多个对象的属性变化
	virtual int OnAllProperty(int object_num) { return 0; }
	// 增加多个对象
	virtual int OnAddMoreObject(int object_num) { return 0; }
	// 移除多个对象
	virtual int OnRemoveMoreObject(int object_num) { return 0; }
	// 给客户端发送计费验证串
	virtual int OnServerChargeValidstring(const char* valid_string) {return 0;}
	// 服务器延迟度量
	virtual int OnServerMeasureDelay(size_t index, size_t server_time) { return 0; }
	// 对象运动差值--基于逻辑帧
	virtual int OnServerFrameMovingDiff(const char* ident, float diff_x, float diff_y,
		float diff_z, float diff_orient) { return 0;}
	// 服务器帧驱动
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
