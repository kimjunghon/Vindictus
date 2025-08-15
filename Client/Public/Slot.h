#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Client)

class CWeapon;
class CArmor;
//class Others;

class CSlot final : public CUIObject
{
private:
	typedef struct tagRenderSlot {
		CWeapon* pRenderWeapon = { nullptr };
		CArmor* pRenderArmor = { nullptr };
	}RENDER_SLOT;

private:
	CSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSlot(const CSlot& Prototype);
	virtual ~CSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT			UpdateItem(Shared_ITEM pItem);
	void			IsPick(_bool IsPick) { m_IsPick = IsPick; }
	void			ClearItem();
	void			ClearRenderSlot();

private:
	Shared_ITEM			m_pItem = {nullptr};

	_bool				m_IsPick = { false };
	SLOT_RENDER_DESC	m_SlotRenderDesc = {};
	_float4x4			m_SlotRenderWorldMatrix = {};
	_float4x4			m_MouseRenderWorldMatirx = {};
	RENDER_SLOT			m_RenderSlot = {};

public:
	static CSlot*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END