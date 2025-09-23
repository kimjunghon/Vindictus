#include "ClientPch.h"
#include "Monster.h"
#include "Body.h"
#include "BehaviorTree.h"
#include "MonsterState.h"
#include "Pool_Instance.h"
#include "Effect.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CColliderPawn{ pDevice, pDeviceContext }
	, m_pPool_Instance {CPool_Instance::GetInstance()}
{
	Safe_AddRef(m_pPool_Instance);
}

CMonster::CMonster(const CMonster& Prototype)
	: CColliderPawn{ Prototype }
	, m_iNumAttacks { Prototype.m_iNumAttacks }
	, m_AttackCoolTime{ Prototype.m_AttackCoolTime }
	, m_AttackTime{ Prototype.m_AttackTime }
	, m_fAttackRange{ Prototype.m_fAttackRange }
	, m_fChaseRange{ Prototype.m_fChaseRange }
	, m_fMinDistance{ Prototype.m_fMinDistance }
	, m_pPool_Instance{ Prototype.m_pPool_Instance }
	, m_eType { Prototype.m_eType }
	, m_Status { Prototype.m_Status }
{
	Safe_AddRef(m_pPool_Instance);
}

void CMonster::Bind_StateFlag()
{
	if(m_pCurrentState)
		m_pCurrentState->Bind_StateFlag(m_iStateFlag);
}

HRESULT CMonster::Ready_AnimNotify(const string& strFilePath)
{
	ifstream File(strFilePath);
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed AnimData Open"));
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
			_uint  iColliderIndex = {};
			ATTACK_TYPE eType = {};
			_float fDamageRatio = {};
			_float2 vTrackPositionRange = {};

			if (Notify.HasMember("AnimName") && Notify["AnimName"].IsString())
				strAnimName = Notify["AnimName"].GetString();

			if (Notify.HasMember("Collider_Index") && Notify["Collider_Index"].IsInt())
				iColliderIndex = Notify["Collider_Index"].GetInt();

			if (Notify.HasMember("AttackType") && Notify["AttackType"].IsInt())
				eType = static_cast<ATTACK_TYPE>(Notify["AttackType"].GetInt());

			if (Notify.HasMember("DamageRatio") && Notify["DamageRatio"].IsFloat())
				fDamageRatio = Notify["DamageRatio"].GetFloat();

			if (Notify.HasMember("OnTrackPosition") && Notify["OnTrackPosition"].IsFloat())
				vTrackPositionRange.x = Notify["OnTrackPosition"].GetFloat();

			if (Notify.HasMember("OffTrackPosition") && Notify["OffTrackPosition"].IsFloat())
				vTrackPositionRange.y = Notify["OffTrackPosition"].GetFloat();

			if (FAILED(Add_AttackCollisionNotify(strAnimName, iColliderIndex, eType, fDamageRatio, vTrackPositionRange)))
				return E_FAIL;
			
		}
	}

	if (Doc.HasMember("ReadyNotify") && Doc["ReadyNotify"].IsArray())
	{
		const Value& ReadyNotifies = Doc["ReadyNotify"];

		for (auto& ReadyNotify : ReadyNotifies.GetArray())
		{
			string strAnimName = "";
			_float2 vReadyTrackPositionRange = {};

			if (ReadyNotify.HasMember("AnimName") && ReadyNotify["AnimName"].IsString())
				strAnimName = ReadyNotify["AnimName"].GetString();

			if (ReadyNotify.HasMember("OnReadyTrackPosition") && ReadyNotify["OnReadyTrackPosition"].IsFloat())
				vReadyTrackPositionRange.x = ReadyNotify["OnReadyTrackPosition"].GetFloat();

			if (ReadyNotify.HasMember("OffReadyTrackPosition") && ReadyNotify["OffReadyTrackPosition"].IsFloat())
				vReadyTrackPositionRange.y = ReadyNotify["OffReadyTrackPosition"].GetFloat();

			if (FAILED(Add_ReadyAttackNotify(strAnimName, vReadyTrackPositionRange)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMonster::Ready_Status(const string& strFilePath)
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

		if(Status.HasMember("AttackTime") && Status["AttackTime"].IsArray())
		{
			const auto& AttackTimes = Status["AttackTime"].GetArray();
			
			_uint iIndex = 0;
			for (auto& AttackTime : AttackTimes)
			{
				if(AttackTime.HasMember("Time") && AttackTime["Time"].IsFloat())
					m_AttackCoolTime[iIndex++] = AttackTime["Time"].GetFloat();
			}
		}
	}


	return S_OK;
}

HRESULT CMonster::Ready_EffectNotify(const string& strFilePath)
{
	ifstream File(strFilePath);
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Effect_AnimDats Open"));
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

			if (FAILED(m_pBody->Add_AnimNotify(strAnimName, fTrackPosition, [this, strBoneName, strEffectName]() {
				CEffect::EFFECT_SPAWN_DESC EffectDesc = {};
				if (false == strcmp(strBoneName.c_str(), "NONE"))
					EffectDesc.SpawnWorldMatrix = m_pTransformCom->Get_WorldMatrix();
				else
					EffectDesc.SpawnWorldMatrix = XMMatrixMultiply(XMLoadFloat4x4(m_pBody->SocketCombinedMatrixPtr(strBoneName)), m_pTransformCom->Get_WorldMatrix());

				EffectDesc.IsEmissive = false;

				m_pPool_Instance->Request_SpawnEffect(strEffectName, &EffectDesc);
				})))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMonster::Add_ReadyAttackNotify(const string& strAnimName, _float2 vTrackPosition)
{
	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.x, [this]() {
		this->Set_ReadyAttack(true);
		})))
		return E_FAIL;

	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.y, [this]() {
		this->Set_ReadyAttack(false);
		})))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Add_AttackCollisionNotify(const string& strAnimName, _uint iAttackColliderIndex, ATTACK_TYPE eType, _float fAttackRatio, _float2 vTrackPosition)
{
	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.x, [this, iAttackColliderIndex, eType, fAttackRatio]() {
		this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, true);
		m_CurrentAttackData.iAttackID = m_iStateFlag + (reinterpret_cast<size_t>(this) << 1);
		m_CurrentAttackData.eAttackType = eType;
		m_CurrentAttackData.fDamage = m_Status.fAttackDamage * fAttackRatio;
		m_CurrentAttackData.vAttackPosition = m_pTransformCom->Get_State(STATE::POSITION);
		this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, &m_CurrentAttackData);
		})))
		return E_FAIL;

	if (FAILED(m_pBody->Add_AnimNotify(strAnimName, vTrackPosition.y, [this, iAttackColliderIndex]() {
		this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, false);
		this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, nullptr);
		})))
		return E_FAIL;

	return S_OK;
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

void CMonster::TurnToTarget(_float fRatio)
{
	if (nullptr == m_pTargetTransform)
		return;

	_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
	_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK),0.f));
	_vector vDir = XMVector3Normalize(XMVectorSetY(XMVectorSubtract(vTargetPos, m_pTransformCom->Get_State(STATE::POSITION)), 0.f));

	_float fComparisonRadian = cosf(XMConvertToRadians(30.f));

	_float vLookDot = XMVectorGetX(XMVector3Dot(vLook, vDir));

	if(vLookDot >= fComparisonRadian)
		m_pTransformCom->LookAt(vTargetPos);
	else
	{
		_matrix RotationMatrix = XMMatrixInverse(nullptr, XMMatrixLookAtLH(XMVectorZero(), vDir, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		_vector vLookQuat = m_pTransformCom->Get_LookQuaternion();
		_vector vDirQuat = XMQuaternionRotationMatrix(RotationMatrix);
		if (vLookDot < 0.f)
			vDirQuat = XMVectorNegate(vDirQuat);

		_vector vRotateQuat = XMQuaternionSlerp(vLookQuat, vDirQuat, fRatio);
		vRotateQuat = XMQuaternionMultiply(vRotateQuat, XMQuaternionInverse(vLookQuat));

		m_pTransformCom->TurnQuaternion(vRotateQuat);
	}
}

void CMonster::MoveToTarget(_float fRatio)
{
	_vector vTargetPos = m_pTargetTransform->Get_State(STATE::POSITION);
	_vector vDir = XMVectorSetY(XMVectorSubtract(vTargetPos, m_pTransformCom->Get_State(STATE::POSITION)), 0.f);

	m_pTransformCom->LookAt(vTargetPos);

	_vector vPosition = XMVectorScale(vDir, fRatio);

	m_pTransformCom->MovePositionToVector(vPosition, m_pNavigationCom);
}

void CMonster::Dead()
{
	m_IsActive = false;
	m_pPool_Instance->ReturnPool(m_eType, this);
}

_bool CMonster::IsAnimationInRangeTrackPosition(_float2 vRange)
{
	if (nullptr == m_pBody)
		return false;

	return m_pBody->IsAnimationInRangeTrackPosition(vRange);
}

_bool CMonster::IsAnimationPassToTrackPosition(_float fTrackPosition)
{
	if (nullptr == m_pBody)
		return false;

	return m_pBody->IsAnimationPassToTrackPosition(fTrackPosition);
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
			return BT_STATE::SUCCESS;
		}
	}

	return BT_STATE::FAILED;
}

BT_STATE CMonster::CanOtherAction()
{
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

void CMonster::OnCollisionAttack(const CCollider::COLLISION_DATA& CollisionData)
{
}

void CMonster::Update_AttackCoolTime(_float fTimeDelta)
{
	for (auto& AttackTime : m_AttackTime)
		AttackTime += fTimeDelta;
}

void CMonster::SpawnHitEffect(const CCollider::COLLISION_DATA& CollisionData, const ATTACK_EFFECT_DATA& EffectData)
{
	_matrix CombinedMatrix = XMMatrixMultiply(XMLoadFloat4x4(EffectData.pBoneMatrixPtr), XMLoadFloat4x4(EffectData.pWorldMatrixPtr));

	_vector vCollisionPos = XMVectorSetW(XMLoadFloat3(&CollisionData.BlockData.vCollisionPos),1.f);

	CombinedMatrix.r[3] = vCollisionPos;

	CEffect::EFFECT_SPAWN_DESC SpawnDesc = {};

	SpawnDesc.SpawnWorldMatrix = CombinedMatrix;
	SpawnDesc.IsEmissive = true;

	m_pPool_Instance->Request_SpawnEffect(EffectData.EffectName, &SpawnDesc);
}

void CMonster::Free()
{
	__super::Free();

	for (auto& pState : m_States)
		Safe_Release(pState);

	Safe_Release(m_pAI);
	Safe_Release(m_pPool_Instance);
}
