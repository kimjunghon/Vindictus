#pragma once

#include "Prototype_Manager.h"
#include "EventBus.h"
#include "Octree.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;


#pragma region ENGINE
public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext);
	void	Update_Engine(_float fTimeDelta);
	HRESULT Clear_Resources(_uint iClearLevelID);

public:
	void	Render_Begin(const _float4* pClearColor);
	HRESULT Draw();
	void	Render_End(HWND hWnd = 0);

public:
	_float Rand_Normal();
	_float Rand(_float fMin, _float fMax);
#pragma endregion



#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
	HRESULT Open_Level(_uint iLevelID, class CLevel* pOpenLevel);
	HRESULT Clear_Resources();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*	Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion
	
#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject_ToTree(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, OBJECTGROUP eObjectgroup, void* pArg = nullptr);
#pragma endregion

#pragma region OCTREE
	HRESULT Create_Octree(COctree::OCTREE_DESC& tOctreeDesc);
	HRESULT Add_Objects(class CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox);
#pragma endregion

#pragma region DYNAMICAABB_TREE
	HRESULT Add_Node(class CGameObject* pGameObject, const BoundingBox& ObjectBoundingBox);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject);
#pragma endregion

#pragma region EVENTBUS
	template <typename EVENT>
	void Subscribe(_uint iEventLevelIndex, function<void(const EVENT&)> Subscriber) {
		m_pEventBus->Subscribe(iEventLevelIndex, Subscriber); }

	void Publish(_uint iEventLevelIndex, const CEvent& Event);
#pragma endregion

private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CGameObject_Manager* m_pObject_Manager = { nullptr };
	class COctree*				m_pOctree = { nullptr };
	class CDynamicAABBTree*		m_pDynamicAABBTree = { nullptr };
	CEventBus*					m_pEventBus = { nullptr };
public:
	void Release_Engine();
	virtual void Free() override;

};

NS_END