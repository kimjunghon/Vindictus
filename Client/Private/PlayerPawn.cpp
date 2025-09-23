#include "ClientPch.h"
#include "PlayerPawn.h"
#include "SocketObject.h"
#include "Camera_Target.h"
#include "PlayerState.h"
#include "StateFactory.h"
#include "PlayerBody.h"
#include "Armor.h"
#include "Weapon.h"
#include "Effect_Trail.h"

CPlayerPawn::CPlayerPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CColliderPawn{ pDevice, pDeviceContext }
	, m_pPlayerInstance{ CPlayerInstance::GetInstance() }
	, m_pPool_Instance { CPool_Instance::GetInstance()}
{
	Safe_AddRef(m_pPlayerInstance);
	Safe_AddRef(m_pPool_Instance);
}

CPlayerPawn::CPlayerPawn(const CPlayerPawn& Prototype)
	: CColliderPawn{ Prototype }
	, m_pPlayerInstance { Prototype.m_pPlayerInstance}
	, m_pPool_Instance{ Prototype.m_pPool_Instance }
	, m_fStaminaDelay { Prototype.m_fStaminaDelay }
	, m_fSpeed { Prototype.m_fSpeed }
{
	Safe_AddRef(m_pPlayerInstance);
	Safe_AddRef(m_pPool_Instance);
}

_bool CPlayerPawn::AnimIsFinished()
{
	return m_pPlayerBody->AnimIsFinished();
}

_bool CPlayerPawn::AnimCanChange()
{
	return m_pPlayerBody->AnimCanChange();
}


HRESULT CPlayerPawn::Initialize_Prototype()
{
	m_fStaminaDelay = 2.f;
	m_fSpeed = 50.f;

	return S_OK;
}

HRESULT CPlayerPawn::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Camera()))
		return E_FAIL;
	
	if (FAILED(Ready_PawnObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

	if (FAILED(Init_Level(pDesc->iCellIndex, pDesc->vPosition)))
		return E_FAIL;

	m_vPlayerMoveDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	m_vPlayerRotationQuat = XMQuaternionIdentity();

	m_vPrevPlayerRotationQuat = XMQuaternionIdentity();

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE) | ENUM_CLASS(IDLE_FLAG::DEFAULT);

	m_pPlayerBody->Forcing_Play_Animation();
	
	m_pGameInstance->Subscribe<EVENT_CHANGE_WEAPON>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [this](const EVENT_CHANGE_WEAPON& Event) {
		this->Event_ChangeWeapon(Event); });
	
	m_pGameInstance->Subscribe<EVENT_CHANGE_ARMOR>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [this](const EVENT_CHANGE_ARMOR& Event) {
		this->Event_ChangeArmor(Event); });

	if (FAILED(Ready_AttackNotify()))
		return E_FAIL;

	if (FAILED(Ready_TrailNotify()))
		return E_FAIL;

	if (FAILED(Ready_EffectNotify()))
		return E_FAIL;

	m_pStatus = m_pPlayerInstance->GetPlayerStatusPtr();
	

	//m_pGameInstance->Update_ShadowLight(m_pTransformCom->Get_State(STATE::POSITION));

	return S_OK;
}

void CPlayerPawn::Priority_Update(_float fTimeDelta)
{
	m_vPrevPosition = m_pTransformCom->Get_State(STATE::POSITION);

	Bind_InputData(fTimeDelta);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CPlayerPawn::Update(_float fTimeDelta)
{
	m_pCurrentState->Update(this, fTimeDelta);

	if (false == m_IsActive)
		return;

	IncreaseStamina(fTimeDelta);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CPlayerPawn::Late_Update(_float fTimeDelta)
{
	if(false == m_IsGrap)
		Compute_WorldMatrix();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);

	Update_HitColliderEnable();

	m_pColliderContainer->Update(this, m_pTransformCom->Get_WorldMatrix());

//	m_pGameInstance->Update_ShadowLight(m_pTransformCom->Get_State(STATE::POSITION));
}

HRESULT CPlayerPawn::Render()
{

	return S_OK;
}

void CPlayerPawn::Change_State(_uint iStateIndex)
{
	if (iStateIndex >= ENUM_CLASS(PLAYER_STATE::END))
		return;
	
	if(m_pCurrentState)
		m_pCurrentState->Exit(this);
	
	m_pCurrentState = m_States[iStateIndex];

	if (iStateIndex == ENUM_CLASS(PLAYER_STATE::ATTACK))
		m_iComboCount = 1;

	m_pCurrentState->Enter(this);
}


void CPlayerPawn::Compute_PlayerMoveDir()
{
	m_vPlayerMoveDir = m_pCamera->Compute_PlayerMoveDir(m_MoveInput.vDir);
	
	m_vPlayerRotationQuat = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, XMMatrixLookToLH(XMVectorZero(), m_vPlayerMoveDir, XMVectorSet(0.f, 1.f, 0.f, 0.f))));

	_vector vLookQuat = m_pTransformCom->Get_LookQuaternion();

	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	_float fDot = XMVectorGetX(XMQuaternionDot(vLookQuat, m_vPlayerRotationQuat));

	if (fDot < 0.f)
		m_vPlayerRotationQuat = XMVectorNegate(m_vPlayerRotationQuat);

}

void CPlayerPawn::Grap()
{
	_matrix GrapMatrix = XMMatrixMultiply(XMLoadFloat4x4(m_GrapData.SocketMatrixPtr), XMLoadFloat4x4(m_GrapData.WorldMatrixPtr));

	_vector vScale = {};
	_vector vRotation = {};
	_vector vPosition = {};

	_vector vOffsetPosition = XMVectorSet(-3.f, -6.f, 20.f, 1.f);
	_vector vOffsetRotate = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90.f), XMConvertToRadians(-90.f), XMConvertToRadians(90.f));
	_matrix OffsetMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vOffsetRotate, vOffsetPosition);

	XMMatrixDecompose(&vScale, &vRotation, &vPosition, GrapMatrix);

	_matrix CombinedTransformationMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	
	CombinedTransformationMatrix = XMMatrixMultiply(OffsetMatrix, CombinedTransformationMatrix);

	m_pTransformCom->Set_WorldMatrix(CombinedTransformationMatrix);
}

HRESULT CPlayerPawn::EquipWeapon(CWeapon* pWeapon)
{
	if (nullptr == pWeapon)
		return E_FAIL;

	_wstring strWeaponName = pWeapon->Get_WeaponName();
	WEAPON_TYPE eWeaponType = pWeapon->Get_WeaponType();

	if (m_strEquipWeapons[ENUM_CLASS(eWeaponType)].size() > 0)
		UnEquipWeapon(ENUM_CLASS(eWeaponType));

	m_strEquipWeapons[ENUM_CLASS(eWeaponType)] = strWeaponName;

	string strSocketBoneName = "";

	eWeaponType == WEAPON_TYPE::SWORD ? strSocketBoneName = "ValveBiped.Anim_Attachment_RH" : strSocketBoneName = "ValveBiped.Anim_Attachment_LF";

	pWeapon->Equip(m_pTransformCom->Get_WorldMatrixPtr(), m_pPlayerBody->SocketCombinedMatrixPtr(strSocketBoneName));

	Add_PawnObject(strWeaponName, pWeapon);

	Safe_AddRef(pWeapon);

//	m_Status.fAttackDamage += pWeapon->Get_ATK();
//	m_pPlayerInstance->UpdatePlayerStatus(m_Status);

	return S_OK;
}

HRESULT CPlayerPawn::UnEquipWeapon(_uint iWeaponTypeIndex)
{
	if(nullptr == Find_PawnObject(m_strEquipWeapons[iWeaponTypeIndex]))
		return S_OK;

	if (FAILED(Remove_PawnObject(m_strEquipWeapons[iWeaponTypeIndex])))
		return E_FAIL;

	m_strEquipWeapons[iWeaponTypeIndex].clear();

	return S_OK;
}

HRESULT CPlayerPawn::EquipArmor(CArmor* pArmor)
{
	if (nullptr == pArmor)
		return E_FAIL;

	_wstring strArmorName = pArmor->Get_ArmorName();
	ARMOR_TYPE eArmorType = pArmor->Get_ArmorType();

	if (eArmorType == ARMOR_TYPE::HEAD)
	{
		if(false == pArmor->IsBroekn())
			m_pPlayerBody->IsHair(false);
	}

	if (m_strEquipArmors[ENUM_CLASS(eArmorType)].size() > 0)
		UnEquipArmor(ENUM_CLASS(eArmorType));

	m_strEquipArmors[ENUM_CLASS(eArmorType)] = strArmorName;

	m_pEquipArmors[ENUM_CLASS(eArmorType)] = pArmor;
	Safe_AddRef(pArmor);

	pArmor->Equip(m_pTransformCom->Get_WorldMatrixPtr(), m_pPlayerBody->Get_ParentModelPtr());

	Add_PawnObject(strArmorName, pArmor);
	Safe_AddRef(pArmor);

	return S_OK;
}

HRESULT CPlayerPawn::UnEquipArmor(_uint iArmorTypeIndex)
{
	if (nullptr == Find_PawnObject(m_strEquipArmors[iArmorTypeIndex]))
		return S_OK;

	if (iArmorTypeIndex == ENUM_CLASS(ARMOR_TYPE::HEAD))
		m_pPlayerBody->IsHair(true);

	if (FAILED(Remove_PawnObject(m_strEquipArmors[iArmorTypeIndex])))
		return E_FAIL;

	Safe_Release(m_pEquipArmors[iArmorTypeIndex]);
	m_pEquipArmors[iArmorTypeIndex] = nullptr;
	
	m_strEquipArmors[iArmorTypeIndex].clear();

	return S_OK;
}

void CPlayerPawn::Event_ChangeWeapon(const EVENT_CHANGE_WEAPON& Event)
{
	CWeapon* pWeapon = m_pPlayerInstance->UpdatePlayerEquipWeapon(Event.iWeaponTypeIndex);

	if (pWeapon)
		EquipWeapon(pWeapon);
	else
		UnEquipWeapon(Event.iWeaponTypeIndex);

}

void CPlayerPawn::Event_ChangeArmor(const EVENT_CHANGE_ARMOR& Event)
{
	CArmor* pArmor = m_pPlayerInstance->UpdatePlayerEquipArmor(Event.iArmorTypeIndex);

	if (pArmor)
		EquipArmor(pArmor);
	else
		UnEquipArmor(Event.iArmorTypeIndex);
}


void CPlayerPawn::OnCollisionHit(_uint iArmorIndex, const CCollider::COLLISION_DATA& CollisionData)
{
	if (nullptr == CollisionData.pDesc)
		return;

	m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::HIT), false);
	
	Change_HitState(iArmorIndex, CollisionData);

	m_pCurrentState->Bind_StateFlag(m_iStateFlag);

	if (FAILED(m_pPlayerBody->Forcing_Play_Animation()))
		return;
}

void CPlayerPawn::Request_HitEffect(ATTACK_TYPE eType, const CCollider::COLLISION_DATA& CollisionData)
{
	_wstring strEffectName = {};

	strEffectName = eType == ATTACK_TYPE::STRONG ? TEXT("Strong_Hit_Prefab") : TEXT("Normal_Hit_Prefab");

	_vector vCollisionPos = XMVectorSetW(XMLoadFloat3(&CollisionData.BlockData.vCollisionPos), 1.f);

	_matrix CollisionMatrix = XMMatrixTranslationFromVector(vCollisionPos);
	
	CEffect::EFFECT_SPAWN_DESC SpawnDesc = {};

	SpawnDesc.SpawnWorldMatrix = CollisionMatrix;
	SpawnDesc.IsEmissive = true;

	m_pPool_Instance->Request_SpawnEffect(strEffectName, &SpawnDesc);
}

void CPlayerPawn::OnCollisionSwordAttack(const CCollider::COLLISION_DATA& CollisionData)
{
	_matrix CombinedMatrix = XMMatrixMultiply(XMLoadFloat4x4(m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_RH")), m_pTransformCom->Get_WorldMatrix());

	_vector vNormal = XMVector3Normalize(XMVectorScale(XMLoadFloat3(&CollisionData.BlockData.vNormal), -1.f));

	_vector vOffset = XMVectorScale(vNormal, CollisionData.BlockData.fDistance);
	
	_matrix OffsetMatrix = XMMatrixTranslationFromVector(vOffset);

	CombinedMatrix = XMMatrixMultiply(OffsetMatrix, CombinedMatrix);

	CEffect::EFFECT_SPAWN_DESC SpawnDesc = {};

	SpawnDesc.SpawnWorldMatrix = CombinedMatrix;
	SpawnDesc.IsEmissive = true;

	m_pPool_Instance->Request_SpawnEffect(TEXT("Sword_Slash"), &SpawnDesc);
}

void CPlayerPawn::OnCollisionGrap(const CCollider::COLLISION_DATA& CollisionData)
{
	if (nullptr == CollisionData.pDesc)
		return;

	m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::HIT), false);
	m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::BODY), false);
	
	GRAP_DATA* pGrapData = static_cast<GRAP_DATA*>(CollisionData.pDesc);

	m_GrapData = *pGrapData;

	m_IsGrap = true;

	Change_State(ENUM_CLASS(PLAYER_STATE::GRAP));

	m_pCurrentState->Bind_StateFlag(m_iStateFlag);

	if (FAILED(m_pPlayerBody->Forcing_Play_Animation()))
		return;
}

void CPlayerPawn::EndCollisionGrap(const CCollider::COLLISION_DATA& CollisionData)
{
	m_IsGrap = false;
	
	if (nullptr == CollisionData.pDesc)
		return;

	m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::HIT), true);
	m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::BODY), true);
	
	ATTACK_DATA* AttackData = static_cast<ATTACK_DATA*>(CollisionData.pDesc);
}

void CPlayerPawn::Change_HitState(_uint iArmorIndex, const CCollider::COLLISION_DATA& CollisionData)
{
	ATTACK_DATA* AttackData = static_cast<ATTACK_DATA*>(CollisionData.pDesc);

	if (!(m_iStateFlag & ENUM_CLASS(STATE_FLAG::GUARD)))
	{
		DecreaseArmorDurability(iArmorIndex, AttackData->fDamage);
		m_pStatus->fHealth -= AttackData->fDamage;
	}

	ATTACK_TYPE eAttackType = AttackData->eAttackType;
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vAttackPosition = AttackData->vAttackPosition;

	_bool IsRotate = false;

	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::GUARD))
	{
		Change_State(ENUM_CLASS(PLAYER_STATE::HIT_GUARD));
		IsRotate = true;
	}
	else if(m_iStateFlag & ENUM_CLASS(STATE_FLAG::HEAVYSTAND))
	{
		Change_State(ENUM_CLASS(PLAYER_STATE::HIT_HEAVYSTAND));
		IsRotate = true;
	}
	else
	{
		_float fDegree = 60.f;

		switch (eAttackType)
		{
		case ATTACK_TYPE::LIGHT:
		{
			m_eHitDir = Compute_HitDir(vPosition, vAttackPosition, fDegree);
			Change_State(ENUM_CLASS(PLAYER_STATE::HIT_LIGHT));
			break;
		}
		case ATTACK_TYPE::MIDDLE:
		{
			m_eHitDir = Compute_HitDir_Look(vPosition, vAttackPosition);
			Change_State(ENUM_CLASS(PLAYER_STATE::HIT_MIDDLE));
			break;
		}
		case ATTACK_TYPE::STRONG:
		{
			m_eHitDir = Compute_HitDir_Look(vPosition, vAttackPosition);
			Change_State(ENUM_CLASS(PLAYER_STATE::HIT_STRONG));
			break;
		}
		}

		Request_HitEffect(eAttackType, CollisionData);
	}

	if (IsRotate)
	{
		_vector vDir = XMVectorSubtract(vAttackPosition, vPosition);

		_float fYaw = atan2f(XMVectorGetX(vDir), XMVectorGetZ(vDir));
		_vector vRotationQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fYaw);

		m_vPrevPlayerRotationQuat = vRotationQuat;
		m_vPlayerRotationQuat = vRotationQuat;

		m_pTransformCom->RotateQuaternion(vRotationQuat);
	}
}

void CPlayerPawn::Update_HitColliderEnable()
{
	if (m_iStateFlag & ENUM_CLASS(STATE_FLAG::ROLL) || m_iStateFlag & ENUM_CLASS(STATE_FLAG::HIT))
		m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::HIT), false);
	else
		m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::HIT), true);
}


HRESULT CPlayerPawn::Bind_HitCollisionCallback(HIT_COLLIDER eHitCollider, ARMOR_TYPE eArmor_Type)
{
	if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(eHitCollider), COLLIDER_STATE::BEGIN, [this, eArmor_Type](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionHit(ENUM_CLASS(eArmor_Type), Data); })))
		return E_FAIL;

	return S_OK;
}

void CPlayerPawn::DecreaseArmorDurability(_uint iArmorIndex, _float fDamage)
{
	_uint iIndex = iArmorIndex;
	
	_float fTestDamage = 10.f;// CollisionData.AttackData.fDamage;// - m_Status.fDefense;

	if (nullptr != m_pEquipArmors[iIndex] && false == m_pEquipArmors[iIndex]->IsBroekn())
	{
		m_pEquipArmors[iIndex]->DecreaseDurability(fTestDamage);
		return;
	}

	vector<_uint> OtherIndices;

	for (_uint i = 0; i < ENUM_CLASS(ARMOR_TYPE::END); i++)
	{
		if (i == iIndex)
			continue;

		if (nullptr != m_pEquipArmors[i] && false == m_pEquipArmors[i]->IsBroekn())
			OtherIndices.push_back(i);
	}

	if (OtherIndices.empty())
		return;

	_uint iRandomIndex = OtherIndices[rand() % OtherIndices.size()];

	DecreaseArmorDurability(iRandomIndex, fTestDamage);
}

void CPlayerPawn::Request_SpawnTrail(const string& strBoneName, _uint iTrailType)
{
	_float2 vSize = {};
	_bool IsEmissive = {};
	_wstring strTrailName = {};
	_float fLifeTime = {};

	switch (iTrailType)
	{
	case 0:
	{
		vSize = _float2(0.f, 25.f);
		IsEmissive = false;
		strTrailName = TEXT("SwordTrail");
		fLifeTime = 0.5f;
	}
		break;
	case 1:
	{
		vSize = _float2(-20.f, 20.f);
		IsEmissive = false;
		strTrailName = TEXT("KickTrail");
		fLifeTime = 1.f;
	}
		break;
	}

	CEffect_Trail::TRAIL_DESC TrailDesc = {};

	TrailDesc.pSocketMatrix = m_pPlayerBody->SocketCombinedMatrixPtr(strBoneName);
	TrailDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	TrailDesc.IsSwing = &m_IsSwing;
	TrailDesc.vLeftPosition = _float3(0.f, vSize.x, 0.f);
	TrailDesc.vRightPosition = _float3(0.f, vSize.y, 0.f);
	TrailDesc.fLifeTime = fLifeTime;
	TrailDesc.fNodeUpdateTime = 0.f;
	TrailDesc.IsEmissive = IsEmissive;

	m_pPool_Instance->Request_SpawnEffect(strTrailName, &TrailDesc);
}

HRESULT CPlayerPawn::Ready_EffectNotify()
{
	ifstream File("../Bin/Resources/AnimDatas/Player_Effect_AnimDatas.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Player_Effect_AnimDatas Open"));
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

			m_pPlayerBody->Add_AnimNotify(strAnimName, fTrackPosition, [this, strBoneName, strEffectName]() {
				CEffect::EFFECT_SPAWN_DESC EffectDesc = {};
				if(false == strcmp(strBoneName.c_str(), "NONE"))
					EffectDesc.SpawnWorldMatrix = m_pTransformCom->Get_WorldMatrix();
				else
					EffectDesc.SpawnWorldMatrix = XMMatrixMultiply(XMLoadFloat4x4(m_pPlayerBody->SocketCombinedMatrixPtr(strBoneName)), m_pTransformCom->Get_WorldMatrix());

				EffectDesc.IsEmissive = true;

				m_pPool_Instance->Request_SpawnEffect(strEffectName, &EffectDesc);
				});
		}
	}

	return S_OK;
}


void CPlayerPawn::Move(_float fTimeDelta)
{
	_vector vMovePosition = XMVectorScale(m_vPlayerMoveDir, m_fSpeed * m_fSpeedRatio * fTimeDelta);
	
	m_pTransformCom->MovePositionToVector(vMovePosition, m_pNavigationCom);
}

void CPlayerPawn::IncreaseStamina(_float fTimeDelta)
{
	m_fCurrentStaminaDelay += fTimeDelta;

	if (m_fCurrentStaminaDelay >= m_fStaminaDelay && m_pStatus->fStamina < m_pStatus->fFullStamina)
		m_pStatus->fStamina += 1.f;
}


HRESULT CPlayerPawn::Init_Level(_int iCellIndex, _float3 vStartPostion)
{
	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(iCellIndex);
	if (nullptr == m_pNavigationCom)
		return E_FAIL;

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&vStartPostion), 1.f);
	
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Camera()
{
	CCamera_Target::CAMERA_TARGET_DESC CameraDesc = {};

	CameraDesc.vEye = _float4(0.f, 200.f, -150.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFov = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 2000.f;
	CameraDesc.fSpeedPerSec = 0.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CameraDesc.fDistance = 200.f;
	CameraDesc.fHeight = 30.f;
	CameraDesc.TargetMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	CCamera* pCamera = { nullptr };

	if (FAILED(m_pGameInstance->Add_CameraToManager(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Target"), TEXT("Player_Camera"), &pCamera, &CameraDesc)))
		return E_FAIL;


	m_pCamera = static_cast<CCamera_Target*>(pCamera);
	if (nullptr == m_pCamera)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Change_Camera(m_pCamera)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_PawnObjects()
{
	if (FAILED(Ready_PlayerBody()))
		return E_FAIL;

	if (FAILED(Ready_Weapons()))
		return E_FAIL;
	
	if (FAILED(Ready_Armors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_PlayerBody()
{
	CPlayerBody::BODY_DESC PlayerBodyDesc = {};
	PlayerBodyDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	PlayerBodyDesc.pStateFlag = &m_iStateFlag;

	if (FAILED(CPawn::Add_PawnObject(TEXT("Player_Body"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player_Body"), &PlayerBodyDesc)))
		return E_FAIL;

	m_pPlayerBody = static_cast<CPlayerBody*>(Find_PawnObject(TEXT("Player_Body")));
	if (nullptr == m_pPlayerBody)
		return E_FAIL;

	m_pAnimMovement = m_pPlayerBody->Get_AnimMovementPtr();
	if (nullptr == m_pAnimMovement)
		return E_FAIL;

	m_pAnimRotation = m_pPlayerBody->Get_AnimRotationPtr();
	if (nullptr == m_pAnimRotation)
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Weapons()
{
	for (_uint i = 0; i < ENUM_CLASS(WEAPON_TYPE::END); i++)
	{
		CWeapon* pWeapon = m_pPlayerInstance->UpdatePlayerEquipWeapon(i);
		if (nullptr == pWeapon)
			continue;

		EquipWeapon(pWeapon);
	}

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Armors()
{

	for (_uint i = 0; i < ENUM_CLASS(ARMOR_TYPE::END); i++)
	{
		CArmor* pArmor = m_pPlayerInstance->UpdatePlayerEquipArmor(i);
		if (nullptr == pArmor)
			continue;

		EquipArmor(pArmor);
	}


	return S_OK;
}

HRESULT CPlayerPawn::Ready_States()
{
	m_States.resize(ENUM_CLASS(PLAYER_STATE::END), nullptr);

	CStateFactory* pStateFactory = CStateFactory::GetInstance();

	m_States[ENUM_CLASS(PLAYER_STATE::IDLE)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::IDLE));
	m_States[ENUM_CLASS(PLAYER_STATE::MOVE)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::MOVE));
	m_States[ENUM_CLASS(PLAYER_STATE::ATTACK)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::ATTACK));
	m_States[ENUM_CLASS(PLAYER_STATE::SMASH0)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::SMASH0));
	m_States[ENUM_CLASS(PLAYER_STATE::SMASH1)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::SMASH1));
	m_States[ENUM_CLASS(PLAYER_STATE::SMASH2)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::SMASH2));
	m_States[ENUM_CLASS(PLAYER_STATE::SMASH3)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::SMASH3));
	m_States[ENUM_CLASS(PLAYER_STATE::SMASH4)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::SMASH4));
	m_States[ENUM_CLASS(PLAYER_STATE::GUARD)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::GUARD));
	m_States[ENUM_CLASS(PLAYER_STATE::HEAVYSTAND)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::HEAVYSTAND));
	m_States[ENUM_CLASS(PLAYER_STATE::ROLL)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::ROLL));
	m_States[ENUM_CLASS(PLAYER_STATE::GUARD_COUNTER)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::GUARD_COUNTER));
	m_States[ENUM_CLASS(PLAYER_STATE::HIT_LIGHT)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::HIT_LIGHT));
	m_States[ENUM_CLASS(PLAYER_STATE::HIT_MIDDLE)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::HIT_MIDDLE));
	m_States[ENUM_CLASS(PLAYER_STATE::HIT_STRONG)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::HIT_STRONG));
	m_States[ENUM_CLASS(PLAYER_STATE::HIT_GUARD)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::HIT_GUARD));
	m_States[ENUM_CLASS(PLAYER_STATE::HIT_HEAVYSTAND)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::HIT_HEAVYSTAND));
	m_States[ENUM_CLASS(PLAYER_STATE::GRAP)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::GRAP));

	m_pCurrentState = m_States[ENUM_CLASS(PLAYER_STATE::IDLE)];

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider()
{
	if (FAILED(Ready_Collider_Bounding()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Body()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Hit()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Attack()))
		return E_FAIL;

	if (FAILED(Ready_Collider_Interactions()))
		return E_FAIL;

	m_pColliderContainer->SetEnableAllColliderChannel(true);

	m_pColliderContainer->SetEnableColliderChannel(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), false);

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Bounding()
{
	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(20.f, 20.f, 20.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if(FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &AABBDesc, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Body()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(10.f, 18.f, 10.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::BODY), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, nullptr)))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::BODY), 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionBlock(Data); })))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::BODY), 0, COLLIDER_STATE::DURING, [this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionBlock(Data); })))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Hit()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(4.f, 7.f, 4.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Head1"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(6.f, 10.f, 6.f);
	OBBDesc.vCenter = _float3(3.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Spine1"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(45.f));
	OBBDesc.vExtents = _float3(6.f, 10.f, 6.f);
	OBBDesc.vCenter = _float3(-4.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Pelvis"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 8.f, 3.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);
	
	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_ForeTwist1"))))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_ForeTwist1"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 11.f, 3.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_Calf"))))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::HIT), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_Calf"))))
		return E_FAIL;

	Bind_HitCollisionCallback(HIT_COLLIDER::HEAD, ARMOR_TYPE::HEAD);
	Bind_HitCollisionCallback(HIT_COLLIDER::UPPER, ARMOR_TYPE::UPPER);
	Bind_HitCollisionCallback(HIT_COLLIDER::LOWER, ARMOR_TYPE::LOWER);
	Bind_HitCollisionCallback(HIT_COLLIDER::R_ARM, ARMOR_TYPE::HAND);
	Bind_HitCollisionCallback(HIT_COLLIDER::L_ARM, ARMOR_TYPE::HAND);
	Bind_HitCollisionCallback(HIT_COLLIDER::L_LEG, ARMOR_TYPE::FOOT);
	Bind_HitCollisionCallback(HIT_COLLIDER::R_LEG, ARMOR_TYPE::FOOT);

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Attack()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(20.f, 25.f, 20.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_RH"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(XMConvertToRadians(60.f), XMConvertToRadians(90.f), XMConvertToRadians(180.f));
	OBBDesc.vExtents = _float3(25.f, 20.f, 20.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_LF"))))
		return E_FAIL;

	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y - 5.f, 0.f, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_Calf"))))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_Calf"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Interactions()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(10.f, 18.f, 10.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::GRAP), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, nullptr)))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionGrap(Data); })))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Bind_Collision_Callback(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), 0, COLLIDER_STATE::END, [this](const CCollider::COLLISION_DATA& Data) {
		this->EndCollisionGrap(Data); })))
		return E_FAIL;

	if (FAILED(m_pColliderContainer->Add_Collider(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), ENUM_CLASS(COLLIDER_OWNER::PLAYER), &OBBDesc, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_AttackNotify()
{
	ifstream File("../Bin/Resources/AnimDatas/Player_AnimData.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Player_AnimData Open"));
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

			_tchar strEffectName[MAX_PATH] = {};

			string strBoneName = {};

			if (Notify.HasMember("EffectName") && Notify["EffectName"].IsString())
			{
				string Name = Notify["EffectName"].GetString();
				
				MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), static_cast<_int>(Name.size()), strEffectName, static_cast<_int>(Name.size()));
			}

			if (Notify.HasMember("BoneName") && Notify["BoneName"].IsString())
				strBoneName = Notify["BoneName"].GetString();


			if (FAILED(Add_AttackCollisionInfo(strAnimName, iColliderIndex, eType, fDamageRatio, strEffectName, strBoneName, vTrackPositionRange)))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CPlayerPawn::Ready_TrailNotify()
{
	ifstream File("../Bin/Resources/AnimDatas/Player_Trail_AnimDatas.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Player_Trail_AnimDatas Open"));
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
			_uint iTrailType = {};
			

			if (Notify.HasMember("AnimName") && Notify["AnimName"].IsString())
				strAnimName = Notify["AnimName"].GetString();

			if (Notify.HasMember("TrackPosition") && Notify["TrackPosition"].IsFloat())
				fTrackPosition = Notify["TrackPosition"].GetFloat();

			if (Notify.HasMember("NotifyType") && Notify["NotifyType"].IsInt())
				eType = static_cast<NOTIFY_TYPE>(Notify["NotifyType"].GetInt());

			if(eType == NOTIFY_TYPE::ON)
			{
				if (Notify.HasMember("BoneName") && Notify["BoneName"].IsString())
					strBoneName = Notify["BoneName"].GetString();

				if (Notify.HasMember("TrailType") && Notify["TrailType"].IsInt())
					iTrailType = Notify["TrailType"].GetInt();

				m_pPlayerBody->Add_AnimNotify(strAnimName, fTrackPosition, [this, strBoneName, iTrailType]() {
					Request_SpawnTrail(strBoneName, iTrailType);
					m_IsSwing = true; });
			}
			else
			{
				m_pPlayerBody->Add_AnimNotify(strAnimName, fTrackPosition, [this]() {
					m_IsSwing = false; });
			}

		}
	}

	return S_OK;
}

HRESULT CPlayerPawn::Add_AttackCollisionInfo(const string& strAnimName, _uint iAttackColliderIndex, ATTACK_TYPE eType, _float fAttackRatio, _wstring strEffectName, string strBoneName, _float2 vTrackPosition)
{
	if (FAILED(m_pPlayerBody->Add_AnimNotify(strAnimName, vTrackPosition.x, [this, iAttackColliderIndex, eType, fAttackRatio, strEffectName, strBoneName]() {
		this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, true);
		m_CurrentAttackData.iAttackID = m_iStateFlag + (reinterpret_cast<size_t>(this) << 1);
		m_CurrentAttackData.eAttackType = eType;
		m_CurrentAttackData.fDamage = m_pStatus->fAttackDamage * fAttackRatio;
		m_CurrentAttackData.vAttackPosition = m_pTransformCom->Get_State(STATE::POSITION);
		m_CurrentAttackData.HitEffect.EffectName = strEffectName;
		m_CurrentAttackData.HitEffect.pBoneMatrixPtr = m_pPlayerBody->SocketCombinedMatrixPtr(strBoneName);
		m_CurrentAttackData.HitEffect.pWorldMatrixPtr = m_pTransformCom->Get_WorldMatrixPtr();

		this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, &m_CurrentAttackData);
		})))
		return E_FAIL;

	if (FAILED(m_pPlayerBody->Add_AnimNotify(strAnimName, vTrackPosition.y, [this, iAttackColliderIndex]() {
		this->m_pColliderContainer->SetEnable(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, false);
		this->m_pColliderContainer->SetDesc(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), iAttackColliderIndex, nullptr);
		})))
		return E_FAIL;

	return S_OK;
}

void CPlayerPawn::Compute_WorldMatrix()
{
	_vector vAnimPosition = *m_pAnimMovement;
	_vector vAnimRotation = *m_pAnimRotation;

	_vector vRotation = {};

	if (false == XMVector4Equal(m_vPlayerRotationQuat, m_vPrevPlayerRotationQuat))
	{
		vRotation = XMQuaternionMultiply(m_vPlayerRotationQuat, XMQuaternionInverse(m_vPrevPlayerRotationQuat));

		vRotation = XMQuaternionSlerp(XMQuaternionIdentity(), vRotation, 0.125f);

		m_vPrevPlayerRotationQuat = XMQuaternionMultiply(vRotation, m_vPrevPlayerRotationQuat);

		vRotation = m_vPrevPlayerRotationQuat;
	}
	else
		vRotation = m_vPlayerRotationQuat;

	m_pTransformCom->RotateQuaternion(vRotation);

	_matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);
	
	m_pTransformCom->MovePositionToMatrix(PositionMatrix, m_pNavigationCom);

	m_pTransformCom->Set_State(STATE::POSITION,
		m_pNavigationCom->Compute_OnCell(m_pTransformCom->Get_State(STATE::POSITION)));
}

void CPlayerPawn::Bind_InputData(_float fTimeDelta)
{
	m_pGameInstance->MoveInput(&m_MoveInput);
	m_pGameInstance->ActionInput(&m_ActionInput);
	m_pGameInstance->CameraInput(&m_CameraInput);


	if (m_pCurrentState)
	{
		m_pCurrentState->InputData(this, m_MoveInput, m_ActionInput);

		m_pCurrentState->Bind_StateFlag(m_iStateFlag);
	}

	m_pCamera->Bind_InputData(m_CameraInput);
	
	if (m_MoveInput.bMove && m_pCurrentState->CanMove())
	{
		Compute_PlayerMoveDir();
		Move(fTimeDelta);
	}

}

CPlayerPawn* CPlayerPawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPlayerPawn* pInstance = new CPlayerPawn(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CPlayerPawn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerPawn::Clone(void* pArg)
{
	CPlayerPawn* pInstance = new CPlayerPawn(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CPlayerPawn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerPawn::Free()
{
	__super::Free();

	for (auto& pState : m_States)
		Safe_Release(pState);

	Safe_Release(m_pColliderContainer);
	Safe_Release(m_pCamera);
	
	Safe_Release(m_pPlayerInstance); 
	Safe_Release(m_pPool_Instance);
	for (_uint i = 0; i < ENUM_CLASS(ARMOR_TYPE::END); i++)
		Safe_Release(m_pEquipArmors[i]);
}
