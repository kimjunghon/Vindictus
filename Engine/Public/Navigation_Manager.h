#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CNavigation;

class CNavigation_Manager final : public CBase
{
private:
	typedef unordered_map<_uint, CNavigation*> NAVIGATIONS;

private:
	CNavigation_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CNavigation_Manager() = default;

public:
	HRESULT			Add_Navigation(_uint iNavigationLevel, const _tchar* pNavigationFilePath);
	HRESULT			Change_Navigation(_uint iNavigationLevel);
	CNavigation*	Clone_CurrentNavigation(_int iCellIndex);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pDeviceContext = { nullptr };

	NAVIGATIONS				m_Navigations;
	CNavigation*			m_pCurrentNavigation = { nullptr };

private:
	CNavigation*			Find_Navigation(_uint iNavigationLevel);

public:
	static CNavigation_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void				Free() override;
};


NS_END