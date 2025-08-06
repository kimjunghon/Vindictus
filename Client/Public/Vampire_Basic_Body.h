#pragma once
#include "Body.h"

NS_BEGIN(Client)

class CAnimMachine;

class CVampire_Basic_Body final : public CBody
{
private:
	CVampire_Basic_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVampire_Basic_Body(const CVampire_Basic_Body& Prototype);
	virtual ~CVampire_Basic_Body() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CAnimMachine* m_pAnimMachine = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CVampire_Basic_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END