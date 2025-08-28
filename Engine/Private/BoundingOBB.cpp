#include "EnginePch.h"
#include "BoundingOBB.h"
#include "BoundingSphere.h"
#include "BoundingAABB.h"

CBoundingOBB::CBoundingOBB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBounding { pDevice, pDeviceContext }
{
}

HRESULT CBoundingOBB::Initialize(const BOUNDING_DESC* pDesc)
{
	const BOUNDING_OBB_DESC* pOBBDesc = static_cast<const BOUNDING_OBB_DESC*>(pDesc);

	_float4		vQuaternion = {};

	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pOBBDesc->vAngles.x, pOBBDesc->vAngles.y, pOBBDesc->vAngles.z));

	m_pOriginalDesc = new BoundingOrientedBox(pOBBDesc->vCenter, pOBBDesc->vExtents, vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

	return S_OK;
}

void CBoundingOBB::Update(_fmatrix WorldMatrix, _bool IsRotate)
{
	_matrix TransformMatrix = WorldMatrix;

	if (false == IsRotate)
	{
		TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);
	}

	m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBoundingOBB::Intersect(COLLIDER eType, CBounding* pTarget, _float* pDisance, _float3* pNormal)
{
	_bool		isColl = { false };

	switch (eType)
	{
	case COLLIDER::AABB:
		isColl = m_pDesc->Intersects(*static_cast<CBoundingAABB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::OBB:
		isColl = Intersect_OBB(static_cast<CBoundingOBB*>(pTarget), pDisance, pNormal);
		break;
	case COLLIDER::SPHERE:
		isColl = m_pDesc->Intersects(*static_cast<CBoundingSphere*>(pTarget)->Get_Desc());
		break;
	}

	return isColl;
}

#ifdef _DEBUG

HRESULT CBoundingOBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif

_bool CBoundingOBB::Intersect_OBB(const CBoundingOBB* pTarget, _float* pDisance, _float3* pNormal)
{
	OBBDESC			OBBDesc[2];

	OBBDesc[0] = Compute_OBBDesc();
	OBBDesc[1] = pTarget->Compute_OBBDesc();

	_vector vAxis[9] = {};
	_uint iIndex = {};

	_float fMinDistance = FLT_MAX;

	for (_uint i = 0; i < 3; i++)
	{
		for (_uint j = 0; j < 3; j++)
		{
			_vector vCrossAxis = XMVector3Cross(XMLoadFloat3(&OBBDesc[0].vAlignDir[i]), XMLoadFloat3(&OBBDesc[1].vAlignDir[j]));

			_float length = XMVectorGetX(XMVector3LengthSq(vCrossAxis));
			if (length > 0.0001f)
			{
				vAxis[iIndex++] = XMVector3Normalize(vCrossAxis);
			}
		}
	}

	_float		fDistance[3];
	_float		fAxisDistance = {};

	for (_uint i = 0; i < 2; i++)
	{
		for (_uint j = 0; j < 3; j++)
		{
			fDistance[0] = fabsf(XMVectorGetX(XMVector3Dot(
				XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fDistance[1] =
				fabsf(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[0].vCenterDir[0]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabsf(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[0].vCenterDir[1]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabsf(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[0].vCenterDir[2]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fDistance[2] =
				fabsf(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[1].vCenterDir[0]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabsf(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[1].vCenterDir[1]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j])))) +
				fabsf(XMVectorGetX(XMVector3Dot(
					XMLoadFloat3(&OBBDesc[1].vCenterDir[2]),
					XMLoadFloat3(&OBBDesc[i].vAlignDir[j]))));

			fAxisDistance = (fDistance[1] + fDistance[2]) - fDistance[0];

			if (fAxisDistance < 0.f)
				return false;

			if (fAxisDistance < fMinDistance)
			{
				fMinDistance = fAxisDistance;
				if(nullptr != pNormal)
					*pNormal = OBBDesc[i].vAlignDir[j];
			}
		}
	}

	for (_uint k = 0; k < iIndex; k++)
	{
		fDistance[0] = fabsf(XMVectorGetX(XMVector3Dot(
			XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter), vAxis[k])));

		fDistance[1] =
			fabsf(XMVectorGetX(XMVector3Dot(
				XMLoadFloat3(&OBBDesc[0].vCenterDir[0]), vAxis[k]))) +
			fabsf(XMVectorGetX(XMVector3Dot(
				XMLoadFloat3(&OBBDesc[0].vCenterDir[1]), vAxis[k]))) +
			fabsf(XMVectorGetX(XMVector3Dot(
				XMLoadFloat3(&OBBDesc[0].vCenterDir[2]), vAxis[k])));

		fDistance[2] =
			fabsf(XMVectorGetX(XMVector3Dot(
				XMLoadFloat3(&OBBDesc[1].vCenterDir[0]), vAxis[k]))) +
			fabsf(XMVectorGetX(XMVector3Dot(
				XMLoadFloat3(&OBBDesc[1].vCenterDir[1]), vAxis[k]))) +
			fabsf(XMVectorGetX(XMVector3Dot(
				XMLoadFloat3(&OBBDesc[1].vCenterDir[2]), vAxis[k])));

		fAxisDistance = (fDistance[1] + fDistance[2]) - fDistance[0];

		if (fAxisDistance < 0.f)
			return false;

		if (fAxisDistance < fMinDistance)
		{
			fMinDistance = fAxisDistance;
			if (nullptr != pNormal)
				XMStoreFloat3(pNormal, vAxis[k]);
		}
	}

	if (XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter), XMLoadFloat3(pNormal))) > 0.f)
		XMStoreFloat3(pNormal, XMLoadFloat3(pNormal) * -1.f);

	*pDisance = fMinDistance;

	return true;
}

CBoundingOBB::OBBDESC CBoundingOBB::Compute_OBBDesc() const
{
	OBBDESC			OBBDesc{};

	_float3			vPoints[8];
	m_pDesc->GetCorners(vPoints);

	OBBDesc.vCenter = m_pDesc->Center;
	XMStoreFloat3(&OBBDesc.vCenterDir[0], (XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[1], (XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterDir[2], (XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])) * 0.5f);

	for (size_t i = 0; i < 3; i++)
		XMStoreFloat3(&OBBDesc.vAlignDir[i], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterDir[i])));

	return OBBDesc;
}

CBoundingOBB* CBoundingOBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc)
{
	CBoundingOBB* pInstance = new CBoundingOBB(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX(TEXT("Failed Created : CBoundingOBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoundingOBB::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}
