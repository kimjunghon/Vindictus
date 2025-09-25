#pragma once

#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
class CLevel;
NS_END

NS_BEGIN(Client)
class CStateFactory;
class CMonsterStateFactory;
class CPlayerInstance;
class CPool_Instance;

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void	Post_Update();
	void	Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pDeviceContext = { nullptr };

	_bool					m_bChange_Level = {false};
	_uint					m_iChange_Level = {};
	_bool					m_bIsLoading = { false };

	CStateFactory*			m_pStateFactory = {nullptr};
	CMonsterStateFactory*	m_pMonsterStateFactory = { nullptr };
	CPlayerInstance*		m_pPlayerInstance = { nullptr };
	CPool_Instance*			m_pPool_Instance = { nullptr };

private:
	void	Event_LevelChange(const EVENT_LEVEL_CHANGE& Event);
	CLevel* Create_NewLevel(_uint iChangeLevel);

	HRESULT	Ready_DefaultColliderChannel();

	HRESULT Ready_Prototype_ForStatic();
	HRESULT Ready_Prototype_ForStatic_Texture();
	HRESULT Ready_Prototype_ForStatic_UI();
	HRESULT Ready_Prototype_ForStatic_Player();
	HRESULT Ready_UI_Container();
	HRESULT Ready_Controller();
	HRESULT Ready_Navigations();
	HRESULT Ready_Sounds(const string& strFolderPath);
	HRESULT Start_Level(LEVEL eStartLevelID);
	

public:
	static CMainApp*	Create();
	virtual void		Free() override;
};

NS_END