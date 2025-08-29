#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone;
class CChannel;

class CAnimation final : public CBase
{
public:
	typedef struct tagAnimNotify
	{
		_float	fTrackPosition;
		function<void()> Callback;
		_bool	IsRun;
	}ANIM_NOTIFY;

private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

#ifdef _DEBUG
public:
	_float			Get_Duration() { return m_fDuration; }
	_float*			Get_CurrentTrackPositionPtr() { return &m_fCurrentTrackPosition; }
	void			Reset_KeyFrameIndex() { fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);}
#endif

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	HRESULT Initialize(ifstream& File, const vector<class CBone*>& Bones);
	void	Update_TransformationMatricesLerp(const vector<class CBone*>& Bones, _bool* pFinished, _float fTimeDelta);
	void	Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool IsLoop, _bool* pFinished, _float fTimeDelta, _bool* IsAnimStart);

	_bool	IsAnimChanging() { return m_bAnimChange; }
	_float	Get_AnimSpeed() { return m_fDuration / m_fTickPerSecond; }

public:
	HRESULT Add_Notify(_float fTrackPosition, function<void()> Callback);
	void	Check_Notify(_float fCurrentTrackPosition);

	void	Reset_Notify();

	_bool	Anim_InRangeOfRatio(_float fBeginRatio, _float fEndRatio);
	_bool	Anim_InRangeOfTrackPositon(_float fBeginTrackPosition, _float fEndTrackPosition);
	_bool	Anim_PassToTrackPosition(_float fTrackPosition);
	void	Enter(_bool IsChange);

private:
	vector<ANIM_NOTIFY>	m_Notifies;

	_float				m_fDuration = {};
	_float				m_fTickPerSecond = {};
	_float				m_fCurrentTrackPosition = {};
	_uint				m_iNumChannels = {};

	_bool				m_bAnimChange = {};
	_bool				m_bAnimChangeFirstCall = {};
	_float				m_fAnimChangeTime = {};
	_float				m_fAnimChangeDelay = {};
	_vector				m_fStartRootPosition = {};

	vector<CChannel*>	m_Channels;
	vector<_uint>		m_CurrentKeyFrameIndices;


public:
	static CAnimation*	Create(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones);
	static CAnimation*	Create(ifstream& File, const vector<CBone*>& Bones);
	CAnimation*			Clone();
	virtual void		Free() override;
};

NS_END