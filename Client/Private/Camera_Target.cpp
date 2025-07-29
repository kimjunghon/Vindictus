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


	m_fMinDistance = 10.f;
	m_fMaxDistance = 300.f;

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
}

HRESULT CCamera_Target::Render()
{
	return S_OK;
}

void CCamera_Target::Bind_InputData(const INPUT_CAMERA_DESC& CameraInput)
{
	m_fDistance -= CameraInput.fDistance * 0.1f;

	if (m_fDistance >= m_fMaxDistance)
		m_fDistance = m_fMaxDistance;
	else if (m_fDistance <= m_fMinDistance)
		m_fDistance = m_fMinDistance;

	_vector vTargetPosition = {};
	_vector vCameraPos = {};

	memcpy(&vTargetPosition, m_TargetMatrix->m[3], sizeof(_vector));
	
	vTargetPosition = XMVectorSetY(vTargetPosition, XMVectorGetY(vTargetPosition) + m_fHeight);
	vCameraPos = XMVectorSubtract(vTargetPosition, XMVectorScale(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fDistance));

	if (CameraInput.bRotate || XMVectorGetX(XMVector4Length(m_vLook)) == 0.f)
	{
		m_vLook = XMVectorSubtract(vTargetPosition, vCameraPos);

		m_fPitch = m_fPitch + CameraInput.vCameraRotate.y;
		m_fYaw = fmod(m_fYaw + CameraInput.vCameraRotate.x, 360.f);

		if (m_fPitch < 0.f)
			m_fPitch = max(m_fPitch, -70.f);
		else
			m_fPitch = min(m_fPitch, 80.f);

		_vector vRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_fPitch), XMConvertToRadians(m_fYaw), 0.f);

		m_vLook = XMVector4Normalize(XMVector4Transform(m_vLook, XMMatrixRotationQuaternion(vRotation)));
	}

	m_vPosition = vTargetPosition - (m_vLook * m_fDistance);

	m_pTransformCom->Set_State(STATE::POSITION, m_vPosition);
	m_pTransformCom->LookAt(vTargetPosition);
}

_vector CCamera_Target::Compute_PlayerMoveDir(_vector vInputDir)
{
	_vector vPlayerMoveDir = {};

	_vector vLook = XMVectorSet(XMVectorGetX(m_vLook), 0.f, XMVectorGetZ(m_vLook), 0.f);
	_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);

	vPlayerMoveDir = XMVector3Normalize(XMVectorAdd(XMVectorScale(XMVector3Normalize(vLook), XMVectorGetZ(vInputDir)),
													XMVectorScale(XMVector3Normalize(vRight), XMVectorGetX(vInputDir))));

	return vPlayerMoveDir;
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
