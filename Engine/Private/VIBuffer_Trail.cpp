#include "EnginePch.h"
#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer { pDevice, pDeviceContext }
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
	: CVIBuffer { Prototype }
	, m_iNumMaxNode { Prototype.m_iNumMaxNode }
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype(_uint iNumMaxNode)
{
	m_iNumMaxNode = iNumMaxNode;

	m_iNumVertices = iNumMaxNode * 2;
	m_iVertexStride = sizeof(VTXTRAIL);
	m_iNumIndices = m_iNumVertices;
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	D3D11_BUFFER_DESC VBDesc = {};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXTRAIL* pVertices = new VTXTRAIL[m_iNumVertices];
	ZeroMemory(pVertices, VBDesc.ByteWidth);
	
	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	D3D11_BUFFER_DESC		IBDesc{};

	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];

	for (_uint i = 0; i < iNumMaxNode; i++)
	{
		_uint iIndex = i * 2;
		pIndices[iIndex] = iIndex;
		pIndices[iIndex+1] = iIndex+1;
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{

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
	m_pDeviceContext->DrawIndexed((m_iNumCurrentNode * 2), 0, 0);

	return S_OK;
}

void CVIBuffer_Trail::Clear()
{
	m_Trails.clear();
}

_bool CVIBuffer_Trail::IsFinished()
{
	return m_Trails.empty();
}

void CVIBuffer_Trail::Update(_float fTimeDelta)
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

		m_iNumCurrentNode = m_Trails.size();

		D3D11_MAPPED_SUBRESOURCE	SubResource{};

		m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		VTXTRAIL* pVertices = static_cast<VTXTRAIL*>(SubResource.pData);

		for (size_t i = 0; i < m_iNumCurrentNode; i++)
		{
			pVertices[i].vLifeTime.x = m_Trails[i].vLifeTime.x;
		}

		m_pDeviceContext->Unmap(m_pVB, 0);

	}
}

void CVIBuffer_Trail::Update_TrailBuffer(TRAIL_NODE Trail, _float fTimeDelta)
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

	VTXTRAIL* pVertices = new VTXTRAIL[(m_iNumCurrentNode * 2)];

	for (_uint i = 0; i < m_iNumCurrentNode; i++)
	{
		_uint iIndex = i * 2;
		
		_float fU = {};

		if (i == 0)
			fU = 0.f;
		else
			fU = (static_cast<_float>(i) / static_cast<_float>((m_iNumCurrentNode - 1)));

		pVertices[iIndex].vPosition = m_Trails[i].vTrailLeft;
		pVertices[iIndex].vTexcoord = _float2(fU, 1.f);
		pVertices[iIndex].vLifeTime = _float2(m_Trails[i].vLifeTime.x, m_Trails[i].vLifeTime.y);

		pVertices[iIndex+1].vPosition = m_Trails[i].vTrailRight;
		pVertices[iIndex+1].vTexcoord = _float2(fU, 0.f);
		pVertices[iIndex+1].vLifeTime = _float2(m_Trails[i].vLifeTime.x, m_Trails[i].vLifeTime.y);
	}

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pVertices, sizeof(VTXTRAIL) * (m_iNumCurrentNode*2));

	m_pDeviceContext->Unmap(m_pVB, 0);

	Safe_Delete_Array(pVertices);
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
}
