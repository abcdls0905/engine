-----------------------------------------------------
--文件名：	lua\util_gui.lua
--内容：	  工具
--说明：	
--日期：	2015.8.25
--作者：	
-----------------------------------------------------



--打印日志
function show_log(context)
	nx_log("[Info-Lua] " ..  nx_string(context))
end



-- 获取界面，如果是不存在：iscreate为true，则创建，iscreate为false，则返回空值,
-- 如果该界面需要多个实例则可以设置实例编号instance_id
function util_get_form(formname, iscreate, isclose,instance_id, ansynload)
    --// 空名 返回
    if ( formname == nil or string.len(formname) == 0 ) then
        return;
    end
	
	local form = nil;
	
	local formname_instance = nil;
	
	if instance_id and instance_id ~= "" then
	    formname_instance = formname .. nx_string(instance_id)
	else
		formname_instance = formname
	end

	if ansynload == nil then
		ansynload = false
	elseif ansynload ~= true then
		ansynload = false
	end
	
	local form = nx_value(formname_instance)

	if not nx_is_valid(form) and iscreate then

		--找不到并且需要创建，则创建
		local gui = nx_value("gui")

		if not (nx_is_valid(gui) and nx_is_valid(gui.Loader)) then
			return nx_null()
		end

		if ansynload then
			form = gui.Loader:LoadFormAsync(nx_resource_path(),  gui.skin_path .. formname .. ".xml", true)
		else
			form = gui.Loader:LoadForm(nx_resource_path(),  gui.skin_path .. formname .. ".xml", true)
		end
		
		if not nx_is_valid(form) then
			local error_text = nx_widestr("msg_CreateFormFailed - ") .. nx_widestr(gui.skin_path) 
						.. nx_widestr(formname) .. nx_widestr(".xml")
			nx_msgbox(nx_string(error_text));
			return 0;
		end
		
		--
		form.Name = formname
		form.FormName = nx_function("FormFileToName", formname)

				
		if instance_id then
	        form.instance_id = instance_id
		end

		if isclose == nil or  isclose then
			form.Visible = false;
			form:Close();
		end

		nx_set_value(formname_instance, form)
		
		return form;
	end

	if nx_is_valid(form) then
		--ui_show_attached_form(form)
	end
	
	return form;

end

-- 自动显示或隐藏界面,此种情况下，界面不存在会自动创建
function util_auto_show_hide_form(formname,instance_id)
	local form = util_get_form(formname, true, true, instance_id);

	if not nx_is_valid(form) then
		return nil
	end


	if form.Visible then
		form.Visible = false;
		form:Close();
	else
		form.Visible = true;
		form:Show();
	end

	return form
end

--自动显示或隐藏层级窗口
function util_auto_show_hide_form_layer(form, instance_id)
	local form = util_get_form(formname, true, false, instance_id)
	if not nx_is_valid(form) then
		return nx_null()
	end

	if form.Visible then
		form.Visible = false
		form:Close()
	else
		form.Visible = true
		form:ShowModal()
	end
end

-- 自动显示或隐藏界面,此种情况下，界面不存在会自动创建
function util_auto_show_hide_form_lock(formname,instance_id)

	local form = util_get_form(formname, true,true,instance_id);

	if not nx_is_valid(form) then
		return 0;
	end


	if form.Visible then
		form.Visible = false;
		form:Close();
	else
		form.Visible = true;
		form:ShowModal();
	end

end

-- 根据visible显示或隐藏界面
function util_show_form(formname, visible, instance_id)


	local form = util_get_form(formname, false,true,instance_id)

	if not visible then
		--关闭界面
		if not nx_is_valid(form) then
			--界面不存在则直接返回,不用再创建了
			return nx_null()
		else
			--界面存在则关闭
			form.Visible = false;
			form:Close();
		end
	else
		--打开界面
		form = util_get_form(formname, true,true,instance_id);

		if nx_is_valid(form) then
			--界面存在则显示
			form.Visible = true
			form:Show()
		end
	end

	return form
end

--根据visible显示或隐藏层级界面
function util_show_form_layer(formname, visible, instance_id)
    local form = util_get_form(formname, false, false, instance_id)
    if not visible then
    	--关闭界面
		if not nx_is_valid(form) then
			--界面不存在则直接返回,不用再创建了
			return nx_null()
		else
			--界面存在则关闭
			form.Visible = false
			form:Close()
		end
    else
    	--打开界面
		form = util_get_form(formname, true, false, instance_id)
		if nx_is_valid(form) then
			--界面存在则显示
			form.Visible = true
			form:ShowModal()

			return form
		end
    end

    return nx_null()
end

-- 判断界面是否显示
function util_is_lockform_visible(formname,instance_id)

	local formname_instance;
	if instance_id then
	    formname_instance = formname .. nx_string(instance_id)
	else
		formname_instance = formname
	end

	local form = nx_value(formname_instance)

	if not nx_is_valid(form) then
		return false;
	end

	return form.Visible;
end

-- 判断界面是否显示
function util_is_form_visible(formname,instance_id)

	local formname_instance;
	if instance_id then
	    formname_instance = formname .. nx_string(instance_id)
	else
		formname_instance = formname
	end

	local form = nx_value(formname_instance)

	if not nx_is_valid(form) then
		return false;
	end

	if not nx_is_valid(form.Parent) then
		return false
	end

	return form.Visible;
end

-- 淡入
function util_form_alpha_in(formname, keeptime,instance_id)

	local form = util_get_form(formname, true,true,instance_id)

	form.BlendAlpha = 0;
	form.Visible = true;
	form:Show();

	local timecount = 0;

	while true do
		timecount = timecount + nx_pause(0);

		if not nx_is_valid(form) then
			break;
		end

		if timecount < keeptime then
			form.BlendAlpha = 255 * timecount / keeptime
		else
			form.BlendAlpha = 255;
			break;
		end
	end
end

-- 淡出
function util_form_alpha_out(formname, keeptime,instance_id)
	local form = util_get_form(formname,false,false,instance_id)

	if not nx_is_valid(form) then
		return 0;
	end

	local old_alpha = form.BlendAlpha
	local timecount = 0;

	while true do
		timecount = timecount + nx_pause(0);

		if not nx_is_valid(form) then
			break;
		end

		if timecount < keeptime then
			form.BlendAlpha = old_alpha - old_alpha * timecount / keeptime
		else
			form.BlendAlpha = 255;
 			form.Visible = false
			form:Close();
			break;
		end
	end
end

-- 自动显示或隐藏界面,此种情况下，界面不存在会自动创建
function util_auto_alpha_in_out_form(formname,instance_id)

	local form = util_get_form(formname, true,true,instance_id);

	if not nx_is_valid(form) then
		return 0;
	end

	if form.Visible then
		util_form_alpha_out(formname, 0.1);

	else
		form.Visible = true;
		util_form_alpha_in(formname, 0.1);
	end

end

-- 将界面移动到指定点
function util_form_moveto(formname, target_x, target_y,instance_id)
	local form = util_get_form(formname,false,false,instance_id)

	local old_x = form.AbsLeft;
	local old_y = form.AbsTop;

	local timecount = 0;

	while true do
		timecount = timecount + nx_pause(0);

		if not nx_is_valid(form) then
			break;
		end

		if timecount < keeptime then
			form.AbsLeft = (target_x - old_x) * timecount / keeptime + old_x;
			form.AbsTop = (target_y - old_y) * timecount / keeptime + old_y;
		else
			form.AbsLeft = target_x
			form.AbsTop = target_y
			break;
		end
	end
end



--通用Messagebox框
function util_msgbox ( content, title,  ok_info, cancel_info )
	local dialog = nx_execute("util_gui", "util_get_form", "form_common\\form_confirm", true, false)

	dialog.Name = "form_confirm"

	--content
	local mltbox = dialog.mltbox_info
	mltbox.HtmlText = nx_widestr(content)

	--
	if ok_info ~= nil then
		dialog.ok_btn.Text = nx_widestr(ok_info)
	end

	if cancel_info ~= nil then
		dialog.cancel_btn.Text = nx_widestr(cancel_info)
	end

	--
	if title ~= nil then
		local label = dialog.lbl_title
		label.Text = nx_widestr(title)
		label.Font = "Default"
		label.ForeColor = "255,0,0,0"
	end
	
	--显示窗口
	dialog:ShowModal()

	--等待返回结果
	local res = nx_wait_event (1e8, dialog, "confirm_return")
	if (res == "ok") then
		if nx_is_valid(dialog) then
			nx_destroy(dialog)
		end

        return true
	end

	if nx_is_valid(dialog) then
		nx_destroy(dialog)
	end

    return false;
end

--
function ui_inputbox ( text )

    local gui = nx_value("gui")

	--创建并加载提示窗口数据
	local dialog = nx_execute("util_gui", "util_get_form", "form_common\\form_input_name", true, false)
	dialog.Name = "skin_editor\\form_input_name"
	dialog.info_label.Text = nx_widestr(text)
	--dialog.name_edit.Text = form.tree_info_right_click_node.Text

	--显示窗口
	dialog:ShowModal()

	--等待返回结果
	local res, input = nx_wait_event(1e8, dialog, "input_name_return")

	--结果为确定则退出程序
	if res == "cancel" then

		nx_destroy(dialog)

        return false, ""
    end

	nx_destroy(dialog)

    return true, input;
end



--弹提示框(只有一个“确定”按扭)：
function util_errorbox(msg_text)
	local dialog = nx_execute("util_gui", "util_get_form", "form_common\\form_error", true, false)

	dialog.mltbox_info:Clear()
	dialog.mltbox_info:AddHtmlText(nx_widestr(msg_text), -1)
	dialog:ShowModal()
	local res = nx_wait_event(1e8, dialog, "error_return")
end



--删除界面
function destroy_form(form)
	if not nx_is_valid(form) then
		return
	end

	local form_name = nx_script_name(form)

	if not is_form_need_destroy(form_name) then
		return
	end

	nx_destroy(form)
	nx_set_value(form_name, nx_null())
end

--向界面逻辑发送消息
function send_commandtoform(formName, command)
	local guiManager = nx_value("GuiManager")
	local form = guiManager:GetFormLogicByName(nx_string(formName))
	guiManager:SendMsgToLogicForm(form,  nx_string(command))
end

--界面打开或关闭时调用（手柄控制时需要调用）
function open_or_close_form(form, bopen)
	--大屏才用
	return
	--[[
	local ControllerAdapter = nx_value("ControllerAdapter")
	local GameControl = nx_value("GameControl")
	if  not nx_is_valid(ControllerAdapter) or not nx_is_valid(GameControl)then
		return
	end
	
	--
	local controler_adapter = nx_value("controler_adapter")
	local roomui = nx_value("roomUI")
	
	if  not nx_is_valid(controler_adapter) then
		controler_adapter = nx_create("controler_adapter", ControllerAdapter)
		nx_set_value("controler_adapter", controler_adapter)
	end
	
	if  not nx_is_valid(roomui) then
		local roomui = nx_create("roomui")
		nx_set_value("roomui", roomui)
	end
	
	--
	local formname = form.FormName 
	
	--
	if nx_boolean(bopen) then
		
		if  not roomui:isNoNeedAddOpenForm(formname)   and not roomui:isEffectForm(formname) then
		
			if not roomui:isEffectFormOpen(formname) then
				--roomui:setEffectForm(formname, true)
			end
		
			controler_adapter:openForm(form)
		end
		
		--
		if roomui:isUnableControlForm(formname) then
			GameControl.Enable = false
		end
	else
		if  not roomui:isNoNeedAddOpenForm(formname) then
			--roomui:closeEffectForm(szName)
			controler_adapter:closeForm(form)
		end
		
		--
		if roomui:isUnableControlForm(formname) then
			GameControl.Enable = true
		end
	end
	]]--
end

