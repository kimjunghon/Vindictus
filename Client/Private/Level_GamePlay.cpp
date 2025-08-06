#include "ClientPch.h"

#include "Level_GamePlay.h"
#include "UIObject.h"
#include "MapObject.h"
#include "Pawn.h"
#include "Controller_KeyBoard.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CLevel { pDevice, pDeviceContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Right()))
		return E_FAIL;

	if (FAILED(Ready_Player(TEXT("Layer_Player"))))
        return E_FAIL;
	
	if(FAILED(Ready_GameObject(TEXT("Layer_GameObject"))))
		return E_FAIL;
	
	if (FAILED(Ready_UI(TEXT("Layer_UI"))))
		return E_FAIL;
	
	if (FAILED(Ready_Controller()))
		return E_FAIL;

    return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_A))
	{
		EVENT_PROGRESSBAR Event;
		Event.eType = PROGRESS_TYPE::HP;
		Event.fRatio = 0.2f;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}

	if (m_pGameInstance->Get_KeyPressing(DIK_A))
	{

		EVENT_PROGRESSBAR Event;
		Event.eType = PROGRESS_TYPE::STAMINA;
		Event.fRatio = 0.5f;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}

	if (m_pGameInstance->Get_KeyUp(DIK_A))
	{
		EVENT_PROGRESSBAR Event;
		Event.eType = PROGRESS_TYPE::HP;
		Event.fRatio = 1.f;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

		Event.eType = PROGRESS_TYPE::STAMINA;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}

	if (m_pGameInstance->Get_KeyDown(DIK_ESCAPE))
	{
		EVENT_UI_CHANGE Event;
		Event.byVisibleType = ENUM_CLASS(GAMEPLAY_UI::DEFAULT) + ENUM_CLASS(GAMEPLAY_UI::OPTION);
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::GAMEPLAY), Event);
	}

	if(m_pGameInstance->Get_KeyDown(DIK_RETURN))
	{
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::TOWN);
		Event.bIsLoading = false;
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}
}

HRESULT CLevel_GamePlay::Render()
{
    SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Right()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("DIRECTONAL"), LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UI(const _wstring& strLayerTag)
{


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Player(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC GameObjectDesc = {};
	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PlayerPawn"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &GameObjectDesc)))

		return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_GameObject(const _wstring& strLayerTag)
{
	if (FAILED(Ready_MapObject(TEXT("Layer_MapObject"))))
		return E_FAIL;

	CGameObject::GAMEOBJECT_DESC GameObjectDesc = {};
	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Glasgavelen"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &GameObjectDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Basic"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &GameObjectDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Elder"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &GameObjectDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Royal"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &GameObjectDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Queen"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_MapObject(const _wstring& strLayerTag)
{
	ifstream File("../Bin/Resources/QueenMap.dat", ios::binary);
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed File Open QueenMap"));
		return E_FAIL;
	}

	size_t iNumObjects = {};
	File.read(reinterpret_cast<_char*>(&iNumObjects), sizeof(size_t));

	for (size_t i = 0; i < iNumObjects; i++)
	{
		size_t iNameLength = {};
		_char szName[MAX_PATH] = {};
		_float4x4 WorldMatrx = {};

		File.read(reinterpret_cast<_char*>(&iNameLength), sizeof(size_t));
		File.read(szName, sizeof(_char) * iNameLength);
		File.read(reinterpret_cast<_char*>(&WorldMatrx), sizeof(_float4x4));

		_char szPrototype[MAX_PATH] = "Prototype_Component_Model_";
		_char szPrototypeTag[MAX_PATH] = {};
		strcpy_s(szPrototypeTag, szPrototype);
		strcat_s(szPrototypeTag, szName);

		_tchar szWidePrototypeTag[MAX_PATH] = {};

		MultiByteToWideChar(CP_UTF8, 0, szPrototypeTag, static_cast<_int>(strlen(szPrototypeTag)), szWidePrototypeTag, MAX_PATH);

		CMapObject::MAP_OBJECT_DESC MapObjectDesc = {};
		MapObjectDesc.iModelLevel = ENUM_CLASS(LEVEL::GAMEPLAY);
		MapObjectDesc.strModelTag = szWidePrototypeTag;
		MapObjectDesc.WorldMatrix = WorldMatrx;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_MapObject"), 
			ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &MapObjectDesc)))
			return E_FAIL;
	}


	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
