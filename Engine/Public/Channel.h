#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone;

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones);
	HRESULT Initialize(ifstream& File, const vector<CBone*>& Bones);
	void	Update_TransformationMatrix(const vector<CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);
	void	Update_AnimChangeTransformationMatrix(const vector<CBone*>& Bones, _float fRatio, _bool* pFirstCall);
private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iBoneIndex = {};
	_uint				m_iNumKeyFrame = {};

	_vector				m_vChangePrevScale = {};
	_vector				m_vChangePrevRotation = {};
	_vector				m_vChangePrevPosition = {};

	vector<KEYFRAME>	m_KeyFrames;
	
public:
	static CChannel*	Create(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones);
	static CChannel*	Create(ifstream& File, const vector<CBone*>& Bones);
	virtual void		Free() override;
};

NS_END