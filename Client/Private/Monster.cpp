#include "ClientPch.h"
#include "Monster.h"
#include "Body.h"
#include "BehaviorTree.h"
#include "MonsterState.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CColliderPawn{ pDevice, pDeviceContext }
	, m_pMonsterInstance {CMonsterInstance::GetInstance()}
{
	Safe_AddRef(m_pMonsterInstance);
}

CMonster::CMonster(const CMonster& Prototype)
	: CColliderPawn{ Prototype }
	, m_iNumAttacks { Prototype.m_iNumAttacks }
	, m_AttackCoolTime{ Prototype.m_AttackCoolTime }
	, m_AttackTime{ Prototype.m_AttackTime }
	, m_fAttackRange{ Prototype.m_fAttackRange }
	, m_fChaseRange{ Prototype.m_fChaseRange }
	, m_fMinDistance{ Prototype.m_fMinDistance }
	, m_pMonsterInstance{ Prototype.m_pMonsterInstance }
	, m_eType { Prototype.m_eType }
	, m_Status { Prototype.m_Status }
{
	Safe_AddRef(m_pMonsterInstance);
}

void CMonster::Bind_StateFlag()
{
	if(m_pCurrentState)
		m_pCurrentState->Bind_StateFlag(m_iStateFlag);
}

_float CMonster::Get_TargetDistance()
{
	if (nullptr == m_pTargetTransform)
		return 0.f;

	_vector vTargetPos = XMVectorSetY(m_pTargetTransform->Get_State(STATE::POSITION), 0.f);
	_vector vPosition = XMVectorSetY(m_pTransformCom->Get_State(STATE::POSITION), 0.f);

	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(vTargetPos, vPosition)));
	
	return fDistance;
}

DIR  CMonster::Compute_TargetDir(_float fDegree)
{
	if (nullptr == m_pTargetTransform)
		return DIR::END;

	return	Compute_HitDir(m_pTransformCom->Get_State(STATE::POSITION), m_pTargetTransform->Get_State(STATE::POSITION), fDegree);
}

void CMonster::LookAtTarget()
{
	if (nullptr == m_pTargetTransform)
		return;

	_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)));

	m_pTransformCom->LookAt(vTargetPos);
}

void CMonster::MoveToTarget(_float fRatio)
{
	_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
	_vector vDir = XMVectorSetY(XMVectorSubtract(vTargetPos, m_pTransformCom->Get_State(STATE::POSITION)), 0.f);

	m_pTransformCom->LookAt(vTargetPos);

	_vector vPosition = XMVectorScale(vDir, fRatio);

	m_pTransformCom->MovePositionToVector(vPosition, m_pNavigationCom);
}

_bool CMonster::IsAnimationInRangeTrackPosition(_float2 vRange)
{
	return m_pBody->IsAnimationInRangeTrackPosition(vRange);
}

_bool CMonster::IsReadyAttack(_uint iStateFlag)
{
	if (m_AttackMapping[iStateFlag].empty())
		return false;

	_float2 vRange = m_AttackMapping[iStateFlag].front().vAttackRange;

	_float2 vReadyRange = _float2(0.f, vRange.x);

	return IsAnimationInRangeTrackPosition(vReadyRange);
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

_bool CMonster::IsNear(_float fNearDistance)
{
	_float fDistance = Get_TargetDistance();

	if (abs(fDistance) <= fNearDistance)
		return true;
	
	return false;
}

_bool CMonster::AnimIsFinished()
{
	return m_pBody->AnimIsFinished();
}

_bool CMonster::AnimCanChange()
{
	return m_pBody->AnimCanChange();
}

_bool CMonster::CanChangeState()
{
	if (nullptr == m_pCurrentState)
		return true;

	return m_pCurrentState->CanStateChange(this);
}

HRESULT CMonster::ChangeState(_uint iStateIndex)
{
	if (nullptr == m_States[iStateIndex])
		return E_FAIL;

	if (m_pCurrentState)
		m_pCurrentState->Exit(this);

	m_pCurrentState = m_States[iStateIndex];

	m_pCurrentState->Enter(this);

	return S_OK;
}

BT_STATE CMonster::CanAttack()
{
	for (_uint i = 0; i < m_iNumAttacks; i++)
	{
		if (m_AttackTime[i] >= m_AttackCoolTime[i])
		{
			m_iCurrentAttack = i;

			cout << m_iCurrentAttack << endl;
			return BT_STATE::SUCCESS;
		}
	}

	return BT_STATE::FAILED;
}

BT_STATE CMonster::CanOtherAction()
{
//	if (m_pBody->AnimCanChange() || m_pBody->AnimIsFinished())
	if(CanChangeState())
		return BT_STATE::FAILED;

	return BT_STATE::RUN;
}

BT_STATE CMonster::CanAttackRange()
{
	if(IsNear(m_fAttackRange))
		return BT_STATE::SUCCESS;

	return BT_STATE::FAILED;
}

void CMonster::Update_AttackColliders(_fmatrix UpdateWorldMatrix, _uint iStateFlag)
{
	if (m_AttackComplete)
		return;

	auto iter = m_AttackMapping.find(iStateFlag);
	if (iter == m_AttackMapping.end())
		return;

	for (auto& AttackMap : iter->second)
	{
		_uint iAttackIndex = AttackMap.iAttackColliderIndex;

		if (false == m_pBody->IsAnimationInRangeTrackPosition(AttackMap.vAttackRange))
		{
			m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->SetEnable(false);
			m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->Set_Desc(nullptr);
			continue;
		}

		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->SetEnable(true);

		m_CurrentAttackData.iAttackID = m_iStateFlag + (reinterpret_cast<size_t>(this) << 1);
		m_CurrentAttackData.eAttackType = AttackMap.eAttackType;
		m_CurrentAttackData.fDamage = m_Status.fAttackDamage * AttackMap.fAttackRatio;
		m_CurrentAttackData.vAttackPosition = m_pTransformCom->Get_State(STATE::POSITION);

		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->Set_Desc(&m_CurrentAttackData);

		m_AttackColliderCombinedMatrix[iAttackIndex] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[iAttackIndex]), UpdateWorldMatrix);
		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->Update(m_AttackColliderCombinedMatrix[iAttackIndex]);

		m_pGameInstance->Add_ActionCollider(this, m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]);
		
	}
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

	for (auto& pState : m_States)
		Safe_Release(pState);

	Safe_Release(m_pAI);
	Safe_Release(m_pMonsterInstance);
}
