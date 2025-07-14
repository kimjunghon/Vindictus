#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)
class CLoadingScreen final : public CUI_Panel
{
public:
	typedef struct tagLoadingDesc : public UIOBJECT_DESC {
		_uint iLoadingLevelID;
	}LOADING_DESC;

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
	_uint		m_iLoadingLevelID = {};

private:
	HRESULT Ready_Children();

public:
	static CLoadingScreen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};


NS_END