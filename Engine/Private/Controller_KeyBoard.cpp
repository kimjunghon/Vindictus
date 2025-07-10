#include "Controller_KeyBoard.h"
#include "GameInstance.h"

CController_KeyBoard::CController_KeyBoard()
{
}

HRESULT CController_KeyBoard::Initialize()
{
    return S_OK;
}

void CController_KeyBoard::MoveInput()
{
    m_byMoveInputData = 0;
    
    if (m_pGameInstance->Get_KeyDown(DIK_LEFT))
        m_byMoveInputData |= MOVE_LEFT;

    if (m_pGameInstance->Get_KeyDown(DIK_RIGHT))
    {
        if (m_byMoveInputData & MOVE_LEFT)
            m_byMoveInputData ^= MOVE_LEFT;
        else
            m_byMoveInputData |= MOVE_RIGHT;
    }

    if (m_pGameInstance->Get_KeyDown(DIK_UP))
        m_byMoveInputData |= MOVE_FRONT;

    if (m_pGameInstance->Get_KeyDown(DIK_DOWN))
    {
        if (m_byMoveInputData & MOVE_FRONT)
            m_byMoveInputData ^= MOVE_FRONT;
        else
            m_byMoveInputData |= MOVE_BACK;
    }
}

void CController_KeyBoard::ActionInput()
{
    m_iActionInputData = 0;

    if (m_pGameInstance->Get_KeyDown(DIK_A) || m_pGameInstance->Get_KeyPressing(DIK_A))
        m_iActionInputData |= ACTION_GUARD;

    if (m_pGameInstance->Get_KeyDown(DIK_S))
        m_iActionInputData |= ACTION_ATTACK;

    if (m_pGameInstance->Get_KeyDown(DIK_D))
        m_iActionInputData |= ACTION_SMASH;

    if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
        m_iActionInputData |= ACTION_DASH;

    if (m_pGameInstance->Get_KeyDown(DIK_LSHIFT) || m_pGameInstance->Get_KeyPressing(DIK_LSHIFT))
        m_iActionInputData |= ACTION_SPRINT;
}

void CController_KeyBoard::CameraInput()
{
    m_vCameraInputData = { 0.f, 0.f, 0.f };

    if (m_pGameInstance->Get_KeyDown(DIK_Q) || m_pGameInstance->Get_KeyPressing(DIK_Q))
        m_vCameraInputData.x -= m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_E) || m_pGameInstance->Get_KeyPressing(DIK_E))
        m_vCameraInputData.x += m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_R) || m_pGameInstance->Get_KeyPressing(DIK_R))
        m_vCameraInputData.y += m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_F) || m_pGameInstance->Get_KeyPressing(DIK_F))
        m_vCameraInputData.y -= m_fSensor;
}

CController_KeyBoard* CController_KeyBoard::Create()
{
    return new CController_KeyBoard();
}

void CController_KeyBoard::Free()
{
    __super::Free();
}
