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

	m_fDistance = pDesc->fDistance;
	m_pTarget_TransformCom = pDesc->pTarget_TransformCom;
	Safe_AddRef(m_pTarget_TransformCom);

	return S_OK;
}

void CCamera_Target::Priority_Update(_float fTimeDelta)
{
}

void CCamera_Target::Update(_float fTimeDelta)
{
}

void CCamera_Target::Late_Update(_float fTimeDelta)
{
	Update_CameraPosition(m_pTarget_TransformCom->Get_State(STATE::POSITION));
}

HRESULT CCamera_Target::Render()
{
	return S_OK;
}

void CCamera_Target::Update_CameraPosition(_fvector vTargetPosition)
{
	_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
	_vector vDistance = XMVectorScale(vDir, m_fDistance);
	_vector vPosition = XMVectorSubtract(vTargetPosition, vDistance);
	
	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
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

	Safe_Release(m_pTarget_TransformCom);
}
