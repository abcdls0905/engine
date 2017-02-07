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
	show_log("main_form_open")
	--最大化
	nx_function("ext_gui_maximize_form", form)
	
	local groupbox_main = form.groupbox_main
	--居中
	nx_function("ext_gui_center", groupbox_main)
	
	local game_client_lua = nx_value("game_client_lua")
	local playerID = game_client_lua.PlayerIdent
	
	--绑定回调测试
	local dataBinder = nx_value("DataBinder")
	dataBinder:AddViewBind(form, 2, nx_current(), "view_change")
	dataBinder:AddRolePropertyBind(form, "HP", nx_current(), "prop_change")
	dataBinder:AddObjPropertyBind(playerID, form,  "HP", nx_current(), "obj_prop_change")
	dataBinder:AddScenePropertyBind(form,  "HP", nx_current(), "sceneprop_change")
	dataBinder:AddTableBind(form, "capital_rec", nx_current(), "table_change")
	dataBinder:AddSceneTableBind(form, "capital_rec", nx_current(), "table_change")
	
	--计时器测试 
	local timemanager = nx_value("timer")
	timemanager:Register(5000, 0, nx_current(), "timer_test", form, -1, -1)
	
	open_or_close_form(form, true)
end

--
function main_form_close(form)
	--nx_msgbox("main_form_close")
	
end

--
function main_form_oncreate_begin(form)
	--nx_msgbox("oncreate_begin")
end

--
function main_form_oncreate_end(form)
	nx_msgbox("oncreate_end")  
	
end

--
function main_form_onshow_begin(form)
	--nx_msgbox("main_form_onshow_begin")
end

--
function main_form_onshow_end(form)
	--nx_msgbox("main_form_onshow_end")
end

--
function main_form_onhide_begin(form)
	--nx_msgbox("main_form_onhide_begin")
end

--
function main_form_onhide_end(form)
	--nx_msgbox("main_form_onhide_end")
end

--
function main_form_onclose_begin(form)
	--nx_msgbox("main_form_onclose_begin")
end

--
function main_form_onclose_end(form)
	--nx_msgbox("main_form_onclose_end")
end

--
function btn_click_close(self)
	--nx_msgbox("btn_click_close")
	local form = self.ParentForm
	open_or_close_form(form, false)
	
	--计时器测试 
	local timemanager = nx_value("timer")
	timemanager:UnRegister(nx_current(), "timer_test", form)
	
	if  nx_is_valid(form) then
		form:Close()
	end
end

--
function click_btn_total_info(self)
	local form = nx_execute("util_gui", "util_get_form", "form_stage_main\\form_cool_summer\\form_cool_summer_introduce", true, false)
	if not nx_is_valid(form) then
		return 
	end

	--form.Visible = true
	--form:Show()
	
	local gui = nx_value("gui")
	
	local guiManager = nx_value("GuiManager")
	local context = gui.TextManager:GetFormatText("ui_tattoo_compound_2")
	local formID = guiManager:GetFormLogicByName("FormMessage")
	guiManager:SendMsgToLogicFormArgs(formID,  "ShowBroadcastInfo", nx_widestr(context), nx_boolean(true))
	--[[
	local context = gui.TextManager:GetFormatText("ui_tattoo_compound_2")
	nx_execute("util_gui", "util_errorbox", context)	
	]]--
	
	--[[
	--
	local title = gui.TextManager:GetFormatText("ui_minitip")
	local context = gui.TextManager:GetFormatText("ui_tattoo_compound_2")
	nx_execute("util_gui", "util_msgbox", context, title)	
	]]--
	
	
	--[[
	--显示确认窗口
	local dialog = nx_execute("util_gui", "util_get_form",  "form_common\\form_confirm", true, false)	
	dialog.mltbox_info.HtmlText = nx_widestr(gui.TextManager:GetFormatText("ui_tattoo_compound_2"))
	dialog:ShowModal()


	--返回结果
	local res = nx_wait_event(1e8, dialog, "confirm_return")
	if "ok" == res then
		form:Show()
	else
		return
	end
	]]--
		
	--[[
	local gui = nx_value("gui")
	local text = gui:GetTextManagerID()
	local name1 = text:GetFormatText("ui_summer_exchange_04")
	
	local game_client_lua = nx_value("game_client_lua")
	local server_player = game_client_lua:GetPlayer()
	
	local name2 = game_client_lua:QueryPropWideStr(server_player, "Name")


	nx_function("gui_anwerbox", nx_widestr(name1), nx_widestr(name2), nx_int(-1), nx_widestr("LUA"))
	]]--
	
	
	
	local scenebox_test_form = nx_execute("util_gui", "util_get_form", "form_stage_main\\form_scenebox_test", true, false)
	if  nx_is_valid(scenebox_test_form) then
		scenebox_test_form.Visible = true
		scenebox_test_form:Show() 
	end
	
end

--
function click_btn_exchange_goods(self)
	--nx_msgbox("click_btn_exchange_goods")
	local form = nx_execute("util_gui", "util_get_form", "form_stage_main\\form_cool_summer\\form_summer_exchange", true, false)
	if not nx_is_valid(form) then
		form:Show()
	end

	form.Visible = true
	form:Show()
end

--
function click_btn_login_everyday(self)
	--nx_msgbox("click_btn_login_everyday")
	local GuiManager = nx_value("GuiManager")
	local form_everyday = GuiManager:GetFormLogicByName("FormCoolSummerLoginAward")
	GuiManager:SendMsgToLogicForm(form_everyday, "show")
	
end

--
function click_btn_play_everyday(self)
	--nx_msgbox("click_btn_play_everyday")
	local GuiManager = nx_value("GuiManager")
	local form_everyday = GuiManager:GetFormLogicByName("FormCoolSummerPlayEveryday")
	GuiManager:SendMsgToLogicForm(form_everyday, "show")
end

--
function click_btn_gift(self)
	--nx_msgbox("click_btn_gift")
	local GuiManager = nx_value("GuiManager")
	local form_everyday = GuiManager:GetFormLogicByName("FormCoolSummerShop")
	GuiManager:SendMsgToLogicForm(form_everyday, "show")
end

--视图变化回调
function view_change(control, changetype, view_id, view_index)
	show_log("view_change:" .. nx_string(control) ..  " changetype: " .. nx_string(changetype) .. " viewid: " .. nx_string(view_id) .. "  viewindex: " .. nx_string(view_index))
	
end


--属性变化回调
function obj_prop_change(control, objID, prop)
	show_log("obj_prop_change:" .. nx_string(control) ..  " objID: " .. nx_string(objID) .. " prop: " .. nx_string(prop))
end

--属性变化回调
function prop_change(control, objID, prop)
	show_log("prop_change:" .. nx_string(control) ..  " objID: " .. nx_string(objID) .. " prop: " .. nx_string(prop))
end

--场景属性变化回调
function sceneprop_change(control, objID, prop)
	show_log("sceneprop_change:" .. nx_string(control) ..  " objID: " .. nx_string(objID) .. " prop: " .. nx_string(prop))
end

--表变化回调
function table_change(control, tablename, changetype, param1, param2)
	show_log("table_change:" .. nx_string(control) ..  " tablename:"  .. nx_string(tablename) .. " changetype: " .. nx_string(changetype) .. " param1: " .. nx_string(param1) .. "  param2: " .. nx_string(param2))
end

--
function timer_test(control, param1, param2)
	show_log("timer_test:" .. nx_string(control) ..  " param1: " .. nx_string(param1) .. " param2: " .. nx_string(param2))
end

--
function show_log(context)
	nx_log("[Info-Lua] " ..  nx_string(context))
	--nx_msgbox(nx_string(context))
end