#include "ClientPch.h"
#include "Vampire.h"
#include "VampireAI.h"
#include "Navigation.h"
#include "Body.h"

CVampire::CVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CVampire::CVampire(const CVampire& Prototype)
	: CMonster { Prototype }
	, m_iStrongFlag { Prototype.m_iStrongFlag }
{
}

HRESULT CVampire::Initialize_Prototype()
{
	m_iStrongFlag = ENUM_CLASS(HIT_FLAG::STRONG_BEGIN) | ENUM_CLASS(HIT_FLAG::STRONG_DURING) | ENUM_CLASS(HIT_FLAG::STRONG_END);

	m_Status.fFullHealth = 100.f;
	m_Status.fHealth = 100.f;
	
	return S_OK;
}

HRESULT CVampire::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_AI()))
		return E_FAIL;



	return S_OK;
}

void CVampire::Priority_Update(_float fTimeDelta)
{
}

void CVampire::Update(_float fTimeDelta)
{
}

void CVampire::Late_Update(_float fTimeDelta)
{
}

HRESULT CVampire::Render()
{
	return S_OK;
}

HRESULT CVampire::Spawn(MONSTER_SPAWN_DATA SpawnData)
{
	m_IsActive = true;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	m_pBody->Forcing_Play_Animation();

	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(SpawnData.iCellIndex);

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&SpawnData.vPosition), 1.f);
	
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	EnableAllColliderChannel();

	return S_OK;
}

BT_STATE CVampire::Is_Dead()
{
	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::DEAD))
	{
		if (m_pBody->AnimIsFinished())
		{
			m_IsActive = false;
		}
		return BT_STATE::SUCCESS;
	}
	return BT_STATE::FAILED;
}

BT_STATE CVampire::Is_Hit()
{
	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::HIT))
	{
		if(m_pBody->AnimIsFinished())
		{
			_uint iStrongFlag = {};
			iStrongFlag = m_iStateFlag & m_iStrongFlag;
			if (iStrongFlag)
			{
				m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT) | iStrongFlag << 1;
			}
			else
			{
				m_iHitAttackID = 0;
				return BT_STATE::FAILED;
			}
		}
		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CVampire::Attack()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_AttackTime[m_iCurrentAttack] = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CVampire::Chase()
{
	_float fDistance = Get_TargetDistance();

	if (abs(fDistance) >= m_fChaseRange)
	{
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::FRONT);

		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CVampire::Patrol()
{
	_float fDistance = Get_TargetDistance();// XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (abs(fDistance) <= m_fMinDistance)
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | ENUM_CLASS(MOVE_FLAG::BACK);
	else
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE) | (rand() % 2 == 0 ? ENUM_CLASS(MOVE_FLAG::LEFT) : ENUM_CLASS(MOVE_FLAG::RIGHT));

	return BT_STATE::SUCCESS;
}

HRESULT CVampire::Ready_AI()
{
	m_pAI = CVampireAI::Create(this);

	if (nullptr == m_pAI)
		return E_FAIL;

	return S_OK;
}

void CVampire::Compute_AnimPosition()
{
	_vector vAnimPosition = XMVectorSetY(*m_pAnimMovement, 0.f);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);

	m_pTransformCom->MovePositionToMatrix(PositionMatrix, m_pNavigationCom);
}

void CVampire::OnCollisionHit(const CCollider::COLLISION_DATA& CollisionData)
{
	if (nullptr == CollisionData.pDesc)
		return;

	ATTACK_DATA* AttackData = static_cast<ATTACK_DATA*>(CollisionData.pDesc);

	if (m_iHitAttackID == AttackData->iAttackID)
		return;

	m_iHitAttackID = AttackData->iAttackID;

	
	ATTACK_TYPE eAttackType = AttackData->eAttackType;
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vAttackPosition = AttackData->vAttackPosition;

	m_Status.fHealth -= AttackData->fDamage;

	if (m_Status.fHealth <= 0.f)
		ChangeDeadState(eAttackType, vPosition, vAttackPosition);
	else
		ChangeHitState(eAttackType, vPosition, vAttackPosition);

	if (FAILED(m_pBody->Forcing_Play_Animation()))
		return;
}

void CVampire::ChangeHitState(ATTACK_TYPE eAttackType, _fvector vHitPosition, _fvector vAttackPosition)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	switch (eAttackType)
	{
	case ATTACK_TYPE::LIGHT:
	case ATTACK_TYPE::MIDDLE:
	{
		_float fDegree = 60.f;

		HIT_DIR eHitDir = Compute_HitDir(vHitPosition, vAttackPosition, fDegree);
		_uint iDirFlag = ENUM_CLASS(HIT_FLAG::FRONT) << ENUM_CLASS(eHitDir);
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT) | iDirFlag;
		break;
	}
	case ATTACK_TYPE::STRONG:
	{
		m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT) | ENUM_CLASS(HIT_FLAG::STRONG_BEGIN);
		_vector vTargetPos = vAttackPosition;
		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)));

		m_pTransformCom->LookAt(vTargetPos);
		break;
	}
	}
}

void CVampire::ChangeDeadState(ATTACK_TYPE eAttackType, _fvector vHitPosition, _fvector vAttackPosition)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::DEAD);

	DisableAllColliderChannel();

	switch (eAttackType)
	{
	case ATTACK_TYPE::LIGHT:
	case ATTACK_TYPE::MIDDLE:
	{
		m_iStateFlag |= ENUM_CLASS(DEAD_FLAG::DEFAULT);
		break;
	}
	case ATTACK_TYPE::STRONG:
	{
		_vector vTargetPos = vAttackPosition;
		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)));

		m_pTransformCom->LookAt(vTargetPos);

		m_iStateFlag |= ENUM_CLASS(DEAD_FLAG::STRONG);
		break;
	}
	}
}
void CVampire::Free()
{
	__super::Free();

}
