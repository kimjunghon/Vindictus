#pragma once
#include "Client_Defines.h"
#include "TextureUI.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CBar final : public CTextureUI
{
private:
	CBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBar(const CBar& Prototype);
	virtual ~CBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Set_Ratio(_float fRatio);

private:
	CVIBuffer*	m_pVIBufferCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };

	_float		m_fRatio = {};
	_float		m_fStartX = {};
private:
	HRESULT		Ready_Components();

public:
	static	CBar*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

NS_END