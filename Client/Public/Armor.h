#pragma once
#include "Client_Defines.h"
#include "PawnObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CBone;
NS_END

NS_BEGIN(Client)

class CArmor final : public CPawnObject
{
public:
	typedef struct tagArmorInfo {
		_wstring	strArmorName;
		_float		fDefense;
		_float		fBrokenDefense;
		_float		fHealth;
	}ARMOR_INFO;

	typedef struct tagArmorDesc : public PAWNOBJECT_DESC {
		_uint				iArmorModelPrototypeLevelIndex;
		_wstring			strArmorModelPrototypeTag;
		ARMOR_TYPE			eArmorType;
		ARMOR_INFO			ArmorInfo;
	}ARMOR_DESC;
	
private:
	CArmor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CArmor(const CArmor& Prototype);
	virtual ~CArmor() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	HRESULT					RenderSlot(SLOT_RENDER_DESC SlotRenderDesc);

	const _wstring&			Get_ArmorName() { return m_ArmorInfo.strArmorName; }
	ARMOR_TYPE				Get_ArmorType() { return m_eArmorType; }
	HRESULT					Equip(const _float4x4* pPawnMatrix, CModel* pParentModelCom);
	HRESULT					UnEquip();

	_bool					IsBroekn() { return m_ArmorInfo.fHealth <= 0.f; }
	void					DecreaseDurability(_float fDecreaseAmount);
private:
	_bool					m_IsEquip = { false };
	ARMOR_TYPE				m_eArmorType = { ARMOR_TYPE::END };
	ARMOR_STATE				m_eArmorState = { ARMOR_STATE::DEFAULT };
	ARMOR_INFO				m_ArmorInfo = {};

	CModel*					m_pModelCom[ENUM_CLASS(ARMOR_STATE::END)] = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

	vector<CBone*>			m_ParentBones[ENUM_CLASS(ARMOR_STATE::END)];

private:
	HRESULT					Ready_Components(_uint iArmorNodelPrototypeLevelIndex, const _wstring& strArmorModelPrototypeTag);
	HRESULT					Bind_ShaderResources();
	HRESULT					Bind_ShaderResources_RenderSlot(SLOT_RENDER_DESC SlotRenderDesc);
	HRESULT					Bind_ParentBones(CModel* pParentModelCom);

	_matrix					Compute_OffsetMatrix();

public:
	static CArmor*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END