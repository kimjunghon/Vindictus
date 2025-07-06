#include "Level_GamePlay.h"

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
}

HRESULT CLevel_GamePlay::Render()
{
    SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer(const _wstring& strLayerTag)
{
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
