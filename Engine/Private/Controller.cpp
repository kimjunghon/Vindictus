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

HRESULT CController::MoveInput(INPUT_MOVE_DESC* pOut)
{
    return S_OK;
}

HRESULT CController::ActionInput(INPUT_ACTION_DESC* pOut)
{
    return S_OK;
}

HRESULT CController::CameraInput(INPUT_CAMERA_DESC* pOut)
{
    return S_OK;
}

HRESULT CController::UI_Input(INPUT_UI_DESC* pOut)
{
    return S_OK;
}

void CController::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
