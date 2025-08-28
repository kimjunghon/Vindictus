#include "ClientPch.h"
#include "PlayerPawn.h"
#include "SocketObject.h"
#include "Camera_Target.h"
#include "GameInstance.h"
#include "PlayerState.h"
#include "StateFactory.h"
#include "PlayerBody.h"
#include "Armor.h"
#include "Weapon.h"

CPlayerPawn::CPlayerPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CColliderPawn{ pDevice, pDeviceContext }
	, m_pPlayerInstance{ CPlayerInstance::GetInstance() }
{
	Safe_AddRef(m_pPlayerInstance);
}

CPlayerPawn::CPlayerPawn(const CPlayerPawn& Prototype)
	: CColliderPawn{ Prototype }
	, m_pPlayerInstance { Prototype.m_pPlayerInstance}
{
	Safe_AddRef(m_pPlayerInstance);
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

	m_fSpeed = 50.f;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE) | ENUM_CLASS(IDLE_FLAG::DEFAULT);

	m_pPlayerBody->Forcing_Play_Animation();
	
	m_pGameInstance->Subscribe<EVENT_CHANGE_WEAPON>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [this](const EVENT_CHANGE_WEAPON& Event) {
		this->Event_ChangeWeapon(Event); });
	
	m_pGameInstance->Subscribe<EVENT_CHANGE_ARMOR>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [this](const EVENT_CHANGE_ARMOR& Event) {
		this->Event_ChangeArmor(Event); });

	//test
	m_Status.fAttackDamage = 20.f;

	if (FAILED(Ready_AttackMapping()))
		return E_FAIL;

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

	__super::Update_Colliders(m_pTransformCom->Get_WorldMatrix());

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
#endif
}

HRESULT CPlayerPawn::Render()
{

#ifdef _DEBUG
	for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
		{
			pCollider->Render();
		}
	}
	//for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::ATTACK].size(); i++)
	//{
	//	m_AttackColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[i]), m_pTransformCom->Get_WorldMatrix());
	//	m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->Render();
	//}
#endif
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


	//_float fYaw = atan2f(XMVectorGetX(m_vPlayerMoveDir), XMVectorGetZ(m_vPlayerMoveDir));
	//m_vPlayerRotationQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fYaw);
}

void CPlayerPawn::Grap()
{
	_matrix GrapMatrix = XMMatrixMultiply(XMLoadFloat4x4(m_GrapData.SocketMatrixPtr), XMLoadFloat4x4(m_GrapData.WorldMatrixPtr));

	_vector vScale = {};
	_vector vRotation = {};
	_vector vPosition = {};

	_vector vOffsetPosition = XMVectorSet(3.f, -5.f, 20.f, 1.f);
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

	DisableColliderChannel(COLLIDER_CHANNEL::HIT);

	ATTACK_DATA* AttackData = static_cast<ATTACK_DATA*>(CollisionData.pDesc);

	if (!(m_iStateFlag & ENUM_CLASS(STATE_FLAG::GUARD)))
		DecreaseArmorDurability(iArmorIndex, AttackData->fDamage);

	ATTACK_TYPE eAttackType = AttackData->eAttackType;
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vAttackPosition = AttackData->vAttackPosition;

	Change_HitState(eAttackType, vPosition, vAttackPosition);

	m_pCurrentState->Bind_StateFlag(m_iStateFlag);

	if (FAILED(m_pPlayerBody->Forcing_Play_Animation()))
		return;
}

void CPlayerPawn::OnCollisionGrap(const CCollider::COLLISION_DATA& CollisionData)
{
	if (nullptr == CollisionData.pDesc)
		return;

	DisableColliderChannel(COLLIDER_CHANNEL::HIT);
	DisableColliderChannel(COLLIDER_CHANNEL::BODY);

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
	
	EnableColliderChannel(COLLIDER_CHANNEL::HIT);
	EnableColliderChannel(COLLIDER_CHANNEL::BODY);
	
	ATTACK_DATA* AttackData = static_cast<ATTACK_DATA*>(CollisionData.pDesc);
}

void CPlayerPawn::Change_HitState(ATTACK_TYPE eAttackType, _fvector vPosition, _fvector vAttackPosition)
{
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
		DisableColliderChannel(COLLIDER_CHANNEL::HIT);
	else
		EnableColliderChannel(COLLIDER_CHANNEL::HIT);
}


HRESULT CPlayerPawn::Bind_HitCollisionCallback(HIT_COLLIDER eHitCollider, ARMOR_TYPE eArmor_Type)
{
	if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::HIT, ENUM_CLASS(eHitCollider), COLLIDER_STATE::BEGIN, [this, eArmor_Type](const CCollider::COLLISION_DATA& Data) {
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


void CPlayerPawn::Move(_float fTimeDelta)
{
	_vector vMovePosition = XMVectorScale(m_vPlayerMoveDir, m_fSpeed * m_fSpeedRatio * fTimeDelta);
	
	m_pTransformCom->MovePositionToVector(vMovePosition, m_pNavigationCom);
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

	CameraDesc.fDistance = 50.f;
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

	if (FAILED(Add_Collider_Grap()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Bounding()
{
	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(20.f, 20.f, 20.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Collider_Bounding(TEXT("Com_Collider_Bounding"), COLLIDER_OWNER::PLAYER, &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Body()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(10.f, 18.f, 10.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Collider_Body(TEXT("Com_Collider_Body"), COLLIDER_OWNER::PLAYER, &OBBDesc)))
		return E_FAIL;

	if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::BODY, 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionBlock(Data); })))
		return E_FAIL;

	if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::BODY, 0, COLLIDER_STATE::DURING, [this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionBlock(Data); })))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Hit()
{
	m_Colliders[COLLIDER_CHANNEL::HIT].resize(ENUM_CLASS(HIT_COLLIDER::END), nullptr);
	m_HitColliderSocketMatrix.resize(ENUM_CLASS(HIT_COLLIDER::END), nullptr);
	m_HitColliderCombinedMatrix.resize(ENUM_CLASS(HIT_COLLIDER::END), XMMatrixIdentity());

	/* Com_Collider_Hit_Head */
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(4.f, 7.f, 4.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Head"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::HEAD), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Head1"))))
		return E_FAIL;

	Bind_HitCollisionCallback(HIT_COLLIDER::HEAD, ARMOR_TYPE::HEAD);

	/* Com_Collider_Hit_Upper */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(6.f, 10.f, 6.f);
	OBBDesc.vCenter = _float3(3.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Upper"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::UPPER), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Spine1"))))
		return E_FAIL;

	Bind_HitCollisionCallback(HIT_COLLIDER::UPPER, ARMOR_TYPE::UPPER);

	/* Com_Collider_Hit_Lower */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(45.f));
	OBBDesc.vExtents = _float3(6.f, 10.f, 6.f);
	OBBDesc.vCenter = _float3(-4.f, 0.f, 0.f);
	
	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_Lower"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::LOWER), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Pelvis"))))
		return E_FAIL;

	Bind_HitCollisionCallback(HIT_COLLIDER::LOWER, ARMOR_TYPE::LOWER);

	/* Com_Collider_Hit_RightArm */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 8.f, 3.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_RightArm"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::R_ARM), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_ForeTwist1"))))
		return E_FAIL;

	Bind_HitCollisionCallback(HIT_COLLIDER::R_ARM, ARMOR_TYPE::HAND);

	/* Com_Collider_Hit_LeftArm */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 8.f, 3.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_LeftArm"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::L_ARM), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_ForeTwist1"))))
		return E_FAIL;

	Bind_HitCollisionCallback(HIT_COLLIDER::L_ARM, ARMOR_TYPE::HAND);

	/* Com_Collider_Hit_LeftLeg */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 11.f, 3.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_LeftLeg"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::L_LEG), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_Calf"))))
		return E_FAIL;

	Bind_HitCollisionCallback(HIT_COLLIDER::L_LEG, ARMOR_TYPE::FOOT);

	/* Com_Collider_Hit_RightLeg */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 11.f, 3.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Hit(TEXT("Com_Collider_Hit_RightLeg"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(HIT_COLLIDER::R_LEG), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_Calf"))))
		return E_FAIL;

	Bind_HitCollisionCallback(HIT_COLLIDER::R_LEG, ARMOR_TYPE::FOOT);

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Attack()
{
	m_Colliders[COLLIDER_CHANNEL::ATTACK].resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
	m_AttackColliderSocketMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), nullptr);
	m_AttackColliderCombinedMatrix.resize(ENUM_CLASS(ATTACK_COLLIDER::END), XMMatrixIdentity());

	/* Com_Collider_Attack_Sword */
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(20.f, 25.f, 20.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Sword"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::SWORD), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_RH"))))
		return E_FAIL;

	/* Com_Collider_Attack_Shield */
	OBBDesc.vAngles = _float3(XMConvertToRadians(60.f), XMConvertToRadians(90.f), XMConvertToRadians(180.f));
	OBBDesc.vExtents = _float3(25.f, 20.f, 20.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Shield"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::SHILED), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_LF"))))
		return E_FAIL;

	/* Com_Collider_Attack_Left_Leg */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y - 5.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Left_Leg"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_Calf"))))
		return E_FAIL;

	/* Com_Collider_Attack_Right_Leg */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(30.f, 30.f, 30.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y - 5.f, 0.f, 0.f);

	if (FAILED(__super::Add_Collider_Attack(TEXT("Com_Collider_Attack_Right_Leg"), COLLIDER_OWNER::PLAYER, &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_Calf"))))
		return E_FAIL;

	DisableColliderChannel(COLLIDER_CHANNEL::ATTACK);

	return S_OK;
}

HRESULT CPlayerPawn::Add_Collider_Grap()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(10.f, 18.f, 10.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::GRAP);
	ColliderDesc.iOwner = ENUM_CLASS(COLLIDER_OWNER::PLAYER);
	ColliderDesc.BoundingDesc = &OBBDesc;

	CCollider* pGrapCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Grap"), reinterpret_cast<CComponent**>(&pGrapCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::GRAP].push_back(pGrapCollider);
	
	if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::GRAP, 0, COLLIDER_STATE::BEGIN, [this](const CCollider::COLLISION_DATA& Data) {
		this->OnCollisionGrap(Data); })))
		return E_FAIL;

	if (FAILED(__super::Bind_Collision_Callback(COLLIDER_CHANNEL::GRAP, 0, COLLIDER_STATE::END, [this](const CCollider::COLLISION_DATA& Data) {
		this->EndCollisionGrap(Data); })))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_AttackMapping()
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

			if (FAILED(Add_AttackCollisionInfo(strAnimName, iColliderIndex, eType, fDamageRatio, vTrackPositionRange)))
				return E_FAIL;
		}
	}

	//_uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(ATTACK_FLAG::COMBO1), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::LIGHT, 1.f, _float2(12.f, 22.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(ATTACK_FLAG::COMBO2), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::LIGHT, 1.f, _float2(15.f, 25.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(ATTACK_FLAG::COMBO3), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::LIGHT, 1.f, _float2(8.f, 18.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(ATTACK_FLAG::COMBO4), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::LIGHT, 1.f, _float2(29.f, 38.f));

	//iFlag = ENUM_CLASS(STATE_FLAG::SMASH);

	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH0), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::MIDDLE, 1.2f, _float2(18.f, 22.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH0_CHARGE_END), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::STRONG, 2.f, _float2(13.f, 18.f));

	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH1), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::MIDDLE, 1.5f, _float2(11.f, 19.f));

	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH2_0), ENUM_CLASS(ATTACK_COLLIDER::SHILED), ATTACK_TYPE::MIDDLE, 1.8f, _float2(23.f, 29.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH2_1), ENUM_CLASS(ATTACK_COLLIDER::SHILED), ATTACK_TYPE::MIDDLE, 1.8f, _float2(24.f, 30.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH2_2), ENUM_CLASS(ATTACK_COLLIDER::SHILED), ATTACK_TYPE::STRONG, 2.f, _float2(19.f, 25.f));

	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH3_0), ENUM_CLASS(ATTACK_COLLIDER::SHILED), ATTACK_TYPE::MIDDLE, 2.f, _float2(18.f, 22.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH3_1), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::MIDDLE, 2.f, _float2(11.f, 18.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH3_2), ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), ATTACK_TYPE::STRONG, 2.5f, _float2(25.f, 36.f));

	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH4), ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), ATTACK_TYPE::STRONG, 3.f, _float2(33.f, 44.f));
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(SMASH_FLAG::SMASH_GUARD_COUNTER), ENUM_CLASS(ATTACK_COLLIDER::SHILED), ATTACK_TYPE::STRONG, 2.f, _float2(8.f, 13.f));
	//
	//iFlag = ENUM_CLASS(STATE_FLAG::GUARD);
	//Add_AttackCollisionInfo(iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_ATTACK), ENUM_CLASS(ATTACK_COLLIDER::SWORD), ATTACK_TYPE::LIGHT, 1.2f, _float2(9.f, 12.f));
	//
	return S_OK;
}

HRESULT CPlayerPawn::Add_AttackCollisionInfo(const string& strAnimName, _uint iAttackColliderIndex, ATTACK_TYPE eType, _float fAttackRatio, _float2 vTrackPosition)
{
	if (FAILED(m_pPlayerBody->Add_AnimNotify(strAnimName, vTrackPosition.x, [this, iAttackColliderIndex, eType, fAttackRatio]() {
		this->m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackColliderIndex]->SetEnable(true);
		m_CurrentAttackData.iAttackID = m_iStateFlag + (reinterpret_cast<size_t>(this) << 1);
		m_CurrentAttackData.eAttackType = eType;
		m_CurrentAttackData.fDamage = m_Status.fAttackDamage * fAttackRatio;
		m_CurrentAttackData.vAttackPosition = m_pTransformCom->Get_State(STATE::POSITION);
		this->m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackColliderIndex]->Set_Desc(&m_CurrentAttackData);
		})))
		return E_FAIL;

	if (FAILED(m_pPlayerBody->Add_AnimNotify(strAnimName, vTrackPosition.y, [this, iAttackColliderIndex]() {
		this->m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackColliderIndex]->SetEnable(false);
		this->m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackColliderIndex]->Set_Desc(nullptr);
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

	Safe_Release(m_pCamera);

	Safe_Release(m_pPlayerInstance);

	for (_uint i = 0; i < ENUM_CLASS(ARMOR_TYPE::END); i++)
		Safe_Release(m_pEquipArmors[i]);
}
