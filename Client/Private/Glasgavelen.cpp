#include "ClientPch.h"
#include "Glasgavelen.h"
#include "PawnObject.h"
#include "GlasgavelenBody.h"
#include "GlasgavelenSword.h"
#include "MonsterState.h"
#include "GlasgavelenAI.h"
#include "EnergyBall.h"
#include "GavelenRock.h"
#include "Effect_Trail.h"
#include "Camera_CS.h"
#include "DamageFont.h"

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

	m_IsEyeLight = false;

	if (FAILED(Request_SpawnEyeTrail(&m_IsBrokenEyeLight)))
		return;

	for (auto& AttackTime : m_RageAttackTime)
		AttackTime = AttackTime * 0.5f;
	
	for (auto& AttackCTime : m_RageAttackCoolTime)
		AttackCTime = 100.f;

	
}

HRESULT CGlasgavelen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	//m_iNumAttacks = ENUM_CLASS(NORMAL_ATTACK::END);

	//m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
	//m_AttackTime.resize(m_iNumAttacks, 50.f);

	//m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::DESEND)] =5000.f;
	//m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::BLAZE)] = 5000.f;
	//m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::DOUBLE)] = 5000.f;
	//m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::GRAP)] = 0;
	//m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::HANG)] = 5000.f;
	////m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::DESEND)] = 30.f;
	////m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::BLAZE)] = 20.f;
	////m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::DOUBLE)] = 25.f;
	////m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::GRAP)] = 40.f;
	////m_AttackCoolTime[ENUM_CLASS(NORMAL_ATTACK::HANG)] = 40.f;

	//m_iNumRageAttack = ENUM_CLASS(RAGE_ATTACK::END);

	//m_RageAttackCoolTime.resize(m_iNumRageAttack, 0.f);
	//m_RageAttackTime.resize(m_iNumRageAttack, 50.f);

	//m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::DESEND)] = 35.f;
	//m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::COMBO)] = 35.f;
	//m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::DOUBLE)] = 25.f;
	//m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::BLAZE)] = 20.f;
	//m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::GRAP)] = 40.f;
	//m_RageAttackCoolTime[ENUM_CLASS(RAGE_ATTACK::HANG)] = 40.f;

	//m_fAttackRange = 150.f;
	//m_fChaseRange = 140.f;
	//m_fMinDistance = 80.f;

	//m_Status.fFullHealth = 500.f;
	//m_Status.fHealth = m_Status.fFullHealth;

	if (FAILED(Ready_Status("../Bin/Resources/StatusData/Gavelen_Status.json")))
		return E_FAIL;

	m_GavelenStatus.fStunDamage = 400.f;
	m_GavelenStatus.fCurrentDamage = 0.f;
	m_GavelenStatus.fRagePercent = 0.5f;
	m_GavelenStatus.fWingBrokenPercent = 0.3f;

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

	if (FAILED(CMonster::Ready_AnimNotify("../Bin/Resources/AnimDatas/Gavelen_AnimData.json")))
		return E_FAIL;

	if (FAILED(Ready_TrailNotify()))
		return E_FAIL;

	if (FAILED(CMonster::Ready_EffectNotify("../Bin/Resources/AnimDatas/Gavelen_Effect_AnimDats.json")))
		return E_FAIL;

	if (FAILED(CMonster::Ready_SoundNotify("../Bin/Resources/AnimDatas/Gavelen_Sound_AnimData.json")))
		return E_FAIL;

	m_pGameInstance->Subscribe<EVENT_GAVELEN_CUTSCENE>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [this](const EVENT_GAVELEN_CUTSCENE& Event) { this->Event_Cutscene(Event); });

	return S_OK;
}

void CGlasgavelen::Priority_Update(_float fTimeDelta)
{
	//test
	if (m_pGameInstance->Get_KeyDown(DIK_1))
	{
		m_AttackTime[ENUM_CLASS(NORMAL_ATTACK::BLAZE)] = 5000.f;
	}

	if (m_pGameInstance->Get_KeyDown(DIK_2))
	{
		m_AttackTime[ENUM_CLASS(NORMAL_ATTACK::HANG)] = 5000.f;
		m_RageAttackTime[ENUM_CLASS(RAGE_ATTACK::HANG)] = 5000.f;
	}

	if (m_pGameInstance->Get_KeyDown(DIK_3))
	{
		m_AttackTime[ENUM_CLASS(NORMAL_ATTACK::GRAP)] = 5000.f;
	}

	m_vPrevPosition = m_pTransformCom->Get_State(STATE::POSITION);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CGlasgavelen::Update(_float fTimeDelta)
{
	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::DEAD))
	{
		m_pCurrentState->Update(this, fTimeDelta);

		for (auto& Pair : m_PawnObjects)
			Pair.second->Update(fTimeDelta);

		return;
	}

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
	
	if (false == m_IsActive)
		return;


	m_pColliderContainer->Update(this, m_pTransformCom->Get_WorldMatrix());
}

HRESULT CGlasgavelen::Render()
{

	return S_OK;
}

HRESULT CGlasgavelen::Spawn(MONSTER_SPAWN_DATA SpawnData)
{
	m_IsActive = true;

	ChangeState(ENUM_CLASS(GAVELEN_STATE::SPAWN));

	Bind_StateFlag();

	m_pBody->Forcing_Play_Animation();

	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(SpawnData.iCellIndex);

	_vector vRotate = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&SpawnData.vPosition), 1.f);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
	m_pTransformCom->RotateQuaternion(vRotate);

	m_pColliderContainer->SetEnableAllColliderChannel(true);

	m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), false);

	return S_OK;
}

void CGlasgavelen::Update_AttackCoolTime(_float fTimeDelta)
{
	if (m_IsRage)
	{		
		for (auto& AttackTime : m_RageAttackTime)
			AttackTime += fTimeDelta;
	}
	else
	{
		for (auto& AttackTime : m_AttackTime)
			AttackTime += fTimeDelta;
	}
}

BT_STATE CGlasgavelen::IsLook()
{
	_float fDegree = 30.f;

	DIR eLookDir = Compute_TargetDir(fDegree);

	if (eLookDir == DIR::FRONT)
		return BT_STATE::FAILED;

	return BT_STATE::SUCCESS;
}

BT_STATE CGlasgavelen::Turn()
{
	ChangeState(ENUM_CLASS(GAVELEN_STATE::TURN));

	return BT_STATE::SUCCESS;
}

BT_STATE CGlasgavelen::CanAttack()
{
	if (m_IsRage)
		return BT_STATE::FAILED;

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

BT_STATE CGlasgavelen::Is_Rage()
{
	if (m_IsRage)
		return BT_STATE::SUCCESS;

	return BT_STATE::FAILED;
}

BT_STATE CGlasgavelen::CanRageAttack()
{
	for (_uint i = 0; i < m_iNumRageAttack; i++)
	{
		if (m_RageAttackTime[i] >= m_RageAttackCoolTime[i])
		{
			if (m_IsBroken && m_iCurrentAttack == ENUM_CLASS(RAGE_ATTACK::DESEND) || m_iCurrentAttack == ENUM_CLASS(RAGE_ATTACK::HANG))
				continue;

			m_iCurrentAttack = i;
			return BT_STATE::SUCCESS;
		}
	}

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

	m_RageAttackTime[m_iCurrentAttack] = 0.f;

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

	m_AttackTime[m_iCurrentAttack] = 0.f;

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
	return BT_STATE::FAILED;
}

BT_STATE CGlasgavelen::Idle()
{
	ChangeState(ENUM_CLASS(GAVELEN_STATE::IDLE));

	return BT_STATE::SUCCESS;
}


HRESULT CGlasgavelen::Add_Bone_Collider(COLLIDER_CHANNEL eChannel, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, const string& strSocketName)
{
	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"), ENUM_CLASS(eChannel),
		ENUM_CLASS(COLLIDER_OWNER::MONSTER), pDesc, m_pBody->SocketCombinedMatrixPtr(strSocketName))))
		return E_FAIL;

	m_ColliderBoneNames[eChannel].push_back(strSocketName);

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Status(const string& strFilePath)
{
	ifstream File(strFilePath);
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed MonsterStatus Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	if (Doc.HasMember("Status") && Doc["Status"].IsObject())
	{
		const Value& Status = Doc["Status"];

		if (Status.HasMember("HP") && Status["HP"].IsFloat())
		{
			m_Status.fFullHealth = Status["HP"].GetFloat();
			m_Status.fHealth = m_Status.fFullHealth;
		}

		if (Status.HasMember("DEF") && Status["DEF"].IsFloat())
			m_Status.fDefense = Status["DEF"].GetFloat();

		if (Status.HasMember("ATK") && Status["ATK"].IsFloat())
			m_Status.fAttackDamage = Status["ATK"].GetFloat();

		if (Status.HasMember("AttackRange") && Status["AttackRange"].IsFloat())
			m_fAttackRange = Status["AttackRange"].GetFloat();

		if (Status.HasMember("ChaseRange") && Status["ChaseRange"].IsFloat())
			m_fChaseRange = Status["ChaseRange"].GetFloat();

		if (Status.HasMember("MinDistance") && Status["MinDistance"].IsFloat())
			m_fMinDistance = Status["MinDistance"].GetFloat();

		if (Status.HasMember("NumAttack") && Status["NumAttack"].IsInt())
			m_iNumAttacks = Status["NumAttack"].GetInt();

		m_AttackCoolTime.resize(m_iNumAttacks, 0.f);
		m_AttackTime.resize(m_iNumAttacks, 0.f);

		if (Status.HasMember("AttackTime") && Status["AttackTime"].IsArray())
		{
			const auto& AttackTimes = Status["AttackTime"].GetArray();

			_uint iIndex = 0;
			for (auto& AttackTime : AttackTimes)
			{
				if (AttackTime.HasMember("Time") && AttackTime["Time"].IsFloat())
				{
					_float fTime = AttackTime["Time"].GetFloat();
					m_AttackTime[iIndex] = fTime;
					m_AttackCoolTime[iIndex++] = fTime;
				}
			}
		}
		
		m_iNumRageAttack = Status["NumRageAttack"].GetInt();
		m_RageAttackCoolTime.resize(m_iNumRageAttack, 0.f);
		m_RageAttackTime.resize(m_iNumRageAttack, 0.f);

		if (Status.HasMember("NumRageAttack") && Status["NumRageAttack"].IsInt())
		{
			const auto& AttackTimes = Status["RageAttackTime"].GetArray();

			_uint iIndex = 0;
			for (auto& AttackTime : AttackTimes)
			{
				if (AttackTime.HasMember("Time") && AttackTime["Time"].IsFloat())
				{
					_float fTime = AttackTime["Time"].GetFloat();
					m_RageAttackTime[iIndex] = fTime;
					m_RageAttackCoolTime[iIndex++] = fTime;
				}
			}
		}
	}

	return S_OK;
}

HRESULT CGlasgavelen::Ready_PawnObjects()
{
	CBody::BODY_DESC BodyObjectDesc = {};
	BodyObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyObjectDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(__super::Add_PawnObject(TEXT("GlasgavelenBody"), ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_GameObject_Glasgavelen_Body"), &BodyObjectDesc)))
		return E_FAIL;

	CPawnObject::PAWNOBJECT_DESC PawnObjectDesc = {};
	PawnObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_PawnObject(TEXT("GlasgavelenSword"), ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_GameObject_Glasgavelen_Sword"), &PawnObjectDesc)))
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
	m_States[ENUM_CLASS(GAVELEN_STATE::CUTSCENE)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::CUTSCENE));
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
	m_States[ENUM_CLASS(GAVELEN_STATE::DEAD)] = pStateFactory->Create(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::DEAD));

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

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_OWNER::MONSTER), &AABBDesc, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Collider_Body()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 50.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::BODY, &OBBDesc, "ValveBiped.Bip01_L_2_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 50.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::BODY, &OBBDesc, "ValveBiped.Bip01_R_2_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 60.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * 0.5f, 0.f, OBBDesc.vExtents.z * -0.5f);

	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::BODY, &OBBDesc, "ValveBiped.Bip01_L_Calf")))
		return E_FAIL;
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 60.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * 0.5f, 0.f, OBBDesc.vExtents.z * 0.5f);

	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::BODY, &OBBDesc, "ValveBiped.Bip01_R_Calf")))
		return E_FAIL;

	for (_uint i = 0; i < ENUM_CLASS(BODY_COLLIDER::END); i++)
	{
		if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::BODY), i, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionBlock(Data); })))
			return E_FAIL;

		if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::BODY), i, COLLIDER_STATE::DURING, [this](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionBlock(Data); })))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Collider_Hit()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::HIT, &OBBDesc, "ValveBiped.Bip01_Head1")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x, 0.f, 0.f);
	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::HIT, &OBBDesc, "ValveBiped.Bip01_L_2_Forearm")))
		return E_FAIL;

	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::HIT, &OBBDesc, "ValveBiped.Bip01_L_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * -1.f, 0.f, 0.f);
	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::HIT, &OBBDesc, "ValveBiped.Bip01_R_2_Forearm")))
		return E_FAIL;


	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x, 0.f, 0.f);
	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::HIT, &OBBDesc, "ValveBiped.Bip01_R_Forearm")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 60.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * 0.5f, 0.f, OBBDesc.vExtents.z * -0.5f);
	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::HIT, &OBBDesc, "ValveBiped.Bip01_L_Calf")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 30.f, 60.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * 0.5f, 0.f, OBBDesc.vExtents.z * 0.5f);
	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::HIT, &OBBDesc, "ValveBiped.Bip01_R_Calf")))
		return E_FAIL;

	for (_uint i = 0; i < ENUM_CLASS(HIT_COLLIDER::END); i++)
	{
		if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::HIT), i, COLLIDER_STATE::BEGIN, [this, i](const CCollider::COLLISION_DATA& Data) {
			this->OnCollisionHit(i, Data); })))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGlasgavelen::Ready_Collider_Attack()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(150.f, 70.f, 70.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x * -1.f + 20.f, 0.f, 0.f);

	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::ATTACK, &OBBDesc, "ValveBiped.Anim_Attachment_LH")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(150.f, 70.f, 70.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.x - 20.f, 0.f, 0.f);
	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::ATTACK, &OBBDesc, "ValveBiped.Anim_Attachment_RH")))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(60.f, 50.f, 60.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(Add_Bone_Collider(COLLIDER_CHANNEL::GRAP, &OBBDesc, "ValveBiped.Bip01_R_2_Hand")))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionGrap(Data); })))
		return E_FAIL;

	return S_OK;
}

void CGlasgavelen::CreateStone(ATTACK_TYPE eType, const _wstring& strHitSoundName, _float fAttackRatio)
{
	m_IsSwing_R = false;

	CGavelenRock::GAVELEN_ROCK_DESC GavelenRock_Desc = {};
	GavelenRock_Desc.eType = eType;
	GavelenRock_Desc.strHitSoundName = strHitSoundName;
	GavelenRock_Desc.fDamage = m_Status.fAttackDamage * fAttackRatio;
	GavelenRock_Desc.pIsSwing = &m_IsSwing_R;
	GavelenRock_Desc.pSocketMatrixPtr = m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_2_Hand");
	GavelenRock_Desc.pOwnerMatrixPtr = m_pTransformCom->Get_WorldMatrixPtr();
	GavelenRock_Desc.pTargetTransform = m_pTargetTransform;

	m_pPool_Instance->Request_SpawnProjectile(TEXT("GavelenRock"), &GavelenRock_Desc);
}

void CGlasgavelen::CreateEneryBall(ATTACK_TYPE eType, const _wstring& strHitSoundName, _float fAttackRatio)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix SocketMatrix = XMLoadFloat4x4(m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Head1"));

	_vector vPosition = XMMatrixMultiply(SocketMatrix, WorldMatrix).r[3];

	CEnergyBall::ENERGYBALL_DESC EnergyBallDesc = {};
	EnergyBallDesc.eType = eType;
	EnergyBallDesc.strHitSoundName = strHitSoundName;
	EnergyBallDesc.fDamage = m_Status.fAttackDamage * fAttackRatio;
	EnergyBallDesc.StartPosition = vPosition;
	EnergyBallDesc.pTargetTransform = m_pTargetTransform;
	EnergyBallDesc.fDir = -1;
	m_pPool_Instance->Request_SpawnProjectile(TEXT("EnergyBall"), &EnergyBallDesc);

	EnergyBallDesc.fDir = 0;
	m_pPool_Instance->Request_SpawnProjectile(TEXT("EnergyBall"), &EnergyBallDesc);

	EnergyBallDesc.fDir = 1;
	m_pPool_Instance->Request_SpawnProjectile(TEXT("EnergyBall"), &EnergyBallDesc);
}

void CGlasgavelen::ThrowStone()
{
	m_IsSwing_R = true;
}

HRESULT CGlasgavelen::Add_StoneNotify(const string& strAnimName, ATTACK_TYPE eType, const _wstring& strHitSoundName, _float fAttackRatio, _float2 vTrackPosition)
{

	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.x, [this, eType, strHitSoundName, fAttackRatio]() {
		this->CreateStone(eType, strHitSoundName, fAttackRatio);
		})))
		return E_FAIL;

	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.y, [this]() {
		this->ThrowStone();
		})))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Add_GrapNotify(const string& strAnimName, _float2 vTrackPosition)
{
	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.x, [this]() {
		m_IsGrap = false;
		this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, true);
		m_GrapData.WorldMatrixPtr = m_pTransformCom->Get_WorldMatrixPtr();
		m_GrapData.SocketMatrixPtr = m_pBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_2_Finger2");
		m_GrapData.OffsetMatrixPtr = m_pBody->OffsetMatrixPtr("ValveBiped.Bip01_R_2_Finger2");
		this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, &m_GrapData);
		})))
		return E_FAIL;

	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.y, [this]() {
		if(false == m_IsGrap)
		{
			this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, false);
			this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, nullptr);
		}
		})))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Add_GrapEndNotify(const string& strAnimName, const _wstring& strHitSoundName, _float fAttackRatio, _float fTrackPosition)
{
	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, fTrackPosition, [this, strHitSoundName, fAttackRatio]() {
		this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, false);
		m_CurrentAttackData.iAttackID = m_iStateFlag + (reinterpret_cast<size_t>(this) << 1);
		m_CurrentAttackData.fDamage = m_Status.fAttackDamage * fAttackRatio;
		m_CurrentAttackData.HitEffect.strSoundName = strHitSoundName;
		this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, &m_CurrentAttackData);
		})))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Add_EnergyBallNotify(const string& strAnimName, ATTACK_TYPE eType, const _wstring& strHitSoundName, _float fAttackRatio, _float fTrackPosition)
{
	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, fTrackPosition, [this, eType, strHitSoundName, fAttackRatio]() {
		this->CreateEneryBall(eType, strHitSoundName, fAttackRatio);
		})))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Add_AttackCollisionNotify(const string& strAnimName, _uint iAttackColliderIndex, ATTACK_TYPE eType, const _wstring& strHitSoundName, _float fAttackRatio, _float2 vTrackPosition)
{
	if (!strcmp(strAnimName.c_str(), "Hang_During"))
		Add_StoneNotify(strAnimName, eType, strHitSoundName, fAttackRatio, vTrackPosition);
	else if (!strcmp(strAnimName.c_str(), "Grappling_Try"))
		Add_GrapNotify(strAnimName, vTrackPosition);
	else if (!strcmp(strAnimName.c_str(), "Grappling_Success"))
		Add_GrapEndNotify(strAnimName, strHitSoundName, fAttackRatio, vTrackPosition.y);
	else if (!strcmp(strAnimName.c_str(), "Blaze"))
		Add_EnergyBallNotify(strAnimName, eType, strHitSoundName, fAttackRatio, vTrackPosition.x);
	else
	{
		if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.x, [this, iAttackColliderIndex, eType, strHitSoundName, fAttackRatio]() {
			this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, true);
			m_CurrentAttackData.iAttackID = m_iStateFlag + (reinterpret_cast<size_t>(this) << 1);
			m_CurrentAttackData.eAttackType = eType;
			m_CurrentAttackData.HitEffect.strSoundName = strHitSoundName;
			m_CurrentAttackData.fDamage = (m_pGameInstance->Rand(m_Status.fAttackDamage - 10.f, m_Status.fAttackDamage + 10.f)) * fAttackRatio;
			m_CurrentAttackData.vAttackPosition = m_pTransformCom->Get_State(STATE::POSITION);
			this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, &m_CurrentAttackData);
			})))
			return E_FAIL;

		if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.y, [this, iAttackColliderIndex]() {
			this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, false);
			this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, nullptr);
			})))
			return E_FAIL;
	}

	return S_OK;
}

void CGlasgavelen::Event_Cutscene(const EVENT_GAVELEN_CUTSCENE& Event)
{
	ChangeState(ENUM_CLASS(GAVELEN_STATE::CUTSCENE));

	Bind_StateFlag();

	m_pBody->Forcing_Play_Animation();

	if (FAILED(Change_Camera()))
		MSG_BOX(TEXT("Failed Change Gavelen Cutscene"));

	if (FAILED(Request_SpawnEyeTrail(&m_IsEyeLight)))
		return;
}

HRESULT CGlasgavelen::Change_Camera()
{
	_vector vOffsetPosition = XMVectorSet(0.f, 0.f, -20.f, 1.f);
	
	CCamera_CS::CAMERA_CS_RESET_DESC CS_Reset_Desc = {};
	CS_Reset_Desc.pOwnerWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	CS_Reset_Desc.OffsetMatrix = XMMatrixTranslationFromVector(vOffsetPosition);

	if (FAILED(m_pGameInstance->Change_Camera(TEXT("Gavelen_CS_Camera"), &CS_Reset_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelen::Request_SpawnEyeTrail(_bool* pIsEyeLight)
{
	m_IsEyeLight = true;

	CEffect_Trail::TRAIL_DESC TrailDesc = {};

	TrailDesc.pSocketMatrix = m_pBody->SocketCombinedMatrixPtr("bone_r_eye");
	TrailDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	TrailDesc.IsSwing = pIsEyeLight;
	TrailDesc.vLeftPosition = _float3(0.f, 0.f, 0.f);
	TrailDesc.vRightPosition = _float3(0.f, 0.f, -5.f);
	TrailDesc.fLifeTime = 1.0f;
	TrailDesc.fNodeUpdateTime = 0.f;
	TrailDesc.IsEmissive = true;

	if (FAILED(m_pPool_Instance->Request_SpawnEffect(TEXT("GavelenEyeTrail"), &TrailDesc)))
		return E_FAIL;

	TrailDesc.pSocketMatrix = m_pBody->SocketCombinedMatrixPtr("bone_l_eye");
	TrailDesc.vLeftPosition = _float3(0.f, 0.f, 0.f);
	TrailDesc.vRightPosition = _float3(0.f, 0.f, 5.f);

	if (FAILED(m_pPool_Instance->Request_SpawnEffect(TEXT("GavelenEyeTrail"), &TrailDesc)))
		return E_FAIL;

	return S_OK;
}

void CGlasgavelen::Dying()
{
	m_pColliderContainer->SetEnableAllColliderChannel(false);

	m_IsEyeLight = false;

	ChangeState(ENUM_CLASS(GAVELEN_STATE::DEAD));
	
	Bind_StateFlag();

	m_pBody->Forcing_Play_Animation();

}

void CGlasgavelen::End_Cutscene()
{
	EVENT_BIND_BOSSHP Event = {};
	Event.fLineHP = m_Status.fFullHealth / 10.f;
	Event.fMaxBossHP = m_Status.fFullHealth;
	Event.pCurrentBossHP = &m_Status.fHealth;
	Event.strBossName = TEXT("글라스기브넨");

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
}

void CGlasgavelen::Dead()
{
	m_IsActive = false;
}


HRESULT CGlasgavelen::Ready_TrailNotify()
{
	ifstream File("../Bin/Resources/AnimDatas/Gavelen_Trail_AnimDatas.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Gavelen_Trail_AnimDatas Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	if (Doc.HasMember("AnimNotify") && Doc["AnimNotify"].IsArray())
	{
		const Value& AnimNotify = Doc["AnimNotify"];

		for (auto& Notify : AnimNotify.GetArray())
		{
			string strAnimName = "";
			_float fTrackPosition = {};
			NOTIFY_TYPE eType = {};

			string strBoneName = "";

			if (Notify.HasMember("AnimName") && Notify["AnimName"].IsString())
				strAnimName = Notify["AnimName"].GetString();

			if (Notify.HasMember("TrackPosition") && Notify["TrackPosition"].IsFloat())
				fTrackPosition = Notify["TrackPosition"].GetFloat();

			if (Notify.HasMember("NotifyType") && Notify["NotifyType"].IsInt())
				eType = static_cast<NOTIFY_TYPE>(Notify["NotifyType"].GetInt());

			if (Notify.HasMember("BoneName") && Notify["BoneName"].IsString())
				strBoneName = Notify["BoneName"].GetString();

			_bool* pSwing = {};
			pSwing = strcmp(strBoneName.c_str(), "ValveBiped.Anim_Attachment_LH") ? &m_IsSwing_R : &m_IsSwing_L;

			if (eType == NOTIFY_TYPE::ON)
			{
				m_pBody->Add_AnimNotify(strAnimName, fTrackPosition, [this, strBoneName, pSwing]() {
					Request_SpawnTrail(strBoneName, pSwing); });
			}
			else
			{
				m_pBody->Add_AnimNotify(strAnimName, fTrackPosition, [this, pSwing]() {
					*pSwing = false; });
			}

		}
	}

	return S_OK;
}

void CGlasgavelen::Request_SpawnTrail(const string& strBoneName, _bool* pSwing)
{
	CEffect_Trail::TRAIL_DESC TrailDesc = {};

	TrailDesc.pSocketMatrix = m_pBody->SocketCombinedMatrixPtr(strBoneName);
	TrailDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	TrailDesc.IsSwing = pSwing;
	TrailDesc.vLeftPosition = _float3(0.f, 0.f, 0.f);
	TrailDesc.vRightPosition = _float3(0.f, 100.f, 0.f);
	TrailDesc.fLifeTime = 0.5f;
	TrailDesc.fNodeUpdateTime = 0.f;
	TrailDesc.IsEmissive = false;

	*pSwing = true;

	m_pPool_Instance->Request_SpawnEffect(TEXT("SwordTrail"), &TrailDesc);
}

HRESULT CGlasgavelen::Ready_EffectNotify()
{
	ifstream File("../Bin/Resources/AnimDatas/Gavelen_Effect_AnimDats.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Gavelen_Effect_AnimDats Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	if (Doc.HasMember("AnimNotify") && Doc["AnimNotify"].IsArray())
	{
		const Value& AnimNotify = Doc["AnimNotify"];

		for (auto& Notify : AnimNotify.GetArray())
		{
			string strAnimName = "";
			_float fTrackPosition = {};

			string strBoneName = "";


			if (Notify.HasMember("AnimName") && Notify["AnimName"].IsString())
				strAnimName = Notify["AnimName"].GetString();

			if (Notify.HasMember("TrackPosition") && Notify["TrackPosition"].IsFloat())
				fTrackPosition = Notify["TrackPosition"].GetFloat();

			if (Notify.HasMember("BoneName") && Notify["BoneName"].IsString())
				strBoneName = Notify["BoneName"].GetString();

			_tchar strEffectName[MAX_PATH] = {};

			if (Notify.HasMember("EffectName") && Notify["EffectName"].IsString())
			{
				string Name = Notify["EffectName"].GetString();

				MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), static_cast<_int>(Name.size()), strEffectName, static_cast<_int>(Name.size()));
			}

			m_pBody->Add_AnimNotify(strAnimName, fTrackPosition, [this, strBoneName, strEffectName]() {
				CEffect::EFFECT_SPAWN_DESC EffectDesc = {};
				if (false == strcmp(strBoneName.c_str(), "NONE"))
					EffectDesc.SpawnWorldMatrix = m_pTransformCom->Get_WorldMatrix();
				else
					EffectDesc.SpawnWorldMatrix = XMMatrixMultiply(XMLoadFloat4x4(m_pBody->SocketCombinedMatrixPtr(strBoneName)), m_pTransformCom->Get_WorldMatrix());

				EffectDesc.IsEmissive = false;

				m_pPool_Instance->Request_SpawnEffect(strEffectName, &EffectDesc);
				});
		}
	}

	return S_OK;
}

void CGlasgavelen::Change_ColliderSocketMatrix()
{
	for (auto& Pair : m_ColliderBoneNames)
	{
		_uint iIndex = {};
		for (auto& Name : Pair.second)
		{
			m_pColliderContainer->Change_Collider_BoneMatrix(ENUM_CLASS(Pair.first), iIndex, m_pBody->SocketCombinedMatrixPtr(Name));
		}
	}
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

void CGlasgavelen::OnCollisionGrap(const CCollider::COLLISION_DATA& CollisionData)
{
	m_IsGrap = true;
}

void CGlasgavelen::OnCollisionHit(_uint HitColliderIndex, const CCollider::COLLISION_DATA& CollisionData)
{
	if (nullptr == CollisionData.pDesc)
		return;

	ATTACK_DATA* AttackData = static_cast<ATTACK_DATA*>(CollisionData.pDesc);

	if (m_iHitAttackID == AttackData->iAttackID)
		return;

	m_iHitAttackID = AttackData->iAttackID;

	_float fFinalDamage = AttackData->fDamage - m_Status.fDefense;

	DecreaseHealth(fFinalDamage);
	
	SpawnHitEffect(CollisionData, AttackData->HitEffect);

}

void CGlasgavelen::DecreaseHealth(_float fDamage)
{
	m_Status.fHealth -= fDamage;
	
	CDamageFont::DAMAGE_DESC DamageDesc = {};
	DamageDesc.eOwner = COLLIDER_OWNER::MONSTER;
	DamageDesc.iDamage = fDamage;
	DamageDesc.vPosition = m_pTransformCom->Get_State(STATE::POSITION);

	m_pPool_Instance->Request_SpawnFont(TEXT("DamageFont"), &DamageDesc);

	if (m_Status.fHealth <= 0.f)
	{
		Dying();
		return;
	}

	if (false == CanChangeState() && (m_iStateFlag & ENUM_CLASS(STATE_FLAG::HIT)))
		return;
	
	m_GavelenStatus.fCurrentDamage += fDamage;

	/*if (m_Status.fHealth <= (m_Status.fFullHealth * m_GavelenStatus.fWingBrokenPercent) && false == m_IsBroken)
	{
		m_IsBroken = true;

		m_GavelenStatus.fWingBrokenPercent = 0.f;

		ChangeState(ENUM_CLASS(GAVELEN_STATE::WINGBREAK));

		Bind_StateFlag();

		m_pBody->Forcing_Play_Animation();
	}*/
	if (m_Status.fHealth <= (m_Status.fFullHealth * m_GavelenStatus.fRagePercent) && false == m_IsRage)
	{
		m_IsRage = true;

		ChangeState(ENUM_CLASS(GAVELEN_STATE::DOWN));

		Bind_StateFlag();

		m_pBody->Forcing_Play_Animation();
	}
	else if (m_GavelenStatus.fCurrentDamage >= m_GavelenStatus.fStunDamage)
	{
		m_GavelenStatus.fCurrentDamage = 0.f;

		ChangeState(ENUM_CLASS(GAVELEN_STATE::HIT));

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
