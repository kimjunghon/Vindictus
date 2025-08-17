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
	: CPawn { pDevice, pDeviceContext }
	, m_pPlayerInstance{ CPlayerInstance::GetInstance() }
{
	Safe_AddRef(m_pPlayerInstance);
}

CPlayerPawn::CPlayerPawn(const CPlayerPawn& Prototype)
	: CPawn { Prototype }
	, m_pPlayerInstance { Prototype.m_pPlayerInstance}
{
	Safe_AddRef(m_pPlayerInstance);
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

	m_vPlayerRoationQuat = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	m_vPrevPlayerRotationQuat = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	m_fSpeed = 50.f;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE) | ENUM_CLASS(IDLE_FLAG::DEFAULT);

	
	m_pGameInstance->Subscribe<EVENT_CHANGE_WEAPON>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [this](const EVENT_CHANGE_WEAPON& Event) {
		this->Event_ChangeWeapon(Event); });
	
	m_pGameInstance->Subscribe<EVENT_CHANGE_ARMOR>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [this](const EVENT_CHANGE_ARMOR& Event) {
		this->Event_ChangeArmor(Event); });

	return S_OK;
}

void CPlayerPawn::Priority_Update(_float fTimeDelta)
{
	Bind_InputData(fTimeDelta);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Priority_Update(fTimeDelta);
}

void CPlayerPawn::Update(_float fTimeDelta)
{
	m_pCurrentState->Update(this, fTimeDelta);

	for (auto& Pair : m_PawnObjects)
		Pair.second->Update(fTimeDelta);
}

void CPlayerPawn::Late_Update(_float fTimeDelta)
{
	Compute_WorldMatrix();

	Update_ColliderMatrix();

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
#endif
}

HRESULT CPlayerPawn::Render()
{

#ifdef _DEBUG
	//for (auto& Pair : m_Colliders)
	//{
	//	for (auto& pCollider : Pair.second)
	//	{
	//		pCollider->Render();
	//	}
	//}
	for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::ATTACK].size(); i++)
	{
		m_AttackColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[i]), m_pTransformCom->Get_WorldMatrix());
		m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->Render();
	}
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

_bool CPlayerPawn::AnimIsFinished()
{
	return m_pPlayerBody->AnimIsFinished();
}

_bool CPlayerPawn::AnimCanChange()
{
	return m_pPlayerBody->AnimCanChange();
}

void CPlayerPawn::Compute_PlayerMoveDir()
{
	m_vPlayerMoveDir = m_pCamera->Compute_PlayerMoveDir(m_MoveInput.vDir);

	_float fYaw = atan2f(XMVectorGetX(m_vPlayerMoveDir), XMVectorGetZ(m_vPlayerMoveDir));
	m_vPlayerRoationQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fYaw);

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
		m_pPlayerBody->EquipHead();

	if (m_strEquipArmors[ENUM_CLASS(eArmorType)].size() > 0)
		UnEquipArmor(ENUM_CLASS(eArmorType));

	m_strEquipArmors[ENUM_CLASS(eArmorType)] = strArmorName;
	
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
		m_pPlayerBody->UnEquipHead();

	if (FAILED(Remove_PawnObject(m_strEquipArmors[iArmorTypeIndex])))
		return E_FAIL;

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

void CPlayerPawn::OnCollisionHit(const CCollider::COLLISION_DATA& CollisionData)
{
	if (CollisionData.pCollider->Get_ColliderOwner() == COLLIDER_OWNER::MONSTER)
	{
		m_eHitType = HIT_TYPE::FRONT;

		Change_State(ENUM_CLASS(PLAYER_STATE::HIT));

		if (FAILED(m_pPlayerBody->Forcing_Play_Animation()))
			return;
	}
}

void CPlayerPawn::Move(_float fTimeDelta)
{
	_vector vMovePosition = XMVectorScale(m_vPlayerMoveDir, m_fSpeed * m_fSpeedRatio * fTimeDelta);
	
	m_pTransformCom->MovePositionToVector(vMovePosition, m_pNavigation);
}

void CPlayerPawn::Update_ColliderMatrix()
{
	for (auto& pBoundingCollider : m_Colliders[COLLIDER_CHANNEL::BOUNDING])
		pBoundingCollider->Update(m_pTransformCom->Get_WorldMatrix());

	for (auto& pBodyCollider : m_Colliders[COLLIDER_CHANNEL::BODY])
		pBodyCollider->Update(m_pTransformCom->Get_WorldMatrix());

	for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::HIT].size(); i++)
	{
		m_HitColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_HitColliderSocketMatrix[i]), m_pTransformCom->Get_WorldMatrix());
		m_Colliders[COLLIDER_CHANNEL::HIT][i]->Update(m_HitColliderCombinedMatrix[i]);
	}

	for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::ATTACK].size(); i++)
	{
		m_AttackColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[i]), m_pTransformCom->Get_WorldMatrix());
		m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->Update(m_AttackColliderCombinedMatrix[i]);
	}
}

HRESULT CPlayerPawn::Init_Level(_int iCellIndex, _float3 vStartPostion)
{
	m_pNavigation = m_pGameInstance->Clone_CurrentNavigation(iCellIndex);
	if (nullptr == m_pNavigation)
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

	if (FAILED(__super::Add_PawnObject(TEXT("Player_Body"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player_Body"), &PlayerBodyDesc)))
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

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Bounding()
{
	CBoundingAABB::BOUNDING_AABB_DESC  AABBDesc = {};
	AABBDesc.vExtents = _float3(20.f, 20.f, 20.f);
	AABBDesc.vCenter = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.eChannel = COLLIDER_CHANNEL::BOUNDING;
	ColliderDesc.eOwner = COLLIDER_OWNER::PLAYER;
	ColliderDesc.BoundingDesc = &AABBDesc;

	CCollider* pBoundingCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Bounding"), reinterpret_cast<CComponent**>(&pBoundingCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::BOUNDING].push_back(pBoundingCollider);

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Collider_Body()
{
	CBoundingOBB::BOUNDING_OBB_DESC OBBDesc = {};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(10.f, 18.f, 10.f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.eChannel = COLLIDER_CHANNEL::BODY;
	ColliderDesc.eOwner = COLLIDER_OWNER::PLAYER;
	ColliderDesc.BoundingDesc = &OBBDesc;

	CCollider* pBodyCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), reinterpret_cast<CComponent**>(&pBodyCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::BODY].push_back(pBodyCollider);

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

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_Head"), &OBBDesc, ENUM_CLASS(HIT_COLLIDER::HEAD), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Head1"))))
		return E_FAIL;

	/* Com_Collider_Hit_Upper */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(6.f, 10.f, 6.f);
	OBBDesc.vCenter = _float3(3.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_Upper"), &OBBDesc, ENUM_CLASS(HIT_COLLIDER::UPPER), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Spine1"))))
		return E_FAIL;

	/* Com_Collider_Hit_Lower */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(45.f));
	OBBDesc.vExtents = _float3(6.f, 10.f, 6.f);
	OBBDesc.vCenter = _float3(-4.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_Lower"), &OBBDesc, ENUM_CLASS(HIT_COLLIDER::LOWER), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_Pelvis"))))
		return E_FAIL;

	/* Com_Collider_Hit_RightArm */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 8.f, 3.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_RightArm"), &OBBDesc, ENUM_CLASS(HIT_COLLIDER::R_ARM), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_ForeTwist1"))))
		return E_FAIL;

	/* Com_Collider_Hit_LeftArm */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 8.f, 3.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_LeftArm"), &OBBDesc, ENUM_CLASS(HIT_COLLIDER::L_ARM), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_ForeTwist1"))))
		return E_FAIL;

	/* Com_Collider_Hit_LeftLeg */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 11.f, 3.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_LeftLeg"), &OBBDesc, ENUM_CLASS(HIT_COLLIDER::L_LEG), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_Calf"))))
		return E_FAIL;

	/* Com_Collider_Hit_RightLeg */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(3.f, 11.f, 3.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(Add_Collider_Hit(TEXT("Com_Collider_Hit_RightLeg"), &OBBDesc, ENUM_CLASS(HIT_COLLIDER::R_LEG), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_Calf"))))
		return E_FAIL;

	return S_OK;
}


HRESULT CPlayerPawn::Add_Collider_Hit(const _wstring& strColliderTag, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix)
{
	if (iColliderIndex >= ENUM_CLASS(HIT_COLLIDER::END) ||
		nullptr == pSocketCombinedMatrix)
		return E_FAIL;
	
	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.eChannel = COLLIDER_CHANNEL::HIT;
	ColliderDesc.eOwner = COLLIDER_OWNER::PLAYER;
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pHitCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pHitCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::HIT][iColliderIndex] = pHitCollider;
	m_HitColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

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
	OBBDesc.vExtents = _float3(10.f, 18.f, 10.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y, 0.f, 0.f);

	if (FAILED(Add_Collider_Attack(TEXT("Com_Collider_Attack_Sword"), & OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::SWORD), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_RH"))))
		return E_FAIL;

	/* Com_Collider_Attack_Shield */
	OBBDesc.vAngles = _float3(XMConvertToRadians(60.f), XMConvertToRadians(90.f), XMConvertToRadians(180.f));
	OBBDesc.vExtents = _float3(15.f, 10.f, 15.f);
	OBBDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Attack(TEXT("Com_Collider_Attack_Shield"), &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::SHILED), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_LF"))))
		return E_FAIL;

	/* Com_Collider_Attack_Left_Leg */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(15.f, 20.f, 15.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y - 5.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Attack(TEXT("Com_Collider_Attack_Left_Leg"), &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::LEFT_LEG), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_L_Calf"))))
		return E_FAIL;

	/* Com_Collider_Attack_Right_Leg */
	OBBDesc.vAngles = _float3(0.f, 0.f, XMConvertToRadians(90.f));
	OBBDesc.vExtents = _float3(15.f, 20.f, 15.f);
	OBBDesc.vCenter = _float3(OBBDesc.vExtents.y - 5.f, 0.f, 0.f);

	if (FAILED(Add_Collider_Attack(TEXT("Com_Collider_Attack_Right_Leg"), &OBBDesc, ENUM_CLASS(ATTACK_COLLIDER::RIGHT_LEG), m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Bip01_R_Calf"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Add_Collider_Attack(const _wstring& strColliderTag, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix)
{
	if (iColliderIndex >= ENUM_CLASS(ATTACK_COLLIDER::END) ||
		nullptr == pSocketCombinedMatrix)
		return E_FAIL;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.eChannel = COLLIDER_CHANNEL::ATTACK;
	ColliderDesc.eOwner = COLLIDER_OWNER::PLAYER;
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pAttackCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pAttackCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::ATTACK][iColliderIndex] = pAttackCollider;
	m_AttackColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

	return S_OK;
}

void CPlayerPawn::Compute_WorldMatrix()
{
	_vector vAnimPosition = XMVectorSetY(*m_pAnimMovement,0.f);
	_vector vAnimRotation = *m_pAnimRotation;

	_vector vRotation = {};

	if (false == XMVector4Equal(m_vPlayerRoationQuat, m_vPrevPlayerRotationQuat))
	{
		vRotation = XMQuaternionMultiply(m_vPlayerRoationQuat, XMQuaternionInverse(m_vPrevPlayerRotationQuat));

		vRotation = XMQuaternionSlerp(XMQuaternionIdentity(), vRotation, 0.125f);

		m_vPrevPlayerRotationQuat = XMQuaternionMultiply(vRotation, m_vPrevPlayerRotationQuat);

		vRotation = m_vPrevPlayerRotationQuat;
	}
	else
		vRotation = m_vPlayerRoationQuat;

	m_pTransformCom->RotateQuaternion(vRotation);

	_matrix PositionMatrix = XMMatrixTranslationFromVector(vAnimPosition);
	
	m_pTransformCom->MovePositionToMatrix(PositionMatrix, m_pNavigation);

	m_pTransformCom->Set_State(STATE::POSITION,
		m_pNavigation->Compute_OnCell(m_pTransformCom->Get_State(STATE::POSITION)));
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
	Safe_Release(m_pNavigation);

	for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
		{
			Safe_Release(pCollider);
		}
	}
	m_Colliders.clear();
}
