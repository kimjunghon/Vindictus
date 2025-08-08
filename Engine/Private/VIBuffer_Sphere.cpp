#include "EnginePch.h"
#include "VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer { pDevice, pDeviceContext }
{
}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere& Prototype)
	: CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_Sphere::Initialize_Prototype(_uint iSphereX, _uint iSphereY)
{
	m_iSphereX = iSphereX;
	m_iSphereY = iSphereY;


	m_iNumVertices = (m_iSphereX + 1) * (m_iSphereY + 1);
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = m_iSphereX * m_iSphereY * 2 * 3;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC VBDesc{};

	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

	for (_uint i = 0; i <= m_iSphereY; i++)
	{
		_float fPhi = PI * (static_cast<_float>(i) / static_cast<_float>(m_iSphereY));

		_float fY = cosf(fPhi);
		_float fCurrentRadius = sinf(fPhi);

		for (_uint j = 0; j <= m_iSphereX; j++)
		{
			_float fTheta = (2.f * PI) * (static_cast<_float>(j) / static_cast<_float>(m_iSphereX));

			_float fX = fCurrentRadius * cosf(fTheta);
			_float fZ = fCurrentRadius * sinf(fTheta);

			_uint iIndex = (i * (m_iSphereX + 1)) + j;


			pVertices[iIndex].vPosition = _float3(fX, fY, fZ);
			pVertices[iIndex].vTexcoord = _float2(static_cast<_float>(j) / static_cast<_float>(m_iSphereX), static_cast<_float>(i) / static_cast<_float>(m_iSphereY));
		}
	}

	D3D11_SUBRESOURCE_DATA VBInitialData{};
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

	_uint* pIndices = new _uint[m_iNumIndices];

	_uint iNumIndex = {};

	for (_uint i = 0; i < m_iSphereY; i++)
	{
		for (_uint j = 0; j < m_iSphereX; j++)
		{
			_uint		iIndex = i * (m_iSphereX + 1) + j;

			_uint		iIndices[4] = {
				iIndex,
				iIndex + (m_iSphereX + 1),
				iIndex + (m_iSphereX + 1) + 1,
				iIndex + 1
			};

			pIndices[iNumIndex++] = iIndices[0];
			pIndices[iNumIndex++] = iIndices[2];
			pIndices[iNumIndex++] = iIndices[1];

			pIndices[iNumIndex++] = iIndices[0];
			pIndices[iNumIndex++] = iIndices[3];
			pIndices[iNumIndex++] = iIndices[2];
		}
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Sphere* CVIBuffer_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSphereX, _uint iSphereY)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(iSphereX, iSphereY)))
	{
		MSG_BOX(TEXT("Failed Created : CVIBuffer_Sphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Sphere::Clone(void* pArg)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CVIBuffer_Sphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();
}
