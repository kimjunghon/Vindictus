#include "EnginePch.h"
#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fDuration { Prototype.m_fDuration }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	, m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
	, m_iNumChannels{ Prototype.m_iNumChannels }
	, m_fAnimChangeDelay{ Prototype.m_fAnimChangeDelay }
	, m_Channels{ Prototype.m_Channels }
	, m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

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

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(File, Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	m_fAnimChangeDelay = 0.2f;

	return S_OK;
}

void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool IsLoop, _bool* pFinished, _float fTimeDelta, _vector& vPrevRootPostion)
{
	if (m_bAnimChange)
	{
		m_fCurrentTrackPosition += fTimeDelta;

		_float fRatio = m_fCurrentTrackPosition / m_fAnimChangeDelay;

		if(m_fCurrentTrackPosition >= m_fAnimChangeDelay)
		{
			m_fCurrentTrackPosition = 0.f;

			fRatio = 1.f;

			m_bAnimChange = false;
		}

		for (auto& pChannel : m_Channels)
			pChannel->Update_AnimChangeTransformationMatrix(Bones, fRatio, &m_bAnimChangeFirstCall);

		if (m_bAnimChangeFirstCall)
			m_bAnimChangeFirstCall = false;
	}
	else
	{
		m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

		if (m_fCurrentTrackPosition >= m_fDuration)
		{
			if (false == IsLoop)
			{
				*pFinished = true;
				m_fCurrentTrackPosition = m_fDuration;
			}
			else
			{
				vPrevRootPostion = XMVectorSet(0.f, 0.f, 0.f, 1.f);
				m_fCurrentTrackPosition = 0.f;
			}
		}

		for (_uint i =0; i< m_iNumChannels; i++)
			m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i]);

	}

}

_bool CAnimation::CurrentAnim_InRangeOfRatio(_float fBeginRatio, _float fEndRatio)
{
	return (m_fCurrentTrackPosition / m_fDuration) >= fBeginRatio && (m_fCurrentTrackPosition / m_fDuration) <= fEndRatio;
}

void CAnimation::Enter()
{
	fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);

	m_fCurrentTrackPosition = 0.f;

	m_bAnimChange = true;
	m_bAnimChangeFirstCall = true;
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

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
