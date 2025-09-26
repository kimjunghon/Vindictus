#include "EnginePch.h"
#include "VIBuffer_Dynamic.h"

CVIBuffer_Dynamic::CVIBuffer_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer { pDevice, pDeviceContext }
{
}

CVIBuffer_Dynamic::CVIBuffer_Dynamic(const CVIBuffer_Dynamic& Prototype)
	: CVIBuffer{ Prototype }
	, m_VBDesc{ Prototype.m_VBDesc }
	, m_pVertices{ Prototype.m_pVertices }
	, m_IBDesc{ Prototype.m_IBDesc }
	, m_pIndices{ Prototype.m_pIndices }
{
}

HRESULT CVIBuffer_Dynamic::Initialize_Prototype()
{
	m_iNumVertices = 4 * 5;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = 6 * 5;
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC VBDesc{};

	m_VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iVertexStride;

	m_pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, m_VBDesc.ByteWidth);

	m_IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_IBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _ushort[m_iNumIndices];
	
	_uint iIndex = {};

	for (_uint i = 0; i < 5; i++)
	{	
		m_pIndices[iIndex++] = (i * 4);
		m_pIndices[iIndex++] = (i * 4) + 1;
		m_pIndices[iIndex++] = (i * 4) + 2;

		m_pIndices[iIndex++] = (i * 4);
		m_pIndices[iIndex++] = (i * 4) + 2;
		m_pIndices[iIndex++] = (i * 4) + 3;
	}

	return S_OK;
}

HRESULT CVIBuffer_Dynamic::Initialize(void* pArg)
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

HRESULT CVIBuffer_Dynamic::Render()
{
	m_pDeviceContext->DrawIndexed(m_iCurrentIndices, 0, 0);

	return S_OK;
}

void CVIBuffer_Dynamic::Update_Buffer(vector<VTXPOSTEX>& Vertices)
{
	_uint iSize = Vertices.size();

	m_iCurrentIndices = (iSize / 4) * 6;

	VTXPOSTEX* pVertices = new VTXPOSTEX[iSize];

	for(_uint i=0; i<iSize; i++)
	{
		pVertices[i].vPosition = Vertices[i].vPosition;
		pVertices[i].vTexcoord= Vertices[i].vTexcoord;
	}

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, pVertices, sizeof(VTXPOSTEX) * iSize);

	m_pDeviceContext->Unmap(m_pVB, 0);

	Safe_Delete_Array(pVertices);
}

CVIBuffer_Dynamic* CVIBuffer_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CVIBuffer_Dynamic* pInstance = new CVIBuffer_Dynamic(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CVIBuffer_Dynamic"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Dynamic::Clone(void* pArg)
{
	CVIBuffer_Dynamic* pInstance = new CVIBuffer_Dynamic(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CVIBuffer_Dynamic"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Dynamic::Free()
{
	__super::Free();

	if (false == m_IsCloned)
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
	}
}
