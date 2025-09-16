#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

using namespace UI;

class CPlayerInstance;

class CMouse final : public CUIObject
{
private:
	typedef struct tagPickEquipItemInfo {
		ITEM_TYPE	eItemType;
		WEAPON_TYPE eWeaponType;
		ARMOR_TYPE	eArmorType;
	}EQUIP_ITEM_INFO;

public:
	typedef struct tagMouseDesc : public UIOBJECT_DESC {
		UI_STATE_DESC StateDesc;
	}UI_MOUSE_DESC;


private:
	CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMouse(const CMouse& Prototype);
	virtual ~CMouse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CPlayerInstance*	m_pPlayerInstance = { nullptr };

	_float				m_fSmallSizeX = {};
	_float				m_fSmallSizeY = {};
	_float				m_fDefaultSizeX = {};
	_float				m_fDefaultSizeY = {};



	_uint				m_iMouseIndex = {};

	Shared_ITEM			m_pItem = {};
	_int				m_iItemIndex = { -1};
	_uint*				m_pUIState = { nullptr };

	_bool				m_IsPickEquipment = {};
	EQUIP_ITEM_INFO		m_EquipItemInfo = {};

	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer*			m_pVIBufferCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

private:
	HRESULT			Ready_Components();
	void			Clear_ItemData();

	_bool			Equip_Item(Shared_ITEM pItem, ITEM_TYPE eItemType, WEAPON_TYPE eWeaponType, ARMOR_TYPE eArmorType, _int iInventoryIndex);
	void			UnEquip_Item(ITEM_TYPE eItemType, WEAPON_TYPE eWeaponType, ARMOR_TYPE eArmorType, _int iInventoryIndex = -1);

	void			Publish_PickEquip(_bool IsPick);
	void			Publish_PickStorage(_bool IsPick);

	void			Event_NonePick(const EVENT_NONE_PICK& Event);
	void			Event_PickEquipment(const EVENT_PICK_EQUIPMENT& Event);
	void			Event_PickStorage(const EVENT_PICK_STORAGE& Event);
	void			Event_EquipStorage(const EVENT_EQUIP_STORAGE& Event);
	void			Event_UnEquipEquipment(const EVENT_UNEQUIP_EQUIPMENT& Event);
	void			Event_ChangeMouse(const EVENT_MOUSE_CHANGE& Event);
public:
	static CMouse*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END