#include "EnginePch.h"
#include "Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent {pDevice, pDeviceContext}
{
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_Shader_WorldMatrix(CShader* pShader, const _char* pConstantWorldMatrixName)
{
	return pShader->Bind_Matrix(pConstantWorldMatrixName, &m_WorldMatrix);
}

void CTransform::MovePositionToVector(_fvector vMovePosition, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE::POSITION);
	
	_vector vNextPosition = XMVectorAdd(vPosition, vMovePosition);

	_float3* pNormal = nullptr;
	
	_bool IsMove = pNavigation->isMove(vNextPosition, &pNormal);

	if (IsMove)
		Set_State(STATE::POSITION, vNextPosition);
	else
	{
		if(nullptr != pNormal)	
		{
			_vector vMoveDir = XMVectorSubtract(vNextPosition, vPosition);
			Sliding(vMoveDir, XMLoadFloat3(pNormal), pNavigation);
		}
	}
}

void CTransform::MovePositionToMatrix(_fmatrix PositionMatrix, CNavigation* pNavigation)
{
	_matrix WorldMatrix = Get_WorldMatrix();

	_matrix NextWorldMatrix = PositionMatrix * WorldMatrix;

	_float3* pNormal = nullptr;

	_bool IsMove = pNavigation->isMove(NextWorldMatrix, &pNormal);

	if (IsMove)
		Set_WorldMatrix(NextWorldMatrix);
	else
	{
		if (nullptr != pNormal)
		{
			_vector vNextPosition = NextWorldMatrix.r[3];
			_vector vPosition = WorldMatrix.r[3];

			_vector vMoveDir = XMVectorSubtract(vNextPosition, vPosition);
			Sliding(vMoveDir, XMLoadFloat3(pNormal), pNavigation);
		}
	}
}

void CTransform::Sliding(_fvector vDir, _fvector vNormal, CNavigation* pNavigation)
{
	_vector vBlockNormal = XMVector3Normalize(vNormal);

	if (XMVectorGetX(XMVector3Length(vBlockNormal)) == 0.f)
		return;

	_vector vSlideDir = vDir - XMVector3Dot(vDir, vBlockNormal) * vBlockNormal;

	_vector vPosition = Get_State(STATE::POSITION);

	_vector vNextPosition = XMVectorAdd(vPosition, vSlideDir);

	_bool IsMove = pNavigation->isMove(vNextPosition);

	if (IsMove)
		Set_State(STATE::POSITION, vNextPosition);
}

void CTransform::Scale(_float3 vScale)
{
	Set_State(STATE::RIGHT, XMVectorScale(XMVector3Normalize(Get_State(STATE::RIGHT)), vScale.x));
	Set_State(STATE::UP, XMVectorScale(XMVector3Normalize(Get_State(STATE::UP)), vScale.y));
	Set_State(STATE::LOOK, XMVectorScale(XMVector3Normalize(Get_State(STATE::LOOK)), vScale.z));
}

void CTransform::Scaling(_float3 vScale)
{
	Set_State(STATE::RIGHT, XMVectorScale(Get_State(STATE::RIGHT), vScale.x));
	Set_State(STATE::UP, XMVectorScale(Get_State(STATE::UP), vScale.y));
	Set_State(STATE::LOOK, XMVectorScale(Get_State(STATE::LOOK), vScale.z));
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition += XMVectorScale(XMVector4Normalize(vLook), m_fSpeedPerSec * fTimeDelta);
	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= XMVectorScale(XMVector4Normalize(vLook), m_fSpeedPerSec * fTimeDelta);
	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVectorScale(XMVector4Normalize(vRight), m_fSpeedPerSec * fTimeDelta);
	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition += XMVectorScale(XMVector4Normalize(vRight), m_fSpeedPerSec * fTimeDelta);
	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3 vScaled = Get_Scaled();

	_vector vRight = XMVectorScale(XMVectorSet(1.f, 0.f, 0.f, 0.f), vScaled.x);
	_vector vUp = XMVectorScale(XMVectorSet(0.f, 1.f, 0.f, 0.f), vScaled.y);
	_vector vLook = XMVectorScale(XMVectorSet(0.f, 0.f, 1.f, 0.f), vScaled.z);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::RotateQuaternion(_fvector Quaternion)
{
	_float3 vScaled = Get_Scaled();

	_vector vRight = XMVectorScale(XMVectorSet(1.f, 0.f, 0.f, 0.f), vScaled.x);
	_vector vUp = XMVectorScale(XMVectorSet(0.f, 1.f, 0.f, 0.f), vScaled.y);
	_vector vLook = XMVectorScale(XMVectorSet(0.f, 0.f, 1.f, 0.f), vScaled.z);

	_matrix RotationMatrix = XMMatrixRotationQuaternion(Quaternion);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector vRight = Get_State(STATE::RIGHT);
	_vector vUp = Get_State(STATE::UP);
	_vector vLook = Get_State(STATE::LOOK);

	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector vLook = XMVectorSubtract(vAt, Get_State(STATE::POSITION));

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3 vScaled = Get_Scaled();
	
	Set_State(STATE::RIGHT,XMVectorScale(XMVector3Normalize(vRight), vScaled.x));
	Set_State(STATE::UP, XMVectorScale(XMVector3Normalize(vUp), vScaled.y));
	Set_State(STATE::LOOK, XMVectorScale(XMVector3Normalize(vLook), vScaled.z));
}

void CTransform::Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = XMVectorSubtract(vTargetPos, Get_State(STATE::POSITION));
	
	if(XMVectorGetX(XMVector3Length(vLook)) >= fLimit)
		vPosition += XMVectorScale(XMVector4Normalize(vLook), m_fSpeedPerSec * fTimeDelta);

	Set_State(STATE::POSITION, vPosition);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTransform* pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
