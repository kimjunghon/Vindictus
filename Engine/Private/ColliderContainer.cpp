#include "EnginePch.h"
#include "ColliderContainer.h"
#include "GameInstance.h"

CColliderContainer::CColliderContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent { pDevice, pDeviceContext }
{
}

CColliderContainer::CColliderContainer(const CColliderContainer& Prototype)
	: CComponent { Prototype }
{
}

HRESULT CColliderContainer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CColliderContainer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CColliderContainer::Update(CGameObject* pOwner, _fmatrix WorldMatrix)
{
	for (auto& iChannel : m_ColliderChannels)
	{
		_uint iIndex = {};
		for (auto& pCollider : m_Colliders[iChannel])
		{
			Update_Collider(iChannel, iIndex, pCollider, WorldMatrix, pOwner);
			iIndex++;
		}
	}
}

HRESULT CColliderContainer::Add_Collider(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iChannel, _uint iOwner, CBounding::BOUNDING_DESC* pDesc, const _float4x4* pBoneMatrix)
{
	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = iChannel;
	ColliderDesc.iOwner = iOwner;
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pCollider = { nullptr };
	pCollider = static_cast<CCollider*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelIndex, strPrototypeTag, &ColliderDesc));

	if (nullptr == pCollider)
		return E_FAIL;

	vector<CCollider*>* pColliders = Find_Colliders(iChannel);
	if (pColliders == nullptr)
	{
		vector<CCollider*> Colliders;
		Colliders.push_back(pCollider);

		m_Colliders.emplace(iChannel, Colliders);
	}
	else
		pColliders->push_back(pCollider);

	if (FAILED(Add_BoneMatrix(iChannel, pBoneMatrix)))
		return E_FAIL;

	if (FAILED(Add_CombineMatrix(iChannel)))
		return E_FAIL;

	m_ColliderChannels.insert(iChannel);

	return S_OK;
}

HRESULT CColliderContainer::Bind_Collision_Callback(_uint iChannel, _uint iColliderIndex, COLLIDER_STATE eState, CCollider::Collision_CallBack Callback)
{
	if (nullptr == m_Colliders[iChannel][iColliderIndex])
		return E_FAIL;

	m_Colliders[iChannel][iColliderIndex]->SetCollisionCallBack(ENUM_CLASS(eState), Callback);

	return S_OK;
}

void CColliderContainer::SetDesc(_uint iChannel, _uint iColliderIndex, void* pDesc)
{
	if (nullptr == m_Colliders[iChannel][iColliderIndex])
		return;

	m_Colliders[iChannel][iColliderIndex]->Set_Desc(pDesc);
}

void CColliderContainer::SetEnable(_uint iChannel, _uint iColliderIndex, _bool IsEnable)
{
	if (nullptr == m_Colliders[iChannel][iColliderIndex])
		return;

	m_Colliders[iChannel][iColliderIndex]->SetEnable(IsEnable);
}

void CColliderContainer::SetEnableAllColliderChannel(_bool IsEnable)
{
	for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
			pCollider->SetEnable(IsEnable);
	}
}

void CColliderContainer::SetEnableColliderChannel(_uint iChannel, _bool IsEnable)
{
	for (auto& pCollider : m_Colliders[iChannel])
		pCollider->SetEnable(IsEnable);
}

HRESULT CColliderContainer::Change_Collider_BoneMatrix(_uint iChannel, _uint iColliderIndex, const _float4x4* pChangeBoneMatrix)
{
	if (nullptr == m_Colliders[iChannel][iColliderIndex])
		return E_FAIL;

	m_ColliderBoneMatrices[iChannel][iColliderIndex] = pChangeBoneMatrix;

	return S_OK;
}

void CColliderContainer::Update_Collider(_uint iChannel, _uint iIndex, CCollider* pCollider, _fmatrix WorldMatrix, CGameObject* pOwner)
{
	const _float4x4* pBoneMatrix = m_ColliderBoneMatrices[iChannel][iIndex];
	_matrix	CombinedMatrix = {};

	if (pBoneMatrix)
		CombinedMatrix = XMMatrixMultiply(XMLoadFloat4x4(pBoneMatrix), WorldMatrix);
	else
		CombinedMatrix = WorldMatrix;

	pCollider->Update(CombinedMatrix);
	m_pGameInstance->Add_Collider(iChannel, pOwner, pCollider);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(pCollider);
#endif
}

HRESULT CColliderContainer::Add_BoneMatrix(_uint iChannel, const _float4x4* pBoneMatrix)
{
	vector<const _float4x4*>* pBoneMatrices = Find_BoneMatrices(iChannel);
	if (pBoneMatrices == nullptr)
	{
		vector<const _float4x4*> BoneMatrices;

		BoneMatrices.push_back(pBoneMatrix);

		m_ColliderBoneMatrices.emplace(iChannel, BoneMatrices);
	}
	else
		pBoneMatrices->push_back(pBoneMatrix);

	return S_OK;
}

HRESULT CColliderContainer::Add_CombineMatrix(_uint iChannel)
{
	vector<_matrix>* pCombinedMatrices = Find_CombineMatrices(iChannel);
	if (pCombinedMatrices == nullptr)
	{
		vector<_matrix> CombinedMatrices;

		CombinedMatrices.push_back(XMMatrixIdentity());

		m_ColliderCombinedMatrices.emplace(iChannel, CombinedMatrices);
	}
	else
		pCombinedMatrices->push_back(XMMatrixIdentity());

	return S_OK;
}

vector<CCollider*>* CColliderContainer::Find_Colliders(_uint iChannel)
{
	auto iter = m_Colliders.find(iChannel);

	if (iter == m_Colliders.end())
		return nullptr;

	return &(iter->second);
}

vector<const _float4x4*>* CColliderContainer::Find_BoneMatrices(_uint iChannel)
{
	auto iter = m_ColliderBoneMatrices.find(iChannel);

	if (iter == m_ColliderBoneMatrices.end())
		return nullptr;

	return &(iter->second);
}

vector<_matrix>* CColliderContainer::Find_CombineMatrices(_uint iChannel)
{
	auto iter = m_ColliderCombinedMatrices.find(iChannel);

	if (iter == m_ColliderCombinedMatrices.end())
		return nullptr;

	return &(iter->second);
}

CColliderContainer* CColliderContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CColliderContainer* pInstance = new CColliderContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CColliderContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CColliderContainer::Clone(void* pArg)
{
	CColliderContainer* pInstance = new CColliderContainer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CColliderContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColliderContainer::Free()
{
	__super::Free();

	for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
		{
			Safe_Release(pCollider);
		}
	}
}
