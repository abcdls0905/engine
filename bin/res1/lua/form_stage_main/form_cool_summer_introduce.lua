-----------------------------------------------------
--文件名：	lua\form_stage_main\form_cool_summer_introduce.lua
--内容：	  清凉一夏_介绍
--说明：	
--日期：	2015.8.25
--作者：	
-----------------------------------------------------




--
function main_form_init(form)
	--nx_msgbox("init:" .. nx_string(form.Name))
	--form.Fixed = false	--使窗口可以移动	
end

--
function main_form_open(form)
	--nx_msgbox("main_form_open")
	--最大化
	nx_function("ext_gui_maximize_form", form)
	
	local groupbox_main = form.groupbox_main
	--居中
	nx_function("ext_gui_center", groupbox_main)
	
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
	if  nx_is_valid(form) then
		form.Visible = false 
	end
end
