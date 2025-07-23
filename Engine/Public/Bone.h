#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	_matrix			Get_CombinedTransformationMatrix() { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	void			Set_TransformationMatrix(_fmatrix Matrix) { XMStoreFloat4x4(&m_TransformationMatrix, Matrix); }
	_bool			Compare_Name(const _char* pName) { return !strcmp(pName, m_szName); }

public:
	HRESULT			Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT			Initialize(ifstream& File, _int iParentBoneIndex);
	void			Update_CombinedTransformationMatrix(const _float4x4& PreTransformationMatrix, const vector<CBone*>& Bones);

private:
	_char			m_szName[MAX_PATH] = {};
	_float4x4		m_TransformationMatrix = {};
	_float4x4		m_CombinedTransformationMatrix = {};
	_int			m_iParentBoneIndex = { -1 };

public:
	static CBone*	Create(const aiNode* pAINode, _int iParentBoneIndex);
	static CBone*	Create(ifstream& File, _int iParentBoneIndex);
	virtual void	Free() override;
};

NS_END