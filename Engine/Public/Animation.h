#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone;
class CChannel;

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	HRESULT Initialize(ifstream& File, const vector<class CBone*>& Bones);
	_bool	Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta);

private:
	_float				m_fDuration = {};
	_float				m_fTickPerSecond = {};
	_float				m_fCurrentTrackPosition = {};
	_uint				m_iNumChannels = {};
	vector<CChannel*>	m_Channels;

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones);
	static CAnimation* Create(ifstream& File, const vector<CBone*>& Bones);
	virtual void Free() override;
};

NS_END