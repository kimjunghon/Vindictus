#pragma once
#include "Client_Defines.h"
#include "UIOBject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)
class CLoadingScreen final : public CUIObject
{
private:
	CLoadingScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLoadingScreen(const CLoadingScreen& Prototype);
	virtual ~CLoadingScreen() = default;

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

public:
	static CLoadingScreen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};


NS_END