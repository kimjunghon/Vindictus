#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCamera_Manager final : public CBase
{
private:
	typedef unordered_map<_wstring, class CCamera*> CAMERAS;

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;
	
public:
	void			Update(_float fTimeDelta);
	
public:
	HRESULT			Add_CameraToManager(_uint iPrototypeIndex, const _wstring& strPrototypeTag, const _wstring& strCameraTag, class CCamera** ppOut = nullptr, void* pArg = nullptr);
	HRESULT			Change_Camera(const _wstring& strCameraTag);
	HRESULT			Change_Camera(class CCamera* pNewCamera);
	class CCamera*	Find_Camera(const _wstring& strCameraTag);
	void			Clear();

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	CAMERAS					m_Cameras;
	class CCamera*			m_pCurrentCamera = { nullptr };
	
public:
	static CCamera_Manager* Create();
	virtual void			Free() override;
};

NS_END