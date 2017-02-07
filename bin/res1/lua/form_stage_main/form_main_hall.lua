-----------------------------------------------------
--文件名：	lua\form_stage_main\form_main_hall.lua
--内容：	 主界面
--说明：	
--日期：	2015.8.25
--作者：	
-----------------------------------------------------

require("util_gui")


--
function main_form_init(form)
	--nx_msgbox("init:" .. nx_string(form.Name))
end

--
function main_form_open(form)
	--nx_msgbox("main_form_open")
	--local sender = nx_value("game_sender")
	
	--local game_client = nx_value("game_client")
	--if nx_is_valid(game_client) then
	--	local player = game_client:GetPlayer()
	--	if nx_is_valid(player) then
	--		local name = player:QueryProp("HP")
	--		nx_msgbox(nx_string(name))
	--	else
	--		nx_msgbox("error")
	--	end
		
	--end
	
	open_or_close_form(form, true)
end

--
function main_form_close()
	--nx_msgbox("main_form_close")
	local form = nx_value("form_stage_main\\form_main_hall")
	open_or_close_form(form, false)
end

--
function main_form_oncreate_begin(form)
	--nx_msgbox("oncreate_begin")
	--form.group_login_main.Visible = false
end

--
function main_form_oncreate_end(form)
	--nx_msgbox("oncreate_end")  
	
	local btn_cool_summer = form.btn_cool_summer
	
	local  gui = nx_value("gui")
	if nx_is_valid(gui) and nx_is_valid(btn_cool_summer) then
		gui:UnRegisterCallback(btn_cool_summer)
  end
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

--单机清凉一夏按钮
function btn_click_cool_summer(control)
	--nx_msgbox("btn_click_cool_summer:" .. nx_string(control.Name))
	local form = nx_execute("util_gui", "util_get_form", "form_stage_main\\form_cool_summer\\form_cool_summer_main", true, false)
	if not nx_is_valid(form) then
		return 
	end

	form.Visible = true;
	form:Show()
	
end

--单机清凉一夏按钮
function btn_click_active_test(control)
	nx_msgbox("btn_click_active_test:" .. nx_string(control.Name))
end