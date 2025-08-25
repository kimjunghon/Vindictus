#include "ClientPch.h"
#include "Vampire.h"
#include "VampireAI.h"
#include "Navigation.h"
#include "Body.h"
#include "MonsterStateFactory.h"

CVampire::CVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CVampire::CVampire(const CVampire& Prototype)
	: CMonster { Prototype }
{
}

HRESULT CVampire::Initialize_Prototype()
{
	m_Status.fFullHealth = 200.f;
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

	ChangeState(ENUM_CLASS(VAMPIRE_STATE::SPAWN));

	Bind_StateFlag();

	m_pBody->Forcing_Play_Animation();

	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(SpawnData.iCellIndex);

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&SpawnData.vPosition), 1.f);
	
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	EnableAllColliderChannel();

	return S_OK;
}

BT_STATE CVampire::Attack()
{
	ChangeState(ENUM_CLASS(VAMPIRE_STATE::ATTACK));

	m_AttackTime[m_iCurrentAttack] = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CVampire::Chase()
{
	_float fDistance = Get_TargetDistance();

	if (false == IsNear(m_fChaseRange))
	{
		ChangeState(ENUM_CLASS(VAMPIRE_STATE::MOVE));
		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CVampire::Patrol()
{
	ChangeState(ENUM_CLASS(VAMPIRE_STATE::PATROL));

	return BT_STATE::SUCCESS;
}

HRESULT CVampire::Ready_AI()
{
	m_pAI = CVampireAI::Create(this);

	if (nullptr == m_pAI)
		return E_FAIL;

	return S_OK;
}

HRESULT CVampire::Ready_VampireState(MONSTER_TYPE eType)
{
	m_States.resize(ENUM_CLASS(VAMPIRE_STATE::END), nullptr);

	CMonsterStateFactory* pStateFactory = CMonsterStateFactory::GetInstance();

	_uint iAttackStateType = {};

	iAttackStateType = eType == MONSTER_TYPE::VAMPIRE_ELDER ? ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE_ELDER) : ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE);

	m_States[ENUM_CLASS(VAMPIRE_STATE::ATTACK)] = pStateFactory->Create(iAttackStateType, ENUM_CLASS(VAMPIRE_STATE::ATTACK));

	m_States[ENUM_CLASS(VAMPIRE_STATE::SPAWN)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::SPAWN));
	m_States[ENUM_CLASS(VAMPIRE_STATE::IDLE)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::IDLE));
	m_States[ENUM_CLASS(VAMPIRE_STATE::MOVE)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::MOVE));
	m_States[ENUM_CLASS(VAMPIRE_STATE::PATROL)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::PATROL));
	m_States[ENUM_CLASS(VAMPIRE_STATE::HIT)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::HIT));
	m_States[ENUM_CLASS(VAMPIRE_STATE::HIT_STRONG)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::HIT_STRONG));
	m_States[ENUM_CLASS(VAMPIRE_STATE::DEAD)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::DEAD));
	m_States[ENUM_CLASS(VAMPIRE_STATE::DEAD_STRONG)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::DEAD_STRONG));

	m_pCurrentState = m_States[ENUM_CLASS(VAMPIRE_STATE::SPAWN)];

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
		ChangeDeadState(eAttackType);
	else
		ChangeHitState(eAttackType);

	Bind_StateFlag();

	if (FAILED(m_pBody->Forcing_Play_Animation()))
		return;
}

void CVampire::ChangeHitState(ATTACK_TYPE eAttackType)
{
	switch (eAttackType)
	{
	case ATTACK_TYPE::LIGHT:
	case ATTACK_TYPE::MIDDLE:
	{
		ChangeState(ENUM_CLASS(VAMPIRE_STATE::HIT));
		break;
	}
	case ATTACK_TYPE::STRONG:
	{
		ChangeState(ENUM_CLASS(VAMPIRE_STATE::HIT_STRONG));
		break;
	}
	}
}

void CVampire::ChangeDeadState(ATTACK_TYPE eAttackType)
{
	DisableAllColliderChannel();

	switch (eAttackType)
	{
	case ATTACK_TYPE::LIGHT:
	case ATTACK_TYPE::MIDDLE:
	{
		ChangeState(ENUM_CLASS(VAMPIRE_STATE::DEAD));
		break;
	}
	case ATTACK_TYPE::STRONG:
	{
		ChangeState(ENUM_CLASS(VAMPIRE_STATE::DEAD_STRONG));
		break;
	}
	}
}
void CVampire::Free()
{
	__super::Free();

}
