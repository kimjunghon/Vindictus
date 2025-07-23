#include "EnginePch.h"
#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(ifstream& File, const vector<class CBone*>& Bones)
{
	File.read(reinterpret_cast<_char*>(&m_fDuration), sizeof(_float));
	File.read(reinterpret_cast<_char*>(&m_fTickPerSecond), sizeof(_float));

	File.read(reinterpret_cast<_char*>(&m_iNumChannels), sizeof(_uint));

	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(File, Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
		m_fCurrentTrackPosition = 0.f;

	for (auto& pChannel : m_Channels)
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);

	return true;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();
	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed Created : CAnimation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Create(ifstream& File, const vector<CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();
	if (FAILED(pInstance->Initialize(File, Bones)))
	{
		MSG_BOX(TEXT("Failed Created : CAnimation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}
