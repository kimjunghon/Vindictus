#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	_matrix				Get_CombinedTransformationMatrix() { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	void				Set_CombinedTransformationMatrix(_fmatrix Matrix) { XMStoreFloat4x4(&m_CombinedTransformationMatrix, Matrix); }
	const _float4x4*	Get_CombinedTransformationMatrixPtr() { return &m_CombinedTransformationMatrix; }

	void				Set_TransformationMatrix(_fmatrix Matrix) { XMStoreFloat4x4(&m_TransformationMatrix, Matrix); }
	_matrix				Get_TransformationMatrix() { return XMLoadFloat4x4(&m_TransformationMatrix); }

	_matrix				Get_PoseCombinedTransformationMatrix() { return XMLoadFloat4x4(&m_PoseCombinedTransformationMatrix); }

	_bool				Compare_Name(const _char* pName) { return !strcmp(pName, m_szName); }

	const _char*		Get_Name() { return m_szName; }
	const _int			Get_ParentBoneIndex() { return m_iParentBoneIndex; }
public:
	HRESULT				Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT				Initialize(ifstream& File, _int iParentBoneIndex);
	void				Update_CombinedTransformationMatrix(const _float4x4& PreTransformationMatrix, const vector<CBone*>& Bones);

	void				Update_PoseCombinedTransformationMatrix(const _float4x4& PreTransformationMatrix, const vector<CBone*>& Bones, const _float4x4& OffsetMatrix);

private:
	_char			m_szName[MAX_PATH] = {};
	_float4x4		m_TransformationMatrix = {};
	_float4x4		m_CombinedTransformationMatrix = {};
	_int			m_iParentBoneIndex = { -1 };

	_float4x4		m_PoseTransformationMatrix = {};
	_float4x4		m_PoseCombinedTransformationMatrix = {};

public:
	static CBone*	Create(const aiNode* pAINode, _int iParentBoneIndex);
	static CBone*	Create(ifstream& File, _int iParentBoneIndex);
	CBone*			Clone();
	virtual void	Free() override;
};

NS_END