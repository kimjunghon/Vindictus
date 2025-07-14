#include "EnginePch.h"
#include "Input_Device.h"

CInput_Device::CInput_Device()
{
}

HRESULT CInput_Device::Initialize(HINSTANCE hInstance, HWND hWnd)
{
    if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_pInputSDK), NULL)))
        return E_FAIL;

    if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
        return E_FAIL;

    m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

    m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE); 

    m_pKeyBoard->Acquire();

    if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
        return E_FAIL;

    m_pMouse->SetDataFormat(&c_dfDIMouse);

    m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    m_pMouse->Acquire();

    return S_OK;
}

void CInput_Device::Update()
{
    memcpy(m_byOldKeyState, m_byKeyState, sizeof(m_byOldKeyState));
    m_pKeyBoard->GetDeviceState(256, m_byKeyState);
    m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

CInput_Device* CInput_Device::Create(HINSTANCE hInstance, HWND hWnd)
{
    CInput_Device* pInstance = new CInput_Device();

    if (FAILED(pInstance->Initialize(hInstance, hWnd)))
    {
        MSG_BOX(TEXT("Failed Created : CInput_Device"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInput_Device::Free()
{
    __super::Free();

    Safe_Release(m_pInputSDK);
    Safe_Release(m_pKeyBoard);
    Safe_Release(m_pMouse);
}
