#pragma once
#include "Bar.h"

NS_BEGIN(Client)

class CBossHPBar final : public CBar
{
private:
	CBossHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBossHPBar(const CBossHPBar& Prototype);
	virtual ~CBossHPBar() = default;

public:
	void			Change_TextureIndex(_uint iTextureIndex) { m_iTextureIndex = iTextureIndex; }
	void			Change_PassIndex(_uint iPassIndex) { m_iPassIndex = iPassIndex; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint		m_iPassIndex = {};
	_uint		m_iTextureIndex = {};

private:
	HRESULT		Bind_ShaderResources();

public:
	static CBossHPBar*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END