#include "EnginePch.h"
#include "VIBuffer_Point_Instance.h"
#include "GameInstance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer_Instance { pDevice, pDeviceContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
	: CVIBuffer_Instance { Prototype }
	, m_vPivot{ Prototype.m_vPivot }
	, m_pSpeeds{ Prototype.m_pSpeeds }
	, m_IsLoop{ Prototype.m_IsLoop }
	, m_eType { Prototype.m_eType }
#ifdef _DEBUG
	, m_Desc{ Prototype.m_Desc }
#endif
{
}



void CVIBuffer_Point_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);


	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.x = 0.f;
		pVertices[i].vPosition = pInstanceVertices[i].vPosition;
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	const POINT_INSTANCE_DESC* pPointDesc = static_cast<const POINT_INSTANCE_DESC*>(pDesc);

#ifdef _DEBUG
	m_Desc = *pPointDesc;
#endif
	m_vSourceColor = pPointDesc->vSourceColor;

	m_eType = pPointDesc->eType;
	m_vPivot = pPointDesc->vPivot;
	m_IsLoop = pPointDesc->IsLoop;
	m_IsCircle = pPointDesc->IsCircle;
	m_vAngle = pPointDesc->vAngle;

	m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
	m_iNumInstance = pPointDesc->iNumInstance;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertexBuffers = 2;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXINSTANCE_PARTICLE[m_iNumInstance];
	m_pSpeeds = new _float[m_iNumInstance];



	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

		_float		fScale = m_pGameInstance->Rand(pPointDesc->vSize.x, pPointDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pPointDesc->vLifeTime.x, pPointDesc->vLifeTime.y);
		m_pSpeeds[i] = m_pGameInstance->Rand(pPointDesc->vSpeed.x, pPointDesc->vSpeed.y);

		if (m_IsCircle)
		{
			_float fRadian = XMConvertToRadians(m_pGameInstance->Rand(m_vAngle.x, m_vAngle.y));
			pInstanceVertices[i].vPosition = _float4(
				cos(fRadian) * pPointDesc->vRange.x,
				m_pGameInstance->Rand(pPointDesc->vCenter.y - pPointDesc->vRange.y * 0.5f, pPointDesc->vCenter.y + pPointDesc->vRange.y * 0.5f),
				sin(fRadian) * pPointDesc->vRange.z,
				1.f
			);
		}
		else
		{
			pInstanceVertices[i].vPosition = _float4(
				m_pGameInstance->Rand(pPointDesc->vCenter.x - pPointDesc->vRange.x * 0.5f, pPointDesc->vCenter.x + pPointDesc->vRange.x * 0.5f),
				m_pGameInstance->Rand(pPointDesc->vCenter.y - pPointDesc->vRange.y * 0.5f, pPointDesc->vCenter.y + pPointDesc->vRange.y * 0.5f),
				m_pGameInstance->Rand(pPointDesc->vCenter.z - pPointDesc->vRange.z * 0.5f, pPointDesc->vCenter.z + pPointDesc->vRange.z * 0.5f),
				1.f
			);
		}


		_vector vDir = XMVectorSetW(XMVector3Normalize(XMVectorSubtract(XMLoadFloat4(&pInstanceVertices[i].vPosition), XMLoadFloat3(&m_vPivot))),0.f);

		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		
		if (XMVectorGetX(XMVector3Length(vDir)) >= 0.1f)
		{
			_vector vLook = XMVector3Normalize(XMVectorSetW(XMVectorSubtract(XMLoadFloat4(m_pGameInstance->Get_CamPosition()), XMLoadFloat4(&pInstanceVertices[i].vPosition)), 0.f));

			XMStoreFloat4(&pInstanceVertices[i].vUp, XMVectorScale(vDir, fScale));
			XMStoreFloat4(&pInstanceVertices[i].vRight, XMVectorScale(XMVector3Normalize(XMVector3Cross(XMVector3Normalize(XMLoadFloat4(&pInstanceVertices[i].vUp)), vLook)), fScale));
			XMStoreFloat4(&pInstanceVertices[i].vLook, XMVectorScale(XMVector3Normalize(XMVector3Cross(XMVector3Normalize(XMLoadFloat4(&pInstanceVertices[i].vUp)), XMLoadFloat4(&pInstanceVertices[i].vRight))), fScale));
		}


		//pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		//pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		//
		//pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);


		pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);

	}

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	m_pVBInstance,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render()
{
	m_pDeviceContext->DrawInstanced(1, m_iNumInstance, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instance::Update(_float fTimeDelta, _bool* pIsFinshed)
{
	switch (m_eType)
	{
	case FX_POINT_TYPE::SPREAD:
		Spread(fTimeDelta, pIsFinshed);
		break;
	case FX_POINT_TYPE::DROP:
		Drop(fTimeDelta, pIsFinshed);
		break;
	case FX_POINT_TYPE::LOOP:
		Loop(fTimeDelta, pIsFinshed);
		break;
	case FX_POINT_TYPE::FX_GRAVITY:
		Gravity(fTimeDelta, pIsFinshed);
		break;
	}
}

void CVIBuffer_Point_Instance::Spread(_float fTimeDelta, _bool* pIsFinished)
{
	D3D11_MAPPED_SUBRESOURCE SubResource = {};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vPosition) - XMLoadFloat3(&m_vPivot), 0.f));

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			if (m_IsLoop)
			{
				pVertices[i].vLifeTime.x = 0.f;
				pVertices[i].vPosition = pInstanceVertices[i].vPosition;
			}
			else
			{
				if(pIsFinished)
					*pIsFinished = true;
			}
		}
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Drop(_float fTimeDelta, _bool* pIsFinished)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);


	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector	vMoveDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			if (m_IsLoop)
			{
				pVertices[i].vLifeTime.x = 0.f;
				pVertices[i].vPosition = pInstanceVertices[i].vPosition;
			}
			else
				*pIsFinished = true;
		}
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Loop(_float fTimeDelta, _bool* pIsFinished)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vPosition) - XMLoadFloat3(&m_vPivot), 0.f));

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vMoveDir * m_pSpeeds[i] * fTimeDelta);

		if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
			m_IsLoop = true;
		else if (pVertices[i].vLifeTime.x <= 0.f)
			m_IsLoop = false;

		m_IsLoop ? pVertices[i].vLifeTime.x -= fTimeDelta : pVertices[i].vLifeTime.x += fTimeDelta;
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Gravity(_float fTimeDelta, _bool* pIsFinished)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	m_fCurrentGravity -= (GRAVITY * fTimeDelta);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vPosition) - XMLoadFloat3(&m_vPivot), 0.f));

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vMoveDir * m_pSpeeds[i] * fTimeDelta);

		pVertices[i].vPosition.y += (m_pSpeeds[i] + m_fCurrentGravity) * fTimeDelta;
		

		pVertices[i].vLifeTime.x += fTimeDelta;
		if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			if (m_IsLoop)
			{
				pVertices[i].vLifeTime.x = 0.f;
				pVertices[i].vPosition = pInstanceVertices[i].vPosition;
				m_fCurrentGravity = 0.f;
			}
			else
			{
				if(pIsFinished)
					*pIsFinished = true;			
				m_fCurrentGravity = 0.f;
			}

		}
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);

}

void CVIBuffer_Point_Instance::Circle(_float fTimeDelta, _bool* pIsFinished)
{

}

void CVIBuffer_Point_Instance::Sort(_fmatrix WorldMatrix)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	sort(pVertices, pVertices + m_iNumInstance, [&](const VTXINSTANCE_PARTICLE& Sour, const VTXINSTANCE_PARTICLE& Dest){
		
		_vector vSourPosition = XMVector3TransformCoord(XMLoadFloat4(&Sour.vPosition), WorldMatrix);
		_vector vDestPosition = XMVector3TransformCoord(XMLoadFloat4(&Dest.vPosition), WorldMatrix);
		vSourPosition = XMVector3TransformCoord(vSourPosition, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
		vDestPosition = XMVector3TransformCoord(vDestPosition, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));

		_float fSourDepth = XMVectorGetZ(vSourPosition);
		_float fDestDepth = XMVectorGetZ(vDestPosition);
		
		return fSourDepth > fDestDepth;
		});

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_IsCloned)
		Safe_Delete_Array(m_pSpeeds);
}
