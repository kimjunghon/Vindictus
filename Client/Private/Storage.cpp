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

	m_pGameInstance->Subscribe<EVENT_ADD_ITEM>(ENUM_CLASS(EVENTTYPE::STATIC), [this](const EVENT_ADD_ITEM& Event) {
		this->Event_Add_Item(Event); });

	return S_OK;
}

void CStorage::Priority_Update(_float fTimeDelta)
{
}

void CStorage::Update(_float fTimeDelta)
{
}

void CStorage::Late_Update(_float fTimeDelta)
{
}

HRESULT CStorage::Render()
{
	return S_OK;
}

HRESULT CStorage::Ready_Children()
{
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