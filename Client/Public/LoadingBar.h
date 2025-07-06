#pragma once
#include "ProgressBar.h"

NS_BEGIN(Client)

class CLoadingBar final : public CProgressBar
{
private:
	CLoadingBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLoadingBar(const CLoadingBar& Prototype);
	virtual ~CLoadingBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLoadingBar*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END