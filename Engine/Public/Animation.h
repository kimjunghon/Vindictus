#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone;
class CChannel;

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	HRESULT Initialize(ifstream& File, const vector<class CBone*>& Bones);
	void	Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool IsLoop, _bool* pFinished, _float fTimeDelta);
	
public:
	_bool	CurrentAnim_InRangeOfRatio(_float fBeginRatio, _float fEndRatio);

	void	Enter();

private:
	_float				m_fDuration = {};
	_float				m_fTickPerSecond = {};
	_float				m_fCurrentTrackPosition = {};
	_uint				m_iNumChannels = {};

	_bool				m_bAnimChange = {};
	_float				m_fAnimChangeDelay = {};

	vector<CChannel*>	m_Channels;
	vector<_uint>		m_CurrentKeyFrameIndices;


public:
	static CAnimation*	Create(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones);
	static CAnimation*	Create(ifstream& File, const vector<CBone*>& Bones);
	CAnimation*			Clone();
	virtual void		Free() override;
};

NS_END