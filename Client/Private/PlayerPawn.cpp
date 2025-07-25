#include "ClientPch.h"
#include "PlayerPawn.h"
#include "PawnObject.h"
#include "Camera_Target.h"
#include "GameInstance.h"
#include "PlayerState.h"
#include "StateFactory.h"
#include "PlayerBody.h"

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
	if (nullptr == pArg)
		return E_FAIL;

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

	return S_OK;
}

void CPlayerPawn::Priority_Update(_float fTimeDelta)
{
	Bind_InputData();

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
	CPlayerBody* pPlayerBody = static_cast<CPlayerBody*>(Find_PawnObject(TEXT("Player_Body")));
	if (nullptr == pPlayerBody)
		return false;
	return pPlayerBody->AnimIsFinished();
}

_bool CPlayerPawn::AnimCanChange()
{
	CPlayerBody* pPlayerBody = static_cast<CPlayerBody*>(Find_PawnObject(TEXT("Player_Body")));
	if (nullptr == pPlayerBody)
		return false;
	return pPlayerBody->AnimCanChange();
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

	CameraDesc.fDistance = 100.f;
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
	CPawnObject::PAWNOBJECT_DESC PawnObjectDesc = {};
	PawnObjectDesc.pPawnMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if(FAILED(__super::Add_PawnObject(TEXT("Player_Body"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_PawnObject_Player_Body"), &PawnObjectDesc)))
		return E_FAIL;

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
	m_States[ENUM_CLASS(PLAYER_STATE::ROLL)] = pStateFactory->Create(ENUM_CLASS(PLAYER_STATE::ROLL));

	Safe_Release(pStateFactory);

	m_pCurrentState = m_States[ENUM_CLASS(PLAYER_STATE::IDLE)];

	return S_OK;
}

void CPlayerPawn::Bind_InputData()
{
	m_pGameInstance->MoveInput(ENUM_CLASS(CONTROLLER_CHANNEL::MAIN), &m_MoveInput);
	m_pGameInstance->ActionInput(ENUM_CLASS(CONTROLLER_CHANNEL::MAIN), &m_ActionInput);
	m_pGameInstance->CameraInput(ENUM_CLASS(CONTROLLER_CHANNEL::MAIN), &m_CameraInput);

	if (m_pCurrentState)
	{
		m_pCurrentState->InputData(this, m_MoveInput, m_ActionInput);

		m_pCurrentState->Bind_StateFlag(m_iStateFlag);

		for (auto& Pair : m_PawnObjects)
		{
			Pair.second->Bind_PawnData(&m_iStateFlag);
		}
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
