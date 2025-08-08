#include "ClientPch.h"
#include "Storage.h"


CStorage::CStorage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
{
}

CStorage::CStorage(const CStorage& Prototype)
	: CUI_Panel { Prototype }
{
}

HRESULT CStorage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStorage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	m_pGameInstance->Subscribe<EVENT_ADD_ITEM>(ENUM_CLASS(EVENTTYPE::STATIC), [this](const EVENT_ADD_ITEM& Event) {
		this->Event_Add_Item(Event); });

	return S_OK;
}

void CStorage::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CStorage::Update(_float fTimeDelta)
{
	__super::Children_Update(fTimeDelta);
}

void CStorage::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CStorage::Render()
{
	return S_OK;
}

HRESULT CStorage::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC Children_Desc{};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX;
	Children_Desc.fSizeY = m_fSizeY;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 0.f;
	Children_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Inventroy_Background");
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	Children_Desc.IsBlend = true;
	Children_Desc.fAlpha = 0.5f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;
	
	Children_Desc.fSizeX -= 50.f;
	Children_Desc.fSizeY -= 40.f;
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Storage");
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Children_Desc.IsBlend = false;
	Children_Desc.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;


	return S_OK;
}

void CStorage::Event_Add_Item(const EVENT_ADD_ITEM& Event)
{

}

HRESULT CStorage::Add_Item(ITEM_TYPE eItemType, CGameObject* pItem)
{
	if (nullptr == pItem)
		return E_FAIL;

	m_Items.emplace_back(eItemType, pItem);

	return S_OK;
}

CStorage* CStorage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStorage* pInstance = new CStorage(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CStorage"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CStorage::Clone(void* pArg)
{
	CStorage* pInstance = new CStorage(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CStorage"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStorage::Free()
{
	__super::Free();
}