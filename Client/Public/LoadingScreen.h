#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer;
NS_END

using namespace UI;

NS_BEGIN(Client)
class CLoadingScreen final : public CUI_Panel
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
	_uint		m_iTextureIndex = {};
	CTexture*	m_pTextureCom = { nullptr };
	CVIBuffer*	m_pVIBufferCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };

private:
	HRESULT Ready_Children();
	HRESULT Ready_Components();
	_uint	ComputeBitIndex(_uint iFlag);
public:
	static CLoadingScreen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};


NS_END