#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CProgressBar final : public CUIObject
{
private:
	CProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CProgressBar(const CProgressBar& Prototype);
	virtual ~CProgressBar() = default;

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

	_float			m_fStartX = {};
	_float			m_fRatio = {};
	_float			m_fCurrentRatio = {};
	_float			m_fFillSpeed = {};
private:
	virtual HRESULT	Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag) override;
	HRESULT			Ready_Components();
	void			Event_ProgressBar(const EVENT_PROGRESSBAR& Event);


public:
	static CProgressBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END