#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iLevelID, class CLevel* pOpenLevel);
	HRESULT Clear_Resources();

public:
	void	Update(_float fTimeDelta);
	HRESULT Render();


private:
	class CLevel*				m_pCurrentLevel = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	_int						m_iCurrentLevelID = {-1};

public:
	static CLevel_Manager*	Create();
	virtual void			Free() override;
};

NS_END