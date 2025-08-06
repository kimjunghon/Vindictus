#pragma once
#include "Client_Defines.h"
#include "Map.h"

NS_BEGIN(Client)

class CTown final : public CMap
{
private:
	CTown(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTown(const CTown& Prototype);
	virtual ~CTown() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_MapObjects();
	HRESULT Ready_Components();

public:
	static CTown* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END