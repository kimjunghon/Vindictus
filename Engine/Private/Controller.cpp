#include "EnginePch.h"
#include "Controller.h"
#include "GameInstance.h"

CController::CController()
    : m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CController::Initialize()
{
    return S_OK;
}

void CController::Update(_float fTimeDelta)
{
}

void CController::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
