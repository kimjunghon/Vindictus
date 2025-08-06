#include "ClientPch.h"
#include "Monster.h"
#include "Body.h"
#include "BehaviorTree.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawn { pDevice, pDeviceContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CPawn { Prototype }
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

	m_pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LAYERTYPE::NONSTATIC), TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
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

void CMonster::Update_AttackCoolTime(_float fTimeDelta)
{
	for (auto& AttackTime : m_AttackTime)
		AttackTime += fTimeDelta;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pAI);
}
