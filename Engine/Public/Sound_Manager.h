#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	typedef map<const _wstring, FMOD_SOUND*> SOUNDS;

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();
	void	Update();
	HRESULT Add_Sounds(const _wstring& strSoundTag, const _char* pFilePath);

	HRESULT Play_Sound(const _wstring& strSoundTag, _uint iChannelIndex, _float fVolume, _bool IsLoop);
	HRESULT Play_Sound_AnyChannel(_uint iMinChannelIndex, const _wstring& strSoundTag, _float fVolume);
	HRESULT Stop_Sound(_uint iChannelIndex);
	HRESULT Stop_Sound_All();
	HRESULT Change_Volume(_uint iChannelIndex, _float fVolume);
	
	void	Clear_Resource();

private:
	SOUNDS					m_Sounds;

	_uint					m_iNumChannel = {};
	vector<FMOD_CHANNEL*>	m_Channels;
	
	FMOD_SYSTEM*			m_pSystem = { nullptr };

private:
	FMOD_SOUND*				Find_Sound(const _wstring& strSoundTag);

public:
	static CSound_Manager*	Create();
	virtual void			Free() override;
};

NS_END