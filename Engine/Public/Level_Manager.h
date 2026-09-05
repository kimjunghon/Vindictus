#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	void	Set_CreateLevelID(_int iCreateLevelID) { m_iCreateLevelID = iCreateLevelID; }
	_int	Get_CreateLevelID() { m_iCreateLevelID; }

public:
	void	Update(_float fTimeDelta);
	HRESULT Render();

public:
	HRESULT Open_Level(_uint iLevelID, class CLevel* pOpenLevel);
	HRESULT Clear_Resources();


private:
	class CLevel*				m_pCurrentLevel = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	_int						m_iCurrentLevelID = { -1 };
	_int						m_iCreateLevelID = { -1 };

public:
	static CLevel_Manager*	Create();
	virtual void			Free() override;
};

NS_END