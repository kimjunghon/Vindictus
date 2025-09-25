#include "EnginePch.h"
#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	m_iNumChannel = 32;

	if (FMOD_OK != FMOD_System_Create(&m_pSystem, FMOD_VERSION))
		return E_FAIL;

	if (FMOD_OK != FMOD_System_Init(m_pSystem, m_iNumChannel, FMOD_INIT_NORMAL, NULL))
		return E_FAIL;

	m_Channels.resize(m_iNumChannel, nullptr);

	return S_OK;
}

void CSound_Manager::Update()
{
	for (auto& Channel : m_Channels)
	{
		FMOD_BOOL IsPlaying = {};
		FMOD_Channel_IsPlaying(Channel, &IsPlaying);

		if (IsPlaying)
			continue;

		Channel = nullptr;
	}
	FMOD_System_Update(m_pSystem);
}

HRESULT CSound_Manager::Add_Sounds(const _wstring& strSoundTag, const _char* pFilePath)
{
	if (nullptr != Find_Sound(strSoundTag))
		return E_FAIL;

	FMOD_SOUND* pSound = { nullptr };

	FMOD_RESULT Result = FMOD_System_CreateSound(m_pSystem, pFilePath, FMOD_DEFAULT, 0, &pSound);

	if (Result != FMOD_OK || 
		nullptr == pSound)
		return E_FAIL;

	m_Sounds.emplace(strSoundTag, pSound);

	if (FMOD_OK != FMOD_System_Update(m_pSystem))
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Play_Sound(const _wstring& strSoundTag, _uint iChannelIndex, _float fVolume, _bool IsLoop)
{
	if (iChannelIndex >= m_iNumChannel)
		return E_FAIL;
	
	FMOD_SOUND* pSound = Find_Sound(strSoundTag);
	if (nullptr == pSound)
		return E_FAIL;

	if(FAILED(Stop_Sound(iChannelIndex)))
		return E_FAIL;

	if (FMOD_OK != FMOD_System_PlaySound(m_pSystem, pSound, nullptr, false, &m_Channels[iChannelIndex]))
		return E_FAIL;
	
	if (FMOD_OK != FMOD_Channel_SetVolume(m_Channels[iChannelIndex], fVolume))
		return E_FAIL;

	if(IsLoop)
	{
		if (FMOD_OK != FMOD_Channel_SetMode(m_Channels[iChannelIndex], FMOD_LOOP_NORMAL))
			return E_FAIL;
	}

	if (FMOD_OK != FMOD_System_Update(m_pSystem))
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Play_Sound_AnyChannel(_uint iMinChannelIndex, const _wstring& strSoundTag, _float fVolume)
{
	if (iMinChannelIndex >= m_iNumChannel)
		return E_FAIL;

	_uint iChannelIndex = iMinChannelIndex;

	while (nullptr != m_Channels[iChannelIndex])
	{
		iChannelIndex++;
		if (iChannelIndex >= m_iNumChannel)
			return E_FAIL;
	}

	FMOD_SOUND* pSound = Find_Sound(strSoundTag);
	if (nullptr == pSound)
		return E_FAIL;

	if (FMOD_OK != FMOD_System_PlaySound(m_pSystem, pSound, nullptr, false, &m_Channels[iChannelIndex]))
		return E_FAIL;

	if (FMOD_OK != FMOD_Channel_SetVolume(m_Channels[iChannelIndex], fVolume))
		return E_FAIL;

	if (FMOD_OK != FMOD_System_Update(m_pSystem))
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Stop_Sound(_uint iChannelIndex)
{
	if (iChannelIndex >= m_iNumChannel)
		return E_FAIL;

	if (nullptr == m_Channels[iChannelIndex])
		return S_OK;

	FMOD_Channel_Stop(m_Channels[iChannelIndex]);

	m_Channels[iChannelIndex] = nullptr;

	if (FMOD_OK != FMOD_System_Update(m_pSystem))
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Stop_Sound_All()
{
	for (auto& pChannel : m_Channels)
	{
		if (nullptr == pChannel)
			continue;

		if (FMOD_OK != FMOD_Channel_Stop(pChannel))
			return E_FAIL;

		pChannel = nullptr;
	}
	
	if (FMOD_OK != FMOD_System_Update(m_pSystem))
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Change_Volume(_uint iChannelIndex, _float fVolume)
{
	if (iChannelIndex >= m_iNumChannel)
		return E_FAIL;

	if (FMOD_OK != FMOD_Channel_SetVolume(m_Channels[iChannelIndex], fVolume))
		return E_FAIL;

	if (FMOD_OK != FMOD_System_Update(m_pSystem))
		return E_FAIL;

	return S_OK;
}

void CSound_Manager::Clear_Resource()
{
	Stop_Sound_All();
}

FMOD_SOUND* CSound_Manager::Find_Sound(const _wstring& strSoundTag)
{
	auto iter = m_Sounds.find(strSoundTag);
	if (iter == m_Sounds.end())
		return nullptr;
	return iter->second;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CSound_Manager"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	Stop_Sound_All();
	
	/*for (auto& Pair : m_Sounds)
	{
		FMOD_Sound_Release(Pair.second);
		Pair.second = nullptr;
	}*/

	m_Sounds.clear();

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}
