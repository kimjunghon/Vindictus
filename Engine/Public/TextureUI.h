#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTextureUI abstract : public CUIObject
{
public:
	typedef struct tagTextureUIDesc : public UIOBJECT_DESC
	{
		_uint iTexturePrototypeLevelIndex{};
		_wstring strTexturePrototypeTag;
	}TEXTURE_UI_DESC;

protected:
	CTextureUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTextureUI(const CTextureUI& Prototype);
	virtual ~CTextureUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	class CTexture* m_pTextureCom = { nullptr };

protected:
	HRESULT Ready_TextueCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END