#include "ClientPch.h"
#include "Camera_Free.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CCamera { pDevice, pDeviceContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
    : CCamera { Prototype}
    , m_fMouseSensor { Prototype.m_fMouseSensor}
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
    m_fMouseSensor = 0.5f;

    return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{


}

void CCamera_Free::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyPressing(DIK_W))
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
    }
    if (m_pGameInstance->Get_KeyPressing(DIK_S))
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }
    if (m_pGameInstance->Get_KeyPressing(DIK_A))
    {
        m_pTransformCom->Go_Left(fTimeDelta);
    }
    if (m_pGameInstance->Get_KeyPressing(DIK_D))
    {
        m_pTransformCom->Go_Right(fTimeDelta);
    }

    _int    iMouseMove = {};


    if(m_pGameInstance->Get_KeyPressing(DIK_LALT))
    {
        if (iMouseMove = m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X))
        {   
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);
        }

        if (iMouseMove = m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y))
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
        }
    }
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Free"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
    CCamera_Free* pInstance = new CCamera_Free(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Free"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Free::Free()
{
    __super::Free();


}
