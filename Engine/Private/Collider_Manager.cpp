#include "EnginePch.h"
#include "Collider_Manager.h"
#include "GameObject.h"

CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Initialize()
{
	return S_OK;
}

void CCollider_Manager::Update()
{
	for (auto& SrcPair : m_Boundings)
	{
		for (auto& DstPair : m_Boundings)
		{
			if (SrcPair.first == DstPair.first)
				continue;

			auto iter = m_CheckCollisions.find({ SrcPair.first ,DstPair.first });

			if (iter != m_CheckCollisions.end())
				continue;

			if (Intersect_Bounding(SrcPair.second, DstPair.second))
			{
				if (Has_ActionCollider(SrcPair.first) && Has_ActionCollider(DstPair.first))
				{
					Intersect_Actions(m_Actions[SrcPair.first], m_Actions[DstPair.first]);
					m_CheckCollisions.emplace(SrcPair.first, DstPair.first);
				}
			}
		}
	}

	m_CheckCollisions.clear();
	Clear_Collider();
}

HRESULT CCollider_Manager::Add_Collider(_uint iChannel, CGameObject* pOwner, CCollider* pCollider)
{
	if (m_iBoundingChannel == iChannel)
		return Add_BoundingCollider(pOwner, pCollider);
	else
		return Add_ActionCollider(pOwner, pCollider);
}

HRESULT CCollider_Manager::Add_Channel(_uint iSrcChannel, _uint iDstChannel, COLLIDER_TYPE eType)
{
	if (Check_Type(iSrcChannel, iDstChannel) != COLLIDER_TYPE::END)
		return E_FAIL;

	m_Channels.emplace(make_pair(iSrcChannel, iDstChannel), eType);

	return S_OK;
}

HRESULT CCollider_Manager::Change_Type_ToChannel(_uint iSrcChannel, _uint iDstChannel, COLLIDER_TYPE eChangeType)
{
	if (Check_Type(iSrcChannel, iDstChannel) == COLLIDER_TYPE::END)
		return E_FAIL;

	m_Channels[make_pair(iSrcChannel, iDstChannel)] = eChangeType;

	return S_OK;
}

HRESULT CCollider_Manager::Add_BoundingCollider(CGameObject* pOwner, CCollider* pBounding_Collider)
{
	if (nullptr == pOwner || nullptr == pBounding_Collider)
		return E_FAIL;

	if (nullptr != Find_BoundingCollider(pOwner))
		return E_FAIL;

	m_Boundings.emplace(pOwner, pBounding_Collider);

	Safe_AddRef(pOwner);
	Safe_AddRef(pBounding_Collider);

	return S_OK;
}

HRESULT CCollider_Manager::Add_ActionCollider(CGameObject* pOwner, CCollider* pAction_Collider)
{
	if (nullptr == pOwner || nullptr == pAction_Collider)
		return E_FAIL;

	auto iter = m_Actions.find(pOwner);
	if(iter == m_Actions.end())
		Safe_AddRef(pOwner);

	m_Actions[pOwner].push_back(pAction_Collider);
	Safe_AddRef(pAction_Collider);

	return S_OK;
}

void CCollider_Manager::Clear_Collider()
{
	for (auto& BoundingPair : m_Boundings)
	{
		Safe_Release(const_cast<CGameObject*&>(BoundingPair.first));
		Safe_Release(BoundingPair.second);
	}

	m_Boundings.clear();

	for (auto& ActionPair : m_Actions)
	{
		Safe_Release(const_cast<CGameObject*&>(ActionPair.first));
		for (auto& pCollider : ActionPair.second)
			Safe_Release(pCollider);
	}

	m_Actions.clear();
}

COLLIDER_TYPE CCollider_Manager::Check_Type(_uint iSrcChannel, _uint iDstChannel)
{
	auto iter = m_Channels.find(make_pair(iSrcChannel, iDstChannel));
	if (iter == m_Channels.end())
		return COLLIDER_TYPE::END;

	return iter->second;
}

_bool CCollider_Manager::Intersect_Bounding(CCollider* pSrcCollider, CCollider* pDstCollider)
{
	_uint iSrcChannel = {};
	_uint iDstChannel = {};

	iSrcChannel = pSrcCollider->Get_ColliderChannel();
	iDstChannel = pDstCollider->Get_ColliderChannel();

	if (Check_Type(iSrcChannel, iDstChannel) == COLLIDER_TYPE::NONE)
		return false;

	return pSrcCollider->Intersect_BoundingChannel(pDstCollider);
}

void CCollider_Manager::Intersect_Actions(const vector<CCollider*>& SrcColliders, const vector<CCollider*>& DstColliders)
{
	for (auto& pSrcCollider : SrcColliders)
	{
		for (auto& pDstCollider : DstColliders)
		{
			_uint iSrcChannel = {};
			_uint iDstChannel = {};

			iSrcChannel = pSrcCollider->Get_ColliderChannel();
			iDstChannel = pDstCollider->Get_ColliderChannel();

			if(Check_Type(iSrcChannel, iDstChannel) == COLLIDER_TYPE::NONE || Check_Type(iSrcChannel, iDstChannel) == COLLIDER_TYPE::END)
				continue;

			if (pSrcCollider->Get_ColliderOwner() == pDstCollider->Get_ColliderOwner())
			{
				// 같은 Owner 타입이면 BLOCK 처리만
				if (Check_Type(iSrcChannel, iDstChannel) != COLLIDER_TYPE::BLOCK)
					continue;
			}

			Check_Collision(pSrcCollider, pDstCollider);
		}
	}

}

void CCollider_Manager::Check_Collision(CCollider* pSrcCollider, CCollider* pDstCollider)
{
	CCollider::COLLISION_DATA DstData = {};
	DstData.IsEnable = pDstCollider->IsEnable();
	DstData.pCollider = pDstCollider;
	DstData.pDesc = pDstCollider->Get_Desc();

	CCollider::COLLISION_DATA SrcData = {};

	SrcData.IsEnable = pSrcCollider->IsEnable();
	SrcData.pCollider = pSrcCollider;
	SrcData.pDesc = pSrcCollider->Get_Desc();

	pSrcCollider->Check_Collision(DstData);
	pDstCollider->Check_Collision(SrcData);
}

const CCollider* CCollider_Manager::Find_BoundingCollider(CGameObject* pOwner)
{
	auto iter = m_Boundings.find(pOwner);
	if (iter == m_Boundings.end())
		return nullptr;

	return iter->second;
}

_bool CCollider_Manager::Has_ActionCollider(CGameObject* pOwner)
{
	auto iter = m_Actions.find(pOwner);
	if (iter == m_Actions.end())
		return false;

	if (iter->second.empty())
		return false;

	return true;
}

CCollider_Manager* CCollider_Manager::Create()
{
	CCollider_Manager* pInstance = new CCollider_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CCollider_Manager"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider_Manager::Free()
{
	__super::Free();

	Clear_Collider();
}
