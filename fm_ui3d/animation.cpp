#include "animation.h"

_UI3D_BEGIN

Animation::Animation()
:m_isplay(false)
,m_ispause(false)
,m_time(0)
,m_maxframe(30)
,m_speed(1)
{
}

Animation::~Animation()
{
	for(size_t i = 0; i < m_FrameGroups.size(); i++)
	{
		for(size_t j = 0; j < m_FrameGroups[i].frames.size(); j++)
		{
			delete(m_FrameGroups[i].frames[j].pAction);
		}
	}
}

void Animation::Create(int maxframe, float speed)
{
	m_maxframe = maxframe;
	m_speed = speed;
	m_total_time = m_maxframe * m_speed;
}

void Animation::CreateFrameGroup(const char* name)
{
	FrameGroup* pgroup = FindGroup(name);
	if(pgroup == 0)
	{
		FrameGroup group;
		group.key = name;
		group.frame_end = 0;
		group.cur_frame = 0;
		m_FrameGroups.push_back(group);
	}
}

void Animation::UpdateFrames(FrameGroup * group)
{
	if(group->cur_frame >= (int)group->frames.size())
		return;
	Frame& frame = group->frames[group->cur_frame];
	float frame_start_time = frame.frame_start * m_speed;
	float frame_end_time = frame.frame_end * m_speed;
	
	if(!frame.isplaying && m_time >= frame_start_time)
	{
		frame.isplaying = true;
		frame.pAction->OnFrameStart();
		frame.pAction->OnFrame(0);
	}
	else if(frame.isplaying && m_time >= frame_end_time)
	{
		frame.pAction->OnFrameEnd();
		frame.isplaying = false;
		group->cur_frame++;
	}
	else if(frame.isplaying && m_time >frame_start_time && m_time <= frame_end_time)
	{
		frame.pAction->OnFrame((m_time - frame_start_time) / (frame_end_time - frame_start_time));
	}
}

void Animation::Update(float time)
{
	if(!m_isplay)
		return;
	
	m_time += time;
	
	for(size_t i = 0; i < m_FrameGroups.size(); i++)
	{
		UpdateFrames(&m_FrameGroups[i]);
	}

	if(m_time > m_total_time)
	{
		//onstop
		m_isplay = false;
		m_time = 0;

		for(size_t i = 0; i < m_FrameGroups.size(); i++)
		{
			m_FrameGroups[i].cur_frame = 0;
		}
	}
}

void Animation::AddAction(IUI3DActionBase* pAction, int frame_num, int frame_skip, const char* framegroup)
{
	FrameGroup* group = 0;
	if(m_FrameGroups.empty())
	{
		m_FrameGroups.resize(1);
		group = &m_FrameGroups[0];
		group->frame_end = 0;
		group->cur_frame = 0;
		if(framegroup == 0)
		{
			group->key = "";
		}
	}

	if(framegroup == 0)
	{
		group = &m_FrameGroups[0];
	}
	else
	{
		group = FindGroup(framegroup);
	}
	
	Frame frame;
	frame.frame_start = group->frame_end + frame_skip;
	frame.frame_end = group->frame_end + frame_skip + frame_num;
	frame.isplaying = false;
	frame.pAction = pAction;
	group->frame_end += frame_num;;
	group->frames.push_back(frame);

	int frame_end = 0;
	for(size_t i = 0; i < m_FrameGroups.size(); i++)
	{
		if(m_FrameGroups[i].frame_end > frame_end)
			frame_end = m_FrameGroups[i].frame_end;
	}
	m_total_time = frame_end * m_speed;
}

void Animation::Play()
{
	m_isplay = true;
}

void Animation::PlayBack()
{
}

Animation::FrameGroup* Animation::FindGroup(const char* framegroup)
{
	for(size_t i = 0; i < m_FrameGroups.size(); i++)
	{
		if(stricmp(m_FrameGroups[i].key.c_str(), framegroup) == 0)
			return &m_FrameGroups[i];
	}
	return 0;
}

_UI3D_END
