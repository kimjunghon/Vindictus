#include "ClientPch.h"
#include "Level_Town.h"

#include "UIObject.h"
#include "MapObject.h"
#include "Pawn.h"
#include "Controller_KeyBoard.h"
#include "Camera_Free.h"

CLevel_Town::CLevel_Town(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel{ pDevice, pDeviceContext }
{
}

HRESULT CLevel_Town::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_GameObject(TEXT("Layer_GameObject"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Town::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Town::Render()
{
	return S_OK;
}

HRESULT CLevel_Town::Ready_Light()
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

HRESULT CLevel_Town::Ready_Player(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC GameObjectDesc = {};
	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_PlayerPawn"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag, &GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Town::Ready_GameObject(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_GameObject_Map_Town"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), strLayerTag)))
		return E_FAIL;


	return S_OK;
}

CLevel_Town* CLevel_Town::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Town* pInstance = new CLevel_Town(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Town"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Town::Free()
{
	__super::Free();
}
