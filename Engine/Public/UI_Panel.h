#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUI_Panel abstract : public CUIObject
{
protected:
	CUI_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Panel(const CUI_Panel& Prototype);
	virtual ~CUI_Panel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Set_Position(_float fX, _float fY) override;
	virtual void	Set_Offset(_float fOffsetX, _float fOffsetY) override;

protected:
	vector<CUIObject*> m_Children;

protected:
	HRESULT			Add_Child(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* UIChildDesc);
	
	void			Children_Priority_Update(_float fTimeDelta);
	void			Children_Update(_float fTimeDelta);
	void			Children_Late_Update(_float fTimeDelta);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END