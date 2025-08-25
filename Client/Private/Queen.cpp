#include "ClientPch.h"
#include "Queen.h"
#include "Body.h"
#include "QueenAI.h"
#include "Queen_Body.h"
#include "MonsterStateFactory.h"
#include "MonsterState.h"

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

	if (FAILED(Ready_AttackMapping()))
		return E_FAIL;

	m_iNumAttacks = ENUM_CLASS(QUEEN_ATTACK::END);

	m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
	m_AttackTime.resize(m_iNumAttacks, 10.f);

	m_AttackCoolTime[ENUM_CLASS(QUEEN_ATTACK::SWOOP)] = 40.f;
	m_AttackCoolTime[ENUM_CLASS(QUEEN_ATTACK::DOUBLE)] = 30.f;
	m_AttackCoolTime[ENUM_CLASS(QUEEN_ATTACK::JUMP)] = 25.f;
	m_AttackCoolTime[ENUM_CLASS(QUEEN_ATTACK::LEFTHAND)] = 15.f;
	m_AttackCoolTime[ENUM_CLASS(QUEEN_ATTACK::RIGHTHAND)] = 15.f;
	m_AttackCoolTime[ENUM_CLASS(QUEEN_ATTACK::MELEE)] = 15.f;
	
	m_fNearAttackCoolTime = 5.f;

	m_fAttackRange = 80.f;
	m_fChaseRange = 60.f;
	m_fMinDistance = 80.f;

	m_fBurrowTime = 200.f;
	m_fBurrowCoolTime = 200.f;
	
	m_Status.fFullHealth = 500.f;
	m_Status.fHealth = m_Status.fFullHealth;

	m_QueenStatus.fStunDamage = 50.f;
	m_QueenStatus.fCurrentDamage = 0.f;
	m_QueenStatus.fFullLegDurabillity = 100.f;
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

	__super::Update_Colliders(m_pBody->Get_BodyCombinedMatrix(), m_iStateFlag);

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
#endif
}

HRESULT CQueen::Render()
{
#ifdef _DEBUG
	/*for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
		{
			pCollider->Render();
		}
	}*/
	for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::ATTACK].size(); i++)
	{
	    m_AttackColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[i]), m_pTransformCom->Get_WorldMatrix());
	    m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->Render();
	}
#endif
	return S_OK;
}

HRESULT CQueen::Spawn(MONSTER_SPAWN_DATA SpawnData)
{
	m_IsActive = true;

	ChangeState(ENUM_CLASS(QUEEN_STATE::SPAWN));

	Bind_StateFlag();

	m_pBody->Forcing_Play_Animation();

	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(SpawnData.iCellIndex);

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&SpawnData.vPosition), 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	EnableAllColliderChannel();

	return S_OK;
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

BT_STATE CQueen::IsStun()
{
	if(m_iStateFlag & ENUM_CLASS(STATE_FLAG::HIT))
	{
		if (m_pBody->AnimIsFinished())
		{
			_uint iStrongFlag = {};
			iStrongFlag = m_iStateFlag & m_iDownFlag;

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

void CQueen::Update_BodyColliders(_fmatrix UpdateWorldMatrix)
{
	for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::BODY].size(); i++)
	{
		if (false == m_Colliders[COLLIDER_CHANNEL::BODY][i]->IsEnable())
			continue;

		m_BodyColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_BodyColliderSocketMatrix[i]), UpdateWorldMatrix);
		m_Colliders[COLLIDER_CHANNEL::BODY][i]->Update(m_BodyColliderCombinedMatrix[i]);
		m_pGameInstance->Add_ActionCollider(this, m_Colliders[COLLIDER_CHANNEL::BODY][i]);
	}
}

void CQueen::Update_AttackCoolTime(_float fTimeDelta)
{
	for (auto& AttackTime : m_AttackTime)
		AttackTime += fTimeDelta;

	if (m_IsBurrow && !(m_iStateFlag & ENUM_CLASS(STATE_FLAG::BURROW)))
		m_fBurrowTime += fTimeDelta;

	if (m_AttackComplete && m_pBody->AnimIsFinished())
		m_AttackComplete = false;
}

void CQueen::MoveTarget(_float fRatio)
{
	_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
	_vector vDir = XMVectorSetY(XMVectorSubtract(vTargetPos, m_pTransformCom->Get_State(STATE::POSITION)), 0.f);
	
	m_pTransformCom->LookAt(vTargetPos);

	_vector vPosition = XMVectorScale(vDir, fRatio);

	m_pTransformCom->MovePositionToVector(vPosition, m_pNavigationCom);
}

HRESULT CQueen::Add_Collider_Body(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix)
{
	if (iColliderIndex >= m_Colliders[COLLIDER_CHANNEL::BODY].size() ||
		nullptr == pSocketCombinedMatrix)
		return E_FAIL;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::BODY);
	ColliderDesc.iOwner = ENUM_CLASS(eOwner);
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pBodyCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pBodyCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::BODY][iColliderIndex] = pBodyCollider;
	m_BodyColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

	return S_OK;
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

	m_pCurrentState = m_States[ENUM_CLASS(QUEEN_STATE::SPAWN)];

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

	if (FAILED(__super::Add_Collider_Bounding(TEXT("Com_Collider_Bounding"), COLLIDER_OWNER::MONSTER, &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CQueen::Ready_Collider_Body_Hit()
{
	m_Colliders[COLLIDER_CHANNEL::BODY].resize(ENUM_CLASS(BODY_HIT_COLLIDER::END), nullptr);
	m_BodyColliderSocketMatrix.resize(ENUM_CLASS(BODY_HIT_COLLIDER::END), nullptr);
	m_BodyColliderCombinedMatrix.resize(ENUM_CLASS(BODY_HIT_COLLIDER::END), XMMatrixIdentity());

	m_Colliders[COLLIDER_CHANNEL::HIT].resize(ENUM_CLASS(BODY_HIT_COLLIDER::END), nullptr);
	m_HitColliderSocketMatrix.resize(ENUM_CLASS(BODY_HIT_COLLIDER::END), nullptr);
	m_HitColliderCombinedMatrix.resize(ENUM_CLASS(BODY_HIT_COLLIDER::END), XMMatrixIdentity());


	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body_Head"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::HEAD), m_pBody->SocketCombinedMatrixPtr("root"))))
		return E_FAIL;

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Head"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::HEAD), m_pBody->SocketCombinedMatrixPtr("root"))))
		return E_FAIL;


	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(80.f, 50.f, 50.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::BODY), m_pBody->SocketCombinedMatrixPtr("Bone03"))))
		return E_FAIL;

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Body"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::BODY), m_pBody->SocketCombinedMatrixPtr("Bone03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body_L_Front_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::L_FRONT_LEG), m_pBody->SocketCombinedMatrixPtr("bindingpoint_left_leg_1_3_03"))))
		return E_FAIL;

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_L_Front_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::L_FRONT_LEG), m_pBody->SocketCombinedMatrixPtr("bindingpoint_left_leg_1_3_03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body_R_Front_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::R_FRONT_LEG), m_pBody->SocketCombinedMatrixPtr("bindingpoint_right_leg_1_3_03"))))
		return E_FAIL;

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_R_Front_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::R_FRONT_LEG), m_pBody->SocketCombinedMatrixPtr("bindingpoint_right_leg_1_3_03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 60.f, 80.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body_Side_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::SIDE_LEG), m_pBody->SocketCombinedMatrixPtr("Bone02"))))
		return E_FAIL;

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Side_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_HIT_COLLIDER::SIDE_LEG), m_pBody->SocketCombinedMatrixPtr("Bone02"))))
		return E_FAIL;

	for (_uint i = 0; i < ENUM_CLASS(BODY_HIT_COLLIDER::END); i++)
	{
		if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::BODY, i, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionBlock(Data); })))
			return E_FAIL;

		if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::BODY, i, COLLIDER_STATE::DURING, [this](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionBlock(Data); })))
			return E_FAIL;

		if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::HIT, i, COLLIDER_STATE::BEGIN, [this, i](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionHit(i, Data); })))
			return E_FAIL;

	}
	
	return S_OK;
}

HRESULT CQueen::Ready_Collider_Attack()
{
	m_Colliders[COLLIDER_CHANNEL::ATTACK].resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
	m_AttackColliderSocketMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
	m_AttackColliderCombinedMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), XMMatrixIdentity());

	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(45.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * -1.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Head"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::HEAD), m_pBody->SocketCombinedMatrixPtr("root"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(150.f, 60.f, 100.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Body"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::BODY), m_pBody->SocketCombinedMatrixPtr("Bone02"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 45.f, 60.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Left_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), m_pBody->SocketCombinedMatrixPtr("bindingpoint_left_leg_1_3_03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 45.f, 60.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Right_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), m_pBody->SocketCombinedMatrixPtr("bindingpoint_right_leg_1_3_03"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(70.f, 30.f, 50.f);
	OBBDesc.vCenter = _float3(20.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Tail"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::TAIL), m_pBody->SocketCombinedMatrixPtr("Bone04"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CQueen::Ready_AttackMapping()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::LEFTLEG)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), ATTACK_TYPE::MIDDLE, 2.f, _float2(121.f, 130.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::RIGHTLEG)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), ATTACK_TYPE::MIDDLE, 2.f, _float2(43.f, 48.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::MELLE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::HEAD), ATTACK_TYPE::LIGHT, 1.5f, _float2(46.f, 53.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::TAIL)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::TAIL), ATTACK_TYPE::MIDDLE, 1.5f, _float2(33.f, 37.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::TAIL)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::TAIL), ATTACK_TYPE::MIDDLE, 1.5f, _float2(75.f, 80.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::TURN_LEFT)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), ATTACK_TYPE::MIDDLE, 1.5f, _float2(118.f, 125.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::TURN_RIGHT)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), ATTACK_TYPE::MIDDLE, 1.5f, _float2(121.f, 129.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::SWOOP)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::BODY), ATTACK_TYPE::STRONG, 2.5f, _float2(95.f, 110.f) });
	/*m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::SWOOP)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG ), ATTACK_TYPE::STRONG, 2.5f, _float2(95.f, 110.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::SWOOP)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), ATTACK_TYPE::STRONG, 2.5f, _float2(95.f, 110.f) });*/

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::DOUBLE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), ATTACK_TYPE::STRONG, 2.5f, _float2(157.f, 166.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::DOUBLE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), ATTACK_TYPE::STRONG, 2.5f, _float2(96.f, 101.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::JUMP)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::BODY), ATTACK_TYPE::STRONG, 2.5f, _float2(140.f, 145.f) });


	iFlag = ENUM_CLASS(STATE_FLAG::BURROW);

	m_AttackMapping[iFlag | ENUM_CLASS(BURROW_FLAG::ATTACK1)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), ATTACK_TYPE::STRONG, 3.f, _float2(93.f, 96.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(BURROW_FLAG::ATTACK1)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), ATTACK_TYPE::STRONG, 3.f, _float2(98.f, 101.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(BURROW_FLAG::MOVE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), ATTACK_TYPE::MIDDLE, 2.f, _float2(94.f, 126.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(BURROW_FLAG::MOVE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), ATTACK_TYPE::MIDDLE, 2.f, _float2(94.f, 126.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(BURROW_FLAG::ATTACK2)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), ATTACK_TYPE::MIDDLE, 2.f, _float2(93.f, 101.f) });


	return S_OK;
}


void CQueen::Compute_WorldMatrix()
{
	_vector vAnimPosition = XMVectorSetY(*m_pAnimMovement, 0.f);
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

	m_Status.fHealth -= AttackData->fDamage;

	if (m_Status.fHealth <= (m_Status.fFullHealth) && false == m_IsBurrow)
		m_IsBurrow = true;

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::BURROW))
		return;

	DecreaseDurabillity(HitColliderIndex, AttackData->fDamage);

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::HIT))
		return;

	m_QueenStatus.fCurrentDamage += AttackData->fDamage;

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
