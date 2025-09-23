#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

using namespace UI;

class CWorldMap : public CUI_Panel
{
private:
	CWorldMap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWorldMap(const CWorldMap& Prototype);
	virtual ~CWorldMap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT			Ready_Children();

public:
	static CWorldMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END