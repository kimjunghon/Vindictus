#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

class CPlayerInstance;

class CStateBar final : public CUI_Panel
{
public:
	typedef struct tagStateBarDesc : public UIOBJECT_DESC {
		PROGRESS_TYPE eType{};
	}STATE_BAR_DESC;

private:
	CStateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CStateBar(const CStateBar& Prototype);
	virtual ~CStateBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CPlayerInstance*	m_pPlayerInstance = { nullptr };
	PLAYER_STATUS*		m_pPlayerStatus = { nullptr };

	PROGRESS_TYPE		m_eType = {};
	_wstring			m_strType = {};
	_float				m_fBackSizeX = {};
	_float				m_fBackSizeY = {};

	_float*				m_pCurrentState = {};
	_float*				m_pMaxState = {};

	_float				m_fRatio = {};
	_float				m_fBarRatio = {};
	_float				m_fLerpBarRatio = {};
	_float				m_fFillSpeed = {};
	class CBar*			m_pBar = { nullptr };
	class CBar*			m_pLerpBar = { nullptr };

private:
	void			Ready_TypeDesc();
	HRESULT			Ready_Children();
	void			Event_ProgressBar(const EVENT_PROGRESSBAR& Event);

	void			Update_PlayerStatus();
	void			Update_BarRatio(_float fTimeDelta);
	void			Update_LerpBarRatio(_float fTimeDelta);

public:
	static CStateBar*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
}; 

NS_END