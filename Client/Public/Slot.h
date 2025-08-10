#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Client)

class CSlot final : public CUIObject
{
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
	void			ClearItem();
private:
	Shared_ITEM		m_pItem = {};


	//TEST
	HRESULT Ready_Components();
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };



public:
	static CSlot*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END