#pragma once

#include "Event.h"
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CButtonObject abstract : public CUIObject
{
public:
	typedef struct tagButtonObjectDesc : public CUIObject::UIOBJECT_DESC {
		function<void()> Callback;
	}BUTTONOBJECT_DESC;

protected:
	CButtonObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CButtonObject(const CUIObject& Prototype);
	virtual ~CButtonObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	function<void()> m_Callback;

protected:
	virtual HRESULT Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag) override;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END