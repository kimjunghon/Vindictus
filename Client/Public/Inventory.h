#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

class CStorage;
class CEquipment;

using namespace UI;

class CInventory final : public CUI_Panel
{
private:
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CInventory(const CInventory& Prototype);
	virtual ~CInventory() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CStorage*		m_pStorage = { nullptr };
	CEquipment*		m_pEquipment = { nullptr };

	_float			m_fClickDelay = { };
	_float			m_fCurrentClickDelay = { };

private:
	HRESULT Ready_Children();


public:
	static CInventory*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END