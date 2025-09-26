#include "ClientPch.h"
#include "Level_Logo.h"
#include "Level_Loading.h"

#include "UIObject.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel { pDevice, pDeviceContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer(TEXT("Layer"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Play_Sound(TEXT("Logo_Bgm"), ENUM_CLASS(SOUND_CHANNEL::BGM), 0.3f, true)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDleta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::TOWN);
		Event.bIsLoading = false;

		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer(const _wstring& strLayerTag)
{

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
