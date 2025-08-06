#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public CTransform::TRANSFORM_DESC 
	{
		_float fX{}; 
		_float fY{}; 
		_float fSizeX{}; 
		_float fSizeY{};
		_uint  iDepth{};
		_float fOffsetX = 0.f;
		_float fOffsetY = 0.f;
	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	_uint Get_Depth() { return m_iDepth; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Set_Position(_float fX, _float fY);
	virtual void	Set_Offset(_float fOffsetX, _float fOffsetY);

protected:

	_float4x4		m_ViewMatrix = {};
	_float4x4		m_ProjMatrix = {};
	_float			m_fX = {};
	_float			m_fY = {};
	_uint			m_iDepth = {};

	_float			m_fSizeX = {};
	_float			m_fSizeY = {};
	_float			m_fOffsetX = {};
	_float			m_fOffsetY = {};
	_float			m_iWinSizeX = {};
	_float			m_iWinSizeY = {};

protected:	
	HRESULT			Begin();

	virtual _bool	IsPick(HWND hWnd);
	

	
public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END