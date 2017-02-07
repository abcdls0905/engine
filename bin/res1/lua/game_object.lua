--游戏网络对象和可见对象管理脚本

--获得场景网络对象
function get_client_scene()
	local client = nx_value("game_client_lua")

	return client:GetScene()
end

--是否主角玩家网络对象
function is_client_player(ident)
	local client = nx_value("game_client_lua")

	return client:IsPlayer(ident)
end

--获得主角玩家网络对象
function get_client_player()
	local client = nx_value("game_client_lua")

	return client:GetPlayer()
end

--获得场景中网络对象
function get_client_scene_obj(ident)
	local client = nx_value("game_client_lua")

	return client:GetSceneObj(ident)
end

--获得容器视窗网络对象
function get_client_view(view_ident)
	local client = nx_value("game_client_lua")

	return client:GetView(view_ident)
end

--获得容器视窗中网络对象
function get_client_view_obj(view_ident, item_ident)
	local client = nx_value("game_client_lua")

	return client:GetViewObj(view_ident, item_ident)
end

--获得场景可视对象
function get_visual_scene()
	local visual = nx_value("game_visual")

	return visual:GetScene()
end

--获得主角玩家可视对象
function get_visual_player()
	local visual = nx_value("game_visual")

	return visual:GetPlayer()
end

--获得场景中可视对象
function get_visual_scene_obj(ident)
	local visual = nx_value("game_visual")

	return visual:GetSceneObj(ident)
end

--获得容器视窗可视对象
function get_visual_view(view_ident)
	local visual = nx_value("game_visual")

	return visual:GetView(view_ident)
end

--获得容器视窗中可视对象
function get_visual_view_obj(view_ident, item_ident)
	local visual = nx_value("game_visual")

	return visual:GetViewObj(view_ident, item_ident)
end

