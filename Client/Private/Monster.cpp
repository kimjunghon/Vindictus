#include "ClientPch.h"
#include "Monster.h"
#include "Body.h"
#include "BehaviorTree.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CColliderPawn{ pDevice, pDeviceContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CColliderPawn{ Prototype }
	, m_iNumAttacks { Prototype.m_iNumAttacks }
	, m_AttackCoolTime{ Prototype.m_AttackCoolTime }
	, m_AttackTime{ Prototype.m_AttackTime }
	, m_fAttackRange{ Prototype.m_fAttackRange }
	, m_fChaseRange{ Prototype.m_fChaseRange }
	, m_fMinDistance{ Prototype.m_fMinDistance }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	if (nullptr == m_pTargetTransform)
		return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
}

void CMonster::Update(_float fTimeDelta)
{
}

void CMonster::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

BT_STATE CMonster::CanAttack()
{
	for (_uint i = 0; i < m_iNumAttacks; i++)
	{
		if (m_AttackTime[i] >= m_AttackCoolTime[i])
		{
			m_iCurrentAttack = i;
			return BT_STATE::SUCCESS;
		}
	}

	return BT_STATE::FAILED;
}

BT_STATE CMonster::CanOtherAction()
{
	if (m_pBody->AnimCanChange() || m_pBody->AnimIsFinished())
		return BT_STATE::FAILED;

	return BT_STATE::RUN;
}

BT_STATE CMonster::CanAttackRange()
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) <= m_fAttackRange)
		return BT_STATE::SUCCESS;

	return BT_STATE::FAILED;
}


BT_STATE CMonster::Chase()
{
	return BT_STATE();
}

BT_STATE CMonster::Patrol()
{
	return BT_STATE();
}

void CMonster::Update_AttackColliders(_uint iStateFlag)
{
	if (m_AttackComplete)
		return;

	auto iter = m_AttackMapping.find(iStateFlag);
	if (iter == m_AttackMapping.end())
		return;

	for (auto& AttackMap : iter->second)
	{
		if (false == m_pBody->IsAnimationInRage(AttackMap.vAttackRange))
			continue;

		CCollider::ATTACK_COLLISON_DATA AttackData = {};
		AttackData.IsDown = AttackMap.IsDown;
		AttackData.fDamage = 10.f;
		//	AttackData.fDamage = m_fAttackData * AttackMap.fAttackRatio;
		AttackData.vAttackPosition = m_pTransformCom->Get_State(STATE::POSITION);

		_uint iAttackIndex = AttackMap.iAttackColliderIndex;

		m_AttackColliderCombinedMatrix[iAttackIndex] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[iAttackIndex]), m_pTransformCom->Get_WorldMatrix());
		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->Update(m_AttackColliderCombinedMatrix[iAttackIndex]);

		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->SetEnable(true);
		m_pGameInstance->Add_ActionCollider(this, m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]);

		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->SetAttackData(AttackData);
	}
}

void CMonster::Bind_HitCollisionCallback(_uint iHitCollisionIndex)
{
}

void CMonster::Bind_AttackCollisionCallback(_uint iAttackCollisionIndex)
{
	if (iAttackCollisionIndex >= m_Colliders[COLLIDER_CHANNEL::ATTACK].size())
		return;

	m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackCollisionIndex]->SetCollisionCallBack([this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionAttack(Data);
		});
}

void CMonster::OnCollisionAttack(const CCollider::COLLISION_DATA& CollisionData)
{
	m_AttackComplete = true;
}

void CMonster::Update_AttackCoolTime(_float fTimeDelta)
{
	for (auto& AttackTime : m_AttackTime)
		AttackTime += fTimeDelta;

	if (m_AttackComplete && m_pBody->AnimIsFinished())
		m_AttackComplete = false;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pAI);
}
