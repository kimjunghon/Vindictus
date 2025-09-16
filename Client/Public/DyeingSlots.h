#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

class CSlot;
class CPlayerInstance;

class CDyeingSlots final  : public CUI_Panel
{
private:
	enum EQUIPMENT {
		EAR, HEAD, NECK,
		SWORD, UPPER, SHILED,
		TOOL, LOWER, HAND,
		BAND, FOOT, NONE, END
	};

private:
	CDyeingSlots(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDyeingSlots(const CDyeingSlots& Prototype);
	virtual ~CDyeingSlots() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			Update_Slots();
	void			Clear_Slots();

	pair<ITEM_TYPE, _uint>* IsPickSlot(HWND hWnd);

private:
	_bool								m_IsOpen = {};

	CPlayerInstance*					m_pPlayerInstance = { nullptr };
	RECT								m_SlotRect = {};
	CSlot*								m_Slots[EQUIPMENT::END] = { nullptr };
	_uint								m_iPickIndex = {};
	_uint								m_iArmorFlag = {};
	_uint								m_iWeaponFlag = {};
	pair<ITEM_TYPE, _uint>				m_SlotItemType = {};

	_uint								m_iNumSlotsX = {};
	_uint								m_iNumSlotsY = {};
	_float								m_fSlotStartOffsetX = {};
	_float								m_fSlotStartOffsetY = {};
	_float								m_fSlotOffsetX = {};
	_float								m_fSlotOffsetY = {};

	unordered_map<_uint, WEAPON_TYPE>	m_WeaponSlotIndex;
	unordered_map<_uint, ARMOR_TYPE>	m_ArmorSlotIndex;

private:
	
private:
	HRESULT			Ready_Children();
	HRESULT			Ready_Slots();
	HRESULT			Ready_SlotIndex();


public:
	static CDyeingSlots*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END