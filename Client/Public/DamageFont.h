#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer;
class CTexture;
NS_END

NS_BEGIN(Client)

class CDamageFont final : public CUIObject
{
public:
	typedef struct tagDamageDesc
	{
		_uint iDamage;
		_vector vPosition;
	}DAMAGE_DESC;

private:
	CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDamageFont(const CDamageFont& Prototype);
	virtual ~CDamageFont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
	HRESULT			Spawn(void* pArg);

private:
	CVIBuffer*	m_pVIBufferCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CTexture*	m_pTextureCom = { nullptr };

	_uint		m_iDamage = {};
	_float		m_fLifeTime = {};
	_float		m_fCurrentLifeTime = {};

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

public:
	static CDamageFont*		Created(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END