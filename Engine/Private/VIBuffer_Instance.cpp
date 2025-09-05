#include "EnginePch.h"
#include "VIBuffer_Instance.h"
#include "Shader.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CVIBuffer { pDevice, pDeviceContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& Prototype)
    : CVIBuffer { Prototype }
    , m_pInstanceVertices{ Prototype.m_pInstanceVertices }
    , m_VBInstanceDesc{ Prototype.m_VBInstanceDesc }
    , m_iNumInstance{ Prototype.m_iNumInstance }
    , m_iNumIndexPerInstance{ Prototype.m_iNumIndexPerInstance }
    , m_iInstanceVertexStride{ Prototype.m_iInstanceVertexStride }
    , m_vSourceColor { Prototype.m_vSourceColor }
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
    return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
    D3D11_SUBRESOURCE_DATA InitialDesc = {};
    InitialDesc.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &InitialDesc, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Resources()
{
    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB,
        m_pVBInstance,
    };

    _uint iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceVertexStride ,
    };

    _uint iOffset[] = {
        0,
        0,
    };

    m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffset);
    m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveType);

    return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
    m_pDeviceContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Shader_Color(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_RawValue(pConstantName, &m_vSourceColor, sizeof(_float3));
}

void CVIBuffer_Instance::Update(_float fTimeDelta, _bool* pIsFinshed)
{
}

void CVIBuffer_Instance::Free()
{
    __super::Free();

    if (false == m_IsCloned)
        Safe_Delete_Array(m_pInstanceVertices);

    Safe_Release(m_pVBInstance);

}
