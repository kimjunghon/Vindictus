#include "EnginePch.h"
#include "VIBuffer_Trail.h"
#include "GameInstance.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer { pDevice, pDeviceContext }
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
	: CVIBuffer { Prototype }
	, m_iNumMaxNode { Prototype.m_iNumMaxNode }
	, m_iNumSample { Prototype.m_iNumSample }
	, m_VBDesc { Prototype.m_VBDesc }
	, m_pVertices { Prototype.m_pVertices }
	, m_IBDesc { Prototype.m_IBDesc }
	, m_pIndices { Prototype.m_pIndices }
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype(_uint iNumMaxNode)
{
	m_iNumSample = 3;

	m_iNumMaxNode = iNumMaxNode;

	m_iNumVertices = (iNumMaxNode-3) * 2 * m_iNumSample;
	m_iVertexStride = sizeof(VTXTRAIL);
	m_iNumIndices = m_iNumVertices;
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iVertexStride;

	m_pVertices = new VTXTRAIL[m_iNumVertices];
	ZeroMemory(m_pVertices, m_VBDesc.ByteWidth);


	m_IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_IBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _ushort[m_iNumIndices];

	for (_uint i = 0; i < m_iNumIndices; i++)
		m_pIndices[i] = i;

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = m_pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = m_pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&m_IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Trail::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint		iOffsets[] = {
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Render()
{
	if (m_iNumCurrentNode <= 3)
		return S_OK;
//	m_pDeviceContext->DrawIndexed((m_iNumCurrentNode * 2), 0, 0);
	m_pDeviceContext->DrawIndexed((m_iNumCurrentNode - 3) * 2 * m_iNumSample, 0, 0);

	return S_OK;
}

void CVIBuffer_Trail::Clear()
{
	m_Trails.clear();
}

_bool CVIBuffer_Trail::IsFinished()
{
	return m_Trails.size() <= 3;
//	return m_Trails.empty();
}

void CVIBuffer_Trail::Update(_float fTimeDelta)
{	
	if (false == m_Trails.empty())
	{
		m_pDeviceContext->Unmap(m_pVB, 0);

		for (auto& Trail : m_Trails)
			Trail.vLifeTime.x += fTimeDelta;

		while (m_Trails.front().vLifeTime.x > m_Trails.front().vLifeTime.y)
		{
			m_Trails.pop_front();

			if (m_Trails.empty())
				break;
		}

		_uint iPrevNode = m_iNumCurrentNode;

		m_iNumCurrentNode = m_Trails.size();

		Update_TrailBuffer(fTimeDelta);
	}
}

void CVIBuffer_Trail::Update_Billboard(_float fTimeDelta)
{
	if (false == m_Trails.empty())
	{
		m_pDeviceContext->Unmap(m_pVB, 0);

		for (auto& Trail : m_Trails)
			Trail.vLifeTime.x += fTimeDelta;

		while (m_Trails.front().vLifeTime.x > m_Trails.front().vLifeTime.y)
		{
			m_Trails.pop_front();

			if (m_Trails.empty())
				break;
		}

		_uint iPrevNode = m_iNumCurrentNode;

		m_iNumCurrentNode = m_Trails.size();

		Update_TrailBuffer_Billboard(fTimeDelta);
	}
}

void CVIBuffer_Trail::Add_TrailBuffer(TRAIL_NODE Trail, _float fTimeDelta)
{
	if(false == m_Trails.empty())
	{
		for (auto& Trail : m_Trails)
			Trail.vLifeTime.x += fTimeDelta;

		while (m_Trails.front().vLifeTime.x > m_Trails.front().vLifeTime.y)
		{
			m_Trails.pop_front();

			if (m_Trails.empty())
				break;
		}
	}

	m_Trails.push_back(Trail);

	if (m_Trails.size() > m_iNumMaxNode)
		m_Trails.pop_front();

	m_iNumCurrentNode = m_Trails.size();

	Update_TrailBuffer(fTimeDelta);
}

void CVIBuffer_Trail::Add_TrailBuffer_Billboard(TRAIL_NODE Trail, _float fTimeDelta)
{
	if (false == m_Trails.empty())
	{
		for (auto& Trail : m_Trails)
			Trail.vLifeTime.x += fTimeDelta;

		while (m_Trails.front().vLifeTime.x > m_Trails.front().vLifeTime.y)
		{
			m_Trails.pop_front();

			if (m_Trails.empty())
				break;
		}
	}

	m_Trails.push_back(Trail);

	if (m_Trails.size() > m_iNumMaxNode)
		m_Trails.pop_front();

	m_iNumCurrentNode = m_Trails.size();

	Update_TrailBuffer_Billboard(fTimeDelta);
}

void CVIBuffer_Trail::Update_TrailBuffer(_float fTimeDelta)
{
	if (m_iNumCurrentNode <= m_iNumSample)
		return;

	VTXTRAIL* pVertices = new VTXTRAIL[(m_iNumCurrentNode - 3) * 2 * m_iNumSample];

	_uint iIndex = {};

	for (_uint i = 0; i < m_iNumCurrentNode - 3; i++)
	{
		for (_uint j = 0; j < m_iNumSample; j++)
		{
			_float fRatio = static_cast<_float>(j) / static_cast<_float>(m_iNumSample);

			_vector vLeftPosition = CatmullRom(m_Trails[i].vTrailLeft, m_Trails[i + 1].vTrailLeft, m_Trails[i + 2].vTrailLeft, m_Trails[i + 3].vTrailLeft, fRatio);
			_vector vRightPosition = CatmullRom(m_Trails[i].vTrailRight, m_Trails[i + 1].vTrailRight, m_Trails[i + 2].vTrailRight, m_Trails[i + 3].vTrailRight, fRatio);

			_float fU = (static_cast<_float>(i) + fRatio) / static_cast<_float>(m_iNumCurrentNode - 3);

			XMStoreFloat3(&pVertices[iIndex].vPosition, vLeftPosition);
			pVertices[iIndex].vTexcoord = _float2(fU, 1.f);
			pVertices[iIndex++].vLifeTime = _float2(m_Trails[i].vLifeTime.x, m_Trails[i].vLifeTime.y);

			XMStoreFloat3(&pVertices[iIndex].vPosition, vRightPosition);
			pVertices[iIndex].vTexcoord = _float2(fU, 0.f);
			pVertices[iIndex++].vLifeTime = _float2(m_Trails[i].vLifeTime.x, m_Trails[i].vLifeTime.y);
		}
	}
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pVertices, sizeof(VTXTRAIL) * ((m_iNumCurrentNode - 3) * 2 * m_iNumSample));

	m_pDeviceContext->Unmap(m_pVB, 0);

	Safe_Delete_Array(pVertices);
}

void CVIBuffer_Trail::Update_TrailBuffer_Billboard(_float fTimeDelta)
{
	if (m_iNumCurrentNode <= 3)
		return;

	VTXTRAIL* pVertices = new VTXTRAIL[(m_iNumCurrentNode - 3) * 2 * m_iNumSample];

	_uint iIndex = {};

	for (_uint i = 0; i < m_iNumCurrentNode - 3; i++)
	{
		for (_uint j = 0; j < m_iNumSample; j++)
		{
			_float fRatio = static_cast<_float>(j) / static_cast<_float>(m_iNumSample);

			_vector vPositions[2] = {};

			Compute_CatmullRom_Billboard(i, fRatio, vPositions);

			_vector vLeftPosition = XMVectorSetW(vPositions[0], 1.f);
			_vector vRightPosition = XMVectorSetW(vPositions[1], 1.f);

			if (XMVectorGetX(vLeftPosition) == 0.f || XMVectorGetX(vRightPosition) == 0.f)
				int a = 10;


			_float fU = (static_cast<_float>(i) + fRatio) / static_cast<_float>(m_iNumCurrentNode - 3);

			XMStoreFloat3(&pVertices[iIndex].vPosition, vLeftPosition);
			pVertices[iIndex].vTexcoord = _float2(fU, 1.f);
			pVertices[iIndex++].vLifeTime = _float2(m_Trails[i].vLifeTime.x, m_Trails[i].vLifeTime.y);

			XMStoreFloat3(&pVertices[iIndex].vPosition, vRightPosition);
			pVertices[iIndex].vTexcoord = _float2(fU, 0.f);
			pVertices[iIndex++].vLifeTime = _float2(m_Trails[i].vLifeTime.x, m_Trails[i].vLifeTime.y);

		}
	}

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pVertices, sizeof(VTXTRAIL) * ((m_iNumCurrentNode - 3) * 2 * m_iNumSample));

	m_pDeviceContext->Unmap(m_pVB, 0);

	Safe_Delete_Array(pVertices);
}

_vector CVIBuffer_Trail::CatmullRom(_float3 vPoint0, _float3 vPoint1, _float3 vPoint2, _float3 vPoint3, _float fRatio)
{
	_vector vP0 = XMLoadFloat3(&vPoint0);
	_vector vP1 = XMLoadFloat3(&vPoint1);
	_vector vP2 = XMLoadFloat3(&vPoint2);
	_vector vP3 = XMLoadFloat3(&vPoint3);

	_float fRatio2 = static_cast<_float>(pow(fRatio, 2.f));
	_float fRatio3 = static_cast<_float>(pow(fRatio, 3.f));

	_vector vResult = 0.5f * ((2.f * vP1) + (vP2 - vP0) * fRatio + (vP0 * 2.f - vP1 * 5.f + vP2 * 4.f - vP3) * fRatio2 + (vP1 * 3.f - vP0 - vP2 * 3.f + vP3) * fRatio3);

	return vResult;
}

void CVIBuffer_Trail::Compute_CatmullRom_Billboard(_uint iNodeIndex, _float fRatio, _vector* pOut)
{	
	_float fNextRatio = fRatio + 0.01f;

	_vector vLeftPosition = CatmullRom(m_Trails[iNodeIndex].vTrailLeft, m_Trails[iNodeIndex + 1].vTrailLeft, m_Trails[iNodeIndex + 2].vTrailLeft, m_Trails[iNodeIndex + 3].vTrailLeft, fRatio);
	_vector vRightPosition = CatmullRom(m_Trails[iNodeIndex].vTrailRight, m_Trails[iNodeIndex + 1].vTrailRight, m_Trails[iNodeIndex + 2].vTrailRight, m_Trails[iNodeIndex + 3].vTrailRight, fRatio);

	_vector vNextLeftPosition = CatmullRom(m_Trails[iNodeIndex].vTrailLeft, m_Trails[iNodeIndex + 1].vTrailLeft, m_Trails[iNodeIndex + 2].vTrailLeft, m_Trails[iNodeIndex + 3].vTrailLeft, fNextRatio);
	_vector vNextRightPosition = CatmullRom(m_Trails[iNodeIndex].vTrailRight, m_Trails[iNodeIndex + 1].vTrailRight, m_Trails[iNodeIndex + 2].vTrailRight, m_Trails[iNodeIndex + 3].vTrailRight, fNextRatio);

	_vector vCurrentCenter = XMVectorScale(XMVectorAdd(vLeftPosition, vRightPosition), 0.5f);
	_vector vNextCenter = XMVectorScale(XMVectorAdd(vNextLeftPosition, vNextRightPosition), 0.5f);

	_vector vLook = XMVector3Normalize(XMVectorSubtract(vNextCenter, vCurrentCenter));

	_vector vCamLook = XMVector3Normalize(XMVectorSubtract(XMLoadFloat4(m_pGameInstance->Get_CamPosition()), vCurrentCenter));

	_vector vRight = XMVector3Normalize(XMVector3Cross(vLook, vCamLook));

	_float fSize = XMVectorGetX(XMVector3Length(XMVectorSubtract(vRightPosition, vLeftPosition)));

	pOut[0] = vCurrentCenter - (vRight * (fSize * 0.5f));
	pOut[1] = vCurrentCenter + (vRight * (fSize * 0.5f));
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumMaxNode)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(iNumMaxNode)))
	{
		MSG_BOX(TEXT("Failed Created CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();


	if (false == m_IsCloned)
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
	}

}
