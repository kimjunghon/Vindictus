#include "EnginePch.h"
#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
	strcpy_s(m_szName, pAINode->mName.data);

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentBoneIndex;

	m_PoseTransformationMatrix = m_TransformationMatrix;

	return S_OK;
}

HRESULT CBone::Initialize(ifstream& File, _int iParentBoneIndex)
{
	size_t iNameLength = {};
	
	File.read(reinterpret_cast<_char*>(&iNameLength), sizeof(size_t));
	File.read(m_szName, sizeof(_char) * iNameLength);
	File.read(reinterpret_cast<_char*>(&m_TransformationMatrix), sizeof(_float4x4));	

	m_iParentBoneIndex = iParentBoneIndex;

	m_PoseTransformationMatrix = m_TransformationMatrix;

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const _float4x4& PreTransformationMatrix, const vector<CBone*>& Bones)
{
	if (m_iParentBoneIndex == -1)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixMultiply(XMLoadFloat4x4(&PreTransformationMatrix), XMLoadFloat4x4(&m_TransformationMatrix)));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_TransformationMatrix), Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix()));
}

void CBone::Update_PoseCombinedTransformationMatrix(const _float4x4& PreTransformationMatrix, const vector<CBone*>& Bones, const _float4x4& OffsetMatrix)
{
	if (m_iParentBoneIndex == -1)
		XMStoreFloat4x4(&m_PoseCombinedTransformationMatrix, XMMatrixMultiply(XMLoadFloat4x4(&PreTransformationMatrix), XMLoadFloat4x4(&m_PoseTransformationMatrix)));
	else
	{
		if(m_iParentBoneIndex == 2)
			XMStoreFloat4x4(&m_PoseCombinedTransformationMatrix, XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&OffsetMatrix), XMLoadFloat4x4(&m_PoseTransformationMatrix)), Bones[m_iParentBoneIndex]->Get_PoseCombinedTransformationMatrix()));
		else
			XMStoreFloat4x4(&m_PoseCombinedTransformationMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_PoseTransformationMatrix),Bones[m_iParentBoneIndex]->Get_PoseCombinedTransformationMatrix()));
	}
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();
	if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
	{
		MSG_BOX(TEXT("Failed Created : CBone"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone* CBone::Create(ifstream& File, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();
	if (FAILED(pInstance->Initialize(File, iParentBoneIndex)))
	{
		MSG_BOX(TEXT("Failed Created : CBone"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
