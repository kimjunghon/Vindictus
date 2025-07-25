#include "EnginePch.h"
#include "GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "RenderState.h"
/* Manager */
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Prototype_Manager.h"
#include "GameObject_Manager.h"
#include "GameObject.h"
#include "Renderer.h"
#include "DynamicAABBTree.h"
#include "Octree.h"
#include "EventBus.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Camera_Manager.h"
#include "Controller_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContext)
{
    m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppDeviceContext);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pInput_Device = CInput_Device::Create(EngineDesc.hInst, EngineDesc.hWnd);
    if (nullptr == m_pInput_Device)
        return E_FAIL;

    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    m_pRenderState = CRenderState::Create(*ppDevice, *ppDeviceContext);
    if (nullptr == m_pRenderState)
        return E_FAIL;

    m_pLevel_Manager = CLevel_Manager::Create();
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;

    m_pRenderer = CRenderer::Create(*ppDevice, *ppDeviceContext);
    if (nullptr == m_pRenderer)
        return E_FAIL;

    m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
    if (nullptr == m_pPrototype_Manager)
        return E_FAIL;

    m_pObject_Manager = CGameObject_Manager::Create();
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    m_pEventBus = CEventBus::Create(EngineDesc.iNumLevels);
    if (nullptr == m_pEventBus)
        return E_FAIL;

    m_pLight_Manager = CLight_Manager::Create();
    if (nullptr == m_pLight_Manager)
        return E_FAIL;

    m_pPipeLine = CPipeLine::Create();
    if (nullptr == m_pPipeLine)
        return E_FAIL;

    m_pCamera_Manager = CCamera_Manager::Create();
    if (nullptr == m_pCamera_Manager)
        return E_FAIL;

    m_pController_Manager = CController_Manager::Create();
    if (nullptr == m_pController_Manager)
        return E_FAIL;

    return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
    m_pInput_Device->Update();
    m_pController_Manager->Update(fTimeDelta);

    m_pObject_Manager->Priority_Update(fTimeDelta);

    m_pCamera_Manager->Update(fTimeDelta);
    m_pPipeLine->Update();

    m_pObject_Manager->Update(fTimeDelta);
    m_pObject_Manager->Late_Update(fTimeDelta);

    m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Clear_Resources(_uint iClearLevelID)
{
    m_pPrototype_Manager->Clear(iClearLevelID);
    m_pEventBus->Clear(iClearLevelID);
    m_pObject_Manager->Clear();
    m_pCamera_Manager->Clear();
    m_pController_Manager->Clear();
    m_pLight_Manager->Clear();

    return S_OK;
}

void CGameInstance::Render_Begin(const _float4* pClearColor)
{
    if (nullptr == m_pGraphic_Device)
        return;

    m_pGraphic_Device->Clear_BackBuffer_View(pClearColor);

    m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Draw()
{
    if (nullptr == m_pLevel_Manager ||
        nullptr == m_pRenderer)
        return E_FAIL;

    m_pRenderer->Draw();

    return m_pLevel_Manager->Render();
}

void CGameInstance::Render_End(HWND hWnd)
{
    if (nullptr == m_pGraphic_Device)
        return;

    m_pGraphic_Device->Present();
}

_float CGameInstance::Rand_Normal()
{
    return static_cast<_float>(rand()) / RAND_MAX;
}

_float CGameInstance::Rand(_float fMin, _float fMax)
{
    return fMin + Rand_Normal() * (fMax - fMin);
}


#pragma region INPUT_DEVICE
_bool CGameInstance::Get_KeyDown(_ubyte byKeyID)
{
    return m_pInput_Device->Get_KeyDown(byKeyID);
}

_bool CGameInstance::Get_KeyUp(_ubyte byKeyID)
{
    return m_pInput_Device->Get_KeyUp(byKeyID);
}

_bool CGameInstance::Get_KeyPressing(_ubyte byKeyID)
{
    return m_pInput_Device->Get_KeyPressing(byKeyID);
}

_byte CGameInstance::Get_MouseState(MOUSEKEYSTATE eMouse)
{
    return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_MouseMove(MOUSEMOVESTATE eState)
{
    return m_pInput_Device->Get_DIMouseMove(eState);
}

#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
    return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
    return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
    m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}
#pragma endregion

#pragma region RENDER_STATE
HRESULT CGameInstance::RSSetState(_uint iRSIndex)
{
    return m_pRenderState->RSSetState(iRSIndex);
}
HRESULT CGameInstance::RSSetState(const _wstring& strRSTag)
{
    return m_pRenderState->RSSetState(strRSTag);
}
HRESULT CGameInstance::BSSetState(_uint iBSIndex)
{
    return m_pRenderState->BSSetState(iBSIndex);
}
HRESULT CGameInstance::BSSetState(const _wstring& strBSTag)
{
    return m_pRenderState->BSSetState(strBSTag);
}
HRESULT CGameInstance::DSSSetState(_uint iDSSIndex)
{
    return m_pRenderState->DSSSetState(iDSSIndex);
}
HRESULT CGameInstance::Add_CustomRS(const _wstring& strRSTag, const D3D11_RASTERIZER_DESC& RS_DESC)
{
    return m_pRenderState->Add_CustomRS(strRSTag, RS_DESC);
}
HRESULT CGameInstance::Add_CustomBS(const _wstring& strBSTag, const D3D11_BLEND_DESC& BS_DESC)
{
    return m_pRenderState->Add_CustomBS(strBSTag, BS_DESC);
}
#pragma endregion

#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Open_Level(_uint iLevelID, CLevel* pOpenLevel)
{
    return m_pLevel_Manager->Open_Level(iLevelID, pOpenLevel);
}
HRESULT CGameInstance::Clear_Resources()
{
    return m_pLevel_Manager->Clear_Resources();
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
    return m_pPrototype_Manager->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag, pPrototype);
}
CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
    return m_pPrototype_Manager->Clone_Prototype(ePrototype, iPrototpyeLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerIndex, const _wstring& strLayerTag, void* pArg)
{
    return m_pObject_Manager->Add_GameObject_ToLayer(iLayerIndex, strLayerTag, iPrototypeLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion

#pragma region OCTREE
HRESULT CGameInstance::Create_Octree(COctree::OCTREE_DESC& tOctreeDesc)
{
    m_pOctree = COctree::Create(tOctreeDesc);

    if (nullptr == m_pOctree)
        return E_FAIL;

    return S_OK;
}

HRESULT CGameInstance::Add_Objects(CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox)
{
    return m_pOctree->Add_ObjectToTree(pGameObject, ObjectBoundingBox);
}
#pragma endregion

#pragma region DYNAMICAABB_TREE
HRESULT CGameInstance::Add_Node(CGameObject* pGameObject, const BoundingBox& ObjectBoundingBox)
{
    return m_pDynamicAABBTree->Add_Node(pGameObject, ObjectBoundingBox);
}
#pragma endregion 

#pragma region RENDERER
HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
    return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}
#pragma endregion

#pragma region LIGHT_MANAGER
const LIGHT_DESC* CGameInstance::Get_LightDesc(const _wstring& strLightTag)
{
    return m_pLight_Manager->Get_LightDesc(strLightTag);
}

HRESULT CGameInstance::Add_Light(const _wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
    return m_pLight_Manager->Add_Light(strLightTag, LightDesc);
}
#pragma endregion

#pragma region EVENTBUS
void CGameInstance::Publish(_uint iEventLevelIndex, const CEvent& Event)
{
    m_pEventBus->Publish(iEventLevelIndex, Event);
}
#pragma endregion

#pragma region CAMERA_MANAGER
HRESULT CGameInstance::Add_CameraToManager(_uint iPrototypeIndex, const _wstring& strPrototypeTag, const _wstring& strCameraTag, CCamera** ppOut, void* pArg)
{
    return m_pCamera_Manager->Add_CameraToManager(iPrototypeIndex, strPrototypeTag, strCameraTag, ppOut, pArg);
}

HRESULT CGameInstance::Change_Camera(const _wstring& strCameraTag)
{
    return m_pCamera_Manager->Change_Camera(strCameraTag);
}
HRESULT CGameInstance::Change_Camera(CCamera* pNewCamera)
{
    return m_pCamera_Manager->Change_Camera(pNewCamera);
}
#pragma endregion

#pragma region PIPELINE
const _float4x4* CGameInstance::Get_Transform_Float4x4(D3DTS eTransformState) const
{
    return m_pPipeLine->Get_Transform_Float4x4(eTransformState);
}
_matrix CGameInstance::Get_Transform_Matrix(D3DTS eTransformState) const
{
    return m_pPipeLine->Get_Transform_Matrix(eTransformState);
}
const _float4x4* CGameInstance::Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const
{
    return m_pPipeLine->Get_Transform_Float4x4_Inverse(eTransformState);
}
_matrix CGameInstance::Get_Transform_Matrix_Inverse(D3DTS eTransformState) const
{
    return m_pPipeLine->Get_Transform_Matrix_Inverse(eTransformState);
}
const _float4* CGameInstance::Get_CamPosition() const
{
    return m_pPipeLine->Get_CamPosition();
}
void CGameInstance::Set_Transform(D3DTS eTransformState, _fmatrix Matrix)
{
    m_pPipeLine->Set_Transform(eTransformState, Matrix);
}
void CGameInstance::Set_Transform(D3DTS eTransformState, const _float4x4& Matrix)
{
    m_pPipeLine->Set_Transform(eTransformState, Matrix);
}
#pragma endregion

#pragma region CONTROLLER_MANAGER
HRESULT CGameInstance::Add_Controller_ToManager(const _wstring& strControllerTag, CController* pController)
{
    return m_pController_Manager->Add_Controller_ToManager(strControllerTag, pController);
}
HRESULT CGameInstance::Change_Controller(_uint iChannelIndex, const _wstring& strControllerTag)
{
    return m_pController_Manager->Change_Controller(iChannelIndex, strControllerTag);
}
HRESULT CGameInstance::Change_Controller(_uint iChannelIndex, CController* pNewController)
{
    return m_pController_Manager->Change_Controller(iChannelIndex, pNewController);
}
HRESULT CGameInstance::MoveInput(_uint iChannelIndex, INPUT_MOVE_DESC* pOut)
{
    return m_pController_Manager->MoveInput(iChannelIndex, pOut);
}
HRESULT CGameInstance::ActionInput(_uint iChannelIndex, INPUT_ACTION_DESC* pOut)
{
    return m_pController_Manager->ActionInput(iChannelIndex, pOut);
}
HRESULT CGameInstance::CameraInput(_uint iChannelIndex, INPUT_CAMERA_DESC* pOut)
{
    return m_pController_Manager->CameraInput(iChannelIndex, pOut);
}
HRESULT CGameInstance::UI_Input(_uint iChannelIndex, INPUT_UI_DESC* pOut)
{
    return m_pController_Manager->UI_Input(iChannelIndex, pOut);
}
#pragma endregion

void CGameInstance::Release_Engine()
{
    Release();

    Safe_Release(m_pGraphic_Device);
    Safe_Release(m_pInput_Device);
    Safe_Release(m_pTimer_Manager);
    Safe_Release(m_pRenderState);
    Safe_Release(m_pLevel_Manager);
    Safe_Release(m_pPrototype_Manager);
    Safe_Release(m_pRenderer);
    Safe_Release(m_pObject_Manager);
    Safe_Release(m_pEventBus);
    Safe_Release(m_pPipeLine);
    Safe_Release(m_pLight_Manager);
    Safe_Release(m_pCamera_Manager);
    Safe_Release(m_pController_Manager);
}

void CGameInstance::Free()
{
    __super::Free();
}
