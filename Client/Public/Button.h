#pragma once

#include "Client_Defines.h"
#include "UIObject.h"


NS_BEGIN(Engine)
class CVIBuffer;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CButton final : public CUIObject
{
public:
	typedef struct tagButtonObjectDesc : public CUIObject::UIOBJECT_DESC {
		function<void()> Callback;
	}BUTTON_DESC;

private:
	CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CButton(const CButton& Prototype);
	virtual ~CButton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer*			m_pVIBufferCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	function<void()>	m_Callback;

private:
	virtual HRESULT	Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag) override;
	HRESULT			Ready_Components();
	
public:
	static CButton*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END