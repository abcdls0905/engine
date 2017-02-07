#pragma once

#include "defines.h"
#include "../visual/i_vis_base.h"
#include <vector>
#include <string>
#include "../math/fm_math.h"
#include "visnode.h"
#include "../visual/i_ui3d_animation.h"

_UI3D_BEGIN

class Animation : public IUI3DAnimation
{
	struct Frame
	{
		int frame_start;	//开始祯
		int frame_end;		//结束祯
		bool isplaying;
		//具体的效果控制逻辑
		IUI3DActionBase* pAction;
	};

	struct FrameGroup
	{
		std::string key;
		std::vector<Frame> frames;
		int frame_end;
		int cur_frame;
	};

	std::vector<FrameGroup> m_FrameGroups;
	std::string m_name;
	float m_total_time;
	float m_time;
	bool m_isplay;
	bool m_ispause;
	int m_maxframe;
	float m_speed; //一祯的速度（秒)
private:
	void UpdateFrames(FrameGroup * group);
	FrameGroup* FindGroup(const char* framegroup);
public:
	Animation();

	~Animation();
	
	void Create(int maxframe = 30, float speed = 1);

	void CreateFrameGroup(const char* name);

	void AddAction(IUI3DActionBase* pAction, int frame_num, int frame_skip = 0, const char* framegroup = 0);

	void Clear();

	void SetLoop(bool loop);
	
	void Play();

	void PlayBack();

	void Pause();

	void Stop();

	void Update(float time);
	
	void Zoom();

	void Load(const char* file);

	void Save(const char* file);

};

_UI3D_END