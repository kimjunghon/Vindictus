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

void CController::Update_Controller()
{
    MoveInput();
    ActionInput();
    CameraInput();
}

void CController::Clear_Data()
{
    m_byMoveInputData = 0;
    m_iActionInputData = 0;
    m_vCameraInputData = { 0.f, 0.f, 0.f };
}


void CController::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
