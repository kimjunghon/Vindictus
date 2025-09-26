#pragma once
#include "UI_Panel.h"

NS_BEGIN(Client)

class CBossHPBar;

class CBossHP final : public CUI_Panel
{
private:
	CBossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBossHP(const CBossHP& Prototype);
	virtual ~CBossHP() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float		m_fLerpSpeed = {};

	_bool		m_IsChangeLine = {};
	_uint		m_iNumMaxLine = {};
	_uint		m_iNumLine = {};
	_float		m_fBarRatio = {};
	_float		m_fLineRatio = {};
	_float		m_fLineHP = {};
	
	_float		m_fMaxBossHP = {};
	_float*		m_pCurrentBossHP = { nullptr };
	_wstring	m_strBossName = {};
	_float2		m_vFontOffset = {};

	CBossHPBar*	m_pHpBar = { nullptr };
	
private:
	HRESULT Ready_Children();
	void	Event_BindBossHP(const EVENT_BIND_BOSSHP& Event);

	void	Update_BarData(_float fTimeDelta);

public:
	static CBossHP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END