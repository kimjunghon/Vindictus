#include "EnginePch.h"
#include "Level.h"
#include "GameInstance.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : m_pDevice {pDevice}
    , m_pDeviceContext { pDeviceContext}
    , m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel::Initialize()
{
    return S_OK;
}

void CLevel::Update(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
    return S_OK;
}

void CLevel::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pGameInstance);
}
