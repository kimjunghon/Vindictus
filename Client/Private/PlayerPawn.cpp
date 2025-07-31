#include "ClientPch.h"
#include "PlayerPawn.h"
#include "SocketObject.h"
#include "Camera_Target.h"
#include "GameInstance.h"
#include "PlayerState.h"
#include "StateFactory.h"
#include "PlayerBody.h"
#include "Armor.h"

CPlayerPawn::CPlayerPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawn { pDevice, pDeviceContext }
{
}

CPlayerPawn::CPlayerPawn(const CPlayerPawn& Prototype)
	: CPawn { Prototype }
{
}

HRESULT CPlayerPawn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerPawn::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Camera()))
		return E_FAIL;
	
	if (FAILED(Ready_PawnObjects()))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	m_vPlayerMoveDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	m_vPlayerRoationQuat = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	m_vPrevPlayerRotationQuat = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	m_fSpeed = 10.f;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE) | ENUM_CLASS(IDLE_FLAG::DEFAULT);

	//test
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-10.f, 0.f, 10.f, 1.f));

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

	for (auto& Pair : m_PawnObjects)
		Pair.second->Late_Update(fTimeDelta);

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

HRESULT CPlayerPawn::EquipArmor(const _wstring& strArmorTag, CArmor* pArmor, ARMOR_TYPE eArmorType)
{
	if (nullptr == pArmor || eArmorType == ARMOR_TYPE::END)
		return E_FAIL;

	if (eArmorType == ARMOR_TYPE::HEAD)
		m_pPlayerBody->EquipHead();

	if (m_strEquipArmors[ENUM_CLASS(eArmorType)].size() > 0)
		UnEquipArmor(eArmorType);

	m_strEquipArmors[ENUM_CLASS(eArmorType)] = strArmorTag;
	
	pArmor->Equip(m_pPlayerBody->Get_ParentModelPtr());

	Add_PawnObject(strArmorTag, pArmor);

	return S_OK;
}

HRESULT CPlayerPawn::UnEquipArmor(ARMOR_TYPE eArmorType)
{
	if (FAILED(Remove_PawnObject(m_strEquipArmors[ENUM_CLASS(eArmorType)])))
		return E_FAIL;

	m_strEquipArmors[ENUM_CLASS(eArmorType)].clear();

	return S_OK;
}

void CPlayerPawn::Move(_float fTimeDelta)
{

	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);

	vPosition = XMVectorAdd(vPosition, XMVectorScale(m_vPlayerMoveDir, m_fSpeed * m_fSpeedRatio * fTimeDelta));

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
}

HRESULT CPlayerPawn::Ready_Components()
{
	return S_OK;
}

HRESULT CPlayerPawn::Ready_Camera()
{
	CCamera_Target::CAMERA_TARGET_DESC CameraDesc = {};

	CameraDesc.vEye = _float4(0.f, 200.f, -150.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFov = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fSpeedPerSec = 0.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CameraDesc.fDistance = 50.f;
	CameraDesc.fHeight = 30.f;
	CameraDesc.TargetMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	CCamera* pCamera = { nullptr };

	if (FAILED(m_pGameInstance->Add_CameraToManager(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Target"), TEXT("Player_Camera"), &pCamera, &CameraDesc)))
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

	if (FAILED(__super::Add_PawnObject(TEXT("Player_Body"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_Body"), &PlayerBodyDesc)))
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
	CSocketObject::SOCKETOBJECT_DESC SocketObjectDesc = {};
	SocketObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	SocketObjectDesc.pSocketMatrix = m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_RH");

	if (FAILED(__super::Add_PawnObject(TEXT("Bastard_Sword"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_BastardSword"), &SocketObjectDesc)))
		return E_FAIL;

	SocketObjectDesc.pSocketMatrix = m_pPlayerBody->SocketCombinedMatrixPtr("ValveBiped.Anim_Attachment_LF");

	if (FAILED(__super::Add_PawnObject(TEXT("Round_Shield"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_RoundShield"), &SocketObjectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerPawn::Ready_Armors()
{
	CArmor::ARMOR_DESC ArmorDesc = {};
	ArmorDesc.eArmorType = ARMOR_TYPE::UPPER;
	ArmorDesc.iArmorModelPrototypeLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Upper");
	ArmorDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	ArmorDesc.tArmorInfo = { 10.f, 5.f, 30.f };

	CArmor* pArmor = static_cast<CArmor*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	if (nullptr == pArmor)
		return E_FAIL;

	EquipArmor(TEXT("LightMale_Upper"), pArmor, ARMOR_TYPE::UPPER);


	ArmorDesc.eArmorType = ARMOR_TYPE::LOWER;
	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Lower");
	pArmor = static_cast<CArmor*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));

	EquipArmor(TEXT("LightMale_Lower"), pArmor, ARMOR_TYPE::LOWER);


	ArmorDesc.eArmorType = ARMOR_TYPE::HAND;
	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Hand");
	pArmor = static_cast<CArmor*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));

	EquipArmor(TEXT("LightMale_Hand"), pArmor, ARMOR_TYPE::HAND);


	ArmorDesc.eArmorType = ARMOR_TYPE::HEAD;
	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Head");
	pArmor = static_cast<CArmor*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));

	EquipArmor(TEXT("LightMale_Head"), pArmor, ARMOR_TYPE::HEAD);


	ArmorDesc.eArmorType = ARMOR_TYPE::FOOT;
	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Foot");
	pArmor = static_cast<CArmor*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));

	EquipArmor(TEXT("LightMale_Foot"), pArmor, ARMOR_TYPE::FOOT);


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

	Safe_Release(pStateFactory);

	m_pCurrentState = m_States[ENUM_CLASS(PLAYER_STATE::IDLE)];

	return S_OK;
}

void CPlayerPawn::Compute_WorldMatrix()
{
	_float3 vScale = m_pTransformCom->Get_Scaled();
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vAnimPosition = *m_pAnimMovement;
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

	_matrix RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	if (XMVectorGetX(XMVector3Length(vAnimPosition)) >= 30.f)
		int a = 10;

	vAnimPosition = XMVector3Transform(vAnimPosition, RotationMatrix);

	vPosition = XMVectorSetW(XMVectorAdd(vPosition, vAnimPosition), 1.f);

	_matrix ScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&vScale));
	_matrix PositionMatrix = XMMatrixTranslationFromVector(vPosition);

	_matrix WorldMatrix = XMMatrixMultiply(XMMatrixMultiply(ScaleMatrix, RotationMatrix), PositionMatrix);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CPlayerPawn::Bind_InputData(_float fTimeDelta)
{
	m_pGameInstance->MoveInput(ENUM_CLASS(CONTROLLER_CHANNEL::MAIN), &m_MoveInput);
	m_pGameInstance->ActionInput(ENUM_CLASS(CONTROLLER_CHANNEL::MAIN), &m_ActionInput);
	m_pGameInstance->CameraInput(ENUM_CLASS(CONTROLLER_CHANNEL::MAIN), &m_CameraInput);


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
}
