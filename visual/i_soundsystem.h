#pragma once

//音效实例句柄
typedef long SOUND_HANDLE;

//音效资源句柄
typedef long SOUNDRES_HANDLE;

typedef void(* CALLBACK_SOUND_EVENT)();

enum {
	SOUND_MODE_DEFAULT,
	SOUND_MODE_3D,
};

struct SOUND_VECTOR
{
	float x;        /* X co-ordinate in 3D space. */
    float y;        /* Y co-ordinate in 3D space. */
    float z;        /* Z co-ordinate in 3D space. */

	SOUND_VECTOR()
	{};

	SOUND_VECTOR(float _x, float _y, float _z)
		:x(_x), y(_y), z(_z)
	{};
};

class ISoundEffect
{
protected:
	bool m_isdel;
public:
	ISoundEffect()
		:m_isdel(false)
	{}

	virtual ~ISoundEffect()
	{}

	virtual void OnPlaying(SOUND_HANDLE id) = 0;
	inline bool IsDel()const { return m_isdel; };
};

class ISoundSystem
{
public:
	virtual void Init(int MaxChannel = 32) = 0;

	virtual void Destroy() = 0;
	
	virtual SOUNDRES_HANDLE LoadSound(const char* file, int mode = SOUND_MODE_DEFAULT) = 0;
	
	virtual SOUND_HANDLE Play(const char* file, int mode, bool paused = false) = 0;
	
	virtual SOUNDRES_HANDLE GetSoundRes(SOUND_HANDLE id) = 0;
	
	virtual bool IsValid(SOUND_HANDLE id) = 0;
	
	virtual unsigned int GetLength(SOUND_HANDLE id) = 0;
	
	virtual void SetPosition(SOUND_HANDLE id, unsigned int pos) = 0;
	
	virtual unsigned int GetPosition(SOUND_HANDLE id) = 0;
	
	virtual void SetVolume(SOUND_HANDLE id, float volume) = 0;
	
	virtual float GetVolume(SOUND_HANDLE id) = 0;
		
	virtual void Set3DParam(SOUND_HANDLE id, const SOUND_VECTOR& pos, const SOUND_VECTOR& vel) = 0;
	
	virtual void Set3DSetListener(const SOUND_VECTOR& pos, const SOUND_VECTOR& vel, const SOUND_VECTOR& forward, const SOUND_VECTOR& up) = 0;
		
	virtual void Set3DMinMaxDistance(SOUND_HANDLE resid, float mindistance, float maxdistance) = 0;
	
	virtual void Pause(SOUND_HANDLE id) = 0;
	
	virtual bool IsPaused(SOUND_HANDLE id) = 0;
	
	virtual void Resume(SOUND_HANDLE id) = 0;
	
	virtual void Stop(SOUND_HANDLE id) = 0;
	
	virtual bool IsPlaying(SOUND_HANDLE id) = 0;
	
	virtual bool IsLoop(SOUND_HANDLE id) = 0;
	
	virtual void SetLoop(SOUND_HANDLE id, bool loop) = 0;
		
	virtual void OnFrame(float seconds) = 0;

	virtual void AddSoundEffect(SOUND_HANDLE id, ISoundEffect* effect) = 0;
};

extern ISoundSystem* GetSoundSystem();

