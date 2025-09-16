#pragma once
#include "Client_Defines.h"
#include "PawnObject.h"

NS_BEGIN(Client)

class CWeapon : public CPawnObject
{
public:
	typedef struct tagWeaponInfo {
		_wstring	strWeaponName;
		_float		fAttackPower;
		_float		fDefense;
	}WEAPON_INFO;

	typedef struct tagWeaponDesc : public PAWNOBJECT_DESC {
		_uint				iWeaponModelPrototypeLevelIndex;
		_wstring			strWeaponModelPrototypeTag;
		WEAPON_TYPE			eWeaponType;
		WEAPON_INFO			WeaponInfo;
		_vector				vOffsetPosition;
		_vector				vRotationQuaternion;
		DYEING_MATERIAL		Materials[ENUM_CLASS(DYEING_PART::END)];
	}WEAPON_DESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	HRESULT					RenderSlot(SLOT_RENDER_DESC SlotRenderDesc);

	WEAPON_INFO				Get_WeaponInfo() { return m_WeaponInfo; }
	const _wstring&			Get_WeaponName() { return m_WeaponInfo.strWeaponName; }
	WEAPON_TYPE				Get_WeaponType() { return m_eWeaponType; }
	virtual HRESULT			Equip(const _float4x4* pParentMatrix, const _float4x4* pSocketMatrix, void* pDesc = nullptr);
	virtual HRESULT			UnEquip();

	DYEING_DATAS*			Get_DyeingDatas() { return &m_DyeingData; }
	void					Dyeing(_uint iMaterialIndex, _float3 vColor);
protected:
	const _float4x4*		m_pSocketMatrix = { nullptr };
	_float4x4				m_CombinedMatrix = { };

	_bool					m_IsEquip = { false };
	WEAPON_TYPE				m_eWeaponType = { WEAPON_TYPE::END };
	WEAPON_INFO				m_WeaponInfo = {};

	DYEING_DATAS			m_DyeingData = {};

	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

protected:
	HRESULT					Ready_Components(_uint iWeaponModelPrototypeLevelIndex, const _wstring& strWeaponModelPrototypeTag);
	HRESULT					Bind_ShaderResources();
	HRESULT					Bind_ShaderResources_RenderSlot(SLOT_RENDER_DESC SlotRenderDesc);
	_float3					RandomColor(DYEING_MATERIAL eMaterial);
public:
	static CWeapon*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END