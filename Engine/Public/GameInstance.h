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

#pragma region INPUT_DEVICE
public:
	_bool	Get_KeyDown(_ubyte byKeyID);
	_bool	Get_KeyUp(_ubyte byKeyID);
	_bool	Get_KeyPressing(_ubyte byKeyID);
	_byte	Get_MouseState(MOUSEKEYSTATE eMouse);
	_long	Get_MouseMove(MOUSEMOVESTATE eState);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);
#pragma endregion

#pragma region RENDER_STATE
public:
	HRESULT RSSetState(_uint iRSIndex);
	HRESULT RSSetState(const _wstring& strRSTag);

	HRESULT BSSetState(_uint iBSIndex);
	HRESULT BSSetState(const _wstring& strBSTag);

	HRESULT DSSSetState(_uint iDSSIndex);

	HRESULT Add_CustomRS(const _wstring& strRSTag, const D3D11_RASTERIZER_DESC& RS_DESC);
	HRESULT Add_CustomBS(const _wstring& strBSTag, const D3D11_BLEND_DESC& BS_DESC);
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Open_Level(_uint iLevelID, class CLevel* pOpenLevel);
	HRESULT Clear_Resources();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*	Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion
	
#pragma region OBJECT_MANAGER
public:
	HRESULT Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject_ToLayer(_uint iLayerIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
	class CComponent* Get_Component(_uint iLayerIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
#pragma endregion

#pragma region OCTREE
public:
	HRESULT Create_Octree(COctree::OCTREE_DESC& tOctreeDesc);
	HRESULT Add_Objects(class CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox);
#pragma endregion

#pragma region DYNAMICAABB_TREE
public:
	HRESULT Add_Node(class CGameObject* pGameObject, const BoundingBox& ObjectBoundingBox);
#pragma endregion

#pragma region RT_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strRTTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRTTag);
	HRESULT Bind_Shader_RenderTarget(const _wstring& strRTTag, CShader* pShader, const _char* pConstantName);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool IsClear = true);
	HRESULT End_MRT();
#ifdef _DEBUG
	HRESULT Ready_Debug(const _wstring& strRTTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RT_Debug(CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region RENDERER
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject);
#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC*	Get_LightDesc(const _wstring& strLightTag);
	HRESULT				Add_Light(const _wstring& strLightTag, const LIGHT_DESC& LightDesc);
	HRESULT				Render_Light(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#pragma endregion

#pragma region EVENTBUS
public:
	template <typename EVENT>
	void Subscribe(_uint iEventTypeIndex, function<void(const EVENT&)> Subscriber) {
		m_pEventBus->Subscribe(iEventTypeIndex, Subscriber); }

	void Publish(_uint iEventTypeIndex, const CEvent& Event);
#pragma endregion

#pragma region CAMERA_MANAGER
public:
	HRESULT Add_CameraToManager(_uint iPrototypeIndex, const _wstring& strPrototypeTag, const _wstring& strCameraTag, class CCamera** ppOut = nullptr, void* pArg = nullptr);
	HRESULT Change_Camera(const _wstring& strCameraTag, void* pArg = nullptr);
	HRESULT Change_Camera(class CCamera* pNewCamera);
	_float	Get_CurrentCamera_Near();
	_float	Get_CurrentCamera_Far();
#pragma endregion

#pragma region PIPELINE
public:
	const _float4x4*	Get_Transform_Float4x4(D3DTS eTransformState) const;
	_matrix				Get_Transform_Matrix(D3DTS eTransformState) const;
	const _float4x4*	Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const;
	_matrix				Get_Transform_Matrix_Inverse(D3DTS eTransformState) const;
	const _float4*		Get_CamPosition() const;
	void				Set_Transform(D3DTS eTransformState, _fmatrix Matrix);
	void				Set_Transform(D3DTS eTransformState, const _float4x4& Matrix);
#pragma endregion

#pragma region CONTROLLER
public:
	HRESULT			Add_Controller_ToManager(const _wstring& strControllerTag, class CController* pController);
	HRESULT			Change_Controller(_uint iChannelIndex, const _wstring& strControllerTag);
	HRESULT			Change_Controller(_uint iChannelIndex, class CController* pNewController);
	HRESULT			MoveInput(INPUT_MOVE_DESC* pOut);
	HRESULT			ActionInput(INPUT_ACTION_DESC* pOut);
	HRESULT			CameraInput(INPUT_CAMERA_DESC* pOut);
	HRESULT			UI_Input(INPUT_UI_DESC* pOut);
#pragma endregion

#pragma region NAVIGATION
	HRESULT				Add_Navigation(_uint iNavigationLevel, const _tchar* pNavigationFilePath);
	HRESULT				Change_Navigation(_uint iNavigationLevel);
	class CNavigation*	Clone_CurrentNavigation(_int iCellIndex);
#pragma endregion

#pragma region COLLIDER
	void				Set_BoudingChannel(_uint iBoundingChannel);
	HRESULT				Add_Channel(_uint iSrcChannel, _uint iDstChannel, COLLIDER_TYPE eType);
	HRESULT				Add_Collider(_uint iChannel, CGameObject* pOwner, CCollider* pCollider);

	HRESULT				Add_BoundingCollider(CGameObject* pOwner, CCollider* pBounding_Collider);
	HRESULT				Add_ActionCollider(CGameObject* pOwner, CCollider* pAction_Collider);
#pragma endregion

#pragma region MOUSE_POINT
	void				Set_MousePoint(POINT ptMouse) { m_ptMousePoint = ptMouse; }
	POINT				Get_MousePoint() { return m_ptMousePoint; }
#pragma endregion

#pragma region SHADOW
	const _float4x4*	Get_ShadowLight_Transform_Float4x4(D3DTS eTransformState) const;	
	HRESULT				Update_ShadowLight(const SHADOW_LIGHT_DESC& ShadowLightDesc);
	void				Update_ShadowLight(_fvector vTargetPosition);

	//const _float*		Get_ShadowLight_Far() const;
	//const _float4x4*	Get_Shadow_ViewMatrix() const;
	//HRESULT				Bind_Shadow_ProjMatrix(CShader* pShader, const _char* pMatrixConstantName, const _char* pIndexConstantName, _fvector vWorldPosition);
	//HRESULT				Bind_Shadow_ProjMatrices(CShader* pShader, const _char* pConstantName);
	//HRESULT				Bind_SplitDistance(CShader* pShader, const _char* pConstantName);
#pragma endregion

#pragma region FRUSTUM
	const _float4*	Get_Frustum_WorldPoints() const;
#pragma endregion

private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CRenderState*			m_pRenderState = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CRT_Manager*			m_pRT_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CGameObject_Manager*	m_pObject_Manager = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };

	class COctree*				m_pOctree = { nullptr };
	CEventBus*					m_pEventBus = { nullptr };
	class CCamera_Manager*		m_pCamera_Manager = { nullptr };
	class CController_Manager*	m_pController_Manager = { nullptr };
	class CNavigation_Manager*	m_pNavigation_Manager = { nullptr };
	class CCollider_Manager*	m_pCollider_Manager = { nullptr };
	class CShadow*				m_pShadow = { nullptr };
	class CFrustum*				m_pFrustum = { nullptr };

	POINT						m_ptMousePoint = {};
public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END