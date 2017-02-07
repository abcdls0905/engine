-----------------------------------------------------
--文件名：	lua\form_stage_main\form_chat.lua
--内容：	聊天
--说明：	
--日期：	2012.3.21
--作者：	
-----------------------------------------------------


--require("utils")


--
function main_form_init(form)
	--nx_msgbox("init:" .. nx_string(form.Name))
end


--
function main_form_oncreate_begin(form)
	--nx_msgbox("oncreate_begin")
	--form.group_login_main.Visible = false
end

--
function main_form_oncreate_end(form)
	--nx_msgbox("oncreate_end")   
	local btn_send = form.btn_send
	local  gui = nx_value("gui")
	
	--
	--gui:UnRegisterCallback(btn_send)
	--gui:RegisterCallback(btn_send, "on_click",  nx_current(), "send_chat")
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
function send_chat()
	--nx_msgbox("send_chat1")
	--local sender = nx_value("game_sender")
end