#include "ClientPch.h"
#include "Queen.h"
#include "Body.h"
#include "QueenAI.h"
#include "Queen_Body.h"
#include "MonsterStateFactory.h"
#include "MonsterState.h"
#include "Camera_CS.h"
#include "Effect_Trail.h"
#include "DamageFont.h"

CQueen::CQueen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CQueen::CQueen(const CQueen& Prototype)
	: CMonster { Prototype }
	, m_fNearAttackCoolTime { Prototype.m_fNearAttackCoolTime }
	, m_QueenStatus { Prototype.m_QueenStatus }
	, m_iDownFlag { Prototype.m_iDownFlag }
	, m_fBurrowTime { Prototype.m_fBurrowTime }
	, m_fBurrowCoolTime { Prototype.m_fBurrowCoolTime }
{
}

HRESULT CQueen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumAttacks = ENUM_CLASS(QUEEN_ATTACK::END);

	CMonster::Ready_Status("../Bin/Resources/StatusData/Queen_Status.json");

	m_fNearAttackCoolTime = 5.f;

	m_fBurrowTime = 200.f;
	m_fBurrowCoolTime = 200.f;
	
	m_QueenStatus.fStunDamage = 300.f;
	m_QueenStatus.fCurrentDamage = 0.f;
	m_QueenStatus.fFullLegDurabillity = 500.f;
	m_QueenStatus.fLegDurabillity = m_QueenStatus.fFullLegDurabillity;
	m_QueenStatus.IsBrokenLeg = false;

	m_iDownFlag = ENUM_CLASS(HIT_FLAG::DOWN_BEGIN) | ENUM_CLASS(HIT_FLAG::DOWN_DURING) | ENUM_CLASS(HIT_FLAG::DOWN_END);

	return S_OK;
}

HRESULT CQueen::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PawnObjects()))
		return E_FAIL;

	if (FAILED(Ready_AI()))
		return E_FAIL;

	if (FAILED(Ready_QueenStates()))
		return E_FAIL;

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	if (FAILED(CMonster::Ready_AnimNotify("../Bin/Resources/AnimDatas/Queen_AnimData.json")))
		return E_FAIL;

	if (FAILED(CMonster::Ready_EffectNotify("../Bin/Resources/AnimDatas/Queen_Effect_AnimDatas.json")))
		return E_FAIL;

	if (FAILED(CMonster::Ready_SoundNotify("../Bin/Resources/AnimDatas/Queen_Sound_AnimData.json")))
		return E_FAIL;

	return S_OK;
}

void CQueen::Priority_Update(_float fTimeDelta)
{
	m_vPrevPosition = m_pTransformCom->Get_State(STATE::POSITION);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CQueen::Update(_float fTimeDelta)
{
	Update_AttackCoolTime(fTimeDelta);
	Check_Near(fTimeDelta);

	m_pAI->Update();

	m_pCurrentState->Update(this, fTimeDelta);

	Bind_StateFlag();

	if (false == m_IsActive)
		return;

	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CQueen::Late_Update(_float fTimeDelta)
{
	Compute_WorldMatrix();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);

	m_pColliderContainer->Update(this, m_pBody->Get_BodyCombinedMatrix());
}

HRESULT CQueen::Render()
{
	return S_OK;
}

void CQueen::End_CutScene()
{
	EVENT_BIND_BOSSHP Event = {};
	Event.fLineHP = m_Status.fFullHealth / 6.f;
	Event.fMaxBossHP = m_Status.fFullHealth;
	Event.pCurrentBossHP = &m_Status.fHealth;
	Event.strBossName = TEXT("Äý");

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
}

HRESULT CQueen::Spawn(MONSTER_SPAWN_DATA SpawnData)
{
	m_IsActive = true;

	if (FAILED(Change_Camera()))
		return E_FAIL;

	if (FAILED(Request_SpawnEyeTrail()))
		return E_FAIL;

	ChangeState(ENUM_CLASS(QUEEN_STATE::SPAWN));

	Bind_StateFlag();

	m_pBody->Forcing_Play_Animation();

	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(SpawnData.iCellIndex);

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&SpawnData.vPosition), 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	m_pColliderContainer->SetEnableAllColliderChannel(true);
	m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), false);

	return S_OK;
}

void CQueen::Dead()
{
	m_IsActive = false;
	EVENT_QUEEN_DEAD Event = {};
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::NONSTATIC), Event);
}

BT_STATE CQueen::Attack()
{
	ChangeState(ENUM_CLASS(QUEEN_STATE::ATTACK));
	
	m_AttackTime[m_iCurrentAttack] = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CQueen::Chase()
{
	if(false == IsNear(m_fChaseRange))
	{
		ChangeState(ENUM_CLASS(QUEEN_STATE::MOVE));

		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CQueen::Patrol()
{
	return BT_STATE::FAILED;
}

BT_STATE CQueen::CanBurrow()
{
	if (m_IsBurrow && m_fBurrowTime >= m_fBurrowCoolTime)
		return BT_STATE::SUCCESS;

	return BT_STATE::FAILED;
}

BT_STATE CQueen::Burrow()
{
	ChangeState(ENUM_CLASS(QUEEN_STATE::BURROW));

	m_fBurrowTime = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CQueen::CanNearAttack()
{
	if (IsNear(m_fMinDistance) &&
		m_fNearAttackTime >= m_fNearAttackCoolTime)
	{
		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CQueen::NearAttack()
{
	ChangeState(ENUM_CLASS(QUEEN_STATE::ATTACK_NEAR));

	m_fNearAttackTime = 0.f;

	return BT_STATE::SUCCESS;
}

BT_STATE CQueen::IsLook()
{
	_float fDegree = 40.f;

	DIR eLookDir = Compute_TargetDir(fDegree);

	if (eLookDir == DIR::FRONT)
		return BT_STATE::FAILED;

	return BT_STATE::SUCCESS;
}

BT_STATE CQueen::Turn()
{
	ChangeState(ENUM_CLASS(QUEEN_STATE::TURN));

	return BT_STATE::SUCCESS;
}

BT_STATE CQueen::Idle()
{
	ChangeState(ENUM_CLASS(QUEEN_STATE::IDLE));

	return BT_STATE::SUCCESS;
}

void CQueen::Update_AttackCoolTime(_float fTimeDelta)
{
	for (auto& AttackTime : m_AttackTime)
		AttackTime += fTimeDelta;

	if (m_IsBurrow && !(m_iStateFlag & ENUM_CLASS(STATE_FLAG::BURROW)))
		m_fBurrowTime += fTimeDelta;
}

void CQueen::MoveTarget(_float fRatio)
{
	_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
	_vector vDir = XMVectorSetY(XMVectorSubtract(vTargetPos, m_pTransformCom->Get_State(STATE::POSITION)), 0.f);
	
	m_pTransformCom->LookAt(vTargetPos);

	_vector vPosition = XMVectorScale(vDir, fRatio);

	m_pTransformCom->MovePositionToVector(vPosition, m_pNavigationCom);
}

HRESULT CQueen::Ready_PawnObjects()
{
	CQueen_Body::QUEEN_BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;
	BodyObjectDesc.IsBrokenLeg = &m_QueenStatus.IsBrokenLeg;

	if (FAILED(__super::Add_PawnObject(TEXT("Queen_Body"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Queen_Body"), &BodyObjectDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("Queen_Body")));

	m_pAnimMovement = m_pBody->Get_AnimMovementPtr();
	m_pAnimRotation = m_pBody->Get_AnimRotationPtr();

	return S_OK;
}

HRESULT CQueen::Ready_AI()
{
	m_pAI = CQueenAI::Create(this);
	if (nullptr == m_pAI)
		return E_FAIL;

	return S_OK;
}

HRESULT CQueen::Ready_QueenStates()
{
	m_States.resize(ENUM_CLASS(QUEEN_STATE::END), nullptr);

	CMonsterStateFactory* pStateFactory = CMonsterStateFactory::GetInstance();

	m_States[ENUM_CLASS(QUEEN_STATE::SPAWN)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::SPAWN));
	m_States[ENUM_CLASS(QUEEN_STATE::IDLE)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::IDLE));
	m_States[ENUM_CLASS(QUEEN_STATE::MOVE)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::MOVE));
	m_States[ENUM_CLASS(QUEEN_STATE::TURN)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::TURN));
	m_States[ENUM_CLASS(QUEEN_STATE::ATTACK)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::ATTACK));
	m_States[ENUM_CLASS(QUEEN_STATE::ATTACK_NEAR)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::ATTACK_NEAR));
	m_States[ENUM_CLASS(QUEEN_STATE::JUMP)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::JUMP));
	m_States[ENUM_CLASS(QUEEN_STATE::BURROW)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::BURROW));
	m_States[ENUM_CLASS(QUEEN_STATE::HIT)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::HIT));
	m_States[ENUM_CLASS(QUEEN_STATE::DOWN)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::DOWN));
	m_States[ENUM_CLASS(QUEEN_STATE::DEAD)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::DEAD));

	return S_OK;
}

HRESULT CQueen::Ready_Collider()
{
	if (FAILED(Ready_Collider_Bounding()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Body_Hit()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Attack()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQueen::Ready_Collider_Bounding()
{
	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(150.f, 150.f, 150.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &AABBDesc, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CQueen::Ready_Collider_Body_Hit()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BODY), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("root"))))
		return E_FAIL;
	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("root"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(80.f, 50.f, 50.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BODY), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("Bone03"))))
		return E_FAIL;
	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("Bone03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BODY), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("bindingpoint_left_leg_1_3_03"))))
		return E_FAIL;
	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("bindingpoint_left_leg_1_3_03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BODY), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("bindingpoint_right_leg_1_3_03"))))
		return E_FAIL;
	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("bindingpoint_right_leg_1_3_03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 60.f, 80.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BODY), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("Bone02"))))
		return E_FAIL;
	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("Bone02"))))
		return E_FAIL;

	for (_uint i = 0; i < ENUM_CLASS(BODY_HIT_COLLIDER::END); i++)
	{

		if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::BODY), i, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionBlock(Data); })))
			return E_FAIL;

		if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::BODY), i, COLLIDER_STATE::DURING, [this](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionBlock(Data); })))
			return E_FAIL;

		if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::HIT), i, COLLIDER_STATE::BEGIN, [this, i](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionHit(i, Data); })))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CQueen::Ready_Collider_Attack()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(45.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * -1.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("root"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(150.f, 60.f, 100.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("Bone02"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 45.f, 60.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("bindingpoint_left_leg_1_3_03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 45.f, 60.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("bindingpoint_right_leg_1_3_03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(70.f, 30.f, 50.f);
	OBBDesc.vCenter = _float3(20.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &OBBDesc, m_pBody->SocketCombinedMatrixPtr("Bone04"))))
		return E_FAIL;

	return S_OK;
}

void CQueen::Compute_WorldMatrix()
{
	_vector vAnimPosition = *m_pAnimMovement;//XMVectorSetY(*m_pAnimMovement, 0.f);
	_vector vAnimRotation = *m_pAnimRotation;
	
	_matrix AnimRotationMatrix = XMMatrixRotationQuaternion(*m_pAnimRotation);
	
	_vector vLook = XMVector3Normalize(XMVectorSetY(AnimRotationMatrix.r[2], 0.f));
	vAnimRotation = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, XMMatrixLookAtLH(XMVectorZero(), vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	
	_vector vTotalRotation = XMQuaternionIdentity();
	
	vTotalRotation = vAnimRotation;

	m_pTransformCom->TurnQuaternion(vTotalRotation);

	_matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);

	m_pTransformCom->MovePositionToMatrix(PositionMatrix, m_pNavigationCom);

	m_pTransformCom->Set_State(STATE::POSITION,
		m_pNavigationCom->Compute_OnCell(m_pTransformCom->Get_State(STATE::POSITION)));
}

void CQueen::OnCollisionHit(_uint HitColliderIndex, const CCollider::COLLISION_DATA& CollisionData)
{
	if (nullptr == CollisionData.pDesc)
		return;

	ATTACK_DATA* AttackData = static_cast<ATTACK_DATA*>(CollisionData.pDesc);

	if (m_iHitAttackID == AttackData->iAttackID)
		return;

	m_iHitAttackID = AttackData->iAttackID;

	_float fFinalDamage = AttackData->fDamage - m_Status.fDefense;

	m_Status.fHealth -= fFinalDamage;

	CDamageFont::DAMAGE_DESC DamageDesc = {};
	DamageDesc.eOwner = COLLIDER_OWNER::MONSTER;
	DamageDesc.iDamage = fFinalDamage;
	DamageDesc.vPosition = m_pTransformCom->Get_State(STATE::POSITION);

	m_pPool_Instance->Request_SpawnFont(TEXT("DamageFont"), &DamageDesc);

	SpawnHitEffect(CollisionData, AttackData->HitEffect);

	if (m_Status.fHealth <= 0.f)
	{
		Dying();
		return;
	}

	if ((m_Status.fHealth / m_Status.fFullHealth) < 0.5f && false == m_IsBurrow)
		m_IsBurrow = true;

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::BURROW))
		return;

	DecreaseDurabillity(HitColliderIndex, fFinalDamage);

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::HIT))
		return;

	m_QueenStatus.fCurrentDamage += fFinalDamage;

	if (m_QueenStatus.fCurrentDamage >= m_QueenStatus.fStunDamage)
	{
		m_QueenStatus.fCurrentDamage = 0.f;

		ChangeState(ENUM_CLASS(QUEEN_STATE::HIT));

		Bind_StateFlag();

		m_pBody->Forcing_Play_Animation();
	}
}

void CQueen::DecreaseDurabillity(_uint HitColliderIndex, _float fDamage)
{
	
	switch (HitColliderIndex)
	{
	case ENUM_CLASS(BODY_HIT_COLLIDER::HEAD):
		break;

	case ENUM_CLASS(BODY_HIT_COLLIDER::BODY):
		break;

	case ENUM_CLASS(BODY_HIT_COLLIDER::L_FRONT_LEG):
		m_QueenStatus.fLegDurabillity -= fDamage;
		break;

	case ENUM_CLASS(BODY_HIT_COLLIDER::R_FRONT_LEG):
		m_QueenStatus.fLegDurabillity -= fDamage;
		break;

	case ENUM_CLASS(BODY_HIT_COLLIDER::SIDE_LEG):
		break;
	}

	if (m_QueenStatus.fLegDurabillity <= 0.f && false == m_QueenStatus.IsBrokenLeg)
	{
		m_QueenStatus.IsBrokenLeg = true;
		ChangeState(ENUM_CLASS(QUEEN_STATE::DOWN));
		Bind_StateFlag();
		m_pBody->Forcing_Play_Animation();
	}

}

void CQueen::Check_Near(_float fTimeDelta)
{
	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(m_pTargetTransform->Get_State(STATE::POSITION), m_pTransformCom->Get_State(STATE::POSITION))));

	if (fabs(fDistance) <= m_fMinDistance)
		m_fNearAttackTime += fTimeDelta;
}

HRESULT CQueen::Change_Camera()
{
	_vector vOffsetPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	CCamera_CS::CAMERA_CS_RESET_DESC CS_Reset_Desc = {};
	CS_Reset_Desc.pOwnerWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	CS_Reset_Desc.OffsetMatrix = XMMatrixTranslationFromVector(vOffsetPosition);

	if (FAILED(m_pGameInstance->Change_Camera(TEXT("Queen_CS_Camera"), &CS_Reset_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CQueen::Request_SpawnEyeTrail()
{
	m_IsEyeLight = true;

	CEffect_Trail::TRAIL_DESC TrailDesc = {};

	TrailDesc.pSocketMatrix = m_pBody->SocketCombinedMatrixPtr("root");
	TrailDesc.pParentMatrix = m_pBody->Get_BodyCombinedMatrixPtr();
	TrailDesc.IsSwing = &m_IsEyeLight;
	TrailDesc.vLeftPosition = _float3(-5.f, 5.f, 8.f);
	TrailDesc.vRightPosition = _float3(5.f, 5.f, 8.f);
	TrailDesc.fLifeTime = 1.5f;
	TrailDesc.fNodeUpdateTime = 0.f;
	TrailDesc.IsEmissive = true;

	if (FAILED(m_pPool_Instance->Request_SpawnEffect(TEXT("EyeTrail"), &TrailDesc)))
		return E_FAIL;

	TrailDesc.vLeftPosition = _float3(-5.f, 5.f, -8.f);
	TrailDesc.vRightPosition = _float3(5.f, 5.f, -8.f);

	if (FAILED(m_pPool_Instance->Request_SpawnEffect(TEXT("EyeTrail"), &TrailDesc)))
		return E_FAIL;

	return S_OK;
}

void CQueen::Dying()
{
	m_pColliderContainer->SetEnableAllColliderChannel(false);

	ChangeState(ENUM_CLASS(QUEEN_STATE::DEAD));

	Bind_StateFlag();

	m_pBody->Forcing_Play_Animation();

	m_IsEyeLight = false;
}

CQueen* CQueen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CQueen* pInstance = new CQueen(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CQueen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CQueen::Clone(void* pArg)
{
	CQueen* pInstance = new CQueen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CQueen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQueen::Free()
{
	__super::Free();
}
