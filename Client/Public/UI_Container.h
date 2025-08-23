#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CUIObject;
NS_END

NS_BEGIN(Client)

using namespace UI;

class CUI_Container final : public CGameObject
{
private:
	CUI_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Container(const CUI_Container& Prototype);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<CUIObject*>	m_UIObjects[ENUM_CLASS(UI_LEVEL::END)];
	_uint				m_iArrayState = {};
	_uint				m_iUIState = {};
	INPUT_UI_DESC		m_UI_Input = {};
	
private:
	void			Bind_InputData();
	HRESULT			Add_UIObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iUI_LevelIndex, void* pArg = nullptr);

	HRESULT			Ready_Mouse_UI();
	HRESULT			Ready_Loading_UI();
	HRESULT			Ready_Logo_UI();
	HRESULT			Ready_GamePlay_UI();



	void			Event_LevelChange(const EVENT_UI_LEVEL_CHANGE& Event);


public:
	static CUI_Container*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

NS_END