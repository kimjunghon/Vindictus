#include "ClientPch.h"

#include "Level_GamePlay.h"
#include "UIObject.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CLevel { pDevice, pDeviceContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    if (FAILED(Ready_Layer(TEXT("Layer"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_A))
	{
		EVENT_PROGRESSBAR Event;
		Event.eType = PROGRESS_TYPE::HP;
		Event.fRatio = 0.2f;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}

	if (m_pGameInstance->Get_KeyPressing(DIK_A))
	{

		EVENT_PROGRESSBAR Event;
		Event.eType = PROGRESS_TYPE::STAMINA;
		Event.fRatio = 0.5f;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}

	if (m_pGameInstance->Get_KeyUp(DIK_A))
	{
		EVENT_PROGRESSBAR Event;
		Event.eType = PROGRESS_TYPE::HP;
		Event.fRatio = 1.f;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

		Event.eType = PROGRESS_TYPE::STAMINA;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}

	if (m_pGameInstance->Get_KeyDown(DIK_ESCAPE))
	{
		EVENT_UI_CHANGE Event;
		Event.byVisibleType = ENUM_CLASS(GAMEPLAY_UI::DEFAULT) + ENUM_CLASS(GAMEPLAY_UI::OPTION);
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::GAMEPLAY), Event);
	}
}

HRESULT CLevel_GamePlay::Render()
{
    SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UI(const _wstring& strLayerTag)
{
	CUIObject::UIOBJECT_DESC UI_Desc{};
	UI_Desc.fX = g_iWinSizeX >> 1;
	UI_Desc.fY = g_iWinSizeY >> 1;
	UI_Desc.fSizeX = g_iWinSizeX;
	UI_Desc.fSizeY = g_iWinSizeY;
	UI_Desc.fOffsetX = 0.f;
	UI_Desc.fOffsetY = 0.f;
	UI_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_HUD"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &UI_Desc)))
		return E_FAIL;

	UI_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_Option"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &UI_Desc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_OptionController"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &UI_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer(const _wstring& strLayerTag)
{



	

    return S_OK;
}


HRESULT CLevel_GamePlay::Ready_GameObject(const _wstring& strLayerTag)
{
	CTransform::TRANSFORM_DESC TransformDesc{};
	TransformDesc.fRotationPerSec = 90.f;
	TransformDesc.fSpeedPerSec = 10.f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
