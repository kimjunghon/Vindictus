#pragma once
#include "Body.h"


NS_BEGIN(Client)

class CGlasgavelenBody final : public CBody
{
private:
	CGlasgavelenBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGlasgavelenBody(const CGlasgavelenBody& Prototype);
	virtual ~CGlasgavelenBody() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Priority_Update(_float fTimeDelta) override;
	virtual void		Update(_float fTimeDelta) override;
	virtual void		Late_Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

public:


private:
	CModel* m_pCurrentModelCom = {};

	CModel* m_pBrokenModelCom = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGlasgavelenBody*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END