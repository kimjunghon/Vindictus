#include "EnginePch.h"
#include "Collider_Manager.h"
#include "GameObject.h"
#include "Collider.h"

CCollider_Manager::CCollider_Manager()
{
}

HRESULT CCollider_Manager::Initialize()
{
	if (FAILED(Ready_ResponseTable()))
		return E_FAIL;

	return S_OK;
}

void CCollider_Manager::Update()
{
	for (auto& SrcPair : m_Boundings)
	{
		for (auto& DstPair : m_Boundings)
		{
			if (SrcPair.second == DstPair.second)
				continue;

			if (Intersect_Bounding(SrcPair.second, DstPair.second))
			{
				if(Has_ActionCollider(SrcPair.first) && Has_ActionCollider(DstPair.first))
					Check_Collision(m_Actions[SrcPair.first], m_Actions[DstPair.first], DstPair.first);
			}
		}
	}

	Clear_Collider();
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
		ActionPair.second.clear();
	}

	m_Actions.clear();
}

_bool CCollider_Manager::Intersect_Bounding(CCollider* pSrcCollider, CCollider* pDstCollider)
{
	if (pSrcCollider->Get_ColliderChannel() != COLLIDER_CHANNEL::BOUNDING || pDstCollider->Get_ColliderChannel() != COLLIDER_CHANNEL::BOUNDING)
		return false;

	return pSrcCollider->Intersect(pDstCollider);
}

void CCollider_Manager::Check_Collision(const vector<CCollider*>& SrcColliders, const vector<CCollider*>& DstColliders, CGameObject* pDstOwner)
{
	for (auto& pSrcCollider : SrcColliders)
	{
		for (auto& pDstCollider : DstColliders)
		{
			COLLIDER_CHANNEL eSrcChannel = {};
			COLLIDER_CHANNEL eDstChannel = {};

			eSrcChannel = pSrcCollider->Get_ColliderChannel();
			eDstChannel = pDstCollider->Get_ColliderChannel();

			if (m_eResponseTable[ENUM_CLASS(eSrcChannel)][ENUM_CLASS(eDstChannel)] == COLLIDER_TYPE::NONE)
				continue;

			if (pSrcCollider->Get_ColliderOwner() == pDstCollider->Get_ColliderOwner())
			{
				// 같은 Owner 타입이면 Body Channel만만
				if (pSrcCollider->Get_ColliderChannel() == COLLIDER_CHANNEL::BODY && pDstCollider->Get_ColliderChannel() == COLLIDER_CHANNEL::BODY)
				{
					if (pSrcCollider->Intersect(pDstCollider))
						OnCollision(pSrcCollider, pDstCollider, pDstOwner);
				}
			}
			else
			{
				if (pSrcCollider->Intersect(pDstCollider))
					OnCollision(pSrcCollider, pDstCollider, pDstOwner);
			}
		}
	}
}

void CCollider_Manager::OnCollision(CCollider* pSrcCollider, CCollider* pDstCollider, CGameObject* pDstOwner)
{
	CCollider::COLLISION_DATA Data = {};
	Data.pOwner = pDstOwner;
	Data.pCollider = pDstCollider;
	Data.IsAttack = false;

	if (pSrcCollider->Get_ColliderChannel() == COLLIDER_CHANNEL::HIT)
	{
		Data.IsAttack = true;
		CCollider::ATTACK_COLLISON_DATA AttackData = {};
		AttackData = pDstCollider->GetAttackData();
	}
	
	pSrcCollider->OnCollision(Data);
}

HRESULT CCollider_Manager::Ready_ResponseTable()
{
	//BOUNDING
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING)][ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING)] = COLLIDER_TYPE::OVERLAP;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING)][ENUM_CLASS(COLLIDER_CHANNEL::BODY)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING)][ENUM_CLASS(COLLIDER_CHANNEL::ATTACK)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING)][ENUM_CLASS(COLLIDER_CHANNEL::HIT)] = COLLIDER_TYPE::NONE;

	//BODY
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::BODY)][ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::BODY)][ENUM_CLASS(COLLIDER_CHANNEL::BODY)] = COLLIDER_TYPE::BLOCK;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::BODY)][ENUM_CLASS(COLLIDER_CHANNEL::ATTACK)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::BODY)][ENUM_CLASS(COLLIDER_CHANNEL::HIT)] = COLLIDER_TYPE::NONE;

	//ATTACK
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::ATTACK)][ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::ATTACK)][ENUM_CLASS(COLLIDER_CHANNEL::BODY)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::ATTACK)][ENUM_CLASS(COLLIDER_CHANNEL::ATTACK)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::ATTACK)][ENUM_CLASS(COLLIDER_CHANNEL::HIT)] = COLLIDER_TYPE::OVERLAP;

	//HIT
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::HIT)][ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::HIT)][ENUM_CLASS(COLLIDER_CHANNEL::BODY)] = COLLIDER_TYPE::NONE;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::HIT)][ENUM_CLASS(COLLIDER_CHANNEL::ATTACK)] = COLLIDER_TYPE::OVERLAP;
	m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::HIT)][ENUM_CLASS(COLLIDER_CHANNEL::HIT)] = COLLIDER_TYPE::NONE;

	return S_OK;
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
