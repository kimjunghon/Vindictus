#pragma once
#include "Body.h"

NS_BEGIN(Client)

class CPlayerBody final : public CBody
{
private:
	CPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayerBody(const CPlayerBody& Prototype);
	virtual ~CPlayerBody() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow() override;

	void				IsHair(_bool IsHair) { m_IsHair = IsHair; }
	
private:
	_bool				m_IsHair = {true};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayerBody*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END