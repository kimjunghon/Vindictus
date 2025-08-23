#include "ClientPch.h"
#include "StorageSlot.h"

CStorageSlot::CStorageSlot(ID3D11Device* pDevice, ID3D11Device* pDeviceContext)
    : CSlot { pDevice, pDeviceContext }
{
}

CStorageSlot::CStorageSlot(const CStorageSlot& Prototype)
    : CSlot { Prototype }
{
}

HRESULT CStorageSlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStorageSlot::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    STORAGE_SLOT_DESC* pDesc = static_cast<STORAGE_SLOT_DESC*>(pArg);
    m_iInventoryIndex = pDesc->iInventoryIndex;

    
    
    return S_OK;
}

void CStorageSlot::Priority_Update(_float fTimeDelta)
{
}

void CStorageSlot::Update(_float fTimeDelta)
{
}

void CStorageSlot::Late_Update(_float fTimeDelta)
{
}

HRESULT CStorageSlot::Render()
{
    return E_NOTIMPL;
}

CStorageSlot* CStorageSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    return nullptr;
}

CGameObject* CStorageSlot::Clone(void* pArg)
{
    return nullptr;
}

void CStorageSlot::Free()
{
}
