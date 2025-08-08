#pragma once
#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Glasgavelen final : public CLevel
{
private:
	CLevel_Glasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Glasgavelen() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Light();
	HRESULT Ready_Player(const _wstring& strLayerTag);
	HRESULT Ready_GameObject(const _wstring& strLayerTag);

public:
	static CLevel_Glasgavelen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void				Free() override;
};


NS_END