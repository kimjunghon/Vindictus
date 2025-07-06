#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public CTransform::TRANSFORM_DESC {
		_float fX{}; 
		_float fY{}; 
		_float fSizeX{}; 
		_float fSizeY{};
		_uint  iHeight{};

		_float fOffsetX = 0.f;
		_float fOffsetY = 0.f;
	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	vector<CUIObject*> m_Children;

	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};
	_float			m_fX = {};
	_float			m_fY = {};
	_uint			m_iHeight = {};

	_float			m_fSizeX = {};
	_float			m_fSizeY = {};
	_float			m_fOffsetX = {};
	_float			m_fOffsetY = {};
	_float			m_iWinSizeX = {};
	_float			m_iWinSizeY = {};

	_bool			m_bMove = {};

protected:	
	HRESULT			Add_StaticTexture_Child(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, UIOBJECT_DESC& UIChildDesc);
	HRESULT			Add_DynamicTexture_Child(_uint iUIPrototypeLevelIndex, const _wstring& strUIPrototypeTag, _uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag, UIOBJECT_DESC& UIChildDesc);
	virtual HRESULT Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag);

	HRESULT			Begin();
	HRESULT			Update_ChildPosition(_float fX, _float fY);
	void			Children_Priority_Update(_float fTimeDelta);
	void			Children_Update(_float fTimeDelta);
	void			Children_Late_Update(_float fTimeDelta);
	HRESULT			Children_Render();

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END