#include "ClientPch.h"
#include "Controller_Mouse.h"

CController_Mouse::CController_Mouse()
{
}

HRESULT CController_Mouse::Initialize()
{
    m_fSensor = 0.5f;

	return S_OK;
}

void CController_Mouse::Update(_float fTimeDelta)
{
}

HRESULT CController_Mouse::MoveInput(INPUT_MOVE_DESC* pOut)
{
    _float4 vMoveDir = {};
    _bool   bMove = false;
    _bool   bSprint = false;

    if (m_pGameInstance->Get_KeyDown(DIK_A) || m_pGameInstance->Get_KeyPressing(DIK_A))
        vMoveDir.x -= 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_D) || m_pGameInstance->Get_KeyPressing(DIK_D))
        vMoveDir.x += 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_W) || m_pGameInstance->Get_KeyPressing(DIK_W))
        vMoveDir.z += 1.f;

    if (m_pGameInstance->Get_KeyDown(DIK_S) || m_pGameInstance->Get_KeyPressing(DIK_S))
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
}

HRESULT CController_Mouse::ActionInput(INPUT_ACTION_DESC* pOut)
{
    if (nullptr == pOut)
        return E_FAIL;

    INPUT_ACTION_DESC ActionDesc = {};

    if (m_pGameInstance->Get_KeyDown(DIK_F))
    {
        ActionDesc.byAction |= ENUM_CLASS(ACTION_INPUT::GUARD);
        if (m_pGameInstance->Get_KeyDown(DIK_W) || m_pGameInstance->Get_KeyPressing(DIK_W))
            ActionDesc.byAction |= ENUM_CLASS(ACTION_INPUT::HEAVYSTAND);
    }

    if (m_pGameInstance->Get_MouseState(MOUSEKEYSTATE::LB))
        ActionDesc.byAction |= ENUM_CLASS(ACTION_INPUT::ATTACK);

    if (m_pGameInstance->Get_MouseState(MOUSEKEYSTATE::RB))
        ActionDesc.byAction |= ENUM_CLASS(ACTION_INPUT::SMASH);

    if (m_pGameInstance->Get_KeyDown(DIK_SPACE))
        ActionDesc.byAction |= ENUM_CLASS(ACTION_INPUT::ROLL);

    if (ActionDesc.byAction > 0)
        ActionDesc.bAction = true;

    *pOut = ActionDesc;

    return S_OK;
}

HRESULT CController_Mouse::CameraInput(INPUT_CAMERA_DESC* pOut)
{
    if (nullptr == pOut)
        return E_FAIL;

    INPUT_CAMERA_DESC CameraDesc = {};
    CameraDesc.bRotate = false;

    CameraDesc.vCameraRotate.x += m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X) * m_fSensor;
    CameraDesc.vCameraRotate.y += m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y) * m_fSensor;

    if (XMVectorGetX(XMVector2Length(XMLoadFloat2(&CameraDesc.vCameraRotate))) > 0.f)
        CameraDesc.bRotate = true;

    CameraDesc.fDistance = static_cast<_float>(m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::WHEEL));

    *pOut = CameraDesc;

    return S_OK;
}

CController_Mouse* CController_Mouse::Create()
{
    CController_Mouse* pInstance = new CController_Mouse();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CController_Mouse"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CController_Mouse::Free()
{
    __super::Free();
}
