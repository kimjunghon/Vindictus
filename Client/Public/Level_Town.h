#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Town final : public CLevel
{
private:
	CLevel_Town(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Town() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Light();
	HRESULT Ready_SpawnData();

	HRESULT Ready_Player(const _wstring& strLayerTag);
	HRESULT Ready_GameObject(const _wstring& strLayerTag);

public:
	static CLevel_Town*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void			Free() override;
};

NS_END