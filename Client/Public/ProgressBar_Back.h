#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer;
class CTexture;
NS_END

NS_BEGIN(Client)

class CProgressBar_Back final : public CUIObject
{
private:
	CProgressBar_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CProgressBar_Back(const CProgressBar_Back& Prototype);
	virtual ~CProgressBar_Back() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer*		m_pVIBufferCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };

private:
	virtual HRESULT	Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag) override;
	HRESULT			Ready_Components();

public:
	static CProgressBar_Back*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END