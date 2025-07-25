#include "EnginePch.h"
#include "PawnObject.h"

CPawnObject::CPawnObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject { pDevice, pDeviceContext}
{
}

CPawnObject::CPawnObject(const CPawnObject& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CPawnObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPawnObject::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    PAWNOBJECT_DESC* pDesc = static_cast<PAWNOBJECT_DESC*>(pArg);
    m_pPawnMatrix = pDesc->pPawnMatrix;

    return S_OK;
}

void CPawnObject::Priority_Update(_float fTimeDelta)
{
}

void CPawnObject::Update(_float fTimeDelta)
{
}

void CPawnObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CPawnObject::Render()
{
    return S_OK;
}

void CPawnObject::Bind_PawnData(void* pData)
{
}

void CPawnObject::Free()
{
    __super::Free();
}
