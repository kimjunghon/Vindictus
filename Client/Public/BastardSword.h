#pragma once
#include "Client_Defines.h"
#include "SocketObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CBastardSword final : public CSocketObject
{
private:
	CBastardSword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBastardSword(const CBastardSword& Prototype);
	virtual ~CBastardSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBastardSword*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END