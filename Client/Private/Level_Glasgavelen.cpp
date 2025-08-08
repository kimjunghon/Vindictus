#include "ClientPch.h"
#include "Level_Glasgavelen.h"

CLevel_Glasgavelen::CLevel_Glasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel{ pDevice, pDeviceContext }
{
}

HRESULT CLevel_Glasgavelen::Initialize()
{
	return E_NOTIMPL;
}

void CLevel_Glasgavelen::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Glasgavelen::Render()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Glasgavelen::Ready_Light()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Glasgavelen::Ready_Player(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_Glasgavelen::Ready_GameObject(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

CLevel_Glasgavelen* CLevel_Glasgavelen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

void CLevel_Glasgavelen::Free()
{
}
