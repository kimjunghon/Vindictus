#include "ClientPch.h"
#include "Camera_Target.h"

CCamera_Target::CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera { pDevice, pDeviceContext }
{
}

CCamera_Target::CCamera_Target(const CCamera_Target& Prototype)
	: CCamera { Prototype }
{
}

HRESULT CCamera_Target::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Target::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_TARGET_DESC* pDesc = static_cast<CAMERA_TARGET_DESC*>(pArg);

	m_TargetMatrix = pDesc->TargetMatrix;
	m_fDistance = pDesc->fDistance;
	m_fHeight = pDesc->fHeight;

	return S_OK;
}

void CCamera_Target::Priority_Update(_float fTimeDelta)
{
}

void CCamera_Target::Update(_float fTimeDelta)
{
//	if (m_pGameInstance->Get_KeyPressing(DIK_W))
//	{
//		m_pTransformCom->Go_Straight(fTimeDelta);
//	}
//	if (m_pGameInstance->Get_KeyPressing(DIK_S))
//	{
//		m_pTransformCom->Go_Backward(fTimeDelta);
//	}
//	if (m_pGameInstance->Get_KeyPressing(DIK_A))
//	{
//		m_pTransformCom->Go_Left(fTimeDelta);
//	}
//	if (m_pGameInstance->Get_KeyPressing(DIK_D))
//	{
//		m_pTransformCom->Go_Right(fTimeDelta);
//	}

	_int    iMouseMove = {};
	if (m_pGameInstance->Get_KeyPressing(DIK_LALT))
	{
		if (iMouseMove = m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * 0.5f);
		}

		if (iMouseMove = m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * 0.5f);
		}
	}
}

void CCamera_Target::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Target::Render()
{
	return S_OK;
}

CCamera_Target* CCamera_Target::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCamera_Target* pInstance = new CCamera_Target(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CCamera_Target"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Target::Clone(void* pArg)
{
	CCamera_Target* pInstance = new CCamera_Target(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CCamera_Target"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Target::Free()
{
	__super::Free();

}
