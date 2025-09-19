#include "ClientPch.h"

#include "MainApp.h"

//LEVEL
#include "Level_Loading.h"
#include "Level_GamePlay.h"
#include "Level_Logo.h"
#include "Level_Town.h"
#include "Level_Field.h"
#include "Level_Queen.h"
#include "Level_Glasgavelen.h"

//UI
#include "Panel.h"
#include "Button.h"
#include "Bar.h"
#include "LoadingScreen.h"
#include "LoadingBar.h"
#include "LoadingPoint.h"
#include "LogoScreen.h"
#include "HUD.h"
#include "OptionMain.h"
#include "OptionController.h"
#include "StateBar.h"
#include "UI_Container.h"
#include "Mouse.h"
#include "Inventory.h"
#include "Storage.h"
#include "Equipment.h"
#include "Slot.h"
#include "BossHPBar.h"
#include "BossHP.h"
#include "Palette.h"
#include "NPCDialog.h"
#include "Dyeing.h"
#include "DyeingSlots.h"
#include "DyeingPalette.h"
#include "ColorPoint.h"

//Controller
#include "Controller_KeyBoard.h"
#include "Controller_UI.h"

//STATE FACTORY
#include "StateFactory.h"
#include "MonsterStateFactory.h"

//Player
#include "Camera_Target.h"
#include "PlayerPawn.h"
#include "PlayerBody.h"
#include "Armor.h"
#include "Weapon.h"

//Map
#include "Map.h"
#include "MapObject.h"

//Effect
#include "Effect_Static.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::GetInstance()}
	, m_pPlayerInstance { CPlayerInstance::GetInstance()}
	, m_pPool_Instance { CPool_Instance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pPlayerInstance);
	Safe_AddRef(m_pPool_Instance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);
	
	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	m_pStateFactory = CStateFactory::GetInstance();
	m_pMonsterStateFactory = CMonsterStateFactory::GetInstance();

	if (FAILED(m_pPlayerInstance->Initialize(g_iInventoryCount)))
		return E_FAIL;

	if (FAILED(m_pPool_Instance->Initialize()))
		return E_FAIL;

	if (FAILED(Ready_DefaultColliderChannel()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic()))
		return E_FAIL;
	
	if (FAILED(Ready_UI_Container()))
		return E_FAIL;

	if (FAILED(Ready_Navigations()))
		return E_FAIL;

	if (FAILED(Ready_Controller()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::TOWN)))
		return E_FAIL;

	m_pGameInstance->Subscribe<EVENT_LEVEL_CHANGE>(ENUM_CLASS(EVENT_TYPE::STATIC), [this](const EVENT_LEVEL_CHANGE& Event) {
		this->Event_LevelChange(Event); });

	return S_OK;
}

void CMainApp::Post_Update()
{
	if (m_bChange_Level)
	{
		if (FAILED(m_pGameInstance->Clear_Resources()))
			MSG_BOX(TEXT("Failed Clear Resrouces"));

		m_pPool_Instance->ClearLevel();
		m_pPlayerInstance->Reset();
		m_pGameInstance->Clear_RT(TEXT("RT_MapLightDepth"));

		EVENT_UI_LEVEL_CHANGE Event_UIChange;
		Event_UIChange.iChange_Level = m_iChange_Level;
		Event_UIChange.bIsLoading = m_bIsLoading;
		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event_UIChange);



		CLevel* pNextLevel = Create_NewLevel(m_iChange_Level);

		if (FAILED(m_pGameInstance->Open_Level(m_iChange_Level, pNextLevel)))
			MSG_BOX(TEXT("Failed Open Level"));

		m_bChange_Level = false;
	}
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);
	m_pGameInstance->Draw();
	m_pGameInstance->Render_End();

	return S_OK;
}

void CMainApp::Event_LevelChange(const EVENT_LEVEL_CHANGE& Event)
{
	m_bChange_Level = true;
	m_iChange_Level = Event.iChange_Level;
	m_bIsLoading = Event.bIsLoading;
}

CLevel* CMainApp::Create_NewLevel(_uint iChangeLevel)
{
	CLevel* pNewLevel = { nullptr };

	if (m_bIsLoading)
	{
		switch (static_cast<LEVEL>(m_iChange_Level))
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pDeviceContext);
			break;
		case LEVEL::TOWN:
		{
			if (FAILED(m_pGameInstance->Change_Navigation(m_iChange_Level)))
				MSG_BOX(TEXT("Failed Change Navigation"));

			pNewLevel = CLevel_Town::Create(m_pDevice, m_pDeviceContext);

			break;
		}
		case LEVEL::FIELD:
		{
			if (FAILED(m_pGameInstance->Change_Navigation(m_iChange_Level)))
				MSG_BOX(TEXT("Failed Change Navigation"));
			pNewLevel = CLevel_Field::Create(m_pDevice, m_pDeviceContext);
			break;
		}
		case LEVEL::QUEEN:
		{
			if (FAILED(m_pGameInstance->Change_Navigation(m_iChange_Level)))
				MSG_BOX(TEXT("Failed Change Navigation"));

			pNewLevel = CLevel_Queen::Create(m_pDevice, m_pDeviceContext);

			break;
		}
		case LEVEL::GLASGAVELEN:
		{
			if (FAILED(m_pGameInstance->Change_Navigation(m_iChange_Level)))
				MSG_BOX(TEXT("Failed Change Navigation"));

			pNewLevel = CLevel_Glasgavelen::Create(m_pDevice, m_pDeviceContext);

			break;
		}
		}
	}
	else
	{
		pNewLevel = CLevel_Loading::Create(m_pDevice, m_pDeviceContext, static_cast<LEVEL>(m_iChange_Level));
		m_iChange_Level = ENUM_CLASS(LEVEL::LOADING);
	}

	return pNewLevel;
}

HRESULT CMainApp::Ready_DefaultColliderChannel()
{
	m_pGameInstance->Set_BoudingChannel(ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING));

	m_pGameInstance->Add_Channel(ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING), COLLIDER_TYPE::OVERLAP);
	m_pGameInstance->Add_Channel(ENUM_CLASS(COLLIDER_CHANNEL::BODY), ENUM_CLASS(COLLIDER_CHANNEL::BODY), COLLIDER_TYPE::BLOCK);
	m_pGameInstance->Add_Channel(ENUM_CLASS(COLLIDER_CHANNEL::ATTACK), ENUM_CLASS(COLLIDER_CHANNEL::HIT), COLLIDER_TYPE::OVERLAP);
	m_pGameInstance->Add_Channel(ENUM_CLASS(COLLIDER_CHANNEL::GRAP), ENUM_CLASS(COLLIDER_CHANNEL::GRAP), COLLIDER_TYPE::OVERLAP);
	m_pGameInstance->Add_Channel(ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), ENUM_CLASS(COLLIDER_CHANNEL::INTERACTION), COLLIDER_TYPE::OVERLAP);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic()
{
#pragma region SHADER
	/* Prototype_Component_Shader_VtxPosTex */
	if(FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Paritlce"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_Particle.hlsl"), VTXPARTICLE::Elements, VTXPARTICLE::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParitlce"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_PointParticle.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTrail"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxTrail.hlsl"), VTXTRAIL::Elements, VTXTRAIL::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Palette"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Palette.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Distortion"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Effect_Distortion.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, COLLIDER::AABB))))
		return E_FAIL;

	/* Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, COLLIDER::OBB))))
		return E_FAIL;

	/* Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pDeviceContext, COLLIDER::SPHERE))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Container"),
		CColliderContainer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
#pragma endregion

#pragma region VIBUFFER
	/* Prototype_Component_VIBuffer_Rect*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Sphere"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pDeviceContext, 32, 32))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pDeviceContext, 48))))
		return E_FAIL;
#pragma endregion

	/* Prototype_GameObject_Camera_Target */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Target"),
		CCamera_Target::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_MapObject */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Map"),
		CMap::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_MapObject */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic_UI()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic_Texture()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic_Player()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic_Texture()
{	
	//test
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Test"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Test.png"), 1))))
		return E_FAIL;

#pragma region LOADING_UI
	/* Ready_Prototype_Component_Texture_LoadingScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingScreen"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingScreen%d.png"), 5))))
		return E_FAIL;

	/* Ready_Prototype_Component_Texture_LoadingBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingBar.png"), 1))))
		return E_FAIL;

	/* Ready_Prototype_Component_Texture_LoadingBar_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingBack.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LoadingBar_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar_Point"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/Point.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region LOGO_UI
	/* Prototype_Component_Texture_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Sky_Box.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Logo_WaterMark */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Logo_WaterMark"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Watermark.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Logo_WaterMark_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Logo_WaterMark_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Watermark_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Logo_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Logo_Button"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Button.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region GAMEPLAY_UI
	/* Prototype_Component_Texture_GamePlay_Back_PlayerHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Back_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Lerp_PlayerHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Lerp_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar_Lerp.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_PlayerHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Back_PlayerStaminaBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Back_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Lerp_PlayerStaminaBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Lerp_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar_Lerp.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_PlayerStaminaBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Option_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Option_Background"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Option_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Keyboard */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Keyboard"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/KeyBoard.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Mouse"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Mouse.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_OptionButton */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_OptionButton"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Option_Button.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Cursor"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Cursor%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Inventroy_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Inventroy_Background"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Inventory_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Storage */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Storage"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Storage.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Equipment */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Equipment"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Equipment.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_BossHP */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_BossHP"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/BossHp%d.png"), 3))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_BossHP_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_BossHP_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/BossHP_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Text_Dialog */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Text_Dialog"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/TextDialog.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Dialog_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Dialog_Button"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/DialogButton.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_DyeingPalette */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_DyeingPalette"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/DyeingPalette%d.png"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_DyeingBack */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_DyeingBack"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/DyeingBack.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_DyeingColorBack */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_DyeingColorBack"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/DyeingColorBack.png"), 1))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic_UI()
{
#pragma region UI
	/* Ready_Prototype_GameObject_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"),
		CButton::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Bar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Bar"),
		CBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

#pragma region LOADING_UI
	/* Prototype_UIObject_LoadingScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingScreen"),
		CLoadingScreen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_LoadingBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingBar"),
		CLoadingBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Panel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"),
		CPanel::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_LoadingPoint */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingPoint"),
		CLoadingPoint::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Mouse"),
		CMouse::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_UIContainer */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_UIContainer"),
		CUI_Container::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

#pragma endregion

#pragma region LOGO_UI
	/* Prototype_UIObject_LogoScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LogoScreen"),
		CLogoScreen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
#pragma endregion

#pragma region GAMEPLAY_UI
	/* Prototype_UIObject_HUD */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_HUD"),
		CHUD::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_StateBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_StateBar"),
		CStateBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Option */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Option"),
		COptionMain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_OptionController */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_OptionController"),
		COptionController::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Inventory */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Inventory"),
		CInventory::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Storage */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Storage"),
		CStorage::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Equipment */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Equipment"),
		CEquipment::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Slot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Slot"),
		CSlot::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_BossHP */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_BossHP"),
		CBossHP::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_BossHPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_BossHPBar"),
		CBossHPBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Palette */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Palette"),
		CPalette::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_NPC_Dialog */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_NPC_Dialog"),
		CNPCDialog::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_NPC_Dialog */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Dyeing"),
		CDyeing::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_DyeingSlots */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_DyeingSlots"),
		CDyeingSlots::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_DyeingPalette */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_DyeingPalette"),
		CDyeingPalette::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_ColorPoint */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_ColorPoint"),
		CColorPoint::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


#pragma endregion

#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic_Player()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(180.0f), 0.f);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * RotationMatrix;

	/* Prototype_Component_Model_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/Piona.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_GameObject_Player_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player_Body"),
		CPlayerBody::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_PlayerPawn */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_PlayerPawn"),
		CPlayerPawn::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"),
		CArmor::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_UI_Container()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_UIContainer"),
		ENUM_CLASS(LAYER_TYPE::STATIC), TEXT("Layer_UI_Container"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Controller()
{
	if (FAILED(m_pGameInstance->Add_Controller_ToManager(TEXT("Controller_KeyBoard"), CController_KeyBoard::Create())))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Change_Controller(ENUM_CLASS(CONTROLLER_CHANNEL::MAIN), TEXT("Controller_KeyBoard"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Controller_ToManager(TEXT("Controller_UI"), CController_UI::Create())))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Change_Controller(ENUM_CLASS(CONTROLLER_CHANNEL::UI), TEXT("Controller_UI"))))
		return E_FAIL;

	return S_OK;
	
}

HRESULT CMainApp::Ready_Navigations()
{
	if (FAILED(m_pGameInstance->Add_Navigation(ENUM_CLASS(LEVEL::TOWN), TEXT("../Bin/Resources/Map/Town_Navigation.dat"))))
		return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_Navigation(ENUM_CLASS(LEVEL::FILED), TEXT("../Bin/Resources/Map/Field_Navigation.dat"))))
	//	return E_FAIL;
	//
	if (FAILED(m_pGameInstance->Add_Navigation(ENUM_CLASS(LEVEL::QUEEN), TEXT("../Bin/Resources/Map/QueenMap_Navigation.dat"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Navigation(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("../Bin/Resources/Map/GavelenMap_Navigation.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	return m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eStartLevelID));
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();
	
	Safe_Release(m_pStateFactory);
	Safe_Release(m_pMonsterStateFactory);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);

	m_pPlayerInstance->Release_PlayerInstance();
	Safe_Release(m_pPlayerInstance);

	m_pPool_Instance->Release_MonsterInstance();
	Safe_Release(m_pPool_Instance);
}
