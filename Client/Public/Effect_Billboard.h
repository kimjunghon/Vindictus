#pragma once
#include "Effect.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Point_Instance;
class CShader;
NS_END

NS_BEGIN(Client)

class CEffect_Billboard : public CEffect
{
private:
	CEffect_Billboard(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Billboard(const CEffect_Billboard& Prototype);
	virtual ~CEffect_Billboard() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT			Spawn(void* pArg) override;

private:
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	_bool						m_IsFinished = {};

private:
	HRESULT	Ready_Component();
	HRESULT	Bind_ShaderResources();

public:
	static CEffect_Billboard*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END