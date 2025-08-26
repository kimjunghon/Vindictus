#include "ClientPch.h"
#include "Glasgavelen.h"
#include "PawnObject.h"
#include "GlasgavelenBody.h"
#include "GlasgavelenSword.h"
#include "MonsterState.h"
#include "GlasgavelenAI.h"

CGlasgavelen::CGlasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster { pDevice, pDeviceContext }
{
}

CGlasgavelen::CGlasgavelen(const CGlasgavelen& Prototype)
	: CMonster { Prototype }
	, m_iNumRageAttack { Prototype.m_iNumRageAttack }
	, m_RageAttackCoolTime { Prototype.m_RageAttackCoolTime}
	, m_RageAttackTime { Prototype.m_RageAttackTime }
	, m_GavelenStatus { Prototype.m_GavelenStatus }
{
}

void CGlasgavelen::WingBreak()
{ 
	static_cast<CGlasgavelenBody*>(m_pBody)->BrokenWing();
}

void CGlasgavelen::Change_BrokenModel()
{

	CGlasgavelenBody* pBrokenBody = static_cast<CGlasgavelenBody*>(m_pBody);
	pBrokenBody->Change_BrokenModel();

	m_pAnimMovement = pBrokenBody->Get_AnimMovementPtr();
	m_pAnimRotation = pBrokenBody->Get_AnimRotationPtr();

	Change_ColliderSocketMatrix();

	if (FAILED(m_pSword->Bind_ParentBones(m_pBody->Get_ParentModelPtr())))
		return;
}

HRESULT CGlasgavelen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumAttacks = ENUM_CLASS(NORMAL_ATTACK::END);

	m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
	m_AttackTime.resize(m_iNumAttacks, 10.f);

	m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::DESEND)] = 40.f;
	m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::BLAZE)] = 30.f;
	m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::DOUBLE)] = 30.f;
	m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::GRAP)] = 20.f;
	m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::HANG)] = 40.f;

	m_iNumRageAttack = ENUM_CLASS(RAGE_ATTACK::END);

	m_RageAttackCoolTime.resize(m_iNumRageAttack, 0.f);
	m_RageAttackTime.resize(m_iNumRageAttack, 10.f);

	m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::DESEND)] = 40.f;
	m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::COMBO)] = 30.f;
	m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::DOUBLE)] = 30.f;
	m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::BLAZE)] = 30.f;
	m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::GRAP)] = 20.f;
	m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::HANG)] = 40.f;

	m_fAttackRange = 100.f;
	m_fChaseRange = 80.f;
	m_fMinDistance = 80.f;

	m_Status.fFullHealth = 500.f;
	m_Status.fHealth = m_Status.fFullHealth;

	m_GavelenStatus.fStunDamage = 100.f;
	m_GavelenStatus.fCurrentDamage = 0.f;
	m_GavelenStatus.fRagePercent = 0.5f;
	m_GavelenStatus.fWingBrokenPercent = 0.3f;

	if (FAILED(Ready_AttackMapping()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PawnObjects()))
		return E_FAIL;

	if (FAILED(Ready_AI()))
		return E_FAIL;

	if (FAILED(Ready_GavelenStates()))
		return E_FAIL;

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	return S_OK;
}

void CGlasgavelen::Priority_Update(_float fTimeDelta)
{
	m_vPrevPosition = m_pTransformCom->Get_State(STATE::POSITION);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CGlasgavelen::Update(_float fTimeDelta)
{
	Update_AttackCoolTime(fTimeDelta);

	m_pAI->Update();

	m_pCurrentState->Update(this, fTimeDelta);

	Bind_StateFlag();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CGlasgavelen::Late_Update(_float fTimeDelta)
{
	Compute_WorldMatrix();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);
	
	__super::Update_Colliders(m_pTransformCom->Get_WorldMatrix(), m_iStateFlag);

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
#endif
}

HRESULT CGlasgavelen::Render()
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
		m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->Update(m_AttackColliderCombinedMatrix[i]);
		m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->Render();
	}
#endif
	return S_OK;
}

HRESULT CGlasgavelen::Spawn(MONSTER_SPAWN_DATA SpawnData)
{
	m_IsActive = true;

	LookAtTarget();

	ChangeState(ENUM_CLASS(GAVELEN_STATE::SPAWN));
	Bind_StateFlag();

	m_pBody->Forcing_Play_Animation();

	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(SpawnData.iCellIndex);

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&SpawnData.vPosition), 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	EnableAllColliderChannel();

	return S_OK;
}


BT_STATE CGlasgavelen::Is_Rage()
{
	if (m_IsRage)
		return BT_STATE::SUCCESS;

	return BT_STATE::FAILED;
}

BT_STATE CGlasgavelen::Rage_Attack()
{
	if (m_iCurrentAttack == ENUM_CLASS(RAGE_ATTACK::GRAP))
		ChangeState(ENUM_CLASS(GAVELEN_STATE::GRAP));
	else if (m_iCurrentAttack == ENUM_CLASS(RAGE_ATTACK::HANG))
		ChangeState(ENUM_CLASS(GAVELEN_STATE::HANG));
	else
		ChangeState(ENUM_CLASS(GAVELEN_STATE::RAGE));

	return BT_STATE::SUCCESS;
}

BT_STATE CGlasgavelen::Attack()
{
	if (m_iCurrentAttack == ENUM_CLASS(NORMAL_ATTACK::GRAP))
		ChangeState(ENUM_CLASS(GAVELEN_STATE::GRAP));
	else if (m_iCurrentAttack == ENUM_CLASS(NORMAL_ATTACK::HANG))
		ChangeState(ENUM_CLASS(GAVELEN_STATE::HANG));
	else
		ChangeState(ENUM_CLASS(GAVELEN_STATE::ATTACK));

	return BT_STATE::SUCCESS;
}

BT_STATE CGlasgavelen::Chase()
{
	if (false == IsNear(m_fChaseRange))
	{
		ChangeState(ENUM_CLASS(GAVELEN_STATE::MOVE));

		return BT_STATE::SUCCESS;
	}

	return BT_STATE::FAILED;
}

BT_STATE CGlasgavelen::Patrol()
{
	return BT_STATE();
}

BT_STATE CGlasgavelen::Idle()
{
	ChangeState(ENUM_CLASS(GAVELEN_STATE::IDLE));

	return BT_STATE::SUCCESS;
}

void CGlasgavelen::Update_BodyColliders(_fmatrix UpdateWorldMatrix)
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

HRESULT CGlasgavelen::Add_Collider_Body(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const string& strSocketName)
{
	if (iColliderIndex >= m_Colliders[COLLIDER_CHANNEL::BODY].size())
		return E_FAIL;

	const _float4x4* pSocketCombinedMatrix = m_pBody->SocketCombinedMatrixPtr(strSocketName);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::BODY);
	ColliderDesc.iOwner = ENUM_CLASS(eOwner);
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pBodyCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pBodyCollider), &ColliderDesc)))
		return E_FAIL;

	m_BodyColliderSocketName.push_back(strSocketName);
	m_Colliders[COLLIDER_CHANNEL::BODY][iColliderIndex] = pBodyCollider;
	m_BodyColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

	return S_OK;
}

HRESULT CGlasgavelen::Add_Collider_Hit(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const string& strSocketName)
{
	if (iColliderIndex >= m_Colliders[COLLIDER_CHANNEL::HIT].size())
		return E_FAIL;

	const _float4x4* pSocketCombinedMatrix = m_pBody->SocketCombinedMatrixPtr(strSocketName);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::HIT);
	ColliderDesc.iOwner = ENUM_CLASS(eOwner);
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pHitCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pHitCollider), &ColliderDesc)))
		return E_FAIL;

	m_HitColliderSocketName.push_back(strSocketName);
	m_Colliders[COLLIDER_CHANNEL::HIT][iColliderIndex] = pHitCollider;
	m_HitColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

	return S_OK;
}

HRESULT CGlasgavelen::Add_Collider_Attack(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const string& strSocketName)
{
	if (iColliderIndex >= m_Colliders[COLLIDER_CHANNEL::ATTACK].size())
		return E_FAIL;

	const _float4x4* pSocketCombinedMatrix = m_pBody->SocketCombinedMatrixPtr(strSocketName);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::HIT);
	ColliderDesc.iOwner = ENUM_CLASS(eOwner);
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pAttackCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pAttackCollider), &ColliderDesc)))
		return E_FAIL;

	m_AttackColliderSocketName.push_back(strSocketName);
	m_Colliders[COLLIDER_CHANNEL::ATTACK][iColliderIndex] = pAttackCollider;
	m_AttackColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

	return S_OK;
}

HRESULT CGlasgavelen::Ready_PawnObjects()
{
	CBody::BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(__super::Add_PawnObject(TEXT("GlasgavelenBody"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Glasgavelen_Body"), &BodyObjectDesc)))
		return E_FAIL;

	CPawnObject::PAWNOBJECT_DESC PawnObjectDesc = {};
	PawnObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_PawnObject(TEXT("GlasgavelenSword"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Glasgavelen_Sword"), &PawnObjectDesc)))
		return E_FAIL;

	m_pBody = static_cast<CBody*>(Find_PawnObject(TEXT("GlasgavelenBody")));
	if (nullptr == m_pBody)
		return E_FAIL;

	m_pAnimMovement = m_pBody->Get_AnimMovementPtr();
	m_pAnimRotation = m_pBody->Get_AnimRotationPtr();

	m_pSword = static_cast<CGlasgavelenSword*>(Find_PawnObject(TEXT("GlasgavelenSword")));
	if (nullptr == m_pSword)
		return E_FAIL;

	if(FAILED(m_pSword->Bind_ParentBones(m_pBody->Get_ParentModelPtr())))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Ready_AI()
{
	m_pAI = CGlasgavelenAI::Create(this);

	if (nullptr == m_pAI)
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Ready_GavelenStates()
{
	m_States.resize(ENUM_CLASS(GAVELEN_STATE::END), nullptr);

	CMonsterStateFactory* pStateFactory = CMonsterStateFactory::GetInstance();

	m_States[ENUM_CLASS(GAVELEN_STATE::SPAWN)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::SPAWN));
	m_States[ENUM_CLASS(GAVELEN_STATE::IDLE)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::IDLE));
	m_States[ENUM_CLASS(GAVELEN_STATE::MOVE)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::MOVE));
	m_States[ENUM_CLASS(GAVELEN_STATE::TURN)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::TURN));
	m_States[ENUM_CLASS(GAVELEN_STATE::ATTACK)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::ATTACK));
//	m_States[ENUM_CLASS(GAVELEN_STATE::ATTACK_NEAR)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::ATTACK_NEAR));
	m_States[ENUM_CLASS(GAVELEN_STATE::DESEND)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::DESEND));
	m_States[ENUM_CLASS(GAVELEN_STATE::RAGE)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::RAGE));
	m_States[ENUM_CLASS(GAVELEN_STATE::RAGE_DESEND)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::RAGE_DESEND));
	m_States[ENUM_CLASS(GAVELEN_STATE::GRAP)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::GRAP));
	m_States[ENUM_CLASS(GAVELEN_STATE::HANG)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::HANG));
	m_States[ENUM_CLASS(GAVELEN_STATE::HIT)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::HIT));
	m_States[ENUM_CLASS(GAVELEN_STATE::DOWN)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::DOWN));
	m_States[ENUM_CLASS(GAVELEN_STATE::WINGBREAK)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::WINGBREAK));
//	m_States[ENUM_CLASS(GAVELEN_STATE::DEAD)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::DEAD));

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Collider()
{
	if (FAILED(Ready_Collider_Bounding()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Body()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Hit()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Attack()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Collider_Bounding()
{
	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(150.f, 300.f, 150.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Collider_Bounding(TEXT("Com_Collider_Bounding"), COLLIDER_OWNER::MONSTER, &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Collider_Body()
{
	m_Colliders[COLLIDER_CHANNEL::BODY].resize(ENUM_CLASS(BODY_COLLIDER::END), nullptr);
	m_BodyColliderSocketMatrix.resize(ENUM_CLASS(BODY_COLLIDER::END), nullptr);
	m_BodyColliderCombinedMatrix.resize(ENUM_CLASS(BODY_COLLIDER::END), XMMatrixIdentity());

	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 50.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body_L_Arm"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_COLLIDER::L_ARM), "ValveBiped.Bip01_L_2_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 50.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body_R_Arm"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_COLLIDER::R_ARM), "ValveBiped.Bip01_R_2_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 60.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * 0.5f, 0.f, OBBDesc.vExtents.z * -0.5f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body_L_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_COLLIDER::L_LEG), "ValveBiped.Bip01_L_Calf")))
		return E_FAIL;


	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 60.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * 0.5f, 0.f, OBBDesc.vExtents.z * 0.5f);

	if (FAILED(Add_Collider_Body(TEXT("Com_Collider_Body_R_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(BODY_COLLIDER::R_LEG), "ValveBiped.Bip01_R_Calf")))
		return E_FAIL;


	for (_uint i = 0; i < ENUM_CLASS(BODY_COLLIDER::END); i++)
	{
		if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::BODY, i, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionBlock(Data); })))
			return E_FAIL;

		if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::BODY, i, COLLIDER_STATE::DURING, [this](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionBlock(Data); })))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Collider_Hit()
{
	m_Colliders[COLLIDER_CHANNEL::HIT].resize(ENUM_CLASS(HIT_COLLIDER::END), nullptr);
	m_HitColliderSocketMatrix.resize(ENUM_CLASS(HIT_COLLIDER::END), nullptr);
	m_HitColliderCombinedMatrix.resize(ENUM_CLASS(HIT_COLLIDER::END), XMMatrixIdentity());

	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_Head"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::HEAD), "ValveBiped.Bip01_Head1")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_L_Upper_Arm"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::L_UPPER_ARM), "ValveBiped.Bip01_L_2_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_L_Arm"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::L_ARM), "ValveBiped.Bip01_L_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * -1.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_R_Upper_Arm"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::R_UPPER_ARM), "ValveBiped.Bip01_R_2_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_R_Arm"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::R_ARM), "ValveBiped.Bip01_R_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * 0.5f, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_L_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::L_LEG), "ValveBiped.Bip01_L_Calf")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * 0.5f, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_R_Leg"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::R_LEG), "ValveBiped.Bip01_R_Calf")))
		return E_FAIL;

	for (_uint i = 0; i < ENUM_CLASS(HIT_COLLIDER::END); i++)
	{
		if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::HIT, i, COLLIDER_STATE::BEGIN, [this, i](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionHit(i, Data); })))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Collider_Attack()
{
	m_Colliders[COLLIDER_CHANNEL::ATTACK].resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
	m_AttackColliderSocketMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
	m_AttackColliderCombinedMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), XMMatrixIdentity());

	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(80.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * -1.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Attack(TEXT("Com_Collider_Attack_L_Sword"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::L_SWORD), "ValveBiped.Anim_Attachment_LH")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(80.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x, 0.f, 0.f);

	if (FAILED(Add_Collider_Attack(TEXT("Com_Collider_Attack_R_Sword"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::R_SWORD), "ValveBiped.Anim_Attachment_RH")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(40.f, 30.f, 40.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Attack(TEXT("Com_Collider_Attack_L_Arm"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::L_UPPER_ARM), "ValveBiped.Bip01_L_2_Hand")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(40.f, 30.f, 40.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Attack(TEXT("Com_Collider_Attack_R_Arm"), COLLIDER_OWNER::MONSTER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::R_UPPER_ARM), "ValveBiped.Bip01_R_2_Hand")))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Ready_AttackMapping()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::DESEND)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::L_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(200.f, 205.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::DESEND)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::R_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(208.f, 211.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::DOUBLE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::R_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(53.f, 57.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(ATTACK_FLAG::DOUBLE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::L_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(73.f, 77.f) });


	iFlag = ENUM_CLASS(STATE_FLAG::RAGE);

	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::DESEND_BEGIN)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::L_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(200.f, 205.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::DESEND_BEGIN)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::R_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(208.f, 211.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::DESEND_END)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::R_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(72.f, 75.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::DESEND_END)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::L_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(78.f, 81.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::ATTACK)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::L_UPPER_ARM), ATTACK_TYPE::STRONG, 3.f, _float2(51.f, 54.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::ATTACK)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::R_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(139.f, 143.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::ATTACK)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::R_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(174.f, 178.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::ATTACK)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::L_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(216.f, 219.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::ATTACK)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::L_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(255.f, 258.f) });

	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::DOUBLE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::R_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(53.f, 57.f) });
	m_AttackMapping[iFlag | ENUM_CLASS(RAGE_FLAG::DOUBLE)].push_back({ ENUM_CLASS(ATTACK_COLLIDER::L_SWORD), ATTACK_TYPE::STRONG, 3.f, _float2(73.f, 77.f) });

	return S_OK;
}

void CGlasgavelen::Change_ColliderSocketMatrix()
{
	for (_uint i = 0; i < m_BodyColliderSocketName.size(); i++)
		m_BodyColliderSocketMatrix[i] = m_pBody->SocketCombinedMatrixPtr(m_BodyColliderSocketName[i]);
	
	for (_uint i = 0; i < m_HitColliderSocketName.size(); i++)
		m_HitColliderSocketMatrix[i] = m_pBody->SocketCombinedMatrixPtr(m_HitColliderSocketName[i]);

	for (_uint i = 0; i < m_AttackColliderSocketName.size(); i++)
		m_AttackColliderSocketMatrix[i] = m_pBody->SocketCombinedMatrixPtr(m_AttackColliderSocketName[i]);
}

void CGlasgavelen::Compute_WorldMatrix()
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

void CGlasgavelen::OnCollisionHit(_uint HitColliderIndex, const CCollider::COLLISION_DATA& CollisionData)
{
	if (nullptr == CollisionData.pDesc)
		return;

	ATTACK_DATA* AttackData = static_cast<ATTACK_DATA*>(CollisionData.pDesc);

	if (m_iHitAttackID == AttackData->iAttackID)
		return;

	m_iHitAttackID = AttackData->iAttackID;

	m_Status.fHealth -= AttackData->fDamage;

	if (m_Status.fHealth <= (m_Status.fFullHealth * m_GavelenStatus.fWingBrokenPercent))
	{
		m_GavelenStatus.fWingBrokenPercent = 0.f;

		ChangeState(ENUM_CLASS(GAVELEN_STATE::WINGBREAK));

		Bind_StateFlag();

		m_pBody->Forcing_Play_Animation();
	}

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::HIT))
		return;

	if (m_Status.fHealth <= (m_Status.fFullHealth * m_GavelenStatus.fRagePercent) && false == m_IsRage)
	{
		m_IsRage = true;

		ChangeState(ENUM_CLASS(GAVELEN_STATE::DOWN));
		
		Bind_StateFlag();

		m_pBody->Forcing_Play_Animation();
	}

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::HIT))
		return;

	m_GavelenStatus.fCurrentDamage += AttackData->fDamage;

	if (m_GavelenStatus.fCurrentDamage >= m_GavelenStatus.fStunDamage)
	{
		m_GavelenStatus.fCurrentDamage = 0.f;

		ChangeState(ENUM_CLASS(QUEEN_STATE::HIT));

		Bind_StateFlag();

		m_pBody->Forcing_Play_Animation();
	}
}

CGlasgavelen* CGlasgavelen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGlasgavelen* pInstance = new CGlasgavelen(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CGlasgavelen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGlasgavelen::Clone(void* pArg)
{
	CGlasgavelen* pInstance = new CGlasgavelen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CGlasgavelen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGlasgavelen::Free()
{
	__super::Free();
}
