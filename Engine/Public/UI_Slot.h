#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)


class ENGINE_DLL CUI_Slot abstract : public CUIObject
{
protected:
	CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Slot(const CUI_Slot& Prototype);
	virtual ~CUI_Slot() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumSlots);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Set_Position(_float fX, _float fY) override;

protected:
	_uint					m_iNumSlots = {};
	vector<CUIObject*>		m_Children;  //CUIOBject ¹è¿­

protected:
	void			Children_Priority_Update(_float fTimeDelta);
	void			Children_Update(_float fTimeDelta);
	void			Children_Late_Update(_float fTimeDelta);
	
	HRESULT			Add_Child(_uint iChildIndex, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, UIOBJECT_DESC* UIChildDesc = nullptr);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END