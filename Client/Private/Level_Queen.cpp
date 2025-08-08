#include "ClientPch.h"
#include "Level_Queen.h"

CLevel_Queen::CLevel_Queen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel{ pDevice, pDeviceContext }
{
}

HRESULT CLevel_Queen::Initialize()
{
	return E_NOTIMPL;
}

void CLevel_Queen::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Queen::Render()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Queen::Ready_Light()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Queen::Ready_Player(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_Queen::Ready_GameObject(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

CLevel_Queen* CLevel_Queen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

void CLevel_Queen::Free()
{
}
