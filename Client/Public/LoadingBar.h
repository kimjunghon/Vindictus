#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

class CLoadingBar final : public CUI_Panel
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

private:
	PROGRESS_TYPE			m_eType = {};

	_float					m_fRatio = {};
	_float					m_fBarRatio = {};
	_float					m_fFillSpeed = {};
	_float					m_fPointX = {};

	class CBar*				m_pBar = { nullptr };
	class CLoadingPoint*	m_pLoadingPoint = { nullptr };

private:
	HRESULT			Ready_Children();
	void			Event_ProgressBar(const EVENT_PROGRESSBAR& Event);
	void			Update_BarRatio(_float fTimeDelta);
	void			Update_PointOffset(_float fTimeDelta);

public:
	static CLoadingBar*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END