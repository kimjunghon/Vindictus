#pragma once
#include "ProgressBar.h"

NS_BEGIN(Client)

class CStateBar abstract : public CProgressBar
{
public:
	typedef struct tagProgressDesc : public UIOBJECT_DESC {
		PROGRESS_TYPE eType{};
	}PROGRESS_DESC;

protected:
	CStateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CStateBar(const CStateBar& Prototype);
	virtual ~CStateBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void	Update(_float fTimeDelta) override;

protected:
	virtual HRESULT	Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag) override;
	virtual HRESULT	Ready_Components() override;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
}; 

NS_END