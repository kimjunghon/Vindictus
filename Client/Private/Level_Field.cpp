#include "ClientPch.h"
#include "Level_Field.h"

CLevel_Field::CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel { pDevice, pDeviceContext }
{
}

HRESULT CLevel_Field::Initialize()
{
	return E_NOTIMPL;
}

void CLevel_Field::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Field::Render()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Field::Ready_Light()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Field::Ready_Player(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_Field::Ready_GameObject(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

CLevel_Field* CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

void CLevel_Field::Free()
{
}
