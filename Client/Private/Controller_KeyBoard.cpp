#include "Controller.h"
#include "ClientPch.h"
#include "Controller_KeyBoard.h"

CController_KeyBoard::CController_KeyBoard()
{
}

HRESULT CController_KeyBoard::Initialize()
{
    return S_OK;
}

void CController_KeyBoard::Update(_float fTimeDelta)
{
}

HRESULT CController_KeyBoard::MoveInput(INPUT_MOVE_DESC* pOut)
{
    if (nullptr == pOut)
        return E_FAIL;

    _float4 vTempDir = {};
    _bool   bTempMove = false;

    if (m_pGameInstance->Get_KeyDown(DIK_LEFT))
        vTempDir.x -= 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_RIGHT))
        vTempDir.x += 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_UP))
        vTempDir.z += 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_DOWN))
        vTempDir.z -= 1.f;

    if (XMVectorGetX((XMVector4Length(XMLoadFloat4(&vTempDir)))) > 0.f)
    {
        bTempMove = true;
        XMVector4Normalize(XMLoadFloat4(&vTempDir));
    }

    pOut->vDir = XMLoadFloat4(&vTempDir);
    pOut->bMove = bTempMove;

    return S_OK;
}

HRESULT CController_KeyBoard::ActionInput(INPUT_ACTION_DESC* pOut)
{
    if (nullptr == pOut)
        return E_FAIL;

    INPUT_ACTION_DESC ActionDesc = {};

    if (m_pGameInstance->Get_KeyDown(DIK_A) || m_pGameInstance->Get_KeyPressing(DIK_A))
        ActionDesc.bGuard = true;

    if (m_pGameInstance->Get_KeyDown(DIK_S))
        ActionDesc.bAttack = true;

    if (m_pGameInstance->Get_KeyDown(DIK_D))
        ActionDesc.bSmash = true;

    if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
        ActionDesc.bDash = true;

    if (m_pGameInstance->Get_KeyDown(DIK_LSHIFT) || m_pGameInstance->Get_KeyPressing(DIK_LSHIFT))
        ActionDesc.bSprint= true;

    *pOut = ActionDesc;

    return S_OK;
}

HRESULT CController_KeyBoard::CameraInput(INPUT_CAMERA_DESC* pOut)
{
    if (nullptr == pOut)
        return E_FAIL;

    INPUT_CAMERA_DESC CameraDesc = {};

    if (m_pGameInstance->Get_KeyDown(DIK_Q) || m_pGameInstance->Get_KeyPressing(DIK_Q))
        CameraDesc.vCameraRotate.x -= m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_E) || m_pGameInstance->Get_KeyPressing(DIK_E))
        CameraDesc.vCameraRotate.x += m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_R) || m_pGameInstance->Get_KeyPressing(DIK_R))
        CameraDesc.vCameraRotate.y += m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_F) || m_pGameInstance->Get_KeyPressing(DIK_F))
        CameraDesc.vCameraRotate.y -= m_fSensor;

    *pOut = CameraDesc;

    return S_OK;
}

HRESULT CController_KeyBoard::UI_Input(INPUT_UI_DESC* pOut)
{
    return S_OK;
}

CController_KeyBoard* CController_KeyBoard::Create()
{
    return new CController_KeyBoard();
}

void CController_KeyBoard::Free()
{
    __super::Free();
}
