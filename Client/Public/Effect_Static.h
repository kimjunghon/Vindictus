#pragma once
#include "Effect.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect_Instance;
NS_END

NS_BEGIN(Client)

class CEffect_Static final : public CEffect
{
private:
	CEffect_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Static(const CEffect_Static& Prototype);
	virtual ~CEffect_Static() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT			Spawn(void* pArg) override;
	virtual void			ReturnToPool() override;

private:
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect_Instance*	m_pVIBufferCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	_bool						m_IsFinished = {};

private:
	HRESULT	Ready_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CEffect_Static*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

NS_END