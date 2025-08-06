#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();

	_bool isFinished() {
		return m_isFinished;
	}

	void Show_LoadingText() {
		SetWindowText(g_hWnd, m_szLoadingText);
	}

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pDeviceContext = { nullptr };
	LEVEL							m_eNextLevelID = { LEVEL::END };
	HANDLE							m_hThread = {};
	_tchar							m_szLoadingText[MAX_PATH] = {};
	_bool							m_isFinished = { false };
	CGameInstance*					m_pGameInstance = { nullptr };
	CRITICAL_SECTION				m_CriticalSection = {};
	_float							m_fLoadingRatio = {0.f};
private:
	HRESULT Loading_For_Logo_Level();
	HRESULT Loading_For_Town_Level();
	HRESULT Loading_For_MapModel(LEVEL eLevel, const _char* pMapFilePath);

	HRESULT Loading_For_GamePlay_Level();
	HRESULT Loading_For_GamePlay_ArmorModel();
	
public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevelID);
	virtual void	Free() override;
};

NS_END