#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameObject_Manager final : public CBase
{
private:
	typedef map<const _wstring, class CLayer*> LAYERS;

private:
	CGameObject_Manager();
	virtual ~CGameObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLayerIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);

public:
	HRESULT Initialize();
	HRESULT Add_GameObject_ToLayer(_uint iLayerIndex, const _wstring& strLayerTag, _uint iPrototypeLevel, const _wstring& strPrototypeTag, void* pArg);

	void	Priority_Update(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Late_Update(_float fTimeDelta);
	void	Clear();

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

	LAYERS*					m_pLayers = { nullptr };

private:
	class CLayer* Find_Layer(_uint iLayerIndex, const _wstring& strLayerTag);

public:
	static CGameObject_Manager* Create();
	virtual void				Free() override;
};

NS_END