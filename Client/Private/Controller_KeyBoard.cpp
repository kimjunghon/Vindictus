#include "Controller.h"
#include "ClientPch.h"
#include "Controller_KeyBoard.h"

CController_KeyBoard::CController_KeyBoard()
{
}

HRESULT CController_KeyBoard::Initialize()
{
    m_fSensor = 1.f;

    return S_OK;
}

void CController_KeyBoard::Update(_float fTimeDelta)
{
}

HRESULT CController_KeyBoard::MoveInput(INPUT_MOVE_DESC* pOut)
{
    if (nullptr == pOut)
        return E_FAIL;

    _float4 vMoveDir = {};
    _bool   bMove = false;
    _bool   bSprint = false;

    if (m_pGameInstance->Get_KeyDown(DIK_LEFT) || m_pGameInstance->Get_KeyPressing(DIK_LEFT))
        vMoveDir.x -= 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_RIGHT) || m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
        vMoveDir.x += 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_UP)|| m_pGameInstance->Get_KeyPressing(DIK_UP))
        vMoveDir.z += 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_DOWN) || m_pGameInstance->Get_KeyPressing(DIK_DOWN))
        vMoveDir.z -= 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_LSHIFT) || m_pGameInstance->Get_KeyPressing(DIK_LSHIFT))
        bSprint = true;

    if (XMVectorGetX((XMVector4Length(XMLoadFloat4(&vMoveDir)))) > 0.f)
    {
        bMove = true;
        XMVector4Normalize(XMLoadFloat4(&vMoveDir));
    }

    pOut->vDir = XMLoadFloat4(&vMoveDir);
    pOut->bMove = bMove;
    pOut->bSprint = bSprint;

    return S_OK;
}

HRESULT CController_KeyBoard::ActionInput(INPUT_ACTION_DESC* pOut)
{
    if (nullptr == pOut)
        return E_FAIL;

    INPUT_ACTION_DESC ActionDesc = {};

    if (m_pGameInstance->Get_KeyDown(DIK_A) || m_pGameInstance->Get_KeyPressing(DIK_A))
    {
        ActionDesc.byAction |= ENUM_CLASS(ACTION_INPUT::GUARD);
        if (m_pGameInstance->Get_KeyDown(DIK_UP) || m_pGameInstance->Get_KeyPressing(DIK_UP))
            ActionDesc.byAction |= ENUM_CLASS(ACTION_INPUT::HEAVYSTAND);
    }

    if (m_pGameInstance->Get_KeyDown(DIK_S))
        ActionDesc.byAction |= ENUM_CLASS(ACTION_INPUT::ATTACK);

    if (m_pGameInstance->Get_KeyDown(DIK_D) || m_pGameInstance->Get_KeyPressing(DIK_D))
        ActionDesc.byAction = ENUM_CLASS(ACTION_INPUT::SMASH);

    if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
        ActionDesc.byAction = ENUM_CLASS(ACTION_INPUT::ROLL);

    if (ActionDesc.byAction > 0)
        ActionDesc.bAction = true;

    *pOut = ActionDesc;

    return S_OK;
}

HRESULT CController_KeyBoard::CameraInput(INPUT_CAMERA_DESC* pOut)
{
    if (nullptr == pOut)
        return E_FAIL;

    INPUT_CAMERA_DESC CameraDesc = {};
    CameraDesc.bRotate = false;

    if (m_pGameInstance->Get_KeyDown(DIK_Q) || m_pGameInstance->Get_KeyPressing(DIK_Q))
        CameraDesc.vCameraRotate.x += m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_E) || m_pGameInstance->Get_KeyPressing(DIK_E))
        CameraDesc.vCameraRotate.x -= m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_R) || m_pGameInstance->Get_KeyPressing(DIK_R))
        CameraDesc.vCameraRotate.y += m_fSensor;

    if (m_pGameInstance->Get_KeyDown(DIK_F) || m_pGameInstance->Get_KeyPressing(DIK_F))
        CameraDesc.vCameraRotate.y -= m_fSensor;

    if (XMVectorGetX(XMVector2Length(XMLoadFloat2(&CameraDesc.vCameraRotate))) > 0.f)
        CameraDesc.bRotate = true;

    CameraDesc.fDistance = static_cast<_float>(m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::WHEEL));


    *pOut = CameraDesc;

    return S_OK;
}

HRESULT CController_KeyBoard::UI_Input(INPUT_UI_DESC* pOut)
{
    return S_OK;
}

CController_KeyBoard* CController_KeyBoard::Create()
{
    CController_KeyBoard* pInstance = new CController_KeyBoard();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CController_KeyBoard"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CController_KeyBoard::Free()
{
    __super::Free();
}
