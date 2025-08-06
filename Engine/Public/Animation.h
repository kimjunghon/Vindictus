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
	void	Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool IsLoop, _bool* pFinished, _float fTimeDelta, _vector& vPrevRootPostion);
	_bool	IsAnimChanging() { return m_bAnimChange; }
public:
	_bool	CurrentAnim_InRangeOfRatio(_float fBeginRatio, _float fEndRatio);

	void	Enter(_bool IsChange);

private:
	_float				m_fDuration = {};
	_float				m_fTickPerSecond = {};
	_float				m_fCurrentTrackPosition = {};
	_uint				m_iNumChannels = {};

	_bool				m_bAnimChange = {};
	_bool				m_bAnimChangeFirstCall = {};
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