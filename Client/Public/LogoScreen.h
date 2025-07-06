#pragma once
#include "Client_Defines.h"
#include "UIObject.h"
NS_BEGIN(Engine)
class CVIBuffer;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CLogoScreen final : public CUIObject
{
private:
	CLogoScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLogoScreen(const CLogoScreen& Prototype);
	virtual ~CLogoScreen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Component();
	HRESULT Ready_Children();
	HRESULT Ready_Button();

public:
	static CLogoScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END