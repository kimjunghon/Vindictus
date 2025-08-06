#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Queen final : public CLevel
{
private:
	CLevel_Queen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Queen() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Light();
	HRESULT Ready_Player(const _wstring& strLayerTag);
	HRESULT Ready_GameObject(const _wstring& strLayerTag);

public:
	static CLevel_Queen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void			Free() override;
};

NS_END