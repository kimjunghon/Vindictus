#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

using namespace UI;

class CPalette;

class CDyeingPalette final : public CUI_Panel
{
private:
	CDyeingPalette(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDyeingPalette(const CDyeingPalette& Prototype);
	virtual ~CDyeingPalette() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			ClearPalette();
public:
	void	Set_DyeingItem(pair<ITEM_TYPE, _uint>* pItemType);

private:
	CPalette*		m_pPalette = { nullptr };
	
private:
	HRESULT			Ready_Children();

public:
	static CDyeingPalette*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END