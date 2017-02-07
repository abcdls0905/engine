-----------------------------------------------------
--文件名：	lua\form_stage_main\form_cool_summer.lua
--内容：	  清凉一夏主界面
--说明：	
--日期：	2015.8.25
--作者：	
-----------------------------------------------------

require("util_gui")


--
function main_form_init(form)
	--nx_msgbox("init:" .. nx_string(form.Name))
	--form.Fixed = false	--使窗口可以移动
end

--
function main_form_open(form)
	--最大化
	nx_function("ext_gui_maximize_form", form)
	
	local groupbox_main = form.groupbox_equip
	--居中
	nx_function("ext_gui_center", groupbox_main)
	
	--
	local scenebox = form.scenebox_role
	
	--
	if not nx_find_custom(scenebox, "luascene") then
		nx_set_custom(scenebox, "luascene", nx_null())
	end
	
	if not nx_find_custom(scenebox, "rolemodel") then
		nx_set_custom(scenebox, "rolemodel", nx_null())
	end
	
	if not nx_find_custom(scenebox, "backmodel") then
		nx_set_custom(scenebox, "backmodel", nx_null())
	end
	
	local gui = nx_value("gui")
	local context = gui.TextManager:GetFormatText("ui_tattoo_compound_2")
	local lbl_name = form.lbl_role_name
	lbl_name.Text = nx_widestr(context)
	--nx_msgbox(context)
	--
	show_model(form, scenebox)
	
	--
	open_or_close_form(form, true)
end


--
function main_form_close(form)
	--nx_msgbox("main_form_close")
	
end

--
function btn_click_close(self)
	--nx_msgbox("btn_click_close")
	local form = self.ParentForm
	
	if  nx_is_valid(form) then
		--
		open_or_close_form(form, false)
		
		form:Close()
	end
end

--拖动事件
function on_drag_scenebox(event, ctrl, click_x, click_y, drag_type, drag_x, drag_y)
	--nx_msgbox(nx_string(click_x) .. "|" .. nx_string(click_y) .. "|" .. nx_string(drag_x) .. "|".. nx_string(drag_y))
	local rolemodel = ctrl.rolemodel
	if not nx_is_valid(rolemodel) then
		return
	end
	
	local angle = rolemodel:GetAngle()
	local angleY = angle[2]
	
	angleY = angleY - drag_x/nx_float(180.0)* nx_float(3.14)
	rolemodel:SetAngle(nx_float(0.0), nx_float(angleY), nx_float(0.0))
end


--
function show_log(context)
	nx_log("[Info-Lua] " ..  nx_string(context))
	nx_msgbox(nx_string(context))
end

--
function show_model(form, scenebox)
	local game_visual = nx_value("game_visual")
	local game_client_lua = nx_value("game_client_lua")
	local server_player = game_client_lua:GetPlayer()
	local client_player = game_visual:GetPlayer()
	local role_composite = nx_value("role_composite")
	local scenebox_rolecomposite = nx_value("scenebox_rolecomposite")
	
	if not nx_is_valid(scenebox_rolecomposite) then
		scenebox_rolecomposite = nx_create("scenebox_rolecomposite", role_composite)
		nx_set_value("scenebox_rolecomposite", scenebox_rolecomposite)
	end
	
	--
	local luascene = scenebox.luascene
	if not nx_is_valid(luaactor) then
		luascene = nx_create("LuaScene", scenebox)
	end

	local luaactor = scenebox.rolemodel
	if not nx_is_valid(luaactor) then
		luaactor = luascene:CreateLuaActor()
		scenebox.rolemodel = luaactor
	end
	
	
	--
	local readIni = nx_create("CFastReadIni")
	readIni.FileName = "skin\\form_stage_main\\form_bag\\form_bag_config.ini"
	if not readIni:LoadFromFile() then
		nx_msgbox("cant find file:" .. nx_string(readIni.FileName))
	end
	
	--
	local camera_info = readIni:ReadStringEx("BagSceneboxConfig", "Camera", "")
	local argList = nx_function("ext_split_string", camera_info)
	local cameraPosX = nx_float(argList[1])
	local cameraPosY = nx_float(argList[2])
	local cameraPosZ = nx_float(argList[3])
	
	local cameraAngleX= nx_float(argList[4])
	local cameraAngleY= nx_float(argList[5])
	local cameraAngleZ= nx_float(argList[6])
	
	local cameraFov = nx_float(argList[7])
	
	--nx_msgbox(nx_string(camera_info))
	--
	local back_info = readIni:ReadStringEx("BagSceneboxConfig", "BackModel", "")
	local argList1 = nx_function("ext_split_string", back_info)
	local modelPosX = nx_float(argList1[1])
	local modelPosY = nx_float(argList1[2])
	local modelPosZ = nx_float(argList1[3])
	
	local modelAngleX= nx_float(argList1[4])
	local modelAngleY= nx_float(argList1[5])
	local modelAngleZ= nx_float(argList1[6])
	
	local modelScaleX= nx_float(argList1[7])
	local modelScaleY= nx_float(argList1[8])
	local modelScaleZ= nx_float(argList1[9] )
	
	local back_path = readIni:ReadStringEx("BagSceneboxConfig", "BackModelPath", "")
	--nx_msgbox(nx_string(back_info))
	
	
	--初始化scenebox 设置背景模型
	nx_function("InitSceneBox", scenebox, cameraPosX, cameraPosY, cameraPosZ, cameraFov, cameraAngleX, cameraAngleY, cameraAngleZ, nx_string(back_path), modelPosX, modelPosY, modelPosZ, modelScaleX, modelScaleY, modelScaleZ, modelAngleX, modelAngleY, modelAngleZ)

	
	--加载模型
	local roleindex = game_client_lua:QueryPropInt(server_player, "RoleIndex")
	local sex = scenebox_rolecomposite:GetPropConfig(roleindex, "Sex", 0)
	
	luaactor.role_index = roleindex
	luaactor.artpack_index = roleindex-1
	luaactor.sex = sex
	
	--角色位置
	local rolescetionindex = "ModelRoleIndex" .. nx_string(roleindex)
	local role_info = readIni:ReadStringEx("BagSceneboxRolepos", rolescetionindex, "")
	--nx_msgbox(nx_string(role_info))
	
	local argList2 = nx_function("ext_split_string", role_info)
	local rolePosX = nx_float(argList2[1])
	local rolePosY = nx_float(argList2[2])
	local rolePosZ = nx_float(argList2[3])
	
	local roleAngleX= nx_float(argList2[4])
	local roleAngleY= nx_float(argList2[5])
	local roleAngleZ= nx_float(argList2[6])
	
	local roleScaleX= nx_float(argList2[7])
	local roleScaleY= nx_float(argList2[8])
	local roleScaleZ= nx_float(argList2[9])
	
	nx_destroy(readIni)
	
	--
	if not scenebox_rolecomposite:CreatePlayerComposite1(luaactor, roleindex, false) then
		luascene:Delete(luaactor)
		
		return
	end
	
	--
	local arg_list = scenebox_rolecomposite:GetPartTypeList("Exposed")
	local count = table.getn(arg_list)
	for i = 1, count do
		local propname = arg_list[i]
		local modelname = scenebox_rolecomposite:GetExposedConfig(roleindex, propname, 0)
		
		scenebox_rolecomposite:SetCompositeWithProp(luaactor, propname, modelname, roleindex)
	end
	
	luaactor:SetPosition(rolePosX, rolePosY, rolePosZ)
	luaactor:SetAngle(roleAngleX, roleAngleY, roleAngleZ)
	luaactor:SetScale(roleScaleX, roleScaleY, roleScaleZ)
	luascene:AddObject(luaactor, 20)
	luaactor.scene = luascene
	
	local weaponid = client_player.weapon_id
	local weaponname = client_player.weapon_name
	local clothid = client_player.cloth_id
	
	luaactor.weapon_id = weaponid
	luaactor.weapon_name = weaponname
	
	luaactor.l_weapon_id = weaponid
	luaactor.l_weapon_name = weaponname
	luaactor.cloth_id = clothid
	
	local realQuality = game_client_lua:QueryPropInt(server_player, "ClothRealQuality")
	luaactor.RoleClothQuality = realQuality
	

	--luaactor.wing_id = client_player.wing_id
	--luaactor.fashion_cloth_id = client_player.fashion_cloth_id
	
	--
	--local fashion_weapon_id = client_player.fashion_weapon_id
	--local fashion_weapon_name = client_player.fashion_weapon_name
	
	--luaactor.fashion_weapon_id = fashion_weapon_id
	--luaactor.fashion_weapon_name = fashion_weapon_name
	--luaactor.l_fashion_weapon_id = fashion_weapon_id
	--luaactor.l_fashion_weapon_name = fashion_weapon_name
	
	--
	--luaactor.show_fashion = client_player.show_fashion
	--luaactor.WingFlyLv = client_player.WingFlyLv
	
	--
	if not game_client_lua:FindProp(server_player, "Cloth") then
		scenebox_rolecomposite:RefreshRoleComposite(luaactor, server_player, "RWeapon", "")
		scenebox_rolecomposite:RefreshRoleComposite(luaactor, server_player, "Wing", "")
	else
		scenebox_rolecomposite:RefreshRoleComposite(luaactor, server_player, "Cloth", "")
	end

end





